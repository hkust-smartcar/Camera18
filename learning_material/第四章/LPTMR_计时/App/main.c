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
 *  @note       山外 LPTMR 计时实验
 */
void main()
{
    uint32 timevar;
    while(1)
    {
        lptmr_time_start_ms();                  //开始计时

        DELAY_MS(50);                           //延时一段时间(由于语句执行需要时间，因而实际的延时时间会更长一些)

        timevar = lptmr_time_get_ms();          //停止计时，获取计时时间

        printf("\n\n计时时间为：%dms",timevar); //打印计时时间

        DELAY_MS(1000);
    }
}

