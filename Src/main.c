#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "gpio_exti.h"
#include "uart_dma.h"

// create a buffer
char str_buffer[20];
extern char uart_data_buffer[UART_DATA_BUFF_SIZE];

int main(void)
{
	// Initialize LED and Button
	led_init();
	pc13_exti_init();

	// Initialize USART2 and DMA1
	uart2_rx_tx_init();
	dma1_init();

	// Configure Stream 5 (USART2_RX) and Stream 6 (USART2_TX)
	dma1_stream5_uart_rx_config();
	sprintf(str_buffer, "Init.....\r\n");
	dma1_stream6_uart_tx_config((uint32_t) str_buffer, strlen(str_buffer));

	// Main loop
	while(1){}
}


void display_led_status(void)
{
	// Get latest user LED status
	int led_status = get_led_status();

	// Check and display user LED status
	if (led_status)
	{
		sprintf(str_buffer, "LED: ON\r\n");
	}
	else
	{
		sprintf(str_buffer, "LED: OFF\r\n");
	}

	dma1_stream6_uart_tx_config((uint32_t) str_buffer, strlen(str_buffer));
	while(!(DMA1->HISR & HIFSR_TCIF6)){}
}

void control_led(char * command)
{
	// Get user LED status
	int led_status = get_led_status();

	// Respond to known commands
	if(!strcmp(command, "LED_ON") && !led_status)
	{
		led_on();
		display_led_status();
	}
	else if(!strcmp(command, "LED_OF") && led_status)
	{
		led_off();
		display_led_status();
	}
	else if(!strcmp(command, "LED_ST"))
	{
		display_led_status();
	}
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & LINE13)
	{
		// Clear PR flag
		EXTI->PR |= LINE13;

		// Toggle LED and display status
		led_toggle();
		display_led_status();
	}
}


void DMA1_Stream6_IRQHandler(void)
{
	if(DMA1->HISR & HIFSR_TCIF6)
	{
		// Clear Stream 6 TC flag
		DMA1->HIFCR |= HIFCR_CTCIF6;
	}
}

void DMA1_Stream5_IRQHandler(void)
{
	if(DMA1->HISR & HIFSR_TCIF5)
	{
		// Clear Stream 5 TC flag
		DMA1->HIFCR |= HIFCR_CTCIF5;

		// Pass received data to cli_buffer
		sprintf(str_buffer, "%s", uart_data_buffer);
		str_buffer[UART_DATA_BUFF_SIZE] = '\0';

		// Call to function that controls LED
		control_led(str_buffer);
	}
}




