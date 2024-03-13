#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f10x.h"                 

#define            DHT11_SDA_PORT           GPIOA
#define            DHT11_SDA_PIN            GPIO_Pin_7
#define            DHT11_SDA_CLK_SRC            RCC_APB2Periph_GPIOA
#define            DHT11_SDA_APBxClock_FUN      RCC_APB2PeriphClockCmd

#define SET_PIN_DHT11   GPIO_SetBits(DHT11_SDA_PORT, DHT11_SDA_PIN)
#define RESET_PIN_DHT11 GPIO_ResetBits(DHT11_SDA_PORT, DHT11_SDA_PIN)
#define READ_BIT_DHT11  GPIO_ReadInputDataBit(DHT11_SDA_PORT, DHT11_SDA_PIN)

void DHT11_main_task(void);
extern uint8_t recDataDHT11[5];
#endif


