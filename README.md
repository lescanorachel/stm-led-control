#STM32 LED Control
This project involves the use of pre-defined typed commands and a push button to control the state of an LED. It demonstrates the use of different peripherals of STM32F411 including GPIO, UART, DMA, and EXTI. The push button and LED used for this demo project are the user push buttons (PC13) and user LED (PA5) on the Nucleo F411 board.

##Peripherals Used
###GPIO
GPIO is configured to use User LED (PA5) as output, User Push Button (PC13) as input, and PA2 & PA3 to function in alternate function modes to be used for UART communication.

###UART
UART is the communication protocol to send the typed commands from computer CLI terminal to the microcontroller and control the LED state.

###DMA
DMA is enabled in the UART  configuration to demonstrate efficiency in only interrupting the main program once commands are typed completely n the computer CLI terminal.

###EXTI
Aside from typed commands, EXTI is also configured for the User LED to respond (toggle) from Push Button input triggers.


##How to Use
Three commands are pre-defined to control the LED:
- LED_ON: To turn on the LED
- LED_OF: To turn off the LED
- LED_ST: To display the current status of the LED on the computer CLI terminal

The push button is programmed to toggle the current state of the LED. That is if it's on, pushing the button will make it off and vice versa.


