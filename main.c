#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"
#include "misc.h"
#include "delay.h"
#include "codec.h"
#include "List.h"
#include "ff.h"
#include <stdbool.h>

FATFS fatfs;
FIL file;
u16 sample_buffer[2048];
volatile s8 num_of_switch=-1;
volatile u16 result_of_conversion=0;
volatile u8 diode_state=0;
volatile s8 change_song=0;
volatile u8 error_state=0;
volatile bool random_mode=0;
bool pause=0;
char song_time[5]={'0', '0', ':', '0', '0'};
bool half_second=0;
char CharBufor;
char SongList='?';

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
		if(USART1->DR == 'v')	num_of_switch=1;
		if(USART1->DR == 'r')	PlayPause();
		if(USART1->DR == 'p')	change_song=-1;
		if(USART1->DR == 'n')	change_song=1;
		if(USART1->DR == 'u')	num_of_switch=5;
		if(USART1->DR == '0' || USART1->DR == '1' || USART1->DR == '2' || USART1->DR == '3'
			|| USART1->DR == '4' || USART1->DR == '5' || USART1->DR == '6'
			|| USART1->DR == '7' || USART1->DR == '8' || USART1->DR == '9')
				CharBufor += USART1->DR;

		if (num_of_switch==1)
		{
			int VolumeBufor = atoi(CharBufor);
			result_of_conversion * (VolumeBufor/100);
		}

		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, (char)(USART1->DR - 32));
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

		USART_ClearFlag(USART1, USART_IT_RXNE);
	}
}

void PlayPause()
{
	if(pause==0)
	{
		pause=1;
		TIM_Cmd(TIM5,DISABLE);
		Codec_PauseResume(0);
		NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT, ENABLE);
	}
	else
	{
		pause=0;
		TIM_Cmd(TIM5,ENABLE);
		Codec_PauseResume(1);
		NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT, DISABLE);
	}
}

void USART_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		GPIO_InitTypeDef GPIO_InitStructureUSART;
		GPIO_InitStructureUSART.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
		GPIO_InitStructureUSART.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructureUSART.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructureUSART.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructureUSART.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructureUSART);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

		NVIC_InitTypeDef NVIC_InitStructureU;
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		NVIC_InitStructureU.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructureU.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructureU.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructureU.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructureU);
		NVIC_EnableIRQ(USART1_IRQn);

		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = 9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		USART_Init(USART1, &USART_InitStructure);

		USART_Cmd(USART1, ENABLE);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		ADC_conversion();
		Codec_VolumeCtrl(result_of_conversion);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, '?');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		if (error_state==1)
		{
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		}
		if (error_state==2)
		{
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		}
		if (error_state==3)
		{
			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
	{
		if(half_second==false)
				{
					half_second=true;
				}
				else
				{
					half_second=false;
				}
				if(half_second==0)
				{
					song_time[4]++;
					if(song_time[4]==':')
					{
						song_time[3]++;
						song_time[4]='0';
					}
					if(song_time[3]=='6')
					{
						song_time[1]++;
						song_time[3]='0';
					}
					if(song_time[1]==':')
					{
						song_time[0]++;
						song_time[1]='0';
					}
					if(song_time[0]==':')
					{
						song_time[0]='0';
					}
				}
				spin_diodes();
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
}
void ERROR_TIM_4()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseInitTypeDef TIMER_4;
	TIMER_4.TIM_Period = 23999;
	TIMER_4.TIM_Prescaler = 999;
	TIMER_4.TIM_ClockDivision = TIM_CKD_DIV1;
	TIMER_4.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIMER_4);
	TIM_Cmd(TIM4,DISABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}
void TIM3_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseInitTypeDef TIMER_3;
	TIMER_3.TIM_Period = 47999;
	TIMER_3.TIM_Prescaler = 999;
	TIMER_3.TIM_ClockDivision = TIM_CKD_DIV1;
	TIMER_3.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIMER_3);
	TIM_Cmd(TIM3, ENABLE);


	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}
void DIODES_init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef  DIODES;
	DIODES.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	DIODES.GPIO_Mode = GPIO_Mode_OUT;
	DIODES.GPIO_OType = GPIO_OType_PP;
	DIODES.GPIO_Speed = GPIO_Speed_100MHz;
	DIODES.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &DIODES);
}
void spin_diodes()
{
	if(random_mode==0)
	{
		GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	}
	else
	{
		GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	}

	if(diode_state==3)
	{
		if(random_mode==0)
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_12);
		}
		else
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		}
		diode_state=0;
	}
	else if(diode_state==2)
	{
		if(random_mode==0)
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
		}
		else
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		}
		diode_state=3;
	}
	else if(diode_state==1)
	{
		if(random_mode==0)
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
		}
		else
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		}
		diode_state=2;
	}
	else if(diode_state==0)
	{
		if(random_mode==0)
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_15);
		}
		else
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_15);
		}
		diode_state=1;
	}
}
void MY_DMA_initM2P()
{
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream5);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_BufferSize = 2048;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&sample_buffer;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI3->DR));
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;

	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream5, ENABLE);

	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);
	SPI_Cmd(SPI3,ENABLE);
}
void ADC_conversion()
{
	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	result_of_conversion = ((ADC_GetConversionValue(ADC1))/16);
}
void TIM5_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimeBaseInitTypeDef TIMER;
	TIMER.TIM_Period = 8399;
	TIMER.TIM_Prescaler = 2999;
	TIMER.TIM_ClockDivision = TIM_CKD_DIV1;
	TIMER.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM5, &TIMER);
	TIM_Cmd(TIM5,DISABLE);

	NVIC_InitTypeDef NVIC_InitStructure5;
	NVIC_InitStructure5.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure5.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure5.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure5.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure5);
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
}
void TIM2_ADC_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TIMER_2;
	TIMER_2.TIM_Period = 2999;
	TIMER_2.TIM_Prescaler = 1999;
	TIMER_2.TIM_ClockDivision = TIM_CKD_DIV1;
	TIMER_2.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIMER_2);
	TIM_Cmd(TIM2, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}
void ADC_init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructureADC;
	GPIO_InitStructureADC.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructureADC.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructureADC.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructureADC);

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);
	ADC_Cmd(ADC1, ENABLE);

	TIM2_ADC_init();
}
bool read_and_send(FRESULT fresult, int position, volatile ITStatus it_status, UINT read_bytes, uint32_t DMA_FLAG)
{
	it_status = RESET;
	while(it_status == RESET)
	{
		it_status = DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG);
	}
	fresult = f_read (&file,&sample_buffer[position],1024*2,&read_bytes);
	DMA_ClearFlag(DMA1_Stream5, DMA_FLAG);

	if(fresult != FR_OK)
	{
		error_state=2;
		return 0;
	}
	if(read_bytes<1024*2||change_song!=0)
	{
		return 0;
	}
	return 1;
}
void play_wav(struct List *song, FRESULT fresult)
{
	struct List *temporary_song=song;
	UINT read_bytes;
	fresult = f_open( &file, temporary_song->file.fname, FA_READ );
	if( fresult == FR_OK )
	{
		fresult=f_lseek(&file,44);
		volatile ITStatus it_status;
		change_song=0;
		song_time[0]=song_time[1]=song_time[3]=song_time[4]='0';
		song_time[2]=':';
		half_second=0;
		TIM_Cmd(TIM5, ENABLE);
		while(!pause)
		{
			if (read_and_send(fresult,0, it_status, read_bytes, DMA_FLAG_HTIF5)==0)
			{
				break;
			}
			if (read_and_send(fresult, 1024, it_status, read_bytes, DMA_FLAG_TCIF5)==0)
			{
				break;
			}
		}
		diode_state=0;
		TIM_Cmd(TIM5, DISABLE);
		fresult = f_close(&file);
	}
}
bool isWAV(FILINFO fileInfo)
{
	int i=0;
	for (i=0;i<10;i++)
	{
		if(fileInfo.fname[i]=='.')
		{
			if(fileInfo.fname[i+1]=='W' && fileInfo.fname[i+2]=='A' && fileInfo.fname[i+3]=='V')
			{
				return 1;
			}
		}
	}
	return 0;
}

void StartInit()
{
	SystemInit();
	DIODES_init();
	ERROR_TIM_4();
	delay_init( 80 );
	SPI_SD_Init();

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick_Config(90000);
}

void PlayInit()
{
	codec_init();
	codec_ctrl_init();
	I2S_Cmd(CODEC_I2S, ENABLE);
	MY_DMA_initM2P();
	ADC_init();
	USART_init();
	TIM5_Init();
	TIM3_Init();
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);
}

int main( void )
{
	StartInit();
	FRESULT fresult;
	DIR Dir;
	FILINFO fileInfo;

	struct List *first=0,*last=0,*pointer;

	disk_initialize(0);
	fresult = f_mount( &fatfs, "",1 );
	if(fresult != FR_OK)
	{
		error_state=1;
		TIM_Cmd(TIM4, ENABLE);
		for(;;)
		{ }
	}
	fresult = f_opendir(&Dir, "\\");
	if(fresult != FR_OK)
	{
		return(fresult);
	}
	u32 number_of_songs=0;
	for(;;)
	{
		fresult = f_readdir(&Dir, &fileInfo);
		if(fresult != FR_OK)
		{
			return(fresult);
		}
		if(!fileInfo.fname[0])
		{
			break;
		}

		if(isWAV(fileInfo)==1)
		{
			if(number_of_songs==0)
			{
				first=last=add_last(last,fileInfo);
			}
			else
			{
				last=add_last(last,fileInfo);
			}
			number_of_songs++;
		}
	}
	if (first==0)
	{
		error_state=3;
		for(;;)
		{ }
	}
	last->next=first;
	first->previous=last;
	pointer=first;

	PlayInit();
	u32 rand_number=0;
	u32 i_loop=0;

	for(;;)
	{
		play_wav(pointer, fresult);
		if(error_state!=0)
		{
			break;
		}
		if(change_song>=0)
		{
			if(random_mode==1 && number_of_songs>1)
			{
				rand_number=RNG_GetRandomNumber()%(number_of_songs-1)+1;
				for(i_loop=1;i_loop<=rand_number;i_loop++)
				{
					pointer=pointer->next;
				}
			}
			else
			{
				pointer=pointer->next;
			}
		}
		else if (change_song==-1)
		{
			if(random_mode==1 && number_of_songs>1)
			{
				rand_number=RNG_GetRandomNumber()%(number_of_songs-1)+1;
				for(i_loop=1;i_loop<=rand_number;i_loop++)
				{
					pointer=pointer->previous;
				}
			}
			else
			{
				pointer=pointer->previous;
			}
		}
	}
	GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15|CODEC_RESET_PIN);
	TIM_Cmd(TIM2, DISABLE);
	TIM_Cmd(TIM4, ENABLE);
	for(;;)
	{ }
	return 0;
}
void SysTick_Handler()
{
	disk_timerproc();
}

