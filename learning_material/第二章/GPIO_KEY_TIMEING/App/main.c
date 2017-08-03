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
 *  @brief      LPTMR定时执行的中断服务函数
 *  @since      v5.0
 *  @note       测试 KEY 定时扫描
 */
void lptmr_hander(void)
{
    LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;         //清除LPT比较标志位

    //下面由用户添加实现代码
    key_IRQHandler();                           //把按键扫描程序加入到定时中断复位函数里，定时执行

}

char * keyname[KEY_MAX]={"KEY_U","KEY_D","KEY_L","KEY_R",
                        "KEY_A","KEY_B","KEY_START","KEY_START"};

char * keystatus[3]={"按下","弹起","长按"};

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       测试 KEY 定时扫描,需要通过串口查看结果，串口 波特率 为 VCAN_BAUD
 */
void main()
{
    KEY_MSG_t keymsg;

    printf("\n******* GPIO 按键定时测试 *******");

    key_init(KEY_MAX);

    lptmr_timing_ms(10);                                // LPTMR 定时 10ms
    set_vector_handler(LPTMR_VECTORn,lptmr_hander);   // 设置中断复位函数到中断向量表里
    enable_irq(LPTMR_IRQn);                           // 使能LPTMR中断

    while(1)
    {
        while(get_key_msg(&keymsg) == 1)
        {
            printf("\n按键%s%s",keyname[keymsg.key],keystatus[keymsg.status]);
        }

        DELAY_MS(500);                                  //可以 调 延时时间，除非 FIFO满溢出，不然不会出现漏识别按键
    }
}


