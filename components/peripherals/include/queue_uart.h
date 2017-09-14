#ifndef QUEUE_UART_H
#define QUEUE_UART_H

#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define RX_BUFF_SIZE (256)
#define TX_BUFF_SIZE (256)

void QueueUart_Init(QueueHandle_t *uart_queue);
int QueueUart_SendPacket(uint8_t *buff, uint8_t length);
int QueueUart_ReadPacket(uint8_t *buff);




#endif
