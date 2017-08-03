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

void PORTE_IRQHandler();

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       SPI 驱动 NRF24L01+
 */
void main(void)
{
    uint32 i=0;
    uint8 buff[DATA_PACKET];
    uint8 *str = "欢迎使用山外 K60开发板！";

    printf("\n\n\n***********无线模块NRF24L01+测试************");

    while(!nrf_init())                  //初始化NRF24L01+ ,等待初始化成功为止
    {
        printf("\n  NRF与MCU连接失败，请重新检查接线。\n");
    }
    set_vector_handler(PORTE_VECTORn ,PORTE_IRQHandler);    			//设置 PORTE 的中断复位函数为 PORTE_VECTORn
    enable_irq(PORTE_IRQn);

    printf("\n      NRF与MCU连接成功！\n");

    while(1)
    {
        sprintf((char *)buff,"%s%d",str,i);         //把str和i合并成一个字符串到buff里，再进行发送
        nrf_tx(buff,DATA_PACKET);                   //发送一个数据包：buff（包为32字节）

        //等待发送过程中，此处可以加入处理任务


        while(nrf_tx_state() == NRF_TXING);         //等待发送完成


        if( NRF_TX_OK == nrf_tx_state () )
        {
            printf("\n发送成功:%d",i);
            i++;                                    //发送成功则加1，可验证是否漏包
        }
        else
        {
            printf("\n发送失败:%d",i);
        }
        DELAY_MS(10);
    }
}

/*!
 *  @brief      PORTE中断服务函数
 *  @since      v5.0
 */
void PORTE_IRQHandler()
{
    uint8  n;    //引脚号
    uint32 flag;

    flag = PORTE_ISFR;
    PORTE_ISFR  = ~0;                                   //清中断标志位

    n = 27;
    if(flag & (1 << n))                                 //PTE27触发中断
    {
        nrf_handler();
    }
}