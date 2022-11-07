#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "gpio.h"  

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��2
void TIM2_Init(u16 arr,u16 psc)
{	 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ��TIM2ʱ��
	
	//��ʼ����ʱ��2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  

	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//��������ж� ,
	
  TIM_Cmd(TIM2,ENABLE ); 	//ʹ�ܶ�ʱ��2
}

extern u8 RELAY_TIME; 
extern u8 InitDisplay; 
extern u8 pass;
extern u8 ICpass;
extern u8 ReInputEn;

void TIM2_IRQHandler(void)//��ʱ��2�жϷ������	 
{ 
	  static u8 time_count = 0;
	
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{ 
			    if(time_count ++ >= 100)//1miao
					{
					      time_count = 0;
						  
                if(RELAY_TIME )  RELAY_TIME--;//�̵�������ʱ�䵹��ʱ
						    else//�̵�������ʱ���ʱ��0
								{
										if(pass==1 || ICpass==1)
										{
											  if(ReInputEn==0)
												{
													pass = 0;ICpass=0;
													InitDisplay = 1;//������ҳ���־��1
												}
										}
								}
					}
					TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //����жϱ�־λ
	  }
}

