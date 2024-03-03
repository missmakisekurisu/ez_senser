#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f10x.h"                  // Device header

#define OLD_ONE (0)
#if(OLD_ONE)
#define dht11_high GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define dht11_low GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define Read_Data GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)

void DHT11_GPIO_Init_OUT(void);
void DHT11_GPIO_Init_IN(void);
void DHT11_Start(void);
unsigned char DHT11_REC_Byte(void);
void DHT11_REC_Data(void);
#else
#define SET_PIN_DHT11   GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define RESET_PIN_DHT11 GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define READ_BIT_DHT11  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)

void DHT11_main_task(void);
extern uint8_t recDataDHT11[5];
#endif
#endif
