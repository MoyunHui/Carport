#include "key.h"
#include "delay.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

} 

u8 KEY_Scan(u8 mode)
{
    if(KEY0==0||KEY1==0||KEY2==0||KEY3==0)
    {
        delay_ms(10);

        if(KEY0 == 0)return KEY0_PRES;
        else if(KEY1 == 0)return KEY1_PRES;
        else if(KEY2 == 0)return KEY2_PRES;
        else if(KEY3 == 0)return KEY3_PRES;
        else return 0;
    }return 0;
}
