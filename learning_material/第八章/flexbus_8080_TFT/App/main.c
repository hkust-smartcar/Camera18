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
 *  @note       山外LCD flexbus测试实验
 */
void  main(void)
{
    uint16  i = 0;
    Site_t site;
    LCD_init();            //初始化

    site.x = 10;
    site.y = 10;
    LCD_str(site, "WildFire", BLUE, RED);
    
    site.y = 30;
    LCD_str(site, "www.vcan123.com", BLUE, RED);

    site.y = 50;
    LCD_num(site, 20130901, BLUE, RED);
    
    site.y = 70;
        
    while(1)
    {
        LCD_num_C(site, i++, BLUE, RED);
    }
}
