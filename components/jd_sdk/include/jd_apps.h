#ifndef JD_APPS_H
#define JD_APPS_H


#define TCP_RX_BUFFER_NUM					1
#define UDP_RX_BUFFER_NUM					1
#define TX_BUFFER_SIZE						800
#define RX_BUFFER_SIZE						800
#define GENERIC_BUFFER_SIZE					800
#define SOCKET_ADDR_SIZE					16
#define HB_BUFFER_SIZE						128
#define DEV_JSON_INFO_SIZE					256
#define SHARED_PKT_MEMORY_SIZE				1600
#define SHARED_PKT_MEM_SIZE_UDP				(SHARED_PKT_MEMORY_SIZE - 100)
#define UDP_REMOTE_ADDR_OFFSET				RX_BUFFER_SIZE

#define EVENT_TABLE_SIZE					4

#define APP_VERSION_SEQUENCE				1
#define APP_MAJOR_VERSION					1
#define APP_MINOR_VERSION					APP_VERSION_SEQUENCE
#define APP_REVISED_VERSION					0
#define APP_VERSION						    ((APP_MAJOR_VERSION << 11) | (APP_MINOR_VERSION << 6) | (APP_REVISED_VERSION))
#define STACK_MAJOR_VERSION					19
#define STACK_MINOR_VERSION					4
#define STACK_REVISED_VERSION				7
#define HARDWARE_MAJOR_VERSION				1
#define HARDWARE_MINOR_VERSION				0
#define HARDWARE_REVISED_VERSION			0

#define MAIN_HOST_NAME				 		"live.smart.jd.com"
#define MAIN_HOST_PORT				 		2002
#define PRODUCT_UUID						"527IXE"

#define PARAM_MAGIC_VALUE					0x112233BB
#define CORTUS_APP_UPDATE					0xAA
#define WIFI_FW_UPDATE						0x55

enum{
    MSG_AUTH= 0,
    MSG_HEART_BEAT,
    MSG_DTATA,
    MSG_HOLD,
    MSG_DATA_UPLOAD,
    MSG_OTAU_REPORT,
};


typedef struct
{
  uint8_t msgType;
  void* pValue;
}jdMsg_t;

#define TIMESTAMP_SIZE						4

#define DEV_CLOUD_CONFIGURED				1
#define CLOUD_CONNECTED						1
#define CLOUD_DISCONNECTED					0

#define JD_CONTROL_SUCCESS             	    0
#define JD_CONTROL_FAILURE             		2

#define JOYLINK_SERVER_DNS_RETRY			2
#define JOYLINK_SERVER_CONNECT_RETRY		2

#define JD_UDP_PORT_1                       80
#define JD_UDP_PORT_2                       4320

#define HB_TIMER_PERIOD						10000 //10ms per tick
#define DEAMON_TIMER_PERIOD					5000
#define UP_TIMER_PERIOD						2000
#define NUM_HB_RESP_LOST					1
#define MAX_PING_RETRY_CNT					3

#define WIFI_MODULE_WIFI_DISCONNECT			0
#define WIFI_MODULE_IN_SNIFFER_MODE			1
#define WIFI_MODULE_CONNECT_TO_ROUTER		2
#define WIFI_MODULE_CONNECT_TO_SERVER		3

#define OTAU_INPROGRESS_DOWNLOAD			0
#define OTAU_INPROGRESS_INSTALL				1
#define OTAU_INPROGRESS_COMPLETE			2
#define OTAU_INPROGRESS_FAILURE				3

#define RESET_TYPE_SNIFFER					1
#define RESET_TYPE_OTAU_SUCCESS				2
#define RESET_TYPE_OTAU_INPROGRESS			3

#define M2M_ALIGN_SIZE(x)					((x) & (~0x3))
#define M2M_ALIGN_RAM(x)					(((x) + 3) & (~0x3))

typedef struct data_pkt {
	uint8_t *buf;
	uint16_t len;
	uint8_t type;
}data_pkt_t;

typedef struct lua_bin_ctrl
{
	uint8_t on_off;
	uint8_t err_code;
}lua_bin_ctrl_t;


typedef void(*jd_net_status_t)(uint8_t status, void *info);
typedef int32_t(*jd_do_recipe_t)(void *command, uint16_t length);
typedef int32_t(*jd_do_control_t)(void *command, uint16_t length);
typedef int32_t(*jd_do_snapshot_t)(void *command, uint16_t length);

void jd_apps_request_server_auth(void);
void jd_apps_send_heart_beat(void);
void jd_apps_data_upload(uint8_t snapshot);

void jd_apps_load_param(void);
void jd_apps_store_param(void);
void jd_apps_generate_EccKey(void);

void jd_apps_init(jd_net_status_t net_status,jd_do_recipe_t do_recipe,jd_do_control_t do_control, jd_do_snapshot_t do_snapshot);

void vTaskNetworkTcpEvent(void *pvParameters);
void vTaskNetworkUdpEvent(void *pvParameters);

#endif
