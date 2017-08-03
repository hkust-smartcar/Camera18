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

//全局变量定义
volatile uint32 irqflag = 0;                //标志位定义，非0 表示进入中断

//函数声明
void FTM0_INPUT_IRQHandler(void);        //FTM0中断服务函数


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       FTM 输入捕捉 测试
 */

void main()
{
    FTM_Input_init(FTM0, FTM_CH7, FTM_Falling,FTM_PS_1);     //初始化FTM输入捕捉模式，下降沿捕捉(FTM0_CH7 为 PTD7)
    port_init_NoALT(FTM0_CH7 ,PULLUP);              //配置端口为上拉（保留原先的复用配置）

    set_vector_handler(FTM0_VECTORn ,FTM0_INPUT_IRQHandler);    //设置FTM0的中断服务函数为 FTM0_INPUT_IRQHandler
    enable_irq (FTM0_IRQn);                                     //使能FTM0中断

    led_init(LED0);                 //初始化LED端口

    while(1)
    {
        if(irqflag != 0)
        {
            led_turn(LED0);         //通过 LED 看效果
            DELAY_MS(200);          //由于是通过按键按下，查看LED状态，因此需要按键延时消抖
            irqflag = 0;
        }
    }
}


void FTM0_INPUT_IRQHandler(void)
{
    uint8 s = FTM0_STATUS;          //读取捕捉和比较状态  All CHnF bits can be checked using only one read of STATUS.
    uint8 CHn;

    FTM0_STATUS = 0x00;             //清中断标志位

    CHn = 7;
    if( s & (1 << CHn) )
    {
        /*     用户任务       */
        irqflag++;

        /*********************/
    }

}

