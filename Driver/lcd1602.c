#include "lcd1602.h"
#include "delay.h"
#include "sys.h"

u8 Dao_xu(u8 data)//������
{
    u8 i = 0 ,temp = 0;;
	  
	  for(i = 0; i < 8; i++)
	 {
	   temp += (((data >> i) & 0x01) << (7 - i));
	 }
	 return temp;
}


/*------------------------------------------------
              д�������
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
{                              
  LCD1602_RS=0;	 //����
	delay_us(1);
  LCD1602_RW=0;
  delay_us(1);
  LCD1602_EN=1;                  
	delay_us(1);
  GPIO_Write(GPIOA,(GPIO_ReadOutputData(GPIOA)&0XFF00)+Dao_xu(com)); //�ڲ�Ӱ��A8-A15���ŵ�ǰ���£�������д��A0-A7����     
	delay_us(100);
  LCD1602_EN=0;
}

/*------------------------------------------------
              д�����ݺ���
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
{                                  
  LCD1602_RS=1;   //����              
	delay_us(1);
  LCD1602_RW=0;
  delay_us(1);	
  LCD1602_EN=1;                  
	delay_us(1);
  GPIO_Write(GPIOA,(GPIO_ReadOutputData(GPIOA)&0XFF00)+Dao_xu(Data)); //�ڲ�Ӱ��A8-A15���ŵ�ǰ���£�������д��A0-A7����   
	delay_us(100);
  LCD1602_EN=0;
}
/*------------------------------------------------
              д���ַ�������
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);  //��һ��   
 	}
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);  //�ڶ���   
 	}        
	while (*s) //�ж��Ƿ��⵽��β�� 
 	{     
		 LCD_Write_Data( *s);//��ʾ�ַ�     
		 s ++;  //ָ���1   
 	}
 }
/*------------------------------------------------
              д���ַ�����
------------------------------------------------*/
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
 {     
 if (y == 0) 
 	{     
 	LCD_Write_Com(0x80 + x);    //��һ��    
 	}    
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);  //�ڶ���     
 	}        
 LCD_Write_Data( Data); //��ʾ�ַ�      
 }
 

 /*------------------------------------------------
                ��������
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 delay_ms(5);
 }

/*------------------------------------------------*/
 void LCD_Init(void) 
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);  //����GPIOA GPIOB GPIOCʱ��
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
    GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����ٶ�50MHZ
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��GPIOA
	
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1|GPIO_Pin_10;  // LCD1602 RS-RW-EN
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //����ٶ�50MHZ
    GPIO_Init(GPIOB, &GPIO_InitStructure);   //GPIOB
	
   LCD_Write_Com(0x38);
   delay_ms(5);  
	 LCD_Write_Com(0x38);
   delay_ms(5);  
	 LCD_Write_Com(0x38);
   delay_ms(5);  
   LCD_Write_Com(0x08);    /*��ʾ�ر�*/
   delay_ms(5); 	
   LCD_Write_Com(0x01);    /*��ʾ����*/
   delay_ms(5); 	
   LCD_Write_Com(0x06);    /*��ʾ����ƶ�����*/ 
	 delay_ms(5); 
   LCD_Write_Com(0x0C);    /*��ʾ�����������*/
	 delay_ms(5); 
}

