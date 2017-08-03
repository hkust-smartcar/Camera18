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

#define DMA_COUNT   10

uint8 BUFF[DMA_COUNT + 1];     //缓存数组，预多一个

volatile uint8 dmaflag = 0;

/*!
 *  @brief      DMA通道0中断
 *  @since      v5.0
 */
void dma_ch0_handler(void)
{
    DMA_IRQ_CLEAN(DMA_CH0);                  //清除通道传输中断标志位

    //DMA_DADDR(DMA_CH0) = BUFF;            //恢复地址 (由于初始化时配置为恢复目的地址，因而此处不需要)
    DMA_EN(DMA_CH0);                        //使能通道CHn 硬件请求

    printf("\nDMA中断发生");
    dmaflag = 1;
}

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 DMA 实验 ，需要短接 PTA6 和 PTA7 ，串口助手发送数据控制传输
 */
void  main(void)
{
    uint8 i;
    char command;

    gpio_init  (PTA6, GPO, LOW);                        //初始化 PTA6 为输出低电平

    dma_portx2buff_init (DMA_CH0, (void *)&PTB_B0_IN, BUFF, PTA7, DMA_BYTE1, DMA_COUNT, DADDR_RECOVER);
            //DMA初始化，源地址：PTB_B0_IN，目的地址：buff,PTA7触发(默认上升沿)，每次传输1字节，共传输 DMA_COUNT 次 ，传输结束后恢复地址

    port_init_NoALT(PTA7,DMA_FALLING );                 //默认是上升沿触发，此处修改为下降沿触发

    set_vector_handler(DMA0_VECTORn ,dma_ch0_handler);    //设置DMA0的中断复位函数为 dma_ch0_handler
    enable_irq (DMA0_IRQn);                                //使能DMA0中断

    DMA_EN(DMA_CH0);                        //使能DMA 硬件请求


    while(1)
    {
        if(dmaflag == 1)
        {
            dmaflag = 0;
            memset(BUFF,0,sizeof(BUFF));
            printf("\n初始化数组BUFF数据");
        }
        uart_getchar(VCAN_PORT,&command);   //通过串口来控制PTA6的输出，即PTA7的输入

        //产生脉冲
        PTA6_T = 1;           //取反
        DELAY_MS(1);
        PTA6_T = 1;           //取反


        //打印 BUFF 的缓冲区数据(便于用户看到数组内容的变化)
        printf("\nDMA触发后 BUFF[%d]={",DMA_COUNT);
        for(i = 0;i < DMA_COUNT ; i++)
        {
            printf("%d,",BUFF[i]);
        }
        printf("%d};",BUFF[DMA_COUNT]);

    }
}


