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
 *  @note       测试 KEY 循环扫描
 */
void main()
{
    key_init(KEY_A);

    printf("\n******* GPIO 查询扫描 按键测试 *******");

    while(1)
    {
        if(key_check(KEY_A) == KEY_DOWN) //检测key状态（带延时消抖）
        {
            printf("\n按键按下");       //通过串口助手查看，提示按键按下

            DELAY_MS(500);              //调整这里的时间，会发现，时间越长，
                                        //快速双击，就没法识别第二次采集
        }
    }
}

