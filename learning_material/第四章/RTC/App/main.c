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
 * @version    v5.1
 * @date       2014-01-12
 */

#include "common.h"
#include "include.h"


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 RTC 实验
 */
void main()
{
    time_s time = {2013,9,1,0,0,0}; //2013-09-01 00时00分00秒
    uint32 sec;

    rtc_init();                     //RTC初始化

    sec = time2sec( time);          //将年月日时分秒格式转换成秒总数

    rtc_set_time(sec);              //设置 RTC 时钟

    while(1)
    {
        sec = rtc_get_time();       //获取时间

        sec2time(sec,& time);       //转换数据类型为年月日时分秒类型

        printf("\n现在时间是：%d-%02d-%02d %02d:%02d:%02d",time.year,time.mon,time.day,time.hour,time.min,time.sec);
                                    //通过串口打印时间

        DELAY_MS( 1000);            //延时： 1000ms
    }
}