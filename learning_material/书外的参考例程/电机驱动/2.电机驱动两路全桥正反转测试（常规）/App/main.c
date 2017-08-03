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

//【山外电机驱动】打破权威的限制，还原电路的本色。
//注意：山外电机驱动 是 加入 MOS 管 隔离反相的，IO 输出 低电平，经过 MOS 管后会反相，变成 高电平，反正既然。


//常规模式: PWM 接入 IN 管脚 ,IO控制 INH 来使能 。
// INH 输入为 高电平 ，使能 BTN7971
// IN  输入为 高电平 , 加速 ； IN 输入为 低电平，刹车（注意，是刹车，不是降速那么简单）
//常规模式 是 不停加速（IN为高），然后减速（IN为低），是飞思卡尔智能车比赛最常用的方案
//效果如何？ 一边踩 油门 ，一边踩 刹车  ，你觉得呢？

//自由滚动模式: PWM 接入 INH 管脚 控制 是否使能 ，IO 控制 IN 管脚
// IN 输入为 高电平  ， 加速
// INH 输入为 高电平 , 使能芯片，进入全速状态 ； INH 输入为 低电平 ，禁用芯片，进入 无驱动状态
//自由滚动模式 是 一时加油，一时松开油门依靠重力加速度往前滚动。
//效果如何？ 经过 BTN7971 ，实际上 表现为 改变 电压值 ，从而 达到 控制 油门量 的目的 ，芯片发热量大大减少，滑动流畅


//点杀模式：PWM 接入 INH 管脚 控制 是否使能 ，IO 控制 IN 管脚
// IN 输入为 低电平  ， 刹车
// INH 输入为 高电平 , 使能芯片，进入刹车状态 ； INH 输入为 低电平 ，禁用芯片，进入 无驱动状态
//如果一直进入 刹车状态，会抱死电机，从而使得 小车 打滑，尤其过弯容易 漂移。
//如果一直进入 无驱动状态 ，电机 依靠小车的惯性一直滚动，虽然车轮与地之间是静摩擦，但达不到最大静摩擦力的效果，因而需要PWM调整占空比来点杀


#define MOTOR1_IO   PTD15
#define MOTOR2_IO   PTA19
#define MOTOR3_IO   PTA5
#define MOTOR4_IO   PTA24

#define MOTOR_FTM   FTM0
#define MOTOR1_PWM  FTM_CH3
#define MOTOR2_PWM  FTM_CH4
#define MOTOR3_PWM  FTM_CH5
#define MOTOR4_PWM  FTM_CH6

#define MOTOR1_PWM_IO  FTM0_CH3
#define MOTOR2_PWM_IO  FTM0_CH4
#define MOTOR3_PWM_IO  FTM0_CH5
#define MOTOR4_PWM_IO  FTM0_CH6

#define MOTOR_HZ    50//(20*1000)
#define MOTOR_DUTY  80

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 电机 测试实验 ,山外 提供的 电机驱动模块，管脚都加入 MOS 管隔离，信号是反相的。
                例如常规的 高电平 使能
 */
void main()
{
    printf("\n*****FTM 电机测试 测试*****\n");

    FTM_PWM_init(MOTOR_FTM, MOTOR1_PWM,MOTOR_HZ,50);      //初始化 电机 PWM
#if 0
    FTM_PWM_init(MOTOR_FTM, MOTOR2_PWM,MOTOR_HZ,100);      //初始化 电机 PWM
    FTM_PWM_init(MOTOR_FTM, MOTOR3_PWM,MOTOR_HZ,100);      //初始化 电机 PWM
    FTM_PWM_init(MOTOR_FTM, MOTOR4_PWM,MOTOR_HZ,100);      //初始化 电机 PWM

    //IO管脚配置
    gpio_init(MOTOR1_IO,GPO,LOW);
    gpio_init(MOTOR2_IO,GPO,LOW);
    gpio_init(MOTOR3_IO,GPO,LOW);
    gpio_init(MOTOR4_IO,GPO,LOW);
#endif

    while(1)
    {


#if 0
        FTM_PWM_Duty(MOTOR_FTM, MOTOR1_PWM,100);
        FTM_PWM_Duty(MOTOR_FTM, MOTOR2_PWM,70);
        FTM_PWM_Duty(MOTOR_FTM, MOTOR3_PWM,70);
        FTM_PWM_Duty(MOTOR_FTM, MOTOR4_PWM,100);
        DELAY_MS(1000);


        FTM_PWM_Duty(MOTOR_FTM, MOTOR1_PWM,70);
        FTM_PWM_Duty(MOTOR_FTM, MOTOR2_PWM,100);
        FTM_PWM_Duty(MOTOR_FTM, MOTOR3_PWM,100);
        FTM_PWM_Duty(MOTOR_FTM, MOTOR4_PWM,70);
        DELAY_MS(1000);
#endif
    }

}