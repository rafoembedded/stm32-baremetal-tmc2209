#include "stm32f1xx.h"

#define IOPA_EN		(1U<<2)
#define TIM2_EN		(1U<<0)
#define CCER_CC1P	(1U<<1)
#define CCER_CC2P	(1U<<5)
#define CR1_CEN		(1U<<0)

void tim2_Enc(void)
{
	/*Enable RCC for GPIOA*/
	RCC->APB2ENR |= IOPA_EN;

	/*Enable RCC for TIM2*/
	RCC->APB1ENR |= TIM2_EN;

	/*Clear GPIO Registers*/
	/*Clear A0*/
	GPIOA->CRL &= ~(0xF<<0);
	/*Clear A1*/
	GPIOA->CRL &= ~(0xF<<4);

	/*Configure GPIO as Input with pull-up*/
	/*Configure A0*/
	GPIOA->CRL |= (0x8<<0);
	/*Configure A1*/
	GPIOA->CRL |= (0x8<<4);

	/*configure pull-up*/
	/*Configure A0*/
	GPIOA->ODR |= (1U<<0);
	/*Configure A1*/
	GPIOA->ODR |= (1U<<1);

	/*configure filter*/
	/*clear registers*/
	TIM2->CR1 &= ~(0x3<<8);
	/*Clock division tDTS = 4 × tCK_INT*/
	TIM2->CR1 |=  (0x2<<8);

	/*clear registers*/
	TIM2->CCMR1 &= ~(0xF<<4);
	/*configure filter ch1: 1111: fSAMPLING=fDTS/32, N=8*/
	TIM2->CCMR1 |= (0xF<<4);
	/*clear registers*/
	TIM2->CCMR1 &= ~(0xF<<12);
	/*configure filter ch2: 1111: fSAMPLING=fDTS/32, N=8*/
	TIM2->CCMR1 |= (0xF<<12);

	/*Configure TI1, TI2*/
	/*Clear CC1S*/
	TIM2->CCMR1 &= ~(0x3<<0);
	/*configure channel as input, IC1 is mapping on TI1*/
	TIM2->CCMR1 |= 	(0x1<<0);
	/*Clear CC2S*/
	TIM2->CCMR1 &= ~(0x3<<8);
	/*configure channel as input, IC1 is mapping on TI2*/
	TIM2->CCMR1 |= 	(0x1<<8);

	/*configure polarity*/
	/*channel 1 configure as non-inverted*/
	TIM2->CCER &= ~(CCER_CC1P);
	/*channel 2 configure as non-inverted*/
	TIM2->CCER &= ~(CCER_CC2P);

	/*configure slave mode*/
	/*clear registers*/
	TIM2->SMCR &= ~(0x7<<0);
	/*Counter counts up/down on both TI1FP1 and TI2FP2 edges depending on the level of the other input.*/
	TIM2->SMCR |=  (0x3<<0);

	/*Counter enable*/
	TIM2->CR1 |= CR1_CEN;

	/*We are waiting for the filter to digest the initial voltage surge*/
	for(volatile int i = 0; i < 3000; i++) { __NOP(); }
	/*clear counter*/
	TIM2->CNT = 0;
}
