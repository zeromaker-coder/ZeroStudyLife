#include "beep.h"

#define BEEP                (D7 )       // 蜂鸣器接口
uint16 count_beep = 0;                  // 蜂鸣器计数器

/**
  * @brief  蜂鸣器初始化
  * @param  无
  * @retval 无
  */
void beep_on(void)
{
    count_beep=5;
}

/**
  * @brief  蜂鸣器循环
  * @param  无
  * @retval 无
  */
void beep_cycle(void)
{
    if(count_beep)
    {
        gpio_set_level(BEEP,GPIO_HIGH);
        count_beep--;
    }
    else
    {
        count_beep=0;
        gpio_set_level(BEEP, GPIO_LOW);
    }
}