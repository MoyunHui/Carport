#ifndef __CONTRL_H
#define __CONTRL_H
#include "sys.h"

enum __ProcessCmd
{
	GET_IN_GARAGE,
	GET_OUT_GARAGE,
	NO_PLACE,
	SLEEP
};


struct __GarageSpace
{
	const char flooar;  //����
	const char beat;  //λ�ú�
	u8 m_ID[4];
	u8 m_balance;   //���
};
extern struct __GarageSpace g_GarageSpace[9];
extern void InGarageData(void);
extern void OutGarageData(void);



#endif
