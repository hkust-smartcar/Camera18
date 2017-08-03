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

#define CAN_TX_MB       MB_NUM_7            //发送MBs索引定义
#define DATA_LEN        8                   //can 发送的数据长度，最大为8


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外CAN 节点 A 发送测试
 */
void  main(void)
{


    uint8           txbuff[10]      = {"wildvcan"};             //需要发送的数据
    CAN_USR_ID_t    can_tx_id       = {0x8F,0,0};               //发送ID号

    printf("\n\nCAN 节点A发送测试");

    can_init(CAN1,CAN_BAUD_20K,CAN_NORMAL,CAN_CLKSRC_BUS);                     //初始化 CAN1 ，波特率 20Kb/s ， 正常模式  ,bus  时钟作为时钟源

    while(1)
    {
        can_tx(CAN1,CAN_TX_MB,can_tx_id,DATA_LEN, txbuff);  //CAN发送数据。缓冲区CAN_TX_MB，报文ID:tx_ID，数据缓冲区txbuff，长度 DATA_LEN

        DELAY_MS(1000);
    }
}


