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


void PORTD_IRQHandler(void);        //PORTD端口中断服务函数
void key_handler(void);             //按键按下的测试中断复位函数

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       测试 port 配置功能，需要接串口来看测试效果
                按键一端接 PTD7 ，另一端接地
                按键按下时，PTD7 接地
                按键没有弹起时，PTD7 浮空，因此需要上拉电阻来把电平拉高
 */
void main()
{
    printf("\n*****按键测试*****\n");

    port_init(PTD7, ALT1 | IRQ_FALLING | PULLUP );          //初始化 PTD7 管脚，复用功能为GPIO ，下降沿触发中断，上拉电阻

    set_vector_handler(PORTD_VECTORn ,PORTD_IRQHandler);    //设置PORTE的中断复位函数为 PORTE_IRQHandler
    enable_irq (PORTD_IRQn);                                //使能PORTE中断

    while(1)
    {
        //disable_irq(PORTD_IRQn);
    }
}


/*!
 *  @brief      PORTD端口中断服务函数
 *  @since      v5.0
 */
void PORTD_IRQHandler(void)
{

#if 0       // 条件编译，两种方法可供选择

    uint8  n = 0;    //引脚号
    n = 7;
    if(PORTD_ISFR & (1 << n))           //PTD7 触发中断
    {
        PORTD_ISFR  = (1 << n);        //写1清中断标志位

        /*  以下为用户任务  */

        key_handler();

        /*  以上为用户任务  */
    }
#else
    PORT_FUNC(D,7,key_handler);
#endif
}

/*!
 *  @brief      按键按下的测试中断复位函数
 *  @since      v5.0
 */
void key_handler(void)
{
    printf("\n按下按键\n");
}
