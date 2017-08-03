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

#define SECTOR_NUM  (FLASH_SECTOR_NUM-1)         //尽量用最后面的扇区，确保安全

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外Flahs读写实验
                K60的程序Flash，每个扇区 2K或4K

                在串口里输出数据：
                一次读取32位的数据为：0x12345678
                一次读取16位的数据为：0x5678
                一次读取8位的数据为：0x78

                从串口的输出数据可以看出 ARM 是小端模式。

                注意：Flash不能过于频繁进行写操作，否则影响寿命
 */
void  main(void)
{
    uint32  data32;
    uint16  data16;
    uint8   data8;

    flash_init();                                       //初始化flash

    flash_erase_sector(SECTOR_NUM);                     //擦除扇区
                                                        //写入flash数据前，需要先擦除对应的扇区(不然数据会乱)

    if( 1 == flash_write(SECTOR_NUM, 0, 0x12345678) )   //写入数据到扇区，偏移地址为0，必须一次写入4字节
        //if是用来检测是否写入成功，写入成功了就读取
    {
        data32 =    flash_read(SECTOR_NUM, 0, uint32);  //读取4字节
        printf("一次读取32位的数据为：0x%08x\n", data32);

        data16 =    flash_read(SECTOR_NUM, 0, uint16);  //读取2字节
        printf("一次读取16位的数据为：0x%04x\n", data16);

        data8  =    flash_read(SECTOR_NUM, 0, uint8);   //读取1字节
        printf("一次读取8位的数据为：0x%02x\n", data8);
    }

    while(1);
}
