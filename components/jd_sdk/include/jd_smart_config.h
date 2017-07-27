#ifndef JD_SMART_CONFIG_H
#define JD_SMART_CONFIG_H

#include <stdint.h>
#include "esp_err.h"
#include "wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	WIFI_STATUS_WAIT = 0,             	/**< Waiting to start connect */
	WIFI_STATUS_FOUND_CHANNEL,          /**< Finding target channel */
	WIFI_STATUS_FINISHED,			    /**< Finished Getting SSID and password of target AP */
	WIFI_STATUS_TIMEOUT,			   	/**< Smart Configuration Timeout! */
	WIFI_STATUS_CONNECTED_AP,
	WIFI_STATUS_DISCONNECTED_AP,
	WIFI_STATUS_GOT_IP,
}wifi_status_t;

typedef void(*jd_sc_callback_t)(uint8_t status, void *pdata);

void jd_smart_config_start(jd_sc_callback_t cb);

esp_err_t jd_smart_config_stop(void);
esp_err_t jd_smart_config_set_timeout(uint16_t time_s);





// void airkiss_test_start(void);













#ifdef __cplusplus
}
#endif


#endif
