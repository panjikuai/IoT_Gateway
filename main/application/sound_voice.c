#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <lwip/sockets.h>
#include "lwip/dns.h"
#include "lwip/inet.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/i2s.h"
#include <math.h>
#include "wm8978.h"
#include "driver/ledc.h"

#include "iot_debug.h"
#include "ledDisplay.h"

#define I2S_NUM             (0)
#define BLOCK_SIZE          (240)

extern const uint8_t alarm_wav_start[]      asm("_binary_alarm_wav_start");
extern const uint8_t alarm_wav_end[]        asm("_binary_alarm_wav_end");
extern const uint8_t dingdong_wav_start[]   asm("_binary_dingdong_wav_start");
extern const uint8_t dingdong_wav_end[]     asm("_binary_dingdong_wav_end");

void playWaveFile(const uint8_t *pFile);
QueueHandle_t soundVoideEventQueue = NULL;

typedef struct{
    uint8_t     ChunkID[4];   //内容为"RIFF"
    uint32_t    ChunkSize;    //存储文件的字节数（不包含ChunkID和ChunkSize这8个字节）
    uint8_t     Format[4];    //内容为"WAVE"
}wave_header_t;

typedef struct{
    uint8_t     Subchunk1ID[4];  //内容为"fmt"
    uint32_t    Subchunk1Size;   //存储该子块的字节数（不含前面的Subchunk1ID和Subchunk1Size这8个字节）
    uint16_t    AudioFormat;     //存储音频文件的编码格式，例如若为PCM则其存储值为1，若为其他非PCM格式的则有一定的压缩。
    uint16_t    NumChannels;     //通道数，单通道(Mono)值为1，双通道(Stereo)值为2，等等
    uint32_t    SampleRate;      //采样率，如8k，44.1k等
    uint32_t    ByteRate;        //每秒存储的bit数，其值=SampleRate * NumChannels * BitsPerSample/8
    uint16_t    BlockAlign;      //块对齐大小，其值=NumChannels * BitsPerSample/8
    uint16_t    BitsPerSample;   //每个采样点的bit数，一般为8,16,32等。
}wave_fmt_t;

typedef struct{
    uint8_t     Subchunk2ID[4];   //内容为“data”
    uint32_t    Subchunk2Size;    //内容为接下来的正式的数据部分的字节数，其值=NumSamples * NumChannels * BitsPerSample/8
}wave_data_header_t;

typedef struct{
    wave_header_t       header;
    wave_fmt_t          fmt;
}wave_format_t;

typedef struct{
    wave_data_header_t data_header;
    uint8_t            data[1];
}wave_data_t;


void I2S_Init(void)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,                                  // Only TX
        .sample_rate = 44100,//SAMPLE_RATE,
        .bits_per_sample = 16,                                                  //16-bit per channel
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,//((wave_format->fmt.NumChannels == 2)? I2S_CHANNEL_FMT_RIGHT_LEFT : I2S_CHANNEL_FMT_ALL_RIGHT),                           //2-channels
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 60,                                                      //
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                //Interrupt level 1
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num     = 14,
        .ws_io_num      = 12,
        .data_out_num   = 26,
        .data_in_num    = -1 //                                                 //Not used
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    REG_SET_FIELD(PIN_CTRL,CLK_OUT1,0);
}

void soundVoice_task(void *pvParameter)
{
    uint32_t soundVoiveEvent;
    while(1){
        if (xQueueReceive( soundVoideEventQueue , &soundVoiveEvent, 0 ) == pdTRUE){
            if (soundVoiveEvent == 0){
                LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP,LIGHT_BLUE,  254, 254,100);
                playWaveFile(dingdong_wav_start);
                LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP,LIGHT_GREEN, 254, 254,100);
                vTaskDelay(200/portTICK_PERIOD_MS);
                LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP,LIGHT_BLUE,  254, 254,100);
                playWaveFile(dingdong_wav_start);
                LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP,LIGHT_GREEN, 254, 254,100);
            }else if (soundVoiveEvent == 1){
                LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP,LIGHT_BLUE,  254, 254,100);
                playWaveFile(alarm_wav_start);
                LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP,LIGHT_RED, 	 254, 254,100);
                vTaskDelay(200/portTICK_PERIOD_MS);
                LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP,LIGHT_BLUE,  254, 254,100);
                playWaveFile(alarm_wav_start);
                LedDisplay_MoveToHueAndSaturationLevel(LIGHT_CHANNEL_UP,LIGHT_RED, 	 254, 254,100);
            }
        }
        vTaskDelay(50/portTICK_PERIOD_MS);
    }
}

void playWaveFile(const uint8_t *pFile)
{
    wave_format_t *wave_format  = (wave_format_t *)pFile;
    wave_data_t   *wave_data    = (wave_data_t *)(pFile + sizeof(wave_header_t)+ 4 + 4 + wave_format->fmt.Subchunk1Size);

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,                                  // Only TX
        .sample_rate = wave_format->fmt.SampleRate,//SAMPLE_RATE,
        .bits_per_sample = wave_format->fmt.BitsPerSample,//16,                                                  //16-bit per channel
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,//((wave_format->fmt.NumChannels == 2)? I2S_CHANNEL_FMT_RIGHT_LEFT : I2S_CHANNEL_FMT_ALL_RIGHT),                           //2-channels
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 60,                                                      //
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                //Interrupt level 1
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num     = 14,
        .ws_io_num      = 12,
        .data_out_num   = 26,
        .data_in_num    = -1 //                                                 //Not used
    };

    i2s_driver_uninstall(I2S_NUM);
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);

    uint32_t maxBlockSize = wave_data->data_header.Subchunk2Size;
    maxBlockSize = (maxBlockSize % BLOCK_SIZE)? (maxBlockSize /BLOCK_SIZE +1): (maxBlockSize / BLOCK_SIZE);
    uint32_t blockCount = 0;
    uint8_t *address = wave_data->data;
    uint32_t blockSize = BLOCK_SIZE;

    while(1){
        blockCount ++;
        if (blockCount >= maxBlockSize){
            i2s_zero_dma_buffer(I2S_NUM);
            return;
        }else{
            if (blockCount == maxBlockSize -1){
                blockSize = wave_data->data_header.Subchunk2Size - (blockCount*BLOCK_SIZE);
            }else{
                blockSize = BLOCK_SIZE;
            }
            address = wave_data->data + (blockCount- 1) * BLOCK_SIZE;
            i2s_write_bytes(I2S_NUM, (const char *)address, blockSize, portMAX_DELAY);
        }
    }

}


void SoundVoice_Init(void)
{
    WM8978_Init();
    I2S_Init();
    soundVoideEventQueue = xQueueCreate( 1, sizeof(uint32_t) );
    xTaskCreate(&soundVoice_task, "SVOICE", 4096, NULL, tskIDLE_PRIORITY+1, NULL);
}