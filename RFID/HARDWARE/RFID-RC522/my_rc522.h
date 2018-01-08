#ifndef __MY_RC522_H
#define __MY_RC522_H
#include "sys.h"

enum __RfidCmd
{
	READ_CARD,
	WRITE_CARD,
	KEY_CARD,
	//SET_TIME,
	SEND_ID,
	NO_CMD
};

extern u8 g_SN[4]; //¿¨ºÅ
extern u8 g_writeData[16];//Ð´Èë¿¨ÖÐ

extern char RfidSearchCard(void);
extern char RfidProcess(enum __RfidCmd rfidCmd);
extern void UART_Send_END(void);
extern void UART_Send_Str(char *s);

#endif
