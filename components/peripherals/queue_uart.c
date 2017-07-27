#include "esp_err.h"
#include "queue_uart.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define ZIGBEE_UART  UART_NUM_2

void queue_uart_init(QueueHandle_t *uart_queue)
{
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
	};
	ESP_ERROR_CHECK(uart_param_config(ZIGBEE_UART, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(ZIGBEE_UART, GPIO_NUM_16, GPIO_NUM_17, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(ZIGBEE_UART, RX_BUFF_SIZE, RX_BUFF_SIZE, 10, uart_queue, 0));
}


int queue_uart_send_packet(uint8_t *buff, uint8_t length)
{
	return uart_write_bytes(ZIGBEE_UART, (char *)buff, length);
}


int queue_uart_read_packet(uint8_t *buff)
{
	return uart_read_bytes(ZIGBEE_UART, buff, RX_BUFF_SIZE, 0);
}

