#ifndef UART_DMA_H_
#define UART_DMA_H_

#include "stm32f4xx.h"
#include <stdint.h>

#define UART_DATA_BUFF_SIZE			6
void uart2_rx_tx_init(void);
void dma1_init(void);
void dma1_stream5_uart_rx_config(void);
void dma1_stream6_uart_tx_config(uint32_t msg_to_send, uint32_t msg_len);

#define HIFCR_CTCIF5		(1U<<11)
#define HIFCR_CTCIF6		(1U<<21)
#define HIFSR_TCIF5			(1U<<11)
#define HIFSR_TCIF6			(1U<<21)

#endif
