#ifndef __OLED_I2C_H
#define __OLED_I2C_H
#include "stm32f10x.h" 
#include "delay.h"

#define USE_REAL_I2C    (0)
#if(USE_REAL_I2C)
#define       OLED_I2Cx                       I2C1
#define       OLED_I2C_APBxClock_FUN          RCC_APB1PeriphClockCmd
#define       OLED_I2C_CLK                    RCC_APB1Periph_I2C1
#define       OLED_I2C_GPIO_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define                     OLED_I2C_GPIO_CLK         RCC_APB2Periph_GPIOB
#define             OLED_I2C_SCL_PORT                        GPIOB
#define             OLED_I2C_SCL_PIN                         GPIO_Pin_6
#define             OLED_I2C_SDA_PORT                        GPIOB
#define             OLED_I2C_SDA_PIN                         GPIO_Pin_7
#define I2Cx_OWN_ADDRESS7   0xad

#define I2C_Speed              400000  

#else
#define             SI2C_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             SI2C_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             SI2C_PORT                            GPIOB
#define             SI2C_SCL_PIN                         GPIO_Pin_6
#define             SI2C_SDA_PIN                         GPIO_Pin_7

#define SSD1306_ADDR    0x78
void ssd1306_generate_a_frame(uint8_t *p, uint16_t size);
#endif

void oled_i2c_init(void);
void HAL_I2C_Mem_Write(uint8_t DevAddress, uint8_t MemAddress,uint8_t *pData, uint16_t Size);


#endif