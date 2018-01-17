#include "string.h"
#include "control.h"
#include "my_rc522.h"


struct __GarageSpace g_GarageSpace[9] = {
			{1, 1, {NULL,NULL,NULL,NULL}, 0},
			{1, 2, {NULL,NULL,NULL,NULL}, 0},
			{1, 3, {NULL,NULL,NULL,NULL}, 0},
			{2, 1, {NULL,NULL,NULL,NULL}, 0},
			{2, 2, {NULL,NULL,NULL,NULL}, 0},
			{2, 3, {NULL,NULL,NULL,NULL}, 0},
			{3, 1, {NULL,NULL,NULL,NULL}, 0},
			{3, 2, {NULL,NULL,NULL,NULL}, 0},
			{3, 3, {NULL,NULL,NULL,NULL}, 0},
};  // 9个车位存放的数据
extern signed char thisCard;


//入库
void InGarageData(void)
{
	switch(thisCard)
	{
	case 0:
		memcpy(g_GarageSpace[0].m_ID,g_GarageSpace[2].m_ID, 4);
		memcpy(g_GarageSpace[2].m_ID,g_GarageSpace[1].m_ID, 4);
		memcpy(g_GarageSpace[1].m_ID,g_SN, 4);
		break;
	case 1:
		memcpy(g_GarageSpace[1].m_ID,g_GarageSpace[2].m_ID, 4);
		memcpy(g_GarageSpace[2].m_ID,g_GarageSpace[0].m_ID, 4);
		memcpy(g_GarageSpace[0].m_ID,g_SN, 4);
		break;
	case 2:
		memcpy(g_GarageSpace[2].m_ID,g_GarageSpace[1].m_ID, 4);
		memcpy(g_GarageSpace[1].m_ID,g_GarageSpace[0].m_ID, 4);
		memcpy(g_GarageSpace[0].m_ID,g_GarageSpace[2].m_ID, 4);
		memcpy(g_GarageSpace[2].m_ID,g_SN, 4);
		break;


	case 3:
		memcpy(g_GarageSpace[0+3].m_ID,g_GarageSpace[2+3].m_ID, 4);
		memcpy(g_GarageSpace[2+3].m_ID,g_GarageSpace[1+3].m_ID, 4);
		memcpy(g_GarageSpace[1+3].m_ID,g_SN, 4);     
		break;                                       
	case 4:                                          
		memcpy(g_GarageSpace[1+3].m_ID,g_GarageSpace[2+3].m_ID, 4);
		memcpy(g_GarageSpace[2+3].m_ID,g_GarageSpace[0+3].m_ID, 4);
		memcpy(g_GarageSpace[0+3].m_ID,g_SN, 4);
		break;
	case 5:
		memcpy(g_GarageSpace[2+3].m_ID,g_GarageSpace[1+3].m_ID, 4);
		memcpy(g_GarageSpace[1+3].m_ID,g_GarageSpace[0+3].m_ID, 4);
		memcpy(g_GarageSpace[0+3].m_ID,g_GarageSpace[2+3].m_ID, 4);
		memcpy(g_GarageSpace[2+3].m_ID,g_SN, 4);
		break;


	case 6:
		memcpy(g_GarageSpace[0+6].m_ID,g_GarageSpace[2+6].m_ID, 4);
		memcpy(g_GarageSpace[2+6].m_ID,g_GarageSpace[1+6].m_ID, 4);
		memcpy(g_GarageSpace[1+6].m_ID,g_SN, 4);     
		break;                                       
	case 7:                                          
		memcpy(g_GarageSpace[1+6].m_ID,g_GarageSpace[2+6].m_ID, 4);
		memcpy(g_GarageSpace[2+6].m_ID,g_GarageSpace[0+6].m_ID, 4);
		memcpy(g_GarageSpace[0+6].m_ID,g_SN, 4);
		break;
	case 8:
		memcpy(g_GarageSpace[2+6].m_ID,g_GarageSpace[1+6].m_ID, 4);
		memcpy(g_GarageSpace[1+6].m_ID,g_GarageSpace[0+6].m_ID, 4);
		memcpy(g_GarageSpace[0+6].m_ID,g_GarageSpace[2+6].m_ID, 4);
		memcpy(g_GarageSpace[2+6].m_ID,g_SN, 4);
		break;
	default:
			break;
	}
}

void OutGarageData(void)
{
	const char c_null[4] = {NULL,NULL,NULL,NULL};
	switch(thisCard)
	{
	case 0:
		memcpy(g_GarageSpace[0].m_ID,g_GarageSpace[2].m_ID, 4);
		memcpy(g_GarageSpace[2].m_ID,g_GarageSpace[1].m_ID, 4);
		memcpy(g_GarageSpace[1].m_ID,c_null, 4);
		break;
	case 1:
		memcpy(g_GarageSpace[1].m_ID,g_GarageSpace[2].m_ID, 4);
		memcpy(g_GarageSpace[2].m_ID,g_GarageSpace[0].m_ID, 4);
		memcpy(g_GarageSpace[0].m_ID,c_null, 4);
		break;
	case 2:
		memcpy(g_GarageSpace[2].m_ID,g_GarageSpace[1].m_ID, 4);
		memcpy(g_GarageSpace[1].m_ID,g_GarageSpace[0].m_ID, 4);
		memcpy(g_GarageSpace[0].m_ID,g_GarageSpace[2].m_ID, 4);
		memcpy(g_GarageSpace[2].m_ID,c_null, 4);
		break;


	case 3:
		memcpy(g_GarageSpace[0+3].m_ID,g_GarageSpace[2+3].m_ID, 4);
		memcpy(g_GarageSpace[2+3].m_ID,g_GarageSpace[1+3].m_ID, 4);
		memcpy(g_GarageSpace[1+3].m_ID,c_null, 4);     
		break;                                       
	case 4:                                          
		memcpy(g_GarageSpace[1+3].m_ID,g_GarageSpace[2+3].m_ID, 4);
		memcpy(g_GarageSpace[2+3].m_ID,g_GarageSpace[0+3].m_ID, 4);
		memcpy(g_GarageSpace[0+3].m_ID,c_null, 4);
		break;
	case 5:
		memcpy(g_GarageSpace[2+3].m_ID,g_GarageSpace[1+3].m_ID, 4);
		memcpy(g_GarageSpace[1+3].m_ID,g_GarageSpace[0+3].m_ID, 4);
		memcpy(g_GarageSpace[0+3].m_ID,g_GarageSpace[2+3].m_ID, 4);
		memcpy(g_GarageSpace[2+3].m_ID,c_null, 4);
		break;


	case 6:
		memcpy(g_GarageSpace[0+6].m_ID,g_GarageSpace[2+6].m_ID, 4);
		memcpy(g_GarageSpace[2+6].m_ID,g_GarageSpace[1+6].m_ID, 4);
		memcpy(g_GarageSpace[1+6].m_ID,c_null, 4);     
		break;                                       
	case 7:                                          
		memcpy(g_GarageSpace[1+6].m_ID,g_GarageSpace[2+6].m_ID, 4);
		memcpy(g_GarageSpace[2+6].m_ID,g_GarageSpace[0+6].m_ID, 4);
		memcpy(g_GarageSpace[0+6].m_ID,c_null, 4);
		break;
	case 8:
		memcpy(g_GarageSpace[2+6].m_ID,g_GarageSpace[1+6].m_ID, 4);
		memcpy(g_GarageSpace[1+6].m_ID,g_GarageSpace[0+6].m_ID, 4);
		memcpy(g_GarageSpace[0+6].m_ID,g_GarageSpace[2+6].m_ID, 4);
		memcpy(g_GarageSpace[2+6].m_ID,c_null, 4);
		break;
	default:
			break;
	}
}
