#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f10x.h" 
enum timer_num{
    TIMER_OLED_FRESH,
    DHT11_TASK,
    TIMER_COUNT,
};
typedef struct{
    uint16_t timer;
    uint16_t timeCnt; 
    uint16_t pendingTime;
    uint8_t flag;
}APP_TIMER_TYPE;
extern volatile APP_TIMER_TYPE appTimer[TIMER_COUNT];
void delay_us(uint32_t delay_us);
void delay_ms(uint16_t delay_ms);
void TIM_init(void);
void TIM_delay_ms(uint16_t);

#endif