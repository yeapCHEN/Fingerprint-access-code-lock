#include "sys.h"
#include "delay.h"
#include "usart1.h"
#include "lcd1602.h"
#include "gpio.h"
#include "as608.h"
#include "string.h"
#include "timer.h"
#include "stmflash.h"
#include "rc522_config.h"
#include "rc522_function.h"

#define FLASH_SAVE_ADDR   0X0801F000 				//����FLASH �����ַ(����Ϊż��)

#define MAX_PEOPLE   5 //���洢5��IC��
#define SIZE 10

#define uchar unsigned char
#define uint  unsigned int

//�����ʶ
unsigned char FlagKeyPress = 0;  //�м����±�־�����������0
unsigned char keycode;    //����ֵ��ȡ����
unsigned char operation=0;		//������־λ
unsigned char pass=0;			//������ȷ��־
unsigned char ICpass=0;			//IC��ȷ��־

unsigned char CurrentPassword[6]={0,0,0,0,0,0}; 														//��ǰ����ֵ
unsigned char InputData[6]={0,0,0,0,0,0};			 //���������ݴ���
unsigned char initpassword[6]={0,0,0,0,0,0};
unsigned char TempPassword[6];		//�������뻺����
unsigned char PressNum=0;			   //��������λ������
unsigned char CorrectCont;			//��ȷ�������
unsigned char ReInputEn=0;		 //������������־	
unsigned char ReInputCont; 			//�����������
unsigned char ErrorCont;			//�����������

unsigned char RELAY_TIME;	//�̵�������ʱ��
unsigned char InitDisplay=1;//������ҳ���־

unsigned char Register = 0;//ע�Ῠ��־
unsigned char Delete = 0;//ɾ������־
unsigned char RegFingerprint = 0;//ע��ָ�Ʊ�־

unsigned short user_id = 0;//�û�ָ��ID

u8 ID_BUF[8],ID_TEMP_Buffer[10]; //ID_TEMP_Bufferע����Ŀ���
u8 ucArray_ID [ 4 ] ;//���IC����

//////////////////////////////////////////////////////////////////////////////////	 
//��������					  
////////////////////////////////////////////////////////////////////////////////// 
void WRITE_DATA_TO_FLASH(u8* ID_Buffer,u8 LEN);//����д��STM32�ڲ�FLASH
void READ_DATA_FOR_FLASH(u8* ID_TEMP_Buffer ,u8 LEN);//��STM32�ڲ�FLASH��������
void CHECK_NEW_MCU(void);
u8 RC522_SCAN(u8* BUF);//ɨ��IC��
void COMPER_ID_MODE(void);//��������ˢ������
void ADD_ID_MODE(void);//ע��IC����
void DEL_ID_MODE(void);//ɾ��IC������


void DataInit()
{
	  unsigned char i=0;				//��������λ������
	  PressNum=0;				//��������λ������
		for(i=0;i<6;i++)InputData[i]=0;    //�����ݴ�������
}

void BuzzerRingsNum(u8 num)//����������
{
		unsigned char i=0;
	  for(i=0; i<num; i++)
		{
				BEEP = 1;
			  delay_ms(100);
			  BEEP = 0;
			  if(num!=1)
				delay_ms(50);
		}
}

void display_user_id()//��ʾָ��ID
{
    LCD_Write_Com(0xc0);
    LCD_Write_Data('U');
    LCD_Write_Data('s');
    LCD_Write_Data('e');
    LCD_Write_Data('r');
    LCD_Write_Data('s');
    LCD_Write_Data(' ');
    LCD_Write_Data('i');
    LCD_Write_Data('d');
	  LCD_Write_Data(user_id+0x30);
    LCD_Write_Data(' ');
    LCD_Write_Data('r');
    LCD_Write_Data('e');
    LCD_Write_Data('g');
    LCD_Write_Data(' ');
	  LCD_Write_Data(' ');
    LCD_Write_Data(' ');
    LCD_Write_Data(' ');
}

//========================��������==================================================
//==================================================================================
void ResetPassword(void)
{
	unsigned char i=0;
	
	if(pass==0)//�������
	{
			BuzzerRingsNum(3);//��������3��
			LCD_Write_String(0,1,"      error     "); 
			delay_ms(1000);
			LCD_Write_String(0,1,"password:       ");
			LCD_Write_Com(0x80+0x40+9);
			LCD_Write_Com(0x0F);//�����˸
		  DataInit();  //���������ݼ���������
	}
	else
	{
    	if(ReInputEn==1)
			{
					if(PressNum==6)// ���������Ϊ6
					{
							ReInputCont++;				
							if(ReInputCont==2)
							{
                if((TempPassword[0]==InputData[0])&&(TempPassword[1]==InputData[1])&&(TempPassword[2]==InputData[2])&&  
									 (TempPassword[3]==InputData[3])&&(TempPassword[4]==InputData[4])&&(TempPassword[5]==InputData[5]))  //��������������������Ա�
										{
												LCD_Write_String(0,1,"ResetPasswordOK ");	
												BuzzerRingsNum(2);
												WRITE_DATA_TO_FLASH(TempPassword,6);  				// ��������д��STM32�ڲ�FLASH
												delay_ms(100);
												READ_DATA_FOR_FLASH(CurrentPassword,6); 			//��STM32�ڲ�FLASH������洢����
												delay_ms(1000);
												LCD_Write_String(0,1,"password:       ");
												LCD_Write_Com(0x80+0x40+9);
												LCD_Write_Com(0x0F);//�����˸
										}
										else//������������벻һ��
										{
												BuzzerRingsNum(3);//��������3��
												LCD_Write_String(0,1,"      error     "); 
												delay_ms(1000);
												LCD_Write_String(0,1,"password:       ");
												LCD_Write_Com(0x80+0x40+9);
												LCD_Write_Com(0x0F);//�����˸
										}
										ReInputEn=0;			//�ر����ù���
										ReInputCont=0;
										CorrectCont=0;
							}
							else
							{
								BuzzerRingsNum(1);
								LCD_Write_String(0,1,"input again     ");		//��ʾ���ٴ�����	
								for(i=0;i<6;i++)
								{
									TempPassword[i]=InputData[i];		//����һ������������ݴ�����						
								}
							}
							DataInit();  //���������ݼ���������
					 }
				}
	}

}

//=======================ȡ�����в���============================================
void Cancel(void)
{	
		unsigned char i;
	
		LCD_Write_String(0,1,"password:       ");
		BuzzerRingsNum(2);				//��ʾ��,������
		for(i=0;i<6;i++)InputData[i]=0;    //�����ݴ�������
		RELAY=0;					//�ر���
		BEEP =0;					//������
		pass=0;					//������ȷ��־����
		ReInputEn=0;			//������������־����
	  ErrorCont=0;			//������������������
		CorrectCont=0;			//������ȷ�����������
		ReInputCont=0;			//������������������� 
		PressNum=0;					//����λ������������
	  ICpass = 0;
	  InitDisplay=1;
}


//==========================ȷ�ϼ�����ͨ����Ӧ��־λִ����Ӧ����===============================
void Ensure(void)
{
	  if(PressNum == 6)//��������6λ���������Ч
		{
			  if((InputData[0]==2)&&(InputData[1]==0)&&(InputData[2]==1)&&(InputData[3]==3)&&(InputData[4]==0)&&(InputData[5]==6))//����������ʱ������201306���������ʼ��
				{
						WRITE_DATA_TO_FLASH(initpassword,6);  				// ǿ�ƽ���ʼ����д��STM32�ڲ��洢
					  delay_ms(100);
					  READ_DATA_FOR_FLASH(CurrentPassword,6); 			//��STM32�ڲ�FLASH������洢����
					  LCD_Write_String(0,1,"Init password...");
					  BEEP = 1;
					  delay_ms(1000);
					  BEEP = 0;
						LCD_Write_String(0,1,"password:       ");
						LCD_Write_Com(0x80+0x40+9);
						LCD_Write_Com(0x0F);//�����˸
				}
				else if((InputData[0]==2)&&(InputData[1]==0)&&(InputData[2]==0)&&(InputData[3]==1)&&(InputData[4]==3)&&(InputData[5]==6))//����200136ָ�����
				{
					  if (FINGERPRINT_Cmd_Delete_All_Model() == 0)
						{
								LCD_Write_String(0,1," clear finger ok");
								BuzzerRingsNum(2);				//��ʾ��,������
						}
						user_id = 0;
						delay_ms(1000);
						InitDisplay = 1;
				}
				else
				{
						if((InputData[0]==CurrentPassword[0])&&(InputData[1]==CurrentPassword[1])&&(InputData[2]==CurrentPassword[2])&&  
							 (InputData[3]==CurrentPassword[3])&&(InputData[4]==CurrentPassword[4])&&(InputData[5]==CurrentPassword[5]))  //����ƥ����ȷ
						{ 
							  CorrectCont++;
							  if(CorrectCont==1)		//��ȷ�����������ֻ��һ����ȷ����ʱ��������
								{
										LCD_Write_String(0,1,"      open      ");  //��ʾ����
										RELAY = 1; 					//�̵�������
										RELAY_TIME = 15; 		//�̵�������15��
										pass = 1;						//������ȷ��־
										BuzzerRingsNum(2);				//��ʾ��,������
								}
								else              	//��������ȷ����ʱ�������������빦��
								{
										LCD_Write_String(0,1,"SetNewWordEnable");
									  BuzzerRingsNum(2);				//��ʾ��,������
									  ReInputEn=1;									//����������������
										CorrectCont=0;									//��ȷ����������
								}
						}
						else
						{
							  ErrorCont++;
							  LCD_Write_String(0,1,"      error     "); //��ʾ����
							  if(ErrorCont==3)//�����������������ʱ������
								{
									   do
										{
											  LCD_Write_String(0,1,"Keyboard Locked!");
											  RELAY=0;					//�ر���
												BEEP = !BEEP;
												delay_ms(55);
										}while(1);
								}
								else
								{
									  /////////////////////�������һ�Σ�����������һ��
									  BEEP=1;
										delay_ms(1000);
									  BEEP=0;
										LCD_Write_String(0,1,"password:       ");
										LCD_Write_Com(0x80+0x40+9);
										LCD_Write_Com(0x0F);//�����˸
								}
						}
				}
				DataInit();  //���������ݼ��������㣬Ϊ��һ��������׼��
		}
}


//������Ӧ���򣬲����Ǽ�ֵ
//���ؼ�ֵ��
//         7       8      9      10(A)//ע��IC��
//         4       5      6      11(B)//ɾ��IC��
//         1       2      3      12(C)//ע��ָ��
//        14(����) 0   15(ȷ��)  13(D)//�޸�����

void KeyPress(uchar keycode)
{
	  unsigned char i;
	
    switch (keycode)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        if(Register==0&&Delete==0&&RegFingerprint==0)
				{
						if(PressNum < 6)                   					//���������������6λʱ���������벢���棬����6λʱ����Ч��
						{  
							LCD_Write_String(0,1,"input:          ");//��ʾ����
							BuzzerRingsNum(1);								//������ʾ��						
							for(i=0; i<=PressNum; i++)
							{
								LCD_Write_Com(0x80+0x46+i);
								LCD_Write_Data('*');			//��ʾ*��
							}
							InputData[PressNum]=keycode;		//�洢����
							PressNum++;
						}
						else										//��������λ������6�󣬺�������
						{
							PressNum=6;
						}
				}
				break;
    case 10:   //A��
			  if(RegFingerprint==0)
				{
						Register = !Register;
						Delete = 0;
						LCD_Write_Com(0x0C);
						if(Register==1)
						{
								LCD_Write_String(0,0,"  Add IC MODE   "); 
						}
						else InitDisplay = 1;
				}
        break;
    case 11:	//B��
			  if(RegFingerprint==0)
				{
						Delete = !Delete;
						Register = 0;
						LCD_Write_Com(0x0C);
						if(Delete==1)
						{
								LCD_Write_String(0,0," Delete IC MODE   "); 
						}
						else InitDisplay = 1;
			  }
        break;
		case 12:	//C��
        if(Register==0
					&&Delete==0)
				{
							RegFingerprint=!RegFingerprint;
							if(RegFingerprint==1)
							{
									display_user_id();
							}
							else InitDisplay = 1;
				}
        break;
    case 13:   //D��
			  if(Register==0
					&&Delete==0&&
				RegFingerprint==0)
				{
						ResetPassword();
				}
        break;
		case 14:   //*��
			  if(Register==0
					&&Delete==0&&
				RegFingerprint==0)
				{
						Cancel();
				}					
        break;
    case 15:   //#��
			  if(Register==0
					&&Delete==0&&
				RegFingerprint==0)
				{
						Ensure();
				}
        break;

    }
}

void  finger_ctrl(void)
{
    u8 ret;
    if(PS_Sta == 1)//��Ӧ������ָ����ȥ
		{
				if (RegFingerprint == 1)//ָ��¼��
				{
						ret = AS608_Add_Fingerprint(user_id);//ָ��������û�
						if (ret== 0)
						{
								user_id ++;
								if (user_id > 9)//���¼9��ָ��
										user_id = 0;

								display_user_id();//��ʾ
								BuzzerRingsNum(1);//��������
						}
				}
				if (RegFingerprint ==0)
				{
						unsigned short ret_userid = 0;

						ret_userid = AS608_Find_Fingerprint();//ɨ��ָ��
						if (ret_userid != 0xFFFE)//˵��ʶ��ָ��
						{
								LCD_Write_String(0,1,"      open      ");  //��ʾ����
								RELAY = 1; 					//�̵�������
								RELAY_TIME = 15; 		//�̵�������15��
								pass = 1;						//������ȷ��־
								BuzzerRingsNum(2);				//��ʾ��,������
						}
				}
		}
}

int main(void)
{	
		delay_init();	    //��ʱ������ʼ��	  
	  NVIC_Configuration();
		KEY_GPIO_Init();    //������ʼ��
		BEEP_AND_RELAY_GPIO_Init();//�������̵�����ʼ��
		delay_ms(500);       //�ϵ�˲�����һ����ʱ�ڳ�ʼ��
	  DataInit();
		LCD_Init();         //��Ļ��ʼ��
	  LCD_Write_String(0,0,"   Loading...   ");
	  LCD_Write_String(0,1,"                ");
	  CHECK_NEW_MCU();//��Ƭ��У��
	  PS_StaGPIO_Init();
	  RC522_Init();
		PcdReset (); //��λRC522 
		M500PcdConfigISOType ( 'A' );//���ù�����ʽ	
	  uart1_Init(57600);   //���ڳ�ʼ��
	  TIM2_Init(999,719);    //��100Hz��������ʱ10ms
		//Tout = ((arr+1)*(psc+1))/Tclk ;  
		//Tclk:��ʱ������Ƶ��(��λMHZ)
		//Tout:��ʱ�����ʱ��(��λus)
		while(1)
		{  
          if(InitDisplay==1)
					{
							InitDisplay = 0;
						  BEEP = 0;
						  DataInit();
						  CorrectCont=0;									//��ȷ����������
						  RELAY = 0; //�̵����ر�
				    	LCD_Write_String(0,0,"===Coded Lock===");
						  LCD_Write_String(0,1,"password:       ");
							LCD_Write_Com(0x80+0x40+9);
							LCD_Write_Com(0x0F);//�����˸
					}	
					finger_ctrl();//ָ�ƴ�����
					if(Delete == 0&&Register==0)
					COMPER_ID_MODE();	
					ADD_ID_MODE();	
					DEL_ID_MODE();					
					//��ȡ����
					keycode = KEYSCAN();
					//��Ч��ֵ0-15
					if ((keycode<16)&&(FlagKeyPress==0))
					{
							FlagKeyPress = 1;
							KeyPress(keycode);
							FlagKeyPress = 0;
					}
					delay_ms(10);
		}
		
}

void WRITE_IC_NUM_TO_FLASH(u8* ID_Buffer,u8 space)//IC����д��STM32�ڲ�FLASH
{
		  STMFLASH_Write(FLASH_SAVE_ADDR + 0x10 * space,(u16*)ID_Buffer,SIZE); 
      delay_ms(100);                             //�ڲ�FLASH������������ֹ�����������Ь
}

void READ_IC_NUM_FOR_FLASH(u8* ID_TEMP_Buffer ,u8 space)//��STM32�ڲ�FLASH����IC����
{
		 STMFLASH_Read(FLASH_SAVE_ADDR + 0x10 * space,(u16*)ID_TEMP_Buffer,SIZE);
}
 
void WRITE_DATA_TO_FLASH(u8* ID_Buffer,u8 LEN)//����д��STM32�ڲ�FLASH
{
		STMFLASH_Write(FLASH_SAVE_ADDR + 0x10*MAX_PEOPLE*2,(u16*)ID_Buffer,LEN); 
    delay_ms(100);                             //�ڲ�FLASH������������ֹ�����������д
}

void READ_DATA_FOR_FLASH(u8* ID_TEMP_Buffer ,u8 LEN)//��STM32�ڲ�FLASH��������
{
		STMFLASH_Read(FLASH_SAVE_ADDR + 0x10*MAX_PEOPLE*2,(u16*)ID_TEMP_Buffer,LEN);
}

void CHECK_NEW_MCU(void)  // ����Ƿ����µĵ�Ƭ�����ǵĻ���մ洢����������
{
	  u8 comper_str[6],i=0;
    u8 clear_str[10] = {0};
		memset(clear_str,'0',sizeof(clear_str));//��clear_str�������ȫ���� ��0��
	  STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)comper_str,5); //��0X0801F010�����ַ�������� 
	  comper_str[5] = '\0';
	  if(strstr(comper_str,"FDYDZ") == NULL)  //�µĵ�Ƭ��
		{
			 WRITE_DATA_TO_FLASH(initpassword,6); //��Ƭ����һ��ʹ��ʱ������Ϊ6��0
			 for(i = 0; i < MAX_PEOPLE; i++)//�Ѵ濨�ڲ�����ȫ������
			 {
					WRITE_IC_NUM_TO_FLASH(clear_str,i);//��IC�ĵ�ַ����ȫ������
			 }
			 STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)"FDYDZ",5); //д�롰FDYDZ���������´�У��
	  }
		READ_DATA_FOR_FLASH(CurrentPassword,6); 					//��STM32�ڲ�FLASH������洢����
}

u8 RC522_SCAN(u8* BUF)//ɨ��IC��
{
	   if(Delete == 1||Register==1)
		 {
				 static u8 i = 0;
				 switch(i)
				 {
					 case 0:    LCD_Write_String(0,1,"      ()        "); break;
					 case 1:    LCD_Write_String(0,1,"     (())       "); break;
					 case 2:    LCD_Write_String(0,1,"    ((()))      "); break;
					 case 3:    LCD_Write_String(0,1,"   (((())))     "); break;
					 case 4:    LCD_Write_String(0,1,"  ((((()))))    "); break;
					 case 5:    LCD_Write_String(0,1,"                "); break;
					 default :  i = 0; break;
				 }
				 i++;
			}
		 if((GET_PID(ucArray_ID) == MI_OK))//��ȡRC522����
		 {
			sprintf(BUF,"%02X%02X%02X%02X", ucArray_ID [ 0 ], ucArray_ID [ 1 ], ucArray_ID [ 2 ], ucArray_ID [ 3 ] );//��IC�������ַ�������ʽ������BUF��
			return 1;
		 }
		
		 
		 
		 
		 
		 
		 
		return 0; 
}


void COMPER_ID_MODE(void)//��������ˢ������
{
		 if(RC522_SCAN(ID_BUF)) //��⵽�п�ˢ��
		 {  
			 u8 i = 0;
			 ICpass = 0;
			 LCD_Write_Com(0x0C);//�رչ��
			 LCD_Write_String(0,1," IC:             ");
			 LCD_Write_String(4,1,ID_BUF);//��ʾ����
			 BEEP = 1; //��������
			 delay_ms(500);
			 BEEP = 0;
       			 
			 for(i = 0; i < MAX_PEOPLE; i++)//��5��IC����ɨ��
			 {
					READ_IC_NUM_FOR_FLASH(ID_TEMP_Buffer,i);//��ȡSTM32�ڲ�FLASH�洢�Ŀ���
					if(strstr(ID_TEMP_Buffer,ID_BUF) != NULL ) //����ƥ����ȷ
					{
						 LCD_Write_String(0,1,"      open      ");  //��ʾ����
						 RELAY = 1; 					//�̵�������
						 RELAY_TIME = 15; 		//�̵�������15��
						 ICpass = 1;						//IC��ȷ��־						
						 break;  
					}
			 }
			 if(ICpass == 0)
			 {
				  InitDisplay = 1;
			 }
		 }
}

void ADD_ID_MODE(void)//ע��IC����
{
	 if(Register == 1)
	 { 
	     if(RC522_SCAN(ID_BUF)) //��⵽�п�ˢ��
			 {
				 u8 i = 0;
         Register = 0;				 
				 LCD_Write_String(0,1," IC:             ");
			   LCD_Write_String(4,1,ID_BUF);//��ʾ����
				 BEEP = 1; //��������
				 delay_ms(500);
         BEEP = 0; 
			   for(i = 0; i < MAX_PEOPLE; i++)//��5��IC����ɨ��
				 {
				    READ_IC_NUM_FOR_FLASH(ID_TEMP_Buffer,i);//��ȡSTM32�ڲ�FLASH�洢�Ŀ���
					  if(strstr(ID_TEMP_Buffer,ID_BUF) != NULL)//��⵽��ͬID
							break;
					  if(ID_TEMP_Buffer[8] == '0' )              //ID_TEMP_Buffer��8λ��0��˵����û�д洢���� XXXXXXXXD
				      break;  
				 }
				 delay_ms(1000);
				 if(i == MAX_PEOPLE)    LCD_Write_String(0,1,"   memery full  "); //IC��������5�ţ��������5��˵��֮ǰ���5�ſ��ˡ����ܼ���ע��IC
				 else
				 {
				     sprintf(ID_TEMP_Buffer,"%s%d",ID_BUF,1);//��IC���ſ�����ID_TEMP_Buffer�У���8λ�洢1��˵�����ű��洢
				     WRITE_IC_NUM_TO_FLASH(ID_TEMP_Buffer,i);//�洢��STM32�ڲ�FLASH
				     LCD_Write_String(0,1,"   save ok!  ");//��ʾ�洢�ɹ�
				 }
				 delay_ms(1000); 
         InitDisplay = 1;				 
			   }
		}
}

void DEL_ID_MODE(void)//ɾ��IC������
{
	 if(Delete == 1)
	 {
	     if(RC522_SCAN(ID_BUF)) //��⵽�п�ˢ��
			 {  
				 u8 i = 0;
				 Delete = 0;
				 LCD_Write_String(0,1," IC:             ");
			   LCD_Write_String(4,1,ID_BUF);//��ʾ����
				 BEEP = 1; //��������
				 delay_ms(500);
         BEEP = 0; 
			   for(i = 0; i < MAX_PEOPLE; i++)//��5��IC����ɨ��
				 {
				    READ_IC_NUM_FOR_FLASH(ID_TEMP_Buffer,i);//��ȡSTM32�ڲ�FLASH�洢�Ŀ���
					  if(strstr(ID_TEMP_Buffer,ID_BUF) != NULL)              //�ҵ�IC����
				      break;  
				 }
				 delay_ms(1000);
				 if(i == MAX_PEOPLE)    LCD_Write_String(0,1,"   find fail  "); //ɨ��5�ζ�û��ɨ�赽Ҫɾ���Ŀ���
				 else
				 {
						sprintf(ID_TEMP_Buffer,"%02X%02X%02X%02X%d",0,0,0,0,0);//ID_TEMP_Buffer����ȫ��д��0
						WRITE_IC_NUM_TO_FLASH(ID_TEMP_Buffer,i);//�洢��STM32�ڲ�FLASH
					  LCD_Write_String(0,1,"   Delete ok!     ");//��ʾ����ɹ�
				 }
				 delay_ms(1000);  
				 InitDisplay = 1;	
			 }
		}
}


