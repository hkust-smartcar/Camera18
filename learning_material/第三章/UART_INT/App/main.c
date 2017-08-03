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
 *  @brief      UART3中断服务函数
 *  @since      v5.0
 */
void uart3_handler(void)
{
    char ch;
    UARTn_e uratn = UART3;

    if(UART_S1_REG(UARTN[uratn]) & UART_S1_RDRF_MASK)   //接收数据寄存器满
    {
        //用户需要处理接收数据
        uart_getchar   (UART3, &ch);                    //无限等待接受1个字节
        uart_putchar   (UART3 , ch);                    //发送字符串
    }
}

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       串口中断接收测试
 */
void main()
{
    //uart_init(UART3,115200);     //初始化串口(UART3 是工程里配置为printf函数输出端口，故已经进行初始化)

    uart_putstr   (UART3 ,"\n\n\n接收中断测试：");           //发送字符串

    set_vector_handler(UART3_RX_TX_VECTORn,uart3_handler);   // 设置中断复位函数到中断向量表里

    uart_rx_irq_en (UART3);                                 //开串口接收中断

    while(1)
    {

    }
}

