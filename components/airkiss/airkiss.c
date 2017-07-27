#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "airkiss.h"
#include "iot_debug.h"
#include "crc.h"

typedef struct{
	uint8_t header[4];
	uint8_t bssMac[6];
	uint8_t targetMac[6];
	uint8_t sourceMac[6];
	uint8_t frameHeader[12];
	uint16_t length;
}WifiSNAPPacketStruct_t;

#define MAC_FLITER_SIZE  10
#define FRAME_HEADER_NUM 12

typedef struct{
	uint8_t sourceAddress[6];
	uint16_t frameNo[FRAME_HEADER_NUM];
	uint8_t __pad__[2];
}macFilter_t;
macFilter_t macFilter[MAC_FLITER_SIZE];
uint8_t fliterSourceMacAddr[6];
uint16_t dataPackBaselength = 0;

#define DATA_LENGTH_BASE 	20
#define DATE_BLOCK_SIZE 	28

#define SSID_LEN_HIGH 	0
#define SSID_LEN_LOW 	1
#define SSID_CRC_HIGH 	2
#define SSID_CRC_LOW 	3

#define PSW_LEN_HIGH 	4
#define PSW_LEN_LOW 	5
#define PSW_CRC_HIGH 	6
#define PSW_CRC_LOW 	7

uint8_t chn_wifi_channel_list[] = {
    AIRKISS_WIFI_CH_1,
	AIRKISS_WIFI_CH_6,
	AIRKISS_WIFI_CH_11,
	AIRKISS_WIFI_CH_13,

    AIRKISS_WIFI_CH_1,
	AIRKISS_WIFI_CH_2,
	AIRKISS_WIFI_CH_3,
	AIRKISS_WIFI_CH_4,
	AIRKISS_WIFI_CH_5,
	AIRKISS_WIFI_CH_6,
	AIRKISS_WIFI_CH_7,
	AIRKISS_WIFI_CH_8,
	AIRKISS_WIFI_CH_9,
	AIRKISS_WIFI_CH_10,
	AIRKISS_WIFI_CH_11,
	AIRKISS_WIFI_CH_12,
	AIRKISS_WIFI_CH_13,
};

uint8_t us_wifi_channel_list[] = {
    AIRKISS_WIFI_CH_1,
	AIRKISS_WIFI_CH_6,
	AIRKISS_WIFI_CH_11,

    AIRKISS_WIFI_CH_1,
	AIRKISS_WIFI_CH_2,
	AIRKISS_WIFI_CH_3,
	AIRKISS_WIFI_CH_4,
	AIRKISS_WIFI_CH_5,
	AIRKISS_WIFI_CH_6,
	AIRKISS_WIFI_CH_7,
	AIRKISS_WIFI_CH_8,
	AIRKISS_WIFI_CH_9,
	AIRKISS_WIFI_CH_10,
	AIRKISS_WIFI_CH_11,
};

typedef struct{
	uint8_t completed;
	uint8_t payload[8];
}airkiss_ssid_psw_header_t;

typedef struct{
	uint8_t completed;
	uint8_t seqCrc;
	uint8_t seqIndex;
	uint8_t length;
	uint8_t data[4];
}airkiss_sequence_struct_t;

typedef struct{
	uint8_t length[DATE_BLOCK_SIZE];
	uint8_t data[DATE_BLOCK_SIZE*4];
}airkiss_sequence_data_t;

airkiss_ssid_psw_header_t	airkiss_ssid_psw;
airkiss_sequence_struct_t	temp_sequence;
uint8_t temp_sequence_count = 0;
airkiss_sequence_data_t		sequence_data;
uint8_t crcValue = 0;
uint8_t lastDateTypeIsData = 0;

#define AIRKISS_CHANNEL_SCAN_INTERVAL (500 / portTICK_PERIOD_MS)

AirkissWiFiParam_t 	airkissWifiParam;
wifi_country_t 		countryCode 		= WIFI_COUNTRY_US;
Airkiss_callback_t 	airkiss_callback 	= NULL;
Airkiss_state_t 	airkissState 	 	= AIRKISS_CHANNEL_LISTERNING;
TimerHandle_t 		channelSwitchTimer 	= NULL;
TimerHandle_t		airkissStunkTimer   = NULL;
QueueHandle_t 		channelSwitchQueue 	= NULL;
TaskHandle_t 		airkissTaskHandler 	= NULL;

//Queue the input length
uint8_t FoundTargetMacAddress(uint16_t *table,uint16_t input)
{
	if (input > 108) return false; //max 102 with AES 

	uint8_t foundTargetMac = false;

	for(uint8_t i = 0; i < FRAME_HEADER_NUM; i++){
		if (input == table[i]){
			break;
		}
		else if (input < table[i]){
			for(uint8_t j = FRAME_HEADER_NUM -1; j > i; j--){
				table[j] = table[j-1];
			}
			table[i] = input;
			break;
		}
	}

	for (uint8_t i = 0; i < FRAME_HEADER_NUM -4; i++){
		if ( table[i]== (table[i+1] -1) && table[i]== (table[i+2] -2) && table[i]== (table[i+3] -3) ){
			foundTargetMac = true;
			dataPackBaselength = table[i];
			break;
		}
	}
	if (foundTargetMac){
		for (uint8_t i = 0; i < FRAME_HEADER_NUM; i++){
			printf("%02x ",table[i]);
		}
		printf("\n");
	}

	return foundTargetMac;
}


uint8_t get_target_mac_address(WifiSNAPPacketStruct_t *snapPacket,uint16_t packLength)
{
	uint8_t invalidMac[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	uint8_t gotSourceAddress = false;
	for (uint8_t i = 0; i < MAC_FLITER_SIZE; i ++){
		if(0 == memcmp(macFilter[i].sourceAddress,snapPacket->sourceMac,6)){
			if(FoundTargetMacAddress(macFilter[i].frameNo,packLength)){
				gotSourceAddress = true;
				// printf("found Source MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
				// 		snapPacket->sourceMac[0],snapPacket->sourceMac[1],snapPacket->sourceMac[2],snapPacket->sourceMac[3],snapPacket->sourceMac[4],snapPacket->sourceMac[5]);
				break;
			}
			break;
		}else if (0 == memcmp(macFilter[i].sourceAddress,invalidMac,6)){
			memcpy(macFilter[i].sourceAddress,snapPacket->sourceMac,6);
			FoundTargetMacAddress(macFilter[i].frameNo,packLength);
			// printf("Add MAC: %02x:%02x:%02x:%02x:%02x:%02x\n ",
			// 		snapPacket->sourceMac[0],snapPacket->sourceMac[1],snapPacket->sourceMac[2],snapPacket->sourceMac[3],snapPacket->sourceMac[4],snapPacket->sourceMac[5]);
			// // new source mac, add it to fliter
			break;
		}
	}
	return gotSourceAddress;
}


void airkiss_promiscuous_callback(void *buf, wifi_promiscuous_pkt_type_t type)
{
	// Only allow data pack
	if (type != WIFI_PKT_DATA){
		return;
	}

	wifi_promiscuous_pkt_t *rxBuffer = (wifi_promiscuous_pkt_t *)buf;
	uint16_t packLength = rxBuffer->rx_ctrl.sig_len;
	WifiSNAPPacketStruct_t *snapPacket = (WifiSNAPPacketStruct_t *)rxBuffer->payload;

	if (airkissState == AIRKISS_CHANNEL_LISTERNING){
		if (get_target_mac_address(snapPacket,packLength)){
			airkissState = AIRKISS_SSID_PWD_PICKUP;
			memcpy(fliterSourceMacAddr,snapPacket->sourceMac,6);	 //got source address and channel
			xTimerStart(airkissStunkTimer,0);						 // start stunck timer
			wifi_second_chan_t secondChannel;
			esp_wifi_get_channel(&airkissWifiParam.channel, &secondChannel);
			if (airkiss_callback){
				airkiss_callback(AIRKISS_SSID_PWD_PICKUP,(void *)&airkissWifiParam);
			}
		}
	}else if (airkissState == AIRKISS_SSID_PWD_PICKUP){
		if(0 != memcmp(fliterSourceMacAddr,snapPacket->sourceMac,6)){//only response target mas address
			return;
		}
		
		packLength -= dataPackBaselength;
		// IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("LEN:%d\n",dataPackBaselength));
		
		uint8_t realData;
		uint8_t magicCode    = packLength >> 4;
		if (magicCode < 8){ //it's magic code
			temp_sequence_count = 0;
			realData = packLength & 0x0F;
			airkiss_ssid_psw.payload[magicCode] = realData;
			airkiss_ssid_psw.completed 	|= (1 << magicCode);
			// magic code and perfix code finished.
			if (airkiss_ssid_psw.completed == 0xFF){
				uint8_t ssid_length = (airkiss_ssid_psw.payload[SSID_LEN_HIGH] << 4) + airkiss_ssid_psw.payload[SSID_LEN_LOW]; 
				uint8_t psw_length  = (airkiss_ssid_psw.payload[PSW_LEN_HIGH] << 4)  + airkiss_ssid_psw.payload[PSW_LEN_LOW];   
				uint8_t ssid_crc 	= (airkiss_ssid_psw.payload[SSID_CRC_HIGH] << 4) + airkiss_ssid_psw.payload[SSID_CRC_LOW];
				uint8_t psw_crc  	= (airkiss_ssid_psw.payload[PSW_CRC_HIGH] << 4)  + airkiss_ssid_psw.payload[PSW_CRC_LOW];
				// IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("ssid_LEN:%d,PSW_LEN:%d\n",ssid_length,psw_length));

				uint8_t ssid_rand_psw_count = 0;
				for (uint8_t i = 0; i < DATE_BLOCK_SIZE; i++){
					if (sequence_data.length[i] == 0)break;
					ssid_rand_psw_count +=sequence_data.length[i];
				}

				if (   ssid_rand_psw_count == (ssid_length + psw_length + 1) && ssid_rand_psw_count <= (DATE_BLOCK_SIZE *4 ) ){
					if ( ssid_crc == calculate_crc8(sequence_data.data,ssid_length)){// do ssid crc and psw crc
						if (psw_crc == calculate_crc8(sequence_data.data + (ssid_length +1),psw_length)){
							airkissState = SMART_CFG_FINISHED;
							xTimerStop(airkissStunkTimer,0);// stop stunk timer.

							memcpy(airkissWifiParam.ssid,sequence_data.data,ssid_length);
							airkissWifiParam.ssid_len = ssid_length;

							airkissWifiParam.randNumber = sequence_data.data[ssid_length];

							memcpy(airkissWifiParam.pwd,sequence_data.data + (ssid_length +1),psw_length);
							airkissWifiParam.pwd_len = psw_length;
							if (airkiss_callback){
								airkiss_callback(SMART_CFG_FINISHED,(void *)&airkissWifiParam);
							}

							// // finished the airkiss
							// IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("Got SSID and PSW\n"));
							// printf("SSID:");
							// for (uint8_t i = 0; i < ssid_length; i++){
							// 	printf("%c",sequence_data.data[i]);
							// }
							// printf("\nPSW:");
							// for (uint8_t i = ssid_length +1; i < ssid_rand_psw_count; i++){
							// 	printf("%c",sequence_data.data[i]);
							// }
							// printf("\n");
						}
					}
				}
			}
		}else{
			uint8_t sequenceCode = packLength >> 7;
			if (sequenceCode == 1){//sequence head
				realData = packLength & 0x7F;

				if (lastDateTypeIsData == 1){
					temp_sequence_count = 0;
					lastDateTypeIsData = 0;
				}

				if (temp_sequence_count == 0){
					temp_sequence.seqCrc = realData;
				}else if (temp_sequence_count == 1){
					temp_sequence.seqIndex = realData;
					crcValue = FastCRC(0, realData);
				}
				temp_sequence_count++;
			}else{
				uint8_t sequenceData = packLength >> 8;
				if (sequenceData == 1){//sequence data
					lastDateTypeIsData = 1;
					realData = packLength & 0xFF;
					if (temp_sequence_count >= 2 && temp_sequence_count < 6){
						temp_sequence.data[temp_sequence_count -2] = realData;
						temp_sequence_count ++;
						crcValue = FastCRC(crcValue, realData);
						// calculate crc
						if ((crcValue & 0x7F)== temp_sequence.seqCrc && temp_sequence.seqIndex < DATE_BLOCK_SIZE){
							sequence_data.length[temp_sequence.seqIndex] = temp_sequence_count -2;
							memcpy(sequence_data.data + (temp_sequence.seqIndex*4),temp_sequence.data, temp_sequence_count -2);
							temp_sequence_count = 0;

							// IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("seq:%d,len:%d\n",temp_sequence.seqIndex,sequence_data.length[temp_sequence.seqIndex]));
							// for (uint8_t i = 0; i < sequence_data.length[temp_sequence.seqIndex]; i++ ){
							// 	printf("%02x ",sequence_data.data[temp_sequence.seqIndex*4 +i]);
							// }
							// printf("\n");
						}
					}else{
						temp_sequence_count = 0;
					}
				}
			}
		}
	}
}


void channelSwitchTimerCallback(TimerHandle_t xTimer)
{
	static uint32_t count = 0;
	uint32_t channel = AIRKISS_WIFI_CH_1;
	if (airkissState == AIRKISS_CHANNEL_LISTERNING){
    	if (countryCode == WIFI_COUNTRY_US){
			if ( count >= sizeof(us_wifi_channel_list) )count = 0;
			channel = us_wifi_channel_list[count];
    	}else{
			if ( count >= sizeof(chn_wifi_channel_list) )count = 0;
    		channel = chn_wifi_channel_list[count];
    	}
		xQueueSend( channelSwitchQueue, ( void * )&channel, 10/portTICK_PERIOD_MS );
		count++;
    }else{
		count = 0;
    	xTimerStop(channelSwitchTimer,0);
    }
}

void airkissStunkTimerCallback( TimerHandle_t xTimer )
{
	airkissState = AIRKISS_CHANNEL_LISTERNING;
	if (channelSwitchTimer != NULL){
		xTimerStart(channelSwitchTimer,0);
	}
}

void airkissTask(void *pvParameter)
{
	while(1){
		uint32_t channel;
		if (xQueueReceive( channelSwitchQueue, &channel, (portTickType)portMAX_DELAY ) == pdTRUE){
			IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("Curr Wifi CHN: %d\n",channel));
			// memset((uint8_t *)&airkiss_ssid_psw,0,sizeof(airkiss_ssid_psw_header_t));
			// memset((uint8_t *)&sequence_data,0,sizeof(airkiss_sequence_data_t));
			memset((uint8_t *)macFilter,0xFF, sizeof(macFilter_t)*MAC_FLITER_SIZE);
			if (0 != esp_wifi_set_channel(channel,0)){
				IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_SERIOUS,("Switch channel error\n"));
			}
		}
		vTaskDelay(2/portTICK_PERIOD_MS);
	}
}

void airkiss_start(Airkiss_callback_t cb)
{
	airkiss_callback = cb;
	memset((uint8_t *)&airkissWifiParam,0,sizeof(AirkissWiFiParam_t));
	memset((uint8_t *)&airkiss_ssid_psw,0,sizeof(airkiss_ssid_psw_header_t));
	memset((uint8_t *)&sequence_data,0,sizeof(airkiss_sequence_data_t));
	memset((uint8_t *)macFilter,0xFF, sizeof(macFilter_t)*MAC_FLITER_SIZE);
	airkissState = AIRKISS_CHANNEL_LISTERNING;

	if ( ESP_OK != esp_wifi_get_country(&countryCode) && countryCode >= WIFI_COUNTRY_MAX){
		countryCode = WIFI_COUNTRY_US;
	}

    esp_wifi_set_promiscuous_rx_cb(airkiss_promiscuous_callback);
    esp_wifi_set_promiscuous(true);

	if (channelSwitchTimer == NULL){
		channelSwitchTimer = xTimerCreate( "AirkissTimer", AIRKISS_CHANNEL_SCAN_INTERVAL, pdTRUE, 0, channelSwitchTimerCallback );
	}
	xTimerStart(channelSwitchTimer,0);

	if (airkissStunkTimer == NULL){
		airkissStunkTimer = xTimerCreate( "AirkissStunkTimer", AIRKISS_CHANNEL_SCAN_INTERVAL* 10, pdFALSE, 0, airkissStunkTimerCallback );
	}
	channelSwitchQueue = xQueueCreate( 1, sizeof(uint32_t) );

    if (airkissTaskHandler == NULL){
		xTaskCreate(&airkissTask, "AIRKISS", 2048, NULL, 6, &airkissTaskHandler);
    }
}