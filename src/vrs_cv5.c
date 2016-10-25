/*
 * vrs_cv5.c
 *
 *  Created on: 19. 10. 2016
 *      Author: Stancoj
 */

#include "vrs_cv5.h"
#include <stddef.h>
#include "stm32l1xx.h"
#include <stdio.h>

uint8_t RxData = 0, format = 0;
uint16_t adc;
//extern uint16_t adc;

void NVCI_ADC_init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVCI_USART1_init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void GPIO_USART1_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //USART1_Tx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ; //USART1_Rx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
}

void USART1_init(void)
{
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	//USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		RxData = USART_ReceiveData(USART1);
		Set_format();
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}

	if(USART_GetITStatus(USART1, USART_IT_TC)!= RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_TC);
		Tx_sendData(adc);
	}
}

void adc_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_HSICmd(ENABLE);

	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//povolenie cyklickej konverzie
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_192Cycles);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); // povolenie prerusenia od EOC
	ADC_ITConfig(ADC1, ADC_IT_OVR, ENABLE); // povolenie prerusenia od OVR
	ADC_Cmd(ADC1, ENABLE);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET){}

	ADC_SoftwareStartConv(ADC1);
}

void PutcUART1(char ch)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

void Set_format(void)
{
	if(RxData == 'm')
	{
		format = format+1;
		if(format > 1)
		{
			format = 0;
		}
		RxData = 0;
	}
}

void Tx_sendData(uint16_t value)
{
	uint8_t i,jednotky,desatiny;
	char chain[6];
	float napatie;

	napatie = (3.33/4096)*value;
	jednotky = (uint8_t)(napatie);
	desatiny = (uint8_t)((napatie*100) - (jednotky*100));

	if(format == 1)
	{
		snprintf(chain, 6, "%d", value);
	}
	else
		snprintf(chain, 6, "%d.%d", jednotky,desatiny);

	for(i = 0; i < 4;i++)
	{
		PutcUART1(chain[i]);
	}

	if(format == 0)PutcUART1('V');
	PutcUART1(13); //enter
}

void ADC1_IRQHandler(void)
{
	//uint16_t adc;
	uint16_t dummy;
	if(ADC1->SR & ADC_SR_EOC){
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
		adc = ADC_GetConversionValue(ADC1);
	}
	if(ADC1->SR & ADC_SR_OVR){
		ADC_ClearITPendingBit(ADC1, ADC_IT_OVR);
		dummy = ADC1->DR;
	}
}
