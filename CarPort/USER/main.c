#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "rc522.h"
#include "my_rc522.h"
#include "string.h"
#include "control.h"
#include "key.h"
#include "pwm.h"

#define NULL 0
/*************************************
*
*RC522 <----->MINISTM32
*
*1--SDA <----->B0
*2--SCK <----->B13
*3--MOSI<----->B15
*4--MISO<----->B14
*5--����
*6--GND <----->GND
*7--RST <----->B1
*8--3.3V<----->3.3V
*
**************************************/
 
signed char thisCard = -1; // ˢ�Ŀ��ڽṹ���λ�� -1Ϊû�ҵ�
int balance = 500;  //���
char outGarageFlag = 0;

void AllInit(void);
void SwipeCard(void);
void Moving(void);

int main(void)
{	
	u8 t = 0;
	AllInit(); // ȫ���ʼ��
	while(1)
	{	
		SwipeCard();
		Moving();
		
		t = KEY_Scan(0);
		switch(t)
		{
			case KEY0_PRES:
				Motor_1_Clockwise_Mov();
				break;
			case KEY1_PRES:
				Motor_1_Anticlockwise_Mov();
				break;
			case KEY2_PRES:
				Motor_2_Clockwise_Mov();
				break;
			case KEY3_PRES:
				Motor_2_Anticlockwise_Mov();
				break;
			default:break;
		}
	}
}



void AllInit(void)
{
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ� 
	InitRc522(); 
	KEY_Init();
	TIM3_PWM_Init(14399,0); //??: PWM=72000/(899+1)=80Khz 
	Motor_1_Stop();
	Motor_2_Stop();
}

void SwipeCard(void)
{
	char status;	//������״̬
	char x;

	status = RfidSearchCard();
	if (status == MI_OK)
	{
		thisCard = -1; // ��ձ�־
		
		for(x = 0; x < 9; x++) // �Ƿ��д洢�˵Ŀ�
		{
			if(g_GarageSpace[x].m_ID[0]==g_SN[0] && g_GarageSpace[x].m_ID[1]==g_SN[1]
			&& g_GarageSpace[x].m_ID[2]==g_SN[2] && g_GarageSpace[x].m_ID[3]==g_SN[3])
			{
				thisCard = x;
				UART_Send_Str("page page3");
				UART_Send_END();
	
				UART_Send_Str("va0.val=2");  // 2Ϊ��ʾ"����ȡ��"
				UART_Send_END();

				outGarageFlag = 1;
				break;
			}
		}

		if(thisCard == -1) //�޼�¼�Ŀ�
		{
			for (x = 0; x < 9; x++)  //�Ƿ��п�λ
			{
				if(g_GarageSpace[x].m_ID[0]==NULL && g_GarageSpace[x].m_ID[1]==NULL
					&& g_GarageSpace[x].m_ID[2]==NULL && g_GarageSpace[x].m_ID[3]==NULL)
				{
					thisCard = x;
					UART_Send_Str("page page1"); //��ת��ҳ��1
					UART_Send_END();

					RfidProcess(SEND_ID);

					//g_writeData[1] = balance%100;  //�������д洢 ��һ���ֽڴ��λ��ʮλ���ڶ����ֽڰ�λ��ǧλ
					//g_writeData[0] = balance/100;
					//RfidProcess(WRITE_CARD);
					RfidProcess(READ_CARD);  //ֻ��ǰ�����ֽ�
					break;
				}
			}

			if (thisCard == -1) //��λ����
			{
				UART_Send_Str("page page4");
				UART_Send_END();
			}
		}
	}
}


void Moving(void)
{
	u8 RX_len;	//������Ϣ����
	char x; // ѭ������
	static u8 s_inGarageFlag = 0;  //�泵������������Ϊ1
	
  char y;
	unsigned char num[] = "0123456789abcdef";
	
	//������յ�fe 01 fc ����� ����fe��ͷ fc��β ͨ��Э�飩
	if(USART_RX_STA1&0x8000)
	{					   
		RX_len=USART_RX_STA1&0x3fff;//�õ��˴ν��յ������ݳ���
		for(x=0;x<RX_len;x++)
		{
			if(USART_RX_BUF[0] == 1)
			{
				s_inGarageFlag = 1;
			}
		}
		USART_RX_STA1=0;  //��ձ�־λ������ٴν�������
	}
	
	if(s_inGarageFlag == 1) // ��ʼͣ��
	{
		UART_Send_Str("page page3");
		UART_Send_END();
		UART_Send_Str("va0.val=1");  // 1Ϊ����ͣ��
		UART_Send_END();
		
		s_inGarageFlag = 0;
		InGarageData(); // ���뿨��
		while(InGarageDriving()){}
		
		
		UART_Send_Str("page page0");
		UART_Send_END();
		
		
//		for(y = 0; y < 9; y++)
//		{
//			for (x = 0; x < 4; x++)
//			{
//				USART1->DR = num[g_GarageSpace[y].m_ID[x]/16]; //��16����ת �ַ�������
//				while((USART1->SR&0X40)==0); //�ȴ����ͽ���
//				USART1->DR = num[g_GarageSpace[y].m_ID[x]%16];
//				while((USART1->SR&0X40)==0); //�ȴ����ͽ���

//				if(x != 3)
//				{
//					USART1->DR = 32; //���Ϳո�
//					while((USART1->SR&0X40)==0); //�ȴ����ͽ���
//				}
//			}
//			USART1->DR = 13; //����ת��
//			while((USART1->SR&0X40)==0); //�ȴ����ͽ���
//		}
		
		
	}
	else if (outGarageFlag == 1) // ��ʼȡ��
	{
		outGarageFlag = 0;
		OutGarageData(); // ȥ������
		
		while(OutGarageDriving()){}
		
		UART_Send_Str("page page0");
		UART_Send_END();
		
		
//		for(y = 0; y < 9; y++)
//		{
//			for (x = 0; x < 4; x++)
//			{
//				USART1->DR = num[g_GarageSpace[y].m_ID[x]/16]; //��16����ת �ַ�������
//				while((USART1->SR&0X40)==0); //�ȴ����ͽ���
//				USART1->DR = num[g_GarageSpace[y].m_ID[x]%16];
//				while((USART1->SR&0X40)==0); //�ȴ����ͽ���

//				if(x != 3)
//				{
//					USART1->DR = 32; //���Ϳո�
//					while((USART1->SR&0X40)==0); //�ȴ����ͽ���
//				}
//			}
//			USART1->DR = 13; //����ת��
//			while((USART1->SR&0X40)==0); //�ȴ����ͽ���
//		}
	}
}


