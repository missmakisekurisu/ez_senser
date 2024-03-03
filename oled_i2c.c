#include "oled_i2c.h"
#if(USE_REAL_I2C)
static void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;


    OLED_I2C_APBxClock_FUN ( OLED_I2C_CLK, ENABLE );
    OLED_I2C_GPIO_APBxClock_FUN ( OLED_I2C_GPIO_CLK, ENABLE );

    GPIO_InitStructure.GPIO_Pin = OLED_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;        
    GPIO_Init(OLED_I2C_SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = OLED_I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;        
    GPIO_Init(OLED_I2C_SDA_PORT, &GPIO_InitStructure);
}


static void I2C_Mode_Configu(void)
{
    I2C_InitTypeDef  I2C_InitStructure;


    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;


    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;

    I2C_InitStructure.I2C_OwnAddress1 =I2Cx_OWN_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;


I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;


    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;


    I2C_Init(OLED_I2Cx, &I2C_InitStructure);

    I2C_Cmd(OLED_I2Cx, ENABLE);
}

void oled_i2c_init(void){
    I2C_GPIO_Config();
    I2C_Mode_Configu();
}

void HAL_I2C_Mem_Write(uint8_t DevAddress, uint8_t MemAddress,uint8_t *pData, uint16_t Size){
    I2C_GenerateSTART(OLED_I2Cx,ENABLE);
    while(SUCCESS != I2C_CheckEvent(OLED_I2Cx,I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(OLED_I2Cx, DevAddress, I2C_Direction_Transmitter);
    while(SUCCESS != I2C_CheckEvent(OLED_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(OLED_I2Cx, MemAddress);
    while(SUCCESS != I2C_CheckEvent(OLED_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

    for(uint8_t i = 0; i< Size; i++){      
        I2C_SendData(OLED_I2Cx, pData[i]);
        while(SUCCESS != I2C_CheckEvent(OLED_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
    } 
    I2C_GenerateSTOP(OLED_I2Cx,ENABLE);
}

#else
#define SET_SCL       GPIO_SetBits(SI2C_PORT, SI2C_SCL_PIN)
#define RESET_SCL     GPIO_ResetBits(SI2C_PORT, SI2C_SCL_PIN)
#define SET_SDA       GPIO_SetBits(SI2C_PORT, SI2C_SDA_PIN)
#define RESET_SDA     GPIO_ResetBits(SI2C_PORT, SI2C_SDA_PIN)

void sim_i2c_gpio_init_transmit(void)
{   
    SI2C_GPIO_APBxClock_FUN(SI2C_GPIO_SCL, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = SI2C_SCL_PIN | SI2C_SDA_PIN,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Mode = GPIO_Mode_Out_PP,
    };
	GPIO_Init(SI2C_PORT, &GPIO_InitStructure);    
}

//used after transmit init
void sim_i2c_gpio_init_receive(void)
{   
    SI2C_GPIO_APBxClock_FUN(SI2C_GPIO_SCL, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = SI2C_SDA_PIN,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Mode = GPIO_Mode_IPU,
    };
	GPIO_Init(SI2C_PORT, &GPIO_InitStructure);
}


#define SI2C_GENERATE_START {SET_SCL;SET_SDA;delay_us(4U);RESET_SDA;RESET_SCL;}
#define SI2C_GENERATE_STOP  {SET_SCL;SET_SDA;delay_us(4U);}
#define SI2C_IGNORE_ACK     {SET_SCL;delay_us(4U);RESET_SCL;delay_us(4U);}


static void sim_i2c_send_8bit(uint8_t data){
    for(uint8_t i = 0; i < 8; i++){
        delay_us(4U);        
        if(data & 0x80){SET_SDA;}else{RESET_SDA;}
        data <<=1;
        //delay_us(1U);
        SET_SCL;
        delay_us(4U);
        RESET_SCL;
                       
    }
//    sim_i2c_gpio_init_receive();
    delay_us(4U);    
}

static void sim_i2c_transmit(uint8_t slaveAddr, uint8_t *data, uint8_t size){
    SI2C_GENERATE_START
    sim_i2c_send_8bit(slaveAddr);
    SI2C_IGNORE_ACK
    
//    while(RESET != GPIO_ReadInputDataBit(SI2C_PORT, SI2C_SDA_PIN));
//    SET_SCL;
//    delay_us(2U);
//    RESET_SCL;
//    delay_us(2U);
    for(uint8_t i = 0; i < size; i++){
        sim_i2c_send_8bit(data[i]);
    }
    SI2C_IGNORE_ACK   
    SI2C_GENERATE_STOP
}

static void ssd1306_wr_cmd(uint8_t cmd){
    sim_i2c_transmit(SSD1306_ADDR, 0x00, 1U);
    sim_i2c_transmit(SSD1306_ADDR, &cmd, 1U);
}

static void ssd1306_wr_data(uint8_t *data, uint8_t size){
    sim_i2c_transmit(SSD1306_ADDR, 0x40, 1U);
    sim_i2c_transmit(SSD1306_ADDR, data, size);
}

//static uint8_t CMD_Data[27]={
//0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x00, 0x10, 0x40, 0x8D,
//					
//0x14, 0x20, 0x02, 0xA1, 0xC0, 0xDA, 0x12, 0x81, 0xEF, 0xD9, 0xF1,
//					
//0xDB, 0x30, 0xA4, 0xA6, 0xAF};
static uint8_t CMD_Data[27]={
0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F,
					
0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA, 0x12,
					
0xD8, 0x30, 0x8D, 0x14, 0xAF};

void ssd1306_init(void){
    TIM_delay_ms(500U);
    for(uint8_t i = 0; i < 27U; i++){
        ssd1306_wr_cmd(CMD_Data[i]);
    } 
    
    ssd1306_wr_cmd(0xA5);
    uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		ssd1306_wr_cmd(0xb0+i);    //设置页地址（0~7）
		ssd1306_wr_cmd(0x00);      //设置显示位置—列低地址
		ssd1306_wr_cmd(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++){
            uint8_t bit =1U;
			ssd1306_wr_data(&bit, 1); 
        }
	} //更新显示
}
#endif
