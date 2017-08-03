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
 *  @note       测试查询接收与发送相关的部分代码
 */
void main()
{
    char ch;

    //uart_init(UART3,115200);     //初始化串口(由于 printf 函数 所用的端口就是 UART3，已经初始化了，因此此处不需要再初始化)

    printf("\n山外初学123论坛:www.vcan123.com");

    uart_putstr    (UART3 , "\n\n\n接受数据，并进行发送:");         //发送字符串

    while(1)
    {
        if(uart_query (UART3) != 0)                                 //查询是否接收到数据
        {
            uart_getchar (UART3,&ch);                               //等待接收一个数据，保存到 ch里
            uart_putchar(UART3, ch);                                //发送1个字节
        }

        if(uart_querychar (UART3, &ch) != 0)                        //查询接收1个字符
        {
            uart_putchar(UART3, ch);                                //发送1个字节
        }

        //注：上面两个 if 的实现功能都是一样的。

        //uart_getchar 和 uart_querychar 的区别在于 ，前者 需要等待接收到数据，后者查询是否接收到，接收到就接收，接收不到就退出
    }
}
