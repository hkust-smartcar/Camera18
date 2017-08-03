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
 *  @note       I2C 驱动 MMA7455
 */
void main(void)
{
    printf("\n\n\n***********三轴加速度测试************");
    
    mma7455_init();
    
    while(1)
    {          
        //注意：读取的结果需要校准的，否则不准的。
        //校准方法请看文档，此处仅讲解通信驱动
        printf("\n\nx:%d,y:%d,z:%d"
               ,(int8)mma7455_read_reg(MMA7455_XOUT8)       //读取x轴参数
               ,(int8)mma7455_read_reg(MMA7455_YOUT8)       //读取y轴参数
               ,(int8)mma7455_read_reg(MMA7455_ZOUT8)       //读取z轴参数
               );
        
        DELAY_MS(500);
    }
}

