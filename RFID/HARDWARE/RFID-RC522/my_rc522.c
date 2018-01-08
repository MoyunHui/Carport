#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "rc522.h"
#include "my_rc522.h"
#include <string.h>

/*全局变量*/
u8 g_CT[2];//卡类型
u8 g_SN[4]; //卡号

u8 g_total=0;

u8 g_readData[16]; //从卡中提取
u8 g_writeData[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//写入卡中

// 初始密码 0xff,0xff,0xff,0xff,0xff,0xff
u8 g_password[6]={0xff,0xff,0xff,0xff,0xff,0xff}; //密码
u8 g_newPassword[6]={0,0,0,0,0,0}; //新密码


/******************************寻卡 防冲撞 选卡*******************************/
char RfidSearchCard(void)
{
	char status = MI_OK; //状态标志
	
	status = PcdReset();//复位RC522
	if(status!=MI_OK) {return status;}
	
	status = PcdRequest(PICC_REQIDL,g_CT);//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节
	if(status!=MI_OK) {return status;}
 
	status = PcdAnticoll(g_SN);//防冲撞，返回卡的序列号 4字节
	if(status!=MI_OK) {return status;}
	
	status = PcdSelect(g_SN);//选卡
	if(status!=MI_OK) {return status;}

	return status;
}


/******************************对卡操作*******************************/
char RfidProcess(enum __RfidCmd rfidCmd)
{
	char status = MI_OK; //状态标志
	u8 block;  //要访问的块
	u8 x; //循环变量
	unsigned char num[] = "0123456789abcdef";

	switch(rfidCmd)
	{
		//=========读卡内余额=======
	case READ_CARD: 
			rfidCmd = NO_CMD;

			block = 8;
			
			status = PcdAuthState(PICC_AUTHENT1A,block,g_password,g_SN);//验证A密匙
			if(status!=MI_OK) {return status;}
			
			status = PcdRead(block,g_readData);//从M1卡某一块读取到的数据存放在Read_Data[]数组中
			if(status!=MI_OK) {return status;}
			
			UART_Send_Str("t3.txt=\"");
			for (x = 0; x < 2; x++)
			{
				USART1->DR = num[g_readData[x]/10];  //发送个位或百位
				while((USART1->SR&0X40)==0); //等待发送结束
				USART1->DR = num[g_readData[x]%10];  //发送十位或千位
				while((USART1->SR&0X40)==0); //等待发送结束
				if(x != 1)
				{
					USART1->DR = 32;
					while((USART1->SR&0X40)==0); //等待发送结束
				}
			}
			UART_Send_Str("\"");
			UART_Send_END();
	
			PcdHalt();//命令卡片进入休眠状态 在再次寻卡前不能进行其他操作
			break;
			
			
		//========写卡内余额========
		//只用前两个字节存储余额 最多9999
	case WRITE_CARD:   
			rfidCmd = NO_CMD;

			block = 8; //写第8块的信息
			
			status = PcdAuthState(PICC_AUTHENT1A,block,g_password,g_SN);//验证A秘钥
			if(status!=MI_OK) {return status;}
	
			status=PcdWrite(block,g_writeData);//将writeData写入M1卡
			if(status!=MI_OK) {return status;}
			   
			//PcdHalt();
			break;
			
			
		//===========修改密码========
	case KEY_CARD: 
			rfidCmd = NO_CMD;    
			
			block = 11;
			
			status = PcdAuthState(PICC_AUTHENT1A,block,g_password,g_SN);//验证A秘钥
			if(status!=MI_OK) {printf("修改密码 AuthState Error\n"); return status;}
			
			status=PcdWrite(block,g_newPassword);//修改密码
			if(status!=MI_OK) {printf("修改密码 Write Error\n"); return status;}
			
			// 打印出新密码
			printf("改密码成功:");
			for(x = 0; x < 6; x++)
			{
				printf("%d", g_newPassword[x]);
			}
			printf("\n");

			// 系统改上新密码
			//memcpy(g_password,g_newPassword,6);
			
			//PcdHalt();
			break;
			
			
		//========发送卡号=======
	case SEND_ID: 
			rfidCmd = NO_CMD;
			
			UART_Send_Str("t2.txt=\"");
			for (x = 0; x < 4; x++)
			{
				USART1->DR = num[g_SN[x]/16]; //将16进制转 字符串发送
				while((USART1->SR&0X40)==0); //等待发送结束
				USART1->DR = num[g_SN[x]%16];
				while((USART1->SR&0X40)==0); //等待发送结束

				if(x != 3)
				{
					USART1->DR = 32; //发送空格
					while((USART1->SR&0X40)==0); //等待发送结束
				}
			}
			UART_Send_Str("\"");
			UART_Send_END();
			
			//PcdHalt();
			break;
	
	default:
		return status;
	}
	
	return status;
}

void UART_Send_Str(char *s)
{
 int i=0;
 while(s[i]!=0)
 {
 	USART1->DR = s[i];
	while((USART1->SR&0X40)==0); //等待发送结束
 	i++;
 }
 
}

void UART_Send_END(void)
{
	 USART1->DR = 0xFF;
	while((USART1->SR&0X40)==0); //等待发送结束
	 USART1->DR = 0xFF;
	while((USART1->SR&0X40)==0); //等待发送结束
	 USART1->DR = 0xFF;
	while((USART1->SR&0X40)==0); //等待发送结束
}




