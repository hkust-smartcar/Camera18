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


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 PIT 延时实验
 */
void main()
{
    led_init(LED0);                                         //初始化LED

    while(1)
    {
        led_turn(LED0);                                     //反转 LED0 状态，使得LED0闪烁

        pit_delay_ms(PIT0, 1000);                           //使用PIT0延时： 1000ms
    }
}

