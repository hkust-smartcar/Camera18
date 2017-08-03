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


uint8_t     usb_com_rx_len = 0;
uint8_t     rx_buf[64];

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 USB 虚拟串口 测试实验
                注意，还没加入 中断接收，如果接收数据太快，就有可能会丢失
 */
void  main(void)
{
    usb_com_init();                                 //初始USB为 虚拟串口模式

    usb_enum_wait();                                //等待 PC 枚举

    while(1)
    {
        CDC_Engine();                               //USB_CDC 常规处理

        usb_com_rx_len = usb_com_rx(rx_buf);        //查询数据接受
        if(usb_com_rx_len > 0)
        {
            usb_com_tx(rx_buf, usb_com_rx_len);     //发送数据
            //usb_com_rx_len = 0;
        }
    }
}
