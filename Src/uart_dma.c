#include "uart_dma.h"

#define UART2EN				(1U<<17)
#define GPIOAEN				(1U<<0)

#define CR1_TE				(1U<<3)
#define CR1_RE				(1U<<2)
#define CR1_UE				(1U<<13)
#define SR_TXE				(1U<<7)

#define CR3_DMAT			(1U<<7)
#define CR3_DMAR			(1U<<6)
#define SR_TC				(1U<<6)
#define CR1_TCIE			(1U<<6)

#define UART_BAUDRATE		115200
#define CLK					16000000

#define DMA1EN				(1U<<21)
#define DMA_SCR_EN			(1U<<0)
#define DMA_SCR_MINC		(1U<<10)
#define DMA_SCR_PINC		(1U<<9)
#define DMA_SCR_CIRC		(1U<<8)
#define DMA_SCR_TCIE		(1U<<4)
#define DMA_SCR_TEIE		(1U<<2)
#define DMA_SFCR_DMDIS		(1U<<2)

#define HIFCR_CDMEIF5		(1U<<8)
#define HIFCR_CTEIF5		(1U<<9)
#define HIFCR_CTCIF5		(1U<<11)

#define HIFCR_CDMEIF6		(1U<<18)
#define HIFCR_CTEIF6		(1U<<19)
#define HIFCR_CTCIF6		(1U<<21)

#define HIFSR_TCIF5			(1U<<11)
#define HIFSR_TCIF6			(1U<<21)


static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate);
static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate);

char uart_data_buffer[UART_DATA_BUFF_SIZE];

void uart2_rx_tx_init(void)
{
//	GPIO configuration
//	Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

//	Set PA2 and PA3 mode to alternate function mode
	GPIOA->MODER &= ~(1U<<4);
	GPIOA->MODER |= (1U<<5);
	GPIOA->MODER &= ~(1U<<6);
	GPIOA->MODER |= (1U<<7);

//	Set PA2 and PA3 to AF7 alternate function type (USART2)
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &= ~(1U<<11);
	GPIOA->AFR[0] |= (1U<<12);
	GPIOA->AFR[0] |= (1U<<13);
	GPIOA->AFR[0] |= (1U<<14);
	GPIOA->AFR[0] &= ~(1U<<15);

//	UART configuration
//	Enable clock access to UART2
	RCC->APB1ENR |= UART2EN;

//	Set the baudrate
	uart_set_baudrate(CLK, UART_BAUDRATE);

//	Select to use  DMA for TX and  RX
	USART2->CR3 = CR3_DMAT;
	USART2->CR3 |= CR3_DMAR;

//	Set transfer direction (TX and RX)
	USART2->CR1 = CR1_TE;
	USART2->CR1 |= CR1_RE;

//	Enable UART mode
	USART2->CR1 |= CR1_UE;

}

void dma1_init(void)
{
//	Enable clock access for dma1
	RCC->AHB1ENR |= DMA1EN;
}

void dma1_stream5_uart_rx_config(void)
{
//	Disable DMA stream
	DMA1_Stream5->CR &= ~DMA_SCR_EN;

//	Wait until  DMA is disabled
	while((DMA1_Stream5->CR & DMA_SCR_EN)){}

//	Clear interrupt flags for stream 5
	DMA1->HIFCR |= (HIFCR_CDMEIF5 | HIFCR_CTEIF5  | HIFCR_CTCIF5);

//	Set peripheral address
	DMA1_Stream5->PAR = (uint32_t) (&(USART2->DR));

//	Set memory address
	DMA1_Stream5->M0AR = (uint32_t) (uart_data_buffer);

//	Set number of transfer
	DMA1_Stream5->NDTR = (uint16_t) UART_DATA_BUFF_SIZE;

//	Select channel 4
	DMA1_Stream5->CR &= ~(1U<<25);
	DMA1_Stream5->CR &= ~(1U<<26);
	DMA1_Stream5->CR |= (1U<<27);

//	Enable memory addr increment
	DMA1_Stream5->CR |= DMA_SCR_MINC;

//	Enable TC interrupt
	DMA1_Stream5->CR |= DMA_SCR_TCIE;

//	Enable circular mode
	DMA1_Stream5->CR |= DMA_SCR_CIRC;

//	Set transfer direction: periph to memory
	DMA1_Stream5->CR &= ~(1U<<6);
	DMA1_Stream5->CR &= ~(1U<<7);

//	Enable DMA stream
	DMA1_Stream5->CR |= DMA_SCR_EN;

//	Enable DMA Stream5 Interrupt in NVIC
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

void dma1_stream6_uart_tx_config(uint32_t msg_to_send, uint32_t msg_len)
{
//	Disable DMA stream
	DMA1_Stream6->CR &= ~DMA_SCR_EN;

//	Wait until  DMA is disabled
	while((DMA1_Stream6->CR & DMA_SCR_EN)){}

//	Clear interrupt flags for stream 6
	DMA1->HIFCR |= (HIFCR_CDMEIF6 | HIFCR_CTEIF6  | HIFCR_CTCIF6);

//	Set peripheral address
	DMA1_Stream6->PAR = (uint32_t) (&(USART2->DR));

//	Set memory address
	DMA1_Stream6->M0AR = (uint32_t) (msg_to_send);

//	Set number of transfer
	DMA1_Stream6->NDTR = (uint16_t) msg_len;

//	Select channel 4
	DMA1_Stream6->CR &= ~(1U<<25);
	DMA1_Stream6->CR &= ~(1U<<26);
	DMA1_Stream6->CR |= (1U<<27);

//	Enable memory addr increment
	DMA1_Stream6->CR |= DMA_SCR_MINC;

//	Enable TC interrupt
	DMA1_Stream6->CR |= DMA_SCR_TCIE;

//	Set transfer direction: memory to periph
	DMA1_Stream6->CR |= (1U<<6);
	DMA1_Stream6->CR &= ~(1U<<7);

//	Enable DMA stream
	DMA1_Stream6->CR |= DMA_SCR_EN;

//	Enable DMA Stream5 Interrupt in NVIC
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
	return ((periph_clk + (baudrate/2U))/baudrate);
}

static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
	USART2->BRR = compute_uart_bd(periph_clk, baudrate);
}

