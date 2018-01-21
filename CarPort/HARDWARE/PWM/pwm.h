#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define Motor_1_Stop()                   TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Disable)
#define Motor_1_Clockwise_Mov()          PAout(4)=1; TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Enable)
#define Motor_1_Anticlockwise_Mov()      PAout(4)=0; TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Enable)

#define Motor_2_Stop()                   TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable)
#define Motor_2_Clockwise_Mov()          PAout(5)=1; TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable)
#define Motor_2_Anticlockwise_Mov()      PAout(5)=0; TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable)

void TIM3_PWM_Init(u16 arr, u16 psc);
#endif
