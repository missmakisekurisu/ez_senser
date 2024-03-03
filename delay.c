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

    // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);

    // �Զ���װ�ؼĴ����ܵ�ֵ(����ֵ)
    TIM_TimeBaseStructure.TIM_Period=1000;

    // �ۼ�TIM_Period��Ƶ�ʺ����һ�����»����ж�
    // ʱ��Ԥ��Ƶ��Ϊ71����������������ʱ��CK_CNT = CK_INT / (71+1)=1M
    TIM_TimeBaseStructure.TIM_Prescaler= 71;

    // ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    // ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;

    // �ظ���������ֵ��������ʱ��û�У����ù�
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;

    // ��ʼ����ʱ��
    TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);

    // ����������жϱ�־λ
    TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);

    // �����������ж�
    TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);

    // ʹ�ܼ�����
    TIM_Cmd(BASIC_TIM, ENABLE);

    // ��ʱ�رն�ʱ����ʱ�ӣ��ȴ�ʹ��
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, DISABLE);
}


// �ж����ȼ�����
static void BASIC_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    // �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQ ;
    // ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // ������ռ���ȼ�Ϊ3
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
    
