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

uint8 imgbuff[CAMERA_SIZE];                             //定义存储接收图像的数组
uint8 img[CAMERA_W*CAMERA_H];

//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void sendimg(uint8 *imgaddr, uint32 imgsize);
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);

#if 0
/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外LCD 激光计数器
 */
void  main(void)
{
    uint16 ms;

    LCD_ui_time_init();

    while(1)
    {
        LCD_ui_time(ms++);
    }
}

#else

#define     DISTURB_TIME    1000        // 干扰时间
#define     TIME_MAX        60000       // 最大计时时间

uint32 time_ms = 0;
uint8  timemode = 0;        //计时模式 （0 是 停止计数状态 ， 1 是 正在计数状态，2 表示 小车第二次触发）

void PIT0_IRQHandler(void);
void PORTA_IRQHandler(void);

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外LCD 激光计数器 ,lptmr 的 计时器 最大可计时 65535 ms ，满足智能车的 60s需求
 */
void  main(void)
{
    KEY_MSG_t keymsg;

    key_init(KEY_MAX);        // 初始化 KEY_START 启动按键，作为 启动 或者 暂停 按键

    pit_init_ms(PIT0,10);
    set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);      //设置PIT0的中断复位函数为 PIT0_IRQHandler
    enable_irq (PIT0_IRQn);                                 //使能PIT0中断

    LCD_ui_time_init();

    led_init(LED0);
    gpio_init(PTA7,GPI,0);

    //激光输出 初始化
    FTM_PWM_init(FTM0, FTM_CH3,180000,80);  //初始化 FTM PWM ，使用 FTM0_CH3，频率为180k ，占空比为 80 / 100
                                            // vcan_port_cfg.h 里 配置 FTM0_CH3 对应为 PTA7

    port_init(PTA7, ALT1 | IRQ_RISING |  PF );          //初始化 PTA7 管脚，复用功能为GPIO ，上升沿触发中断，上拉电阻
    set_vector_handler(PORTA_VECTORn ,PORTA_IRQHandler);    //设置PORTA的中断复位函数为 PORTA_IRQHandler
    PORTA_ISFR  = (1 << 7);                 //写1清中断标志位
    enable_irq (PORTA_IRQn);                //使能PORTA中断


    while(1)
    {
        led(LED0,GPIO_GET(PTA7));           //LED 显示 状态
        if(timemode == 1)
        {
            time_ms =  lptmr_time_get_ms();
            if(time_ms >= TIME_MAX)         //最大计时 TIME_MAX 秒
            {

                //时间到
                disable_irq (PORTA_IRQn);   //禁止PORTA中断
                timemode = 0;
                LCD_ui_time(TIME_MAX);      //显示时间

            }
            else
            {
                LCD_ui_time(time_ms);       //显示时间
            }
        }
        else if(timemode == 2)
        {
            timemode = 0;
            LCD_ui_time(time_ms);               //显示时间
            while(get_key_msg(&keymsg) == 1);   //清空按键消息
        }

        while(get_key_msg(&keymsg) == 1)
        {
            if((keymsg.key == KEY_START) && (keymsg.status == KEY_DOWN))      //按键KEY_START按下
            {
                //  timemode == 1 ,是 由 中断 触发
                if(timemode == 1)       // 由 1 变 0 ，表示 复位 或 手工暂停
                {
                    disable_irq (PORTA_IRQn);        //禁止PORTA中断
                    timemode = 0;
                }else  if(timemode == 0)                   // 0 是停止计数，会把时间清 0
                {
                    PORTA_ISFR  = (1 << 7);         //写1清中断标志位
                    enable_irq (PORTA_IRQn);        //使能PORTA中断
                    LCD_ui_time(0);                 //显示时间为 0
                }
            }
        }
    }
}

/*!
 *  @brief      PIT0中断服务函数
 *  @since      v5.0
 */
void PIT0_IRQHandler(void)
{
    PIT_Flag_Clear(PIT0);       //清中断标志位

    //下面由用户添加实现代码
    key_IRQHandler();                           //把按键扫描程序加入到定时中断复位函数里，定时执行
}

/*!
 *  @brief      PORTA端口中断服务函数
 *  @since      v5.0
 */
void PORTA_IRQHandler(void)
{
    uint8  n ;    //引脚号

    n = 7;
    if(PORTA_ISFR & (1 << n))           //PTA7 触发中断
    {
        /*  以下为用户任务  */
        if(timemode == 0)               // 0 是停止状态，则进入 计时状态
        {
            lptmr_time_start_ms();      //开始计时(ms)
            timemode = 1;
            time_ms = 0;
        }
        else /*if(timemode == 1) */     // 计时中再次进入 计时
        {
            time_ms =  lptmr_time_get_ms(); //获取时间

            if(time_ms > DISTURB_TIME)
            {
                disable_irq (PORTA_IRQn);                                //使能PORTA中断
                timemode = 2;               //超过干扰时间后，认为确认 时间到了
                lptmr_time_close();         //关闭计时器
            }
        }




        /*  以上为用户任务  */

        PORTA_ISFR  = (1 << n);         //写1清中断标志位
    }

}


#endif

