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
 * @version    v5.1
 * @date       2014-02-02
 */

#include "common.h"
#include "include.h"

#define CAN_RX_MB       MB_NUM_6            //接收MBs索引定义
#define CAN_TX_MB       MB_NUM_7            //发送MBs索引定义

#define DATA_LEN        8                   //can 发送的数据长度，最大为8

uint8           can_rx_flag     = 0;        //接收到数据标志
uint8           can_rx_data[DATA_LEN+1];    //接收到的数据
uint8           can_rx_len;                 //接收到的数据长度
CAN_USR_ID_t    can_rx_id;                  //接收到的ID号

void can1_mb_handler(void);                 //声明 CAN1报文缓冲区中断服务函数

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外CAN环回测试实验
 */
void  main(void)
{
    uint8           txbuff[10]      = {"wildvcan"};             //需要发送的数据
    CAN_USR_ID_t    can_rxbuff_id   = {0x85,0,0};               //接收报文缓冲区ID
    CAN_USR_ID_t    can_tx_id       = {0x54,0,0};               //发送ID号

    printf("\n\nCAN 回环测试");

    can_init(CAN1,CAN_BAUD_20K,CAN_LOOPBACK,CAN_CLKSRC_BUS);    //初始化 CAN1 ，波特率 20Kb/s ， 环回模式 ，时钟源为 bus

    //can_rxbuff_mask     (CAN1, CAN_RX_MB,0,1);                  //设置掩码为0，使用独立掩码

    can_rxbuff_enble(CAN1,CAN_RX_MB,can_rxbuff_id);             //使能接收缓冲区

    set_vector_handler(CAN1_ORed_MB_VECTORn,can1_mb_handler);   //配置CAN接收中断服务函数到中断向量表
    can_irq_en(CAN1,CAN_RX_MB);                                 //使能can接收中断


    can_tx(CAN1,CAN_TX_MB,can_tx_id,DATA_LEN, txbuff);          //CAN发送数据。缓冲区CAN_TX_MB，报文ID:tx_ID，数据缓冲区txbuff，长度 DATA_LEN

    while(1)
    {
        if(can_rx_flag )                                        //判断是否进入过接收中断
        {
            can_rx_flag = 0;

            printf("\n\n接收到 CAN 报文!");

            printf("\n报文 ID  : 0x%X",*(uint32 *)&can_rx_id);  //打印 接收 ID

            printf("\n报文长度 : 0x%X",can_rx_len);             //打印 数据长度

            can_rx_data[can_rx_len] = 0;                        //确保数据字符串是以 0 结尾

            printf("\n报文数据 : %s",can_rx_data);              //打印 数据内容

            //接收到数据后才再发送一次
            can_tx(CAN1,CAN_TX_MB,can_tx_id,DATA_LEN, txbuff);  //CAN发送数据。缓冲区CAN_TX_MB，报文ID:tx_ID，数据缓冲区txbuff，长度 DATA_LEN

        }

        DELAY_MS(1000);

    }
}



/*!
 *  @brief      CAN1报文缓冲区中断服务函数
 *  @since      v5.0
 */
void can1_mb_handler(void)
{
    can_rx(CAN1,CAN_RX_MB,&can_rx_id,&can_rx_len,can_rx_data);  //CAN 从 CAN_RX_MB 接收数据 ，接收到的 ID 保存在 can_rx_id 里，长度保存在 can_rx_len，数据保存在 can_rx_data

    if(can_rx_len != 0)
    {
        can_rx_flag = 1;
    }
    else
    {
        can_rx_flag = 0;
    }

    can_clear_flag(CAN1,CAN_RX_MB);                             //清除缓冲区中断标志位
}

