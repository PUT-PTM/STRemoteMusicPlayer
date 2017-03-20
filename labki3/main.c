#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

int main(void)
{
	SystemInit();

	/* GPIOD Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 419;
	TIM_TimeBaseStructure.TIM_Prescaler = 399;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM4, ENABLE);
/*
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
*/

	/*
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOE, &GPIO_InitStructure);
*/
	unsigned int i;

	TIM_OCInitTypeDef TIM_OCInitStructure;
	    // PWM1 Mode configuration:
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOD, &GPIO_InitStructure);


	//zad5 12/13/14/15 -wybor cyfry
	int a=-1;


	for(;;)
		{
			TIM4->CCR1=a;

			if(TIM_GetFlagStatus(TIM4, TIM_FLAG_Update))
			{
				if(a<419)
					a+=1;
				else
					a=0;

			    		TIM_ClearFlag(TIM4, TIM_FLAG_Update);
			}
		}
			//GPIO_SetBits(GPIOE, GPIO_Pin_12);
			//GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_11);
			//GPIO_ResetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);



}

/*
			switch(a)
			{
			case 0:

				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
				break;

			case 1:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5 | GPIO_Pin_6);
				break;

			case 2:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_10);
				break;

			case 3:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10);
				break;

			case 4:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_9 | GPIO_Pin_10);
				break;

			case 5:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_9 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10);
				break;
			case 6:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_9 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_8);
				break;

			case 7:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_4);
				break;

			case 8:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10);
				break;
			case 9:
				GPIO_SetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11);
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
				GPIO_ResetBits(GPIOE,  GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_9| GPIO_Pin_10);
				a=-1;
				break;

			}
*/
