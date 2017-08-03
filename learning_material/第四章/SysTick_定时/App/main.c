/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,山外科技
 *     All rights reserved.
 *     技术讨论：山外初学论坛 http://www.vcan123.com
 *
 *     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留山外科技的版权声明。
 *
 * @file       main.c
 * @brief      山外K60 平台主程序
 * @author     山外科技
 * @version    v5.0
 * @date       2013-08-28
 */

#include "common.h"
#include "include.h"


//函数声明
extern void SysTick_IRQHandler(void);

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 滴答定时器systick 定时中断实验
 */
void main()
{
    led_init(LED0);                                             //初始化LED0，PIT0中断用到LED0

    systick_timing_ms(100);                                     //初始化滴答定时器，定时时间为： 100ms
    set_vector_handler(SysTick_VECTORn ,SysTick_IRQHandler);    //设置滴答定时器的中断服务函数为 SysTick_IRQHandler

    while(1)
    {
        //这里不需要执行任务，等待中断来闪烁LED0
    }
}

/*!
 *  @brief      滴答定时器SysTick中断服务函数
 *  @since      v5.0
 */
void SysTick_IRQHandler(void)
{
    led_turn(LED0);             //闪烁 LED0
}

