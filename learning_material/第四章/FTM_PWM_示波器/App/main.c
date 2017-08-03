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
 *  @note       FTM PWM 测试
 */
void main(void)
{
    printf("\n*****FTM PWM 测试*****\n");

    FTM_PWM_init(FTM0, FTM_CH3,200*1000,30);        //初始化 FTM PWM ，使用 FTM0_CH3，频率为200k ，占空比为 30 / FTM0_PRECISON
                                                    // vcan_port_cfg.h 里 配置 FTM0_CH3 对应为 PTA6

    while(1)
    {
        DELAY_MS(500);
        FTM_PWM_Duty(FTM0, FTM_CH3,30);     //设置占空比 为 30 / FTM0_PRECISON
        DELAY_MS(500);
        FTM_PWM_Duty(FTM0, FTM_CH3,60);     //设置占空比   60 / FTM0_PRECISON
    }
}
