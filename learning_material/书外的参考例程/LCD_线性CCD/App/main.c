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

//定义存储接收CCD图像的数组
uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];

void PIT0_IRQHandler();
void bin(uint8 *bin,uint8 * img,uint8 * difimg,uint16 len,uint8 maxdif);

void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval);
void maxvar(uint8 *buf,uint16 len,uint8  maxval);

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 线性CCD 测试实验
                修改 PIT0 的定时时间即可修改曝光时间
 */
void  main(void)
{
#define BIN_MAX 0x80
    uint8 time = 6;
    Site_t site1={0,0};                         //显示图像左上角位置
    Site_t site1b={0,0+30};                     //显示图像左上角位置
    Site_t site1max={TSL1401_SIZE+20,0};        //显示最大差分值位置

    Site_t site2={0,70};                        //显示图像左上角位置
    Site_t site2b={0,70+30};                    //显示图像左上角位置

    Site_t site3={0,140};                        //显示图像左上角位置
    Site_t site3b={0,140+30};                    //显示图像左上角位置


    Size_t imgsize={TSL1401_SIZE,1};            //图像大小
    Size_t size={TSL1401_SIZE,30};              //显示区域大小
    uint8  max[TSL1401_SIZE];
    uint8  avg[TSL1401_SIZE];


    LCD_init();                                 //初始化

    //初始化 线性CCD
    tsl1401_set_addrs(TSL1401_MAX,(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[2]);
    tsl1401_init(time);                         //初始化 线性CCD ，配置 中断时间为 time
    //tsl1401_led_en(TSL1401_MAX);                //开启补光灯
    //tsl1401_led_dis(TSL1401_MAX);             //关闭补光灯

    //配置 中断函数 和 使能中断
    set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);
    enable_irq(PIT0_IRQn);



    while(1)
    {
        //采集 线性CCD 图像
        tsl1401_get_img();

        //LCD 显示图像
        LCD_Img_gray_Z(site1,size,(uint8 *)&CCD_BUFF[0],imgsize);
        LCD_Img_gray_Z(site2,size,(uint8 *)&CCD_BUFF[1],imgsize);
        LCD_Img_gray_Z(site3,size,(uint8 *)&CCD_BUFF[2],imgsize);

        //LCD 显示波形图
        LCD_wave_display(site1,size,(uint8 *)&CCD_BUFF[0],BIN_MAX,BLUE );
        LCD_wave_display(site2,size,(uint8 *)&CCD_BUFF[1],BIN_MAX,BLUE );
        LCD_wave_display(site3,size,(uint8 *)&CCD_BUFF[2],BIN_MAX,BLUE );

        //限制最大值
        maxvar((uint8 *)&CCD_BUFF[0],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[1],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);

        //求波形差分
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE,&max[0],&avg[0]);
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],TSL1401_SIZE,&max[2],&avg[2]);

        //LCD 显示差分图
        LCD_wave_display(site1,size,(uint8 *)&CCD_BUFF[TSL1401_MAX+0],max[0],GREEN);
        LCD_wave_display(site2,size,(uint8 *)&CCD_BUFF[TSL1401_MAX+1],max[1],GREEN);
        LCD_wave_display(site3,size,(uint8 *)&CCD_BUFF[TSL1401_MAX+2],max[2],GREEN);

        //根据差分波形二值化图像
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[TSL1401_MAX+0],TSL1401_SIZE,max[0]);
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[TSL1401_MAX+1],TSL1401_SIZE,max[1]);
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],(uint8 *)&CCD_BUFF[TSL1401_MAX+2],TSL1401_SIZE,max[2]);

        //LCD 显示二值化图
        LCD_Img_gray_Z(site1b,size,(uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],imgsize);
        LCD_Img_gray_Z(site2b,size,(uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],imgsize);
        LCD_Img_gray_Z(site3b,size,(uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],imgsize);


        //显示最大差分值
        LCD_num_BC(site1max, max[0],3,FCOLOUR,BCOLOUR);




    }
}


/*!
 *  @brief      PIT0中断服务函数
 *  @since      v5.0
 *  @note       由于 TSL1401_INT_TIME 配置 为 PIT0 ，因而使用 PIT0
 */
void PIT0_IRQHandler()
{
    tsl1401_time_isr();
    PIT_Flag_Clear(PIT0);
}

void maxvar(uint8 *buf,uint16 len,uint8  maxval)
{
    while(len--)
    {
        if(buf[len] > maxval)
        {
            buf[len]= maxval;
        }
    }

}


/*!
 *  @brief      计算差分绝对值
 *  @since      v5.0
 *  @note       山外差分法补充说明：差分最大值maxval 和 差分平均值avgval 这两个
                参数是为了便于定义确定阈值而加入的，可删除。差分平均值，一般返回结果
                都非常小，因此顶层用不上，建议删掉（此处保留是为了给大家验证）
 */
void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval)
{
    int8 tmp;
    uint8 max = 0;
    uint32 sum = 0;
    uint16 lentmp = len;
    while(--lentmp)                 //仅循环 len-1 次
    {
        tmp = *(src+1)- *src;
        tmp = ABS(tmp) ;
        if(tmp > max )
        {
             max = tmp;
        }

        sum += tmp;
        *dst = tmp;
        src++;
        dst++;
    }
    *dst = 0;               // 最后一个 点配置为 0
    *maxval = max;           // 返回最大绝对差值
    *avgval = (uint8)(sum/(len-1));  //前 len -1 个数的平均值
}

/*!
 *  @brief      简单的一个二值化 算法（不稳定,仅测试）
 *  @since      v5.0
 */
// diff_threshold 差分阈值 ,不同的角度，不同的环境而有所不同
//可根据 maxdif 最大差分值来配置，或者直接固定阈值
#define diff_threshold    ((maxdif> 12) ? ((maxdif*80)/100) :10)     // 差分阈值
//#define diff_threshold    10
#define safe_isolation    3
void bin(uint8 *bin,uint8 * img,uint8 * difimg,uint16 len,uint8 maxdif)
{
    uint16 tmplen = len;
    uint8  thldnum = 0;        //阈值次数
    uint8  thresholdimg;
    uint8  tmpnum;

    memset(bin,0xFF,len);  //全部当作

    while(tmplen--)
    {
        if((tmplen == 0)|| (tmplen > len))
        {
            return;
        }

        if(difimg[tmplen] > diff_threshold)                  //找到 差分阈值
        {
            thldnum++;

            //寻找最大差分阈值
            while(tmplen--)
            {
                if((tmplen == 0)|| (tmplen > len))
                {
                    return;
                }

                if(difimg[tmplen] < difimg[tmplen+1] )    //tmplen+1 为最大阈值
                {
                     break;
                }
            }

            //tmplen + 1 是 差分最大值 ，切换到 颜色扫描
            if((img[tmplen] <= img[tmplen+1]) ||(img[tmplen+1] <= img[tmplen+2]) )  // 前面 黑色 ，后面 白色
            {
                //选择 差分值最大值的前一个 作为 阈值
                thresholdimg = (img[tmplen+1] + img[tmplen+2])/2;

                //扫描下一个 高于 此阈值 (比此点更白)
                while(img[tmplen] <= thresholdimg)
                {
                    bin[tmplen] = 0;                //黑色
                    tmplen--;
                    if(tmplen == 0)      //结尾了 ,直接退出
                    {
                        if(img[tmplen] <= thresholdimg)
                        {
                             bin[tmplen] = 0;                //黑色
                        }
                        return ;
                    }
                    else if (tmplen > len)
                    {
                         return;
                    }
                }
                tmplen -= safe_isolation;
                if((tmplen == 0)|| (tmplen > len))
                {
                    return;
                }

                //等待差分值降低
                while(difimg[tmplen] > diff_threshold)
                {
                    tmplen--;
                    if((tmplen == 0)|| (tmplen > len))
                    {
                        return;
                    }
                }
            }
            else
            {
                //前白 后 黑
                if(thldnum == 1)
                {
                    //后面的 内容都是 黑色的
                    tmpnum  = tmplen + 1;
                    while(tmpnum < len)
                    {
                        bin[tmpnum] = 0;                //黑色
                        tmpnum ++;
                    }
                }
            }


        }
    }
}






