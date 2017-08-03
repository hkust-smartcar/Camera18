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
#include "math.h"

/*! 
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 DAC 输出正弦波 实验
 */
void main()
{
    float val = 0;
    uint16 result;
    
    
    dac_init(DAC1);

    while(1)
    {
        result =(uint16) (
                            ((sin(val)+1.0)/2.0 )   //sin 的取值范围是 -1 ~ 1 ，加1 后变成 0~2 ,再 除以 2 确保范围在 0~1 之间
                                *((1<<12) - 1)      //DAC 是 12bit
                         );

        dac_out(DAC1, result);                      //输出 DAC ，可通过示波器看到正弦波
        
        val += 0.1;
    }
}