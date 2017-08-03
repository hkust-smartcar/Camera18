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
 *  @note       山外三轴 陀螺仪 测试实验
 */

#define XOUT    ADC1_DM0
#define YOUT    ADC0_SE16
#define ZOUT    ADC0_SE17

#define Gyro1   ADC1_SE16
#define Gyro2   ADC1_DP0
#define Ang     ADC0_SE18

void main()
{
    uint16 x,y,z,gyro1,gyro2,ang;                           //保存ADC转换结果
    
    Site_t site = {3,5};

    adc_init(XOUT);
    adc_init(YOUT);
    adc_init(ZOUT);
    adc_init(Gyro2);         //角加速度  Angular2
    adc_init(Gyro1);         //角加速度  Angular1
    adc_init(Ang);

    LCD_init();
    LCD_str(site,"XOUT",FCOLOUR,BCOLOUR);   //显示8*16字符串
    site.y += 20;
    LCD_str(site,"YOUT",FCOLOUR,BCOLOUR);   //显示8*16字符串
    site.y += 20;    
    LCD_str(site,"ZOUT",FCOLOUR,BCOLOUR);   //显示8*16字符串
    site.y += 20;  
    LCD_str(site,"Gyro1",FCOLOUR,BCOLOUR);   //显示8*16字符串
    site.y += 20;   
    LCD_str(site,"Gyro2",FCOLOUR,BCOLOUR);   //显示8*16字符串
    site.y += 20;
    LCD_str(site,"Ang",FCOLOUR,BCOLOUR);   //显示8*16字符串
    
    site.x = 3 + 6*8; 
    
    
    while(1)
    {
        site.y = 5;
        
        x = adc_once(XOUT,ADC_8bit);
        LCD_num_C (site, x , FCOLOUR , BCOLOUR);
        site.y += 20;  
        
        y = adc_once(YOUT,ADC_8bit);
        LCD_num_C (site, y , FCOLOUR , BCOLOUR);
        site.y += 20;    
        
        z = adc_once(ZOUT,ADC_8bit);
        LCD_num_C (site, z , FCOLOUR , BCOLOUR);
        site.y += 20;   
        
        gyro1 = adc_once(Gyro1,ADC_8bit);
        LCD_num_C (site, gyro1 , FCOLOUR , BCOLOUR);
        site.y += 20;   
        
        gyro2 = adc_once(Gyro2,ADC_8bit);
        LCD_num_C (site, gyro2 , FCOLOUR , BCOLOUR);
        site.y += 20; 
        
        ang = adc_once(Ang,ADC_8bit);
        LCD_num_C (site, ang , FCOLOUR , BCOLOUR);
    }

}