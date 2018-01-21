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
*5--悬空
*6--GND <----->GND
*7--RST <----->B1
*8--3.3V<----->3.3V
*
**************************************/
 
signed char thisCard = -1; // 刷的卡在结构体的位置 -1为没找到
int balance = 500;  //余额
char outGarageFlag = 0;

void AllInit(void);
void SwipeCard(void);
void Moving(void);

int main(void)
{	
	u8 t = 0;
	AllInit(); // 全体初始化
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
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(9600);	 //串口初始化为9600
	LED_Init();		  	 //初始化与LED连接的硬件接口 
	InitRc522(); 
	KEY_Init();
	TIM3_PWM_Init(14399,0); //??: PWM=72000/(899+1)=80Khz 
	Motor_1_Stop();
	Motor_2_Stop();
}

void SwipeCard(void)
{
	char status;	//操作卡状态
	char x;

	status = RfidSearchCard();
	if (status == MI_OK)
	{
		thisCard = -1; // 清空标志
		
		for(x = 0; x < 9; x++) // 是否有存储了的卡
		{
			if(g_GarageSpace[x].m_ID[0]==g_SN[0] && g_GarageSpace[x].m_ID[1]==g_SN[1]
			&& g_GarageSpace[x].m_ID[2]==g_SN[2] && g_GarageSpace[x].m_ID[3]==g_SN[3])
			{
				thisCard = x;
				UART_Send_Str("page page3");
				UART_Send_END();
	
				UART_Send_Str("va0.val=2");  // 2为显示"正在取车"
				UART_Send_END();

				outGarageFlag = 1;
				break;
			}
		}

		if(thisCard == -1) //无记录的卡
		{
			for (x = 0; x < 9; x++)  //是否有空位
			{
				if(g_GarageSpace[x].m_ID[0]==NULL && g_GarageSpace[x].m_ID[1]==NULL
					&& g_GarageSpace[x].m_ID[2]==NULL && g_GarageSpace[x].m_ID[3]==NULL)
				{
					thisCard = x;
					UART_Send_Str("page page1"); //跳转到页面1
					UART_Send_END();

					RfidProcess(SEND_ID);

					//g_writeData[1] = balance%100;  //对余额进行存储 第一个字节存个位和十位，第二个字节百位和千位
					//g_writeData[0] = balance/100;
					//RfidProcess(WRITE_CARD);
					RfidProcess(READ_CARD);  //只读前两个字节
					break;
				}
			}

			if (thisCard == -1) //车位满了
			{
				UART_Send_Str("page page4");
				UART_Send_END();
			}
		}
	}
}


void Moving(void)
{
	u8 RX_len;	//接收信息长度
	char x; // 循环变量
	static u8 s_inGarageFlag = 0;  //存车动作可以启动为1
	
  char y;
	unsigned char num[] = "0123456789abcdef";
	
	//如果接收到fe 01 fc 就入库 （以fe开头 fc结尾 通信协议）
	if(USART_RX_STA1&0x8000)
	{					   
		RX_len=USART_RX_STA1&0x3fff;//得到此次接收到的数据长度
		for(x=0;x<RX_len;x++)
		{
			if(USART_RX_BUF[0] == 1)
			{
				s_inGarageFlag = 1;
			}
		}
		USART_RX_STA1=0;  //清空标志位后才能再次接收数据
	}
	
	if(s_inGarageFlag == 1) // 开始停车
	{
		UART_Send_Str("page page3");
		UART_Send_END();
		UART_Send_Str("va0.val=1");  // 1为正在停车
		UART_Send_END();
		
		s_inGarageFlag = 0;
		InGarageData(); // 存入卡号
		while(InGarageDriving()){}
		
		
		UART_Send_Str("page page0");
		UART_Send_END();
		
		
//		for(y = 0; y < 9; y++)
//		{
//			for (x = 0; x < 4; x++)
//			{
//				USART1->DR = num[g_GarageSpace[y].m_ID[x]/16]; //将16进制转 字符串发送
//				while((USART1->SR&0X40)==0); //等待发送结束
//				USART1->DR = num[g_GarageSpace[y].m_ID[x]%16];
//				while((USART1->SR&0X40)==0); //等待发送结束

//				if(x != 3)
//				{
//					USART1->DR = 32; //发送空格
//					while((USART1->SR&0X40)==0); //等待发送结束
//				}
//			}
//			USART1->DR = 13; //发送转行
//			while((USART1->SR&0X40)==0); //等待发送结束
//		}
		
		
	}
	else if (outGarageFlag == 1) // 开始取车
	{
		outGarageFlag = 0;
		OutGarageData(); // 去除卡号
		
		while(OutGarageDriving()){}
		
		UART_Send_Str("page page0");
		UART_Send_END();
		
		
//		for(y = 0; y < 9; y++)
//		{
//			for (x = 0; x < 4; x++)
//			{
//				USART1->DR = num[g_GarageSpace[y].m_ID[x]/16]; //将16进制转 字符串发送
//				while((USART1->SR&0X40)==0); //等待发送结束
//				USART1->DR = num[g_GarageSpace[y].m_ID[x]%16];
//				while((USART1->SR&0X40)==0); //等待发送结束

//				if(x != 3)
//				{
//					USART1->DR = 32; //发送空格
//					while((USART1->SR&0X40)==0); //等待发送结束
//				}
//			}
//			USART1->DR = 13; //发送转行
//			while((USART1->SR&0X40)==0); //等待发送结束
//		}
	}
}


