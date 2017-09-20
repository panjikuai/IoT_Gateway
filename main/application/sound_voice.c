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

#define SAMPLE_RATE         (36000)
#define I2S_NUM             (0)
#define WAVE_FREQ_HZ        (1000)
#define PI 3.14159265
#define SAMPLE_PER_CYCLE    (SAMPLE_RATE/WAVE_FREQ_HZ)

extern const uint8_t alarm_wav_start[] asm("_binary_alarm_wav_start");
extern const uint8_t alarm_wav_end[]   asm("_binary_alarm_wav_end");


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
    wave_data_header_t  data_header;
    uint8_t            data[1];
}wave_format_t;

wave_format_t *wave_format;

void i2s_init(void)
{
    wave_format = (wave_format_t *)alarm_wav_start;

    //for 36Khz sample rates, we create 100Hz sine wave, every cycle need 36000/100 = 360 samples (4-bytes each sample)
    //using 6 buffers, we need 60-samples per buffer
    //2-channels, 16-bit each channel, total buffer is 360*4 = 1440 bytes
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,                                  // Only TX
        .sample_rate = 44100,//wave_format->fmt.SampleRate,//SAMPLE_RATE,
        .bits_per_sample = 16,//wave_format->fmt.BitsPerSample,//16,                                                  //16-bit per channel
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
        .data_in_num    = -1 //                                                      //Not used
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    REG_SET_FIELD(PIN_CTRL,CLK_OUT1,0);
}

void sound_voice_task(void *pvParameter)
{
    // unsigned int i;
    // unsigned int sample_val_arr[SAMPLE_PER_CYCLE] = { 0 };
    // float sin_float, triangle_float, triangle_step = 65536.0 / SAMPLE_PER_CYCLE;
    // vTaskDelay(50/portTICK_PERIOD_MS);
    // triangle_float = -32767;

    // uint16_t index = 0;
    // uint16_t sampleCycle = SAMPLE_PER_CYCLE;

    uint32_t blockSize = wave_format->data_header.Subchunk2Size/(wave_format->fmt.NumChannels * wave_format->fmt.BitsPerSample/8)/SAMPLE_PER_CYCLE;
    uint32_t blockCount = 0;

    vTaskDelay(2000/portTICK_PERIOD_MS);
    IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("blockSize: %d, blockCount: %d\r",blockSize,blockCount));
    IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("SampleRate: %d, BitsPerSample: %d,NumChannels :%d\n",wave_format->fmt.SampleRate,wave_format->fmt.BitsPerSample,wave_format->fmt.NumChannels ));
    // uint32_t size = blockSize > SAMPLE_PER_CYCLE? SAMPLE_PER_CYCLE : blockSize;
    while(1){

        // triangle_float = -32767;
        // index++;
        // if ( index <100 ){
        //     sampleCycle = SAMPLE_PER_CYCLE/5;
        //     triangle_step = 65536.0 / sampleCycle;
        // }else if ( index < 200){
        //     sampleCycle = SAMPLE_PER_CYCLE;
        //     triangle_step = 65536.0 / sampleCycle;
        // }else{
        //     index = 0;
        // }
  

        // triangle_step = 65536.0 / sampleCycle;


        // for(i = 0; i < sampleCycle; i++) {
        //     sin_float = sin(i * PI / 180.0);
        //     if(sin_float >= 0)
        //         triangle_float += triangle_step;
        //     else
        //         triangle_float -= triangle_step;
        //     sin_float  *= 32767;
        //     sample_val_arr[i] = 0;
        //     sample_val_arr[i] += (short)triangle_float;

        //     sample_val_arr[i] = sample_val_arr[i] << 16;
        //     sample_val_arr[i] += (short) sin_float;
        // }

        i2s_write_bytes(I2S_NUM, (const char *)(wave_format->data + blockCount * SAMPLE_PER_CYCLE*4), SAMPLE_PER_CYCLE*4, portMAX_DELAY);
        blockCount++;
        //IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("blockSize: %d, blockCount: %d\n",blockSize,blockCount));
        if (blockCount >= blockSize){
            blockCount = 0;
            vTaskDelay(2000/portTICK_PERIOD_MS);
        }
        
    }
}

void SoundVoice_Init(void)
{
    WM8978_Init();
    i2s_init();
    // xTaskCreate(&sound_voice_task, "SOUND_VOICE", 4096, NULL, tskIDLE_PRIORITY+5, NULL);
}