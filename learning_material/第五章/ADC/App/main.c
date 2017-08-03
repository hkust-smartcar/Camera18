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
 *  @since      v5.1
 *  @note       山外 ADC 实验
 */
void main()
{
    uint16 var;
    adc_init(ADC1_SE16);              //ADC初始化

    while(1)
    {
        var = adc_once   (ADC1_SE16, ADC_8bit);
        printf("\nADC采样结果为:%d",var);
        printf("     相应电压值为%dmV",(3300*var)/((1<<8)-1));

        DELAY_MS(500);

    }
}