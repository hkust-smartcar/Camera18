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
 *  @note       山外 LPTMR 脉冲计数实验，需要短接 PTA7 和 PTA19
 */
void  main(void)
{
#define INT_COUNT  0xFFFF          //LPT 产生中断的计数次数

    uint16 count;

    FTM_PWM_init(FTM0, FTM_CH4, 10000, 50);                       //FTM模块产生PWM，用 FTM0_CH4 ，即 PTA7 ，频率为 100，占空比 50%
                                                                //修改频率，验证 不同PWM下计数值是多少。

    lptmr_pulse_init(LPT0_ALT1, INT_COUNT, LPT_Rising);         //初始化脉冲计数器，用LPT0_ALT1，即PTA19输入，每隔INT_COUNT产生中断（需要开中断才能产生中断），上升沿触发

    while(1)
    {
        lptmr_pulse_clean();                                    //清空脉冲计数器计算值（马上清空，这样才能保证计数值准确）

        DELAY_MS(1000);                                         //利用 延时时间，LPTMR模块进行 计算，累加 FTM 产生的PWM脉冲

        count           =   lptmr_pulse_get();                  //保存脉冲计数器计算值

        printf("LPTMR脉冲计数为:%d\n",   count);                 //打印计数值
    }
}

