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


#include "jd_smart_config.h"
#include "iot_debug.h"

typedef enum {
	WIFI_CH_1 = 1,
	WIFI_CH_2,
	WIFI_CH_3,
	WIFI_CH_4,
	WIFI_CH_5,
	WIFI_CH_6,
	WIFI_CH_7,
	WIFI_CH_8,
	WIFI_CH_9,
	WIFI_CH_10,
	WIFI_CH_11,
	WIFI_CH_12,
	WIFI_CH_13,
	WIFI_CH_ALL = ((uint8_t) 255)
}jd_sc_channel_t;

typedef struct{
    uint8_t	u8FrameType;
    /*!< It must use values from tenuWifiFrameType.
    */
    uint8_t	u8FrameSubtype;
    /*!< It must use values from tenuSubTypes.
    */
    uint8_t	u8ServiceClass;
    /*!< Service class from Wi-Fi header.
    */
    uint8_t	u8Priority;
    /*!< Priority from Wi-Fi header.
    */
    uint8_t	u8HeaderLength;
    /*!< Frame Header length.
    */
    uint8_t	u8CipherType;
    /*!< Encryption type for the rx packet.
    */
    uint8_t	au8SrcMacAddress[6];
    /* ZERO means DO NOT FILTER Source address.
    */
    uint8_t	au8DstMacAddress[6];
    /* ZERO means DO NOT FILTER Destination address.
    */
    uint8_t	au8BSSID[6];
    /* ZERO means DO NOT FILTER BSSID.
    */
    uint16_t	u16DataLength;
    /*!< Data payload length (Header excluded).
    */
    uint16_t	u16FrameLength;
    /*!< Total frame length (Header + Data).
    */
    uint32_t	u32DataRateKbps;
    /*!< Data Rate in Kbps.
    */
    int8_t		s8RSSI;
    /*!< RSSI.
    */
    uint8_t	__PAD24__[3];
    /*!< Padding bytes for forcing 4-byte alignment
    */
}WifiRxPacketInfo_t;


typedef struct{
	uint8_t header[4];
	uint8_t bssMac[6];
	uint8_t targetMac[6];
	uint8_t sourceMac[6];
	uint8_t frameHeader[12];
	uint16_t length;
}WifiRxPacketInfoNew_t;

//htole16


QueueHandle_t switch_channel_queue = NULL;

typedef enum{
  SMART_CFG_CHANNEL_LISTERNING= 0,
  SMART_CFG_SSID_PWD_LEN_PICKUP,
  SMART_CFG_SSID_PWD_KEY_PICKUP,
  SMART_CFG_INIT_SCAN_AP,
  SMART_CFG_GETTING_SECURITY_TYPE,
  SMART_CFG_GOT_SECURITY_TYPE,
  SMART_CFG_FINISHED,
}smartconfig_state_t;

#define CHANNEL_SCAN_INTERVAL (300 / portTICK_PERIOD_MS)

jd_sc_callback_t jd_sc_callback = NULL;
TaskHandle_t scTaskHandler = NULL;

wifi_country_t country_code = WIFI_COUNTRY_US;
WiFiConfigParam_t smartconfig_param;
uint8_t src_mac_addr[6];
smartconfig_state_t g_sc_state = SMART_CFG_CHANNEL_LISTERNING;
uint8_t g_ssid_recv_ready = false;
uint8_t g_pwd_recv_ready = false;

TimerHandle_t smartconfig_timer = NULL;
TimerHandle_t channel_switch_timer = NULL;
TimerHandle_t sc_stunk_timer = NULL;

void wifi_promiscuous_callback(void *buf, wifi_promiscuous_pkt_type_t type)
{
	uint8_t frameIndex;
	uint8_t recvReady = true;
	WifiRxPacketInfo_t *rxPacket;
	wifi_promiscuous_pkt_t *rxBuffer = (wifi_promiscuous_pkt_t *)buf;

	if (type != WIFI_PKT_DATA) return;
	rxPacket = (WifiRxPacketInfo_t *)(rxBuffer->payload + 4);
	if (rxPacket->au8DstMacAddress[0] != 0x01 || rxPacket->au8DstMacAddress[1] != 0x00) return;
	if (g_sc_state != SMART_CFG_CHANNEL_LISTERNING && 0 != memcmp(src_mac_addr,rxPacket->au8SrcMacAddress,6)) return;

	frameIndex = rxPacket->au8DstMacAddress[4];
	switch(g_sc_state){
	case SMART_CFG_CHANNEL_LISTERNING:
		g_sc_state = SMART_CFG_SSID_PWD_LEN_PICKUP;
		smartconfig_param.channel = rxBuffer->rx_ctrl.channel;
		memcpy(src_mac_addr,rxPacket->au8SrcMacAddress,sizeof(src_mac_addr));
		if (jd_sc_callback != NULL){
			jd_sc_callback(WIFI_STATUS_FOUND_CHANNEL,(void *)&smartconfig_param);
		}
		g_ssid_recv_ready = false;
		g_pwd_recv_ready = false;

		xTimerStart(sc_stunk_timer,0);
		break;
	case SMART_CFG_SSID_PWD_LEN_PICKUP:
		if (frameIndex == 6){
			smartconfig_param.ssid_len = rxPacket->au8DstMacAddress[5];
			if (smartconfig_param.ssid_len <= SSID_MAX_LEN && smartconfig_param.pwd_len > 0) {
				g_sc_state = SMART_CFG_SSID_PWD_KEY_PICKUP;
				//printf("*found ap ssidLen %d, pwdLen: %d\r\n",smartconfig_param.ssid_len, smartconfig_param.pwd_len);
			}
		}
		if (frameIndex == 7){
			smartconfig_param.pwd_len = rxPacket->au8DstMacAddress[5];
			if (smartconfig_param.pwd_len <= PWD_MAX_LEN && smartconfig_param.ssid_len > 0) {
				g_sc_state = SMART_CFG_SSID_PWD_KEY_PICKUP;
				//printf("*found ap ssidLen %d, pwdLen: %d\r\n",smartconfig_param.ssid_len, smartconfig_param.pwd_len);
			}
		}
		break;
	case SMART_CFG_SSID_PWD_KEY_PICKUP:
		if (g_ssid_recv_ready == false){
			if ( frameIndex >= 8 && (frameIndex - 8 )< smartconfig_param.ssid_len){
				smartconfig_param.ssid[frameIndex - 8] = rxPacket->au8DstMacAddress[5];
				recvReady = true;
				for (uint8_t i = 0;i < smartconfig_param.ssid_len; i ++){
					if( smartconfig_param.ssid[i] == 0 ){
						recvReady = false;
						break;
					}
				}
				g_ssid_recv_ready = recvReady;
			}
		}
		if (g_pwd_recv_ready == false){
			if (frameIndex >= ( 8 + smartconfig_param.ssid_len) && (frameIndex - 8 - smartconfig_param.ssid_len) < smartconfig_param.pwd_len){
				smartconfig_param.pwd[frameIndex - 8 - smartconfig_param.ssid_len] = rxPacket->au8DstMacAddress[5];
				recvReady = true;
				for (uint8_t i = 0;i < smartconfig_param.pwd_len; i ++){
					if( smartconfig_param.pwd[i] == 0 ){
						recvReady = false;
						break;
					}
				}
				g_pwd_recv_ready = recvReady;
			}
		}
		if (g_pwd_recv_ready == true  && g_ssid_recv_ready == true){
			g_sc_state = SMART_CFG_FINISHED;
			ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));

    		if (smartconfig_timer != NULL){
    			xTimerDelete( smartconfig_timer, 0 );
    		}

    		if (channel_switch_timer !=NULL){
    			xTimerDelete( channel_switch_timer, 0 );
    		}

    		if (sc_stunk_timer !=NULL){
    			xTimerDelete( sc_stunk_timer, 0 );
    		}

        	vTaskDelete(scTaskHandler);
        	scTaskHandler = NULL;

			if (jd_sc_callback != NULL){
				jd_sc_callback(WIFI_STATUS_FINISHED,(void *)&smartconfig_param);
			}

		}
		break;
	default:
		break;
	}
}

void smartconfig_Task(void *pvParameter)
{
	while(1){

		uint32_t channel;
		if (xQueueReceive( switch_channel_queue, &channel, (portTickType)portMAX_DELAY ) == pdTRUE){
			IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_INFO,("current Wifi CHN: %d\n",channel));
			if (0 != esp_wifi_set_channel(channel,0)){
				IoT_DEBUG(SMART_CONFIG_DBG | IoT_DBG_SERIOUS,("Switch channel error\n"));
			}
		}
		vTaskDelay(10/portTICK_PERIOD_MS);
	}
}

void channel_switch_timer_callback( TimerHandle_t xTimer )
{
	static uint32_t channel = WIFI_CH_1;
	if (g_sc_state == SMART_CFG_CHANNEL_LISTERNING){
    	xQueueSend( switch_channel_queue, ( void * )&channel, 10 );
    	channel++;

    	if (country_code == WIFI_COUNTRY_US){
    		if (channel > WIFI_CH_11)channel = WIFI_CH_1;
    	}else{
    		if (channel > WIFI_CH_13)channel = WIFI_CH_1;
    	}
    }else{
    	xTimerStop(channel_switch_timer,0);
    }
}

void sc_stunk_timer_callback( TimerHandle_t xTimer )
{
	memset((uint8_t *)&smartconfig_param,0,sizeof(WiFiConfigParam_t));
	g_sc_state = SMART_CFG_CHANNEL_LISTERNING;
	memset(src_mac_addr,0,sizeof(src_mac_addr));
	if (channel_switch_timer != NULL){
		xTimerStart(channel_switch_timer,0);
	}
}


void jd_smart_config_start(jd_sc_callback_t cb)
{
	jd_sc_callback = cb;
	memset((uint8_t *)&smartconfig_param,0,sizeof(WiFiConfigParam_t));
	g_sc_state = SMART_CFG_CHANNEL_LISTERNING;

	if ( ESP_OK != esp_wifi_get_country(&country_code) && country_code >= WIFI_COUNTRY_MAX){
		country_code = WIFI_COUNTRY_US;
	}

    esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_callback);
    esp_wifi_set_promiscuous(true);

	if (channel_switch_timer == NULL){
		channel_switch_timer = xTimerCreate( "SCSCTimer", CHANNEL_SCAN_INTERVAL, pdTRUE, 0, channel_switch_timer_callback );
	}
	xTimerStart(channel_switch_timer,0);

	if (sc_stunk_timer == NULL){
		sc_stunk_timer = xTimerCreate( "SCSTTimer", CHANNEL_SCAN_INTERVAL* WIFI_CH_13 *3, pdFALSE, 0, sc_stunk_timer_callback );
	}

	switch_channel_queue = xQueueCreate( 1, sizeof(uint32_t) );

    if (scTaskHandler == NULL){
		xTaskCreate(&smartconfig_Task, "SMART_CFG", 2048, NULL, 6, &scTaskHandler);
    }
}

esp_err_t jd_smart_config_stop(void)
{
	if (smartconfig_timer != NULL){
		xTimerDelete( smartconfig_timer, 0 );
	}

	if (channel_switch_timer != NULL){
		xTimerDelete( channel_switch_timer, 0 );
	}

	if (sc_stunk_timer != NULL){
		xTimerDelete( sc_stunk_timer, 0 );
	}

    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(NULL));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
    if (scTaskHandler != NULL){
		vTaskDelete(scTaskHandler);
		scTaskHandler = NULL;
    }
	return ESP_OK;
}


void smartconfig_timeout_callback( TimerHandle_t xTimer )
{
	if (smartconfig_timer != NULL){
		xTimerDelete( smartconfig_timer, 0 );
	}

	if (channel_switch_timer != NULL){
		xTimerDelete( channel_switch_timer, 0 );
	}

	if (sc_stunk_timer != NULL){
		xTimerDelete( sc_stunk_timer, 0 );
	}

    if (scTaskHandler != NULL){
    	vTaskDelete(scTaskHandler);
    	scTaskHandler = NULL;
    }

    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(NULL));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));

	if (jd_sc_callback != NULL){
		jd_sc_callback(WIFI_STATUS_TIMEOUT,(void *)&smartconfig_param);
	}
}

esp_err_t jd_smart_config_set_timeout(uint16_t time_s)
{
	if (time_s != 0){
		if (smartconfig_timer == NULL){
			smartconfig_timer = xTimerCreate( "SmartConfigTimer", (uint32_t)time_s*1000, pdFALSE, 0, smartconfig_timeout_callback );
		}else{
			xTimerChangePeriod( smartconfig_timer, (uint32_t)time_s*1000, 0 );
		}
		xTimerStart(smartconfig_timer,0);
	}else{
		if (smartconfig_timer != NULL){
			xTimerStop(smartconfig_timer,0);
		}
	}
	return ESP_OK;
}

// #define FLITER_SIZE  10
// #define FRAME_HEADER_NUM 20

// typedef struct{
// 	uint8_t sourceAddress[6];
// 	uint16_t frameNo[FRAME_HEADER_NUM];
// 	uint8_t __pad__[2];
// }filter_t;

// filter_t fliter[FLITER_SIZE];

// //Queue the input length
// uint8_t FoundTargetMacAddress(uint16_t *table,uint16_t input)
// {
// 	uint8_t foundTargetMac = false;

// 	for(uint8_t i = 0; i < FRAME_HEADER_NUM; i++){
// 		if (input == table[i]){
// 			break;
// 		}
// 		else if (input < table[i]){
// 			for(uint8_t j = FRAME_HEADER_NUM -1; j > i; j--){
// 				table[j] = table[j-1];
// 			}
// 			table[i] = input;
// 			break;
// 		}
// 	}

// 	for (uint8_t i = 0; i < FRAME_HEADER_NUM -4; i++){
// 		if ( table[i]== (table[i+1] -1) && table[i]== (table[i+2] -2) && table[i]== (table[i+3] -3) ){
// 			foundTargetMac = true;
// 			break;
// 		}
// 	}
// 	if (foundTargetMac){
// 		for (uint8_t i = 0; i < FRAME_HEADER_NUM; i++){
// 			printf("%02x ",table[i]);
// 		}
// 		printf("\n");
// 	}

// 	return foundTargetMac;
// }



// enum{
// 	STATE_LISTEN_CHANNAL_AND_SOURCE_MAC  = 0,
// 	STATE_LISTEN_MAGIC_CODE,
// 	STATE_LISTEN_PREFIX_CODE,
// 	STATE_LISTEN_SEQUENCE_DATA
// };


// typedef struct{
// 	uint8_t sequenceCrc;
// 	uint8_t sequencyIndex;
// 	uint8_t data[4];
// 	uint8_t __pad__[2];
// }sequency_data_t;


// void airkiss_promiscuous_callback(void *buf, wifi_promiscuous_pkt_type_t type)
// {
// 	if (type != WIFI_PKT_DATA){
// 		return;
// 	}

// 	uint16_t rawPackageLength;
// 	// uint8_t sourceAddr[6] = {0x10,0x05,0x01,0x96,0xA3,0x04};
// 	uint8_t sourceAddr[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
// 	wifi_promiscuous_pkt_t *rxBuffer = (wifi_promiscuous_pkt_t *)buf;
// 	rawPackageLength = rxBuffer->rx_ctrl.sig_len;
// 	WifiRxPacketInfoNew_t *rxPacket = (WifiRxPacketInfoNew_t *)rxBuffer->payload;
// 	uint8_t foundSourceMac = false;

// 	for (uint8_t i = 0; i < FLITER_SIZE; i ++){
// 		if(0 == memcmp(fliter[i].sourceAddress,rxPacket->sourceMac,6)){
// 			if(FoundTargetMacAddress(fliter[i].frameNo,rawPackageLength)){
// 				printf("found Source MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
// 						rxPacket->sourceMac[0],rxPacket->sourceMac[1],rxPacket->sourceMac[2],rxPacket->sourceMac[3],rxPacket->sourceMac[4],rxPacket->sourceMac[5]);
// 				foundSourceMac = true;
// 				break;
// 			}
// 			break;
// 		}else if (0 == memcmp(fliter[i].sourceAddress,sourceAddr,6)){

// 			printf("Add MAC: %02x:%02x:%02x:%02x:%02x:%02x ",
// 					rxPacket->sourceMac[0],rxPacket->sourceMac[1],rxPacket->sourceMac[2],rxPacket->sourceMac[3],rxPacket->sourceMac[4],rxPacket->sourceMac[5]);
// 			printf("len: %02x\n",rawPackageLength);
// 			// new source mac, add it to fliter
// 			memcpy(fliter[i].sourceAddress,rxPacket->sourceMac,6);
// 			FoundTargetMacAddress(fliter[i].frameNo,rawPackageLength);
// 			break;
// 		}else{

// 		}
// 	}
// }


// void airkiss_test_start(void)
// {
// 	memset((uint8_t *)fliter,0xFF, sizeof(filter_t)*FLITER_SIZE);
//     esp_wifi_set_promiscuous_rx_cb(airkiss_promiscuous_callback);
//     esp_wifi_set_promiscuous(true);
// 	esp_wifi_set_channel(WIFI_CH_6,0);
// 	while(1);
// }