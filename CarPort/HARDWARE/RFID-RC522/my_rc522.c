#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "rc522.h"
#include "my_rc522.h"
#include <string.h>

/*ȫ�ֱ���*/
u8 g_CT[2];//������
u8 g_SN[4]; //����

u8 g_total=0;

u8 g_readData[16]; //�ӿ�����ȡ
u8 g_writeData[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//д�뿨��

// ��ʼ���� 0xff,0xff,0xff,0xff,0xff,0xff
u8 g_password[6]={0xff,0xff,0xff,0xff,0xff,0xff}; //����
u8 g_newPassword[6]={0,0,0,0,0,0}; //������


/******************************Ѱ�� ����ײ ѡ��*******************************/
char RfidSearchCard(void)
{
	char status = MI_OK; //״̬��־
	
	status = PcdReset();//��λRC522
	if(status!=MI_OK) {return status;}
	
	status = PcdRequest(PICC_REQIDL,g_CT);//Ѱ��������δ��������״̬�Ŀ������ؿ�Ƭ���� 2�ֽ�
	if(status!=MI_OK) {return status;}
 
	status = PcdAnticoll(g_SN);//����ײ�����ؿ������к� 4�ֽ�
	if(status!=MI_OK) {return status;}
	
	status = PcdSelect(g_SN);//ѡ��
	if(status!=MI_OK) {return status;}

	return status;
}


/******************************�Կ�����*******************************/
char RfidProcess(enum __RfidCmd rfidCmd)
{
	char status = MI_OK; //״̬��־
	u8 block;  //Ҫ���ʵĿ�
	u8 x; //ѭ������
	unsigned char num[] = "0123456789abcdef";

	switch(rfidCmd)
	{
		//=========���������=======
	case READ_CARD: 
			rfidCmd = NO_CMD;

			block = 8;
			
			status = PcdAuthState(PICC_AUTHENT1A,block,g_password,g_SN);//��֤A�ܳ�
			if(status!=MI_OK) {return status;}
			
			status = PcdRead(block,g_readData);//��M1��ĳһ���ȡ�������ݴ����Read_Data[]������
			if(status!=MI_OK) {return status;}
			
			UART_Send_Str("t3.txt=\"");
			for (x = 0; x < 2; x++)
			{
				USART1->DR = num[g_readData[x]/10];  //���͸�λ���λ
				while((USART1->SR&0X40)==0); //�ȴ����ͽ���
				USART1->DR = num[g_readData[x]%10];  //����ʮλ��ǧλ
				while((USART1->SR&0X40)==0); //�ȴ����ͽ���
				if(x != 1)
				{
					USART1->DR = 32;
					while((USART1->SR&0X40)==0); //�ȴ����ͽ���
				}
			}
			UART_Send_Str("\"");
			UART_Send_END();
	
			PcdHalt();//���Ƭ��������״̬ ���ٴ�Ѱ��ǰ���ܽ�����������
			break;
			
			
		//========д�������========
		//ֻ��ǰ�����ֽڴ洢��� ���9999
	case WRITE_CARD:   
			rfidCmd = NO_CMD;

			block = 8; //д��8�����Ϣ
			
			status = PcdAuthState(PICC_AUTHENT1A,block,g_password,g_SN);//��֤A��Կ
			if(status!=MI_OK) {return status;}
	
			status=PcdWrite(block,g_writeData);//��writeDataд��M1��
			if(status!=MI_OK) {return status;}
			   
			//PcdHalt();
			break;
			
			
		//===========�޸�����========
	case KEY_CARD: 
			rfidCmd = NO_CMD;    
			
			block = 11;
			
			status = PcdAuthState(PICC_AUTHENT1A,block,g_password,g_SN);//��֤A��Կ
			if(status!=MI_OK) {printf("�޸����� AuthState Error\n"); return status;}
			
			status=PcdWrite(block,g_newPassword);//�޸�����
			if(status!=MI_OK) {printf("�޸����� Write Error\n"); return status;}
			
			// ��ӡ��������
			printf("������ɹ�:");
			for(x = 0; x < 6; x++)
			{
				printf("%d", g_newPassword[x]);
			}
			printf("\n");

			// ϵͳ����������
			//memcpy(g_password,g_newPassword,6);
			
			//PcdHalt();
			break;
			
			
		//========���Ϳ���=======
	case SEND_ID: 
			rfidCmd = NO_CMD;
			
			UART_Send_Str("t2.txt=\"");
			for (x = 0; x < 4; x++)
			{
				USART1->DR = num[g_SN[x]/16]; //��16����ת �ַ�������
				while((USART1->SR&0X40)==0); //�ȴ����ͽ���
				USART1->DR = num[g_SN[x]%16];
				while((USART1->SR&0X40)==0); //�ȴ����ͽ���

				if(x != 3)
				{
					USART1->DR = 32; //���Ϳո�
					while((USART1->SR&0X40)==0); //�ȴ����ͽ���
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
	while((USART1->SR&0X40)==0); //�ȴ����ͽ���
 	i++;
 }
 
}

void UART_Send_END(void)
{
	 USART1->DR = 0xFF;
	while((USART1->SR&0X40)==0); //�ȴ����ͽ���
	 USART1->DR = 0xFF;
	while((USART1->SR&0X40)==0); //�ȴ����ͽ���
	 USART1->DR = 0xFF;
	while((USART1->SR&0X40)==0); //�ȴ����ͽ���
}




