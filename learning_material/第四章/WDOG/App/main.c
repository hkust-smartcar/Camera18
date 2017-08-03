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
 *  @note       看门狗测试代码
 */
void main(void)
{
    printf("\n*****看门狗测试*****\n");

    wdog_init_ms(1000) ;        //初始化看门狗，调整这里的时间，看看每次复位的时间是否与设定的相同（如果时间太短，复位时初始化时间比较长，误差比较大的
    while(1)
    {
        DELAY_MS(500);
        wdog_feed();  printf("喂狗了，汪汪~~\n\n");     //注释与没注释，可以看到复位与没复位的（通过串口助手）

    }
}

