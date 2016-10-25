/*
 * vrs_cv5.h
 *
 *  Created on: 19. 10. 2016
 *      Author: Stancoj
 */
#include <stddef.h>
#include "stm32l1xx.h"

#ifndef VRS_CV5_H_
#define VRS_CV5_H_
	void NVCI_ADC_init(void);
	void adc_init(void);
//	void LED_init(void);
	void USART1_init(void);
	void NVCI_USART1_init(void);
	void USART1_init(void);
	void Tx_sendData(uint16_t value);
	void PutcUART1(char value);
	void Set_format(void);
#endif /* VRS_CV5_H_ */
