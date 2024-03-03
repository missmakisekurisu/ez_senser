 #include "delay.h"
void delay_us(uint32_t delay_us)
{   
  volatile unsigned int num;
  volatile unsigned int t;


  for (num = 0; num < delay_us; num++)
  {
    t = 6;
    while (t != 0)
    {
      t--;
    }
  }
}

void delay_ms(uint16_t delay_ms)
{   
  volatile unsigned int num;
  for (num = 0; num < delay_ms; num++)
  {
    delay_us(1000);
  }
}



#define            BASIC_TIM                   TIM4
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM4
#define            BASIC_TIM_IRQ               TIM4_IRQn
#define            BASIC_TIM_IRQHandler        TIM4_IRQHandler


volatile static uint16_t SimpledelayTime = 0;
volatile APP_TIMER_TYPE appTimer[TIMER_COUNT]={
    {0, 3000U, 100, 0},
    {0, 2000u, 100, 0},
};
    


static void BASIC_TIM_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    // 开启定时器时钟,即内部时钟CK_INT=72M
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);

    // 自动重装载寄存器周的值(计数值)
    TIM_TimeBaseStructure.TIM_Period=1000;

    // 累计TIM_Period个频率后产生一个更新或者中断
    // 时钟预分频数为71，则驱动计数器的时钟CK_CNT = CK_INT / (71+1)=1M
    TIM_TimeBaseStructure.TIM_Prescaler= 71;

    // 时钟分频因子 ，基本定时器没有，不用管
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    // 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;

    // 重复计数器的值，基本定时器没有，不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;

    // 初始化定时器
    TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);

    // 清除计数器中断标志位
    TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);

    // 开启计数器中断
    TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);

    // 使能计数器
    TIM_Cmd(BASIC_TIM, ENABLE);

    // 暂时关闭定时器的时钟，等待使用
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, DISABLE);
}


// 中断优先级配置
static void BASIC_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQ ;
    // 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
static RCC_ClocksTypeDef RCC_Clocks;
void TIM_init(void){
    BASIC_TIM_Config();
    BASIC_TIM_NVIC_Config();
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);
    RCC_GetClocksFreq(&RCC_Clocks);
}

static void update_app_timer(APP_TIMER_TYPE *t){
    for(uint8_t i = 0; i < TIMER_COUNT; i++){
        if(t[i].timer++ >= t[i].timeCnt || t[i].flag){
            t[i].flag ++; 
            t[i].timer = 0;    
        }
        if(t[i].flag >= t[i].pendingTime){
            t[i].flag = 0;
        }
    }
}

void  BASIC_TIM_IRQHandler (void)
{
    if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) {
        SimpledelayTime++;
        update_app_timer(appTimer);
        TIM_ClearITPendingBit(BASIC_TIM , TIM_FLAG_Update);
    }
}


void TIM_delay_ms(uint16_t ms){
    for(;SimpledelayTime < ms;);    
    SimpledelayTime = 0u;
}
    
