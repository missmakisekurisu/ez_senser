#include "stm32f10x_conf.h"
#include "dht11.h"
#include "delay.h"
#include "oled_i2c.h"
#include "oled.h"
//#include "bmp.h"

static uint32_t test = 0;
static uint8_t c_char[] = {'C','H','E','E','R',' ','U','P','!','\0'};
static uint8_t humidity_char[] = {'h','u','m','i','d','i','t','y','\0'};
static uint8_t heat_char[] = {'h','e','a','t','\0'};

static void display_humidity_temperature(void){
    if(appTimer[TIMER_OLED_FRESH].flag){
        OLED_Clear();        
        OLED_ShowNum(80,0,recDataDHT11[0],2,16);
        OLED_ShowString(0,0,humidity_char, 16);
        OLED_ShowNum(80,2,recDataDHT11[2],2,16);
        OLED_ShowString(0,2,heat_char, 16);
				OLED_ShowString(0,4,c_char, 16);
    }
}
int main(){
    TIM_init();    

    OLED_Init();
    OLED_Clear();

    //test_oled();    
    for(;1;){      
        DHT11_main_task();
        display_humidity_temperature();
        test++;
    }
    return 0;
}