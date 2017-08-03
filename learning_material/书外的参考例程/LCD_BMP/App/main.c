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


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外LCD BMP 图像测试实验
 */
void  main(void)
{
    Site_t site     = {0, 0};                           //显示图像左上角位置

    LCD_init();

    //目前 BMP 代码 支持 24位真彩色 、 16位 RGB555 增强彩色 、1位单色 图片
    //不支持缩放显示
    SD2LCD_BMP("0:/vcan_bit.bmp",site);                 //显示 BMP 图像

    while(1)
    {


    }
}