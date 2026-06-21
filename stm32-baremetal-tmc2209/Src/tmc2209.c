#include "stm32f1xx.h"

#define FCK			(8000000)
#define BAUD		(115200)

#define IOPB_EN		(1U<<3)
#define USART3_EN	(1U<<18)

#define CR1_UE		(1U<<13)
#define CR1_M		(1U<<12)
#define CR2_STOP	(0x3<<12)
#define CR1_TE		(1U<<3)
#define CR1_RE		(1U<<2)
#define CR2_LINEN	(1U<<14)
#define CR2_CLKEN	(1U<<11)
#define CR3_SCEN	(1U<<5)
#define CR3_IREN	(1U<<1)
#define CR3_HDSEL	(1U<<3)

#define SR_TXE		(1U<<7)
#define SR_TC		(1U<<6)

void usart3_halfduplex(void)
{
	/*Enable RCC for GPIOB*/
	RCC->APB2ENR |= IOPB_EN;

	/*Clear PB10 register*/
	GPIOB->CRH &= ~(0xF<<8);

	/*Configure PB10 as Alternate function output Open-drain*/
	GPIOB->CRH |= (0xF<<8);

	/*Enable RCC for USART3*/
	RCC->APB1ENR |= USART3_EN;

	/*Word length*/
	/*1 Start bit, 8 Data bits, n Stop bit*/
	USART3->CR1 &= ~(CR1_M);

	/*STOP bits*/
	USART3->CR2 &= ~(CR2_STOP);

	/*Configure BaudRate*/
	USART3->BRR = (FCK + (BAUD / 2)) / BAUD;

	/*Clear LIN mode register*/
	USART3->CR2 &= ~(CR2_LINEN);

	/*Clear Clock enable register*/
	USART3->CR2 &= ~(CR2_CLKEN);

	/*Clear Smartcard mode register*/
	USART3->CR3 &= ~(CR3_SCEN);

	/*Clear IrDA mode register*/
	USART3->CR3 &= ~(CR3_IREN);

	/*Half-duplex selection*/
	USART3->CR3 |= CR3_HDSEL;

	/*USART enable*/
	USART3->CR1 |= CR1_UE;

	/*Transmitter enable*/
	USART3->CR1 |= CR1_TE;

	/*Receiver enable*/
	USART3->CR1 |= CR1_RE;
}

void send_array(uint8_t *arr, uint8_t size)
{
	/*Receiver disable*/
	USART3->CR1 &= ~(CR1_RE);

	for (int i = 0; i < size; i++)
	{
		/*Wait for the Transmit Data Register to be empty*/
		while(!(USART3->SR & SR_TXE)){}

		/*Send byte*/
		USART3->DR = arr[i];
	}

	/*Wait for Transmission Complete*/
	 while(!(USART3->SR & SR_TC)){}

	/*Receiver enable*/
	USART3->CR1 |= CR1_RE;
}

uint8_t tmc2209_crc(uint8_t *data, uint8_t length)
{
    uint8_t crc = 0;

    for(uint8_t i = 0; i < length; i++)
    {
        uint8_t currentByte = data[i];

        for(uint8_t j = 0; j < 8; j++)
        {
            // LSB -> MSB
            if ((crc >> 7) ^ (currentByte & 0x01))
            {
                crc = (crc << 1) ^ 0x07;
            }
            else
            {
                crc = (crc << 1);
            }

            currentByte >>= 1;
        }
    }

    return crc;
}
