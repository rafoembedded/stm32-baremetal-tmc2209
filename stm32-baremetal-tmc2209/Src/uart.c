#include "stm32f1xx.h"

#define FCK				(8000000)
#define BAUD			(115200)

#define USART1_EN		(1U<<14)
#define IOPA_EN			(1U<<2)
#define CR1_UE			(1U<<13)
#define CR1_M			(1U<<12)
#define CR2_STOP		(0x3<<12)
#define CR1_TE			(1U<<3)
#define SR_TXE			(1U<<7)
#define SR_TC			(1U<<6)

/*
 * Set baudrate
 *
 * baud = fck / (16 * usartdiv)
 * BRR = 16 * usartdiv
 * BRR = fck / baud
 *
 * round up
 * BRR = (fck + (boud / 2)) / boud
 *
 */

/*
 * A9 -- TX
 */

void usart1_tx_init(void)
{
	/*Enable clock for GPIOA*/
	RCC->APB2ENR |= IOPA_EN;

	/*Clear A9 registers*/
	GPIOA->CRH &= ~(0xF<<4);

	/*Configure A9 as Alternate function output Push-pull speed 50MHz*/
	GPIOA->CRH |= (0xB<<4);

	/*Enable clock for USART1*/
	RCC->APB2ENR |= USART1_EN;

	/*USART enable*/
	USART1->CR1 |= CR1_UE;

	/*USART Word length as 1 Start bit, 8 Data bits, n Stop bit*/
	USART1->CR1 &= ~CR1_M;

	/*Configure number of stop bits*/
	USART1->CR2 &= ~CR2_STOP;

	/*Set baud*/
//	double usartdiv = (double)FCK / (BAUD * 16);
//	uint32_t mantissa = usartdiv;
//	uint32_t fraction = (usartdiv - mantissa) * 16 + 0.5;
//	USART1->BRR = ((mantissa & 0xFFF) << 4) | ((fraction & 0xF) << 0);

	USART1->BRR = (FCK + (BAUD / 2)) / BAUD;

	/*Transmitter enable*/
	USART1->CR1 |= CR1_TE;
}

void uart_send_char(char ch)
{
	while (!(USART1->SR & SR_TXE)){}
	USART1->DR = ch;
}

void __io_putchar (int ch)
{
	uart_send_char(ch);
}
