#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "audio.h"

extern const u8 rawAudio[123200];

int ADC_RESULT=0;
int glosnosc=1;

void TIM2_IRQHandler(void)
{
             if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
             {
            	 for(int i=0; i<123200; i++)
            	 DAC_SetChannel1Data(DAC_Align_12b_R, rawAudio[i]*glosnosc/1000);
            	 TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
             }
}

void TIM3_IRQHandler(void)
{
             if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
             {
                    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
             }
}

void TIM4_IRQHandler(void)
{

             if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
             {

                 TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
             }
}

void init2()
{
	NVIC_InitTypeDef NVIC_InitStructure2;
	NVIC_InitStructure2.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure2.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure2.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure2.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure2);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void init3()
{
	NVIC_InitTypeDef NVIC_InitStructure3;
	NVIC_InitStructure3.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure3);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

void init4()
{
	NVIC_InitTypeDef NVIC_InitStructure4;
	NVIC_InitStructure4.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure4.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure4.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure4.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure4);
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

int main(void)
{
	SystemInit();


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 9999;
	TIM_TimeBaseStructure.TIM_Prescaler = 41999;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure2;
	TIM_TimeBaseStructure2.TIM_Period = 49;
	TIM_TimeBaseStructure2.TIM_Prescaler = 104;
	TIM_TimeBaseStructure2.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure2.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure2);
	TIM_Cmd(TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure4;
	TIM_TimeBaseStructure4.TIM_Period = 104;
	TIM_TimeBaseStructure4.TIM_Prescaler = 999;
	TIM_TimeBaseStructure4.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure4.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure4);
	TIM_Cmd(TIM4, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	init2();
	//init3();
	//init4();


	GPIO_InitTypeDef  GPIO_InitStructureADC1;
	GPIO_InitStructureADC1.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructureADC1.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructureADC1.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructureADC1);

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);


	ADC_InitTypeDef ADC_InitStructure1;
	ADC_InitStructure1.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure1.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure1.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure1.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure1.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure1.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure1.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure1);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

	ADC_Cmd(ADC1, ENABLE);



	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DAC_InitTypeDef DAC_InitStructure;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1, ENABLE);

	DAC_SetChannel1Data(DAC_Align_12b_R, 0xFFF);


	GPIO_InitTypeDef  GPIO_InitStructureD;
	GPIO_InitStructureD.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructureD.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructureD.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructureD.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructureD.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructureD);


	for(;;)
	{

		ADC_SoftwareStartConv(ADC1);
				while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
				ADC_RESULT = ADC_GetConversionValue(ADC1);
				//glosnosc = ADC_RESULT;
				GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				if(ADC_RESULT<1000)
					GPIO_SetBits(GPIOD, GPIO_Pin_12);
				else if(ADC_RESULT<2000)
					GPIO_SetBits(GPIOD, GPIO_Pin_13);
				else if(ADC_RESULT<3000)
					GPIO_SetBits(GPIOD, GPIO_Pin_14);
				else if(ADC_RESULT<4000)
					GPIO_SetBits(GPIOD, GPIO_Pin_15);
				else
					GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	}


}
