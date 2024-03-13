#include "stm32f10x.h"                  // Device header
#include  "dht11.h"
#include  "delay.h"

uint8_t recDataDHT11[5] = {0};

static void master_output_init(void){
    DHT11_SDA_APBxClock_FUN(DHT11_SDA_CLK_SRC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = DHT11_SDA_PIN,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Mode = GPIO_Mode_Out_PP,
    };
	GPIO_Init(DHT11_SDA_PORT, &GPIO_InitStructure);
}


static void master_receive_init(void){
    DHT11_SDA_APBxClock_FUN(DHT11_SDA_CLK_SRC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = DHT11_SDA_PIN,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Mode = GPIO_Mode_IPU,
    };
	GPIO_Init(DHT11_SDA_PORT, &GPIO_InitStructure);
}	


static uint8_t read_byte_DHT11(void){
    uint8_t data = 0U;
    for(int i = 0;i < 8;i++){
        //transfer start with 50us RESET
        while(READ_BIT_DHT11 == RESET);
        //SET signal duration = 26-28us: this bit = 0; 
        //SET signal duration = 70us: this bit = 1;
        delay_us(35); 
        if(READ_BIT_DHT11 == SET){
            data += (1 <<(7-i));
        }
        //if bit = 1, need to wait utill SET signal end
        while(READ_BIT_DHT11 == SET);
    }
    return data;
}



static void DHT11_read_task(void){
    uint8_t data[5] = {0};
    //wire idle: set Pin for 2ms
    master_output_init();
    SET_PIN_DHT11;
    TIM_delay_ms(10u);
    //master send trigger signal: reset Pin for required at least 18ms
    RESET_PIN_DHT11;
    TIM_delay_ms(20u);
    //master shift into receive mode: IPU
    master_receive_init();
    delay_us(30);
    //wait DHT11 response in 20-40us, it will reset Pin for 80us
    while(READ_BIT_DHT11 == RESET); 
    //DHT11 response end, set Pin for 80us
    while(READ_BIT_DHT11 == SET);
    //transfer start, 5 bytes
    for(int i = 0;i < 5;i++){
        data[i] =  read_byte_DHT11();      
    }
    //wait for DHT11 end transfer: reset Pin for 50us
    while(READ_BIT_DHT11 == RESET);
    //wire recover to idle
    master_output_init();
    SET_PIN_DHT11;
    //check sum
    uint16_t sum = 0;
    for(int j = 0;j <4;j++){
        sum += data[j];
    }
   if(data[4] == (uint8_t)sum){
        recDataDHT11[0] = data[0];
        recDataDHT11[1] = data[1];
        recDataDHT11[2] = data[2];
        recDataDHT11[3] = data[3];
    }

}

void DHT11_main_task(void){
    if(appTimer[DHT11_TASK].flag){
        DHT11_read_task();
    }
}	

