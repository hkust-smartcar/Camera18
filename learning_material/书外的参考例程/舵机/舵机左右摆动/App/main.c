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



#define S3010_FTM   FTM1
#define S3010_CH    FTM_CH0
#define S3010_HZ    (100)


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 S3010舵机 测试实验 ,山外 提供的 电机驱动模块，管脚都加入 MOS 管隔离，信号是反相的。
                例如常规的 高电平 使能
 */
void main()
{
    uint8 i;
    FTM_PWM_init(S3010_FTM, S3010_CH,S3010_HZ,100);      //初始化 舵机 PWM

    while(1)
    {

        for(i = 13;i<25;i++)
        {
            FTM_PWM_Duty(S3010_FTM, S3010_CH,100-i);
            DELAY_MS(200);
        }

        for(;i>13;i--)
        {
            FTM_PWM_Duty(S3010_FTM, S3010_CH,100-i);
            DELAY_MS(200);
        }


    }

}