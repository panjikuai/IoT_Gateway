#ifndef QUEUE_UART_H
#define QUEUE_UART_H

#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define RX_BUFF_SIZE (256)
#define TX_BUFF_SIZE (256)

void queue_uart_init(QueueHandle_t *uart_queue);
int queue_uart_send_packet(uint8_t *buff, uint8_t length);
int queue_uart_read_packet(uint8_t *buff);




#endif
