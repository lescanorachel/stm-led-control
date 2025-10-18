#include "gpio.h"

#define GPIOAEN			(1U<<0)
#define LED_BS5			(1U<<5) /* Bit set pin 5 */
#define LED_BR5			(1U<<21) /* Bit reset pin 5 */
#define LED_PIN			(1U<<5)

void led_init(void)
{
	// Enable clock access for GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// Set PA5 mode to output mode
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11);
}

void led_on(void)
{
	// Set PA5 high
	GPIOA->BSRR |= LED_BS5;
}

void led_off(void)
{
	// Set PA5 low
	GPIOA->BSRR |= LED_BR5;
}

void led_toggle(void)
{
	// Toggle PA5
	GPIOA->ODR ^= LED_PIN;
}

int get_led_status(void)
{
	// Get PA5 led status
	if (GPIOA->ODR & (1U<<5))
	{
		return 1;
	}
	return 0;
}




