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

uint8  nrf_rx_buff[TSL1401_MAX*TSL1401_SIZE + 2*COM_LEN + DATA_PACKET];         //预多
uint8  nrf_tx_buff[TSL1401_MAX*TSL1401_SIZE + 2*COM_LEN + DATA_PACKET];         //预多
uint8 *ccdbuff0 = (uint8 *)(((uint8 *)&nrf_tx_buff) + COM_LEN);                 //图像地址
uint8 *ccdbuff1 = (uint8 *)(((uint8 *)&nrf_tx_buff) + COM_LEN + TSL1401_SIZE);     //图像地址
uint8 *ccdbuff2 = (uint8 *)(((uint8 *)&nrf_tx_buff) + COM_LEN + 2*TSL1401_SIZE);     //图像地址

//函数声明
void PIT0_IRQHandler();
void PORTE_IRQHandler();

uint8  var1 = 20, var2;
uint16 var3, var4;
uint32 var5, var6;


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 无线调试 测试实验
 */
void  main(void)
{
    Site_t site1={0,70};                            //显示图像左上角位置
    Site_t site2={0,90};                            //显示图像左上角位置
    Site_t site3={0,110};                           //显示图像左上角位置
    Size_t imgsize={TSL1401_SIZE,1};                //图像大小
    Size_t size={TSL1401_SIZE,15};                  //显示区域大小

    uint32 i;
    com_e     com;
    nrf_result_e nrf_result;

    /************************ 配置 K60 的优先级  ***********************/
    //K60 的默认优先级 都为 0
    //参考帖子：急求中断嵌套的例程 - 智能车讨论区 - 山外初学123论坛
    //          http://www.vcan123.com/forum.php?mod=viewthread&tid=499&page=1#pid3270
    NVIC_SetPriorityGrouping(4);            //设置优先级分组,4bit 抢占优先级,没有亚优先级

    NVIC_SetPriority(PORTE_IRQn,0);         //配置优先级
    NVIC_SetPriority(PIT0_IRQn,1);          //配置优先级


    /************************ LCD 液晶屏 初始化  ***********************/
    LCD_init();

    /************************ 无线模块 NRF 初始化  ***********************/
    while(!nrf_init());
    //配置中断复位函数
    set_vector_handler(PORTE_VECTORn ,PORTE_IRQHandler);                //设置 PORTE 的中断复位函数为 PORTE_VECTORn
    enable_irq(PORTE_IRQn);
    nrf_msg_init();                                                     //无线模块消息初始化

    //线性CCD模块，需要 发送 空命令给 调试板模块，这样可以清掉 接收图像失败而产生多余数据
    i = 20;
    while(i--)
    {
        nrf_msg_tx(COM_RETRAN,nrf_tx_buff);                              //发送多个 空 命令过去，用于清空 接收端的缓存数据
    }

    /*********************** 按键消息 初始化  ***********************/
    key_event_init();                                                   //按键消息初始化
    //pit_init_ms(PIT0,10);                                             //pit 定时中断(用于按键定时扫描)
    set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);                  //设置 PIT0 的中断复位函数为 PIT0_IRQHandler
    enable_irq(PIT0_IRQn);

    /************************ 线性CCD 初始化  ***********************/
    tsl1401_set_addrs(TSL1401_MAX,ccdbuff0,ccdbuff1,ccdbuff2);          //配置采集
    tsl1401_init(10);                                                   //初始化 线性CCD ，配置 中断时间为 10
    //tsl1401_led_en(TSL1401_MAX);                                        //使能CCD 的 补光灯

    //线性CCD 和 key 共用 pit0 定时中断

    while(1)
    {
        /************************ 图像采集和显示  ***********************/
        //采集 线性CCD 图像
        tsl1401_get_img();

        //LCD 显示图像
        LCD_Img_gray_Z(site1,size,ccdbuff0,imgsize);
        LCD_Img_gray_Z(site2,size,ccdbuff1,imgsize);
        LCD_Img_gray_Z(site3,size,ccdbuff2,imgsize);

        /************************ 无线发送和接收数据  ***********************/
        do
        {
            nrf_result = nrf_msg_rx(&com, nrf_rx_buff);
            if(nrf_result == NRF_RESULT_RX_VALID)
            {
                switch(com)
                {
                    case COM_CCD:
                        break;

                    case COM_VAR:

                        break;
                    default:
                        break;
                }
            }
        }while(nrf_result != NRF_RESULT_RX_NO);         //接收不到数据 才退出

        //把图像 发送出去
        nrf_msg_tx(COM_CCD,nrf_tx_buff);

        while(nrf_tx_state() == NRF_TXING);             //等待发送完成


        /*********************** 按键消息 处理  ***********************/
        deal_key_event();

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


/*!
 *  @brief      PIT0中断服务函数
 *  @since      v5.0
 */
void PIT0_IRQHandler()
{
    tsl1401_time_isr();
    key_IRQHandler();
    PIT_Flag_Clear(PIT0);
}

