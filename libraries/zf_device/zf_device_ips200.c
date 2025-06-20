/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file             zf_device_ips200
* @company          成都逐飞科技有限公司
* @author           逐飞科技(QQ790875685)
* @version          查看doc内version文件 版本说明
* @Software         MounRiver Studio V1.51
* @Target core      CH32V307VCT6
* @Taobao           https://seekfree.taobao.com/
* @date             2021-11-25
* @note             接线定义：
*                   ------------------------------------
*                   模块管脚                                       单片机管脚
*                   SCL                 查看 zf_device_ips200.h 中 IPS200_SCL_PIN 宏定义
*                   SDA                 查看 zf_device_ips200.h 中 IPS200_SDA_PIN 宏定义
*                   RST                 查看 zf_device_ips200.h 中 IPS200_RST_PIN 宏定义
*                   DC                  查看 zf_device_ips200.h 中 IPS200_DC_PIN 宏定义
*                   CS                  查看 zf_device_ips200.h 中 IPS200_CS_PIN 宏定义
*                   BLK                 查看 zf_device_ips200.h 中 IPS200_BLK_PIN 宏定义
*                   电源引脚
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   最大分辨率135*240
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_font.h"
#include "zf_common_function.h"
#include "zf_driver_delay.h"
#include "zf_driver_spi.h"

#include "zf_device_ips200.h"

#define IPS_LEN   320
#define IPS_WIGHT 240

uint16 ips200_pencolor = IPS200_DEFAULT_PENCOLOR;
uint16 ips200_bgcolor = IPS200_DEFAULT_BGCOLOR;

ips200_dir_enum ips200_display_dir = IPS200_DEFAULT_DISPLAY_DIR;
uint16 ips200_x_max = IPS_LEN;
uint16 ips200_y_max = IPS_WIGHT;

#if IPS200_USE_SOFT_SPI
static soft_spi_info_struct             ips200_spi;
#define ips200_write_8bit_data(dat)    soft_spi_write_8bit(&ips200_spi, dat)
#define ips200_write_16bit_data(dat)   soft_spi_write_16bit(&ips200_spi, dat)
#else
#define ips200_write_8bit_data(dat)    spi_write_8bit(IPS200_SPI, dat)
#define ips200_write_16bit_data(dat)   spi_write_16bit(IPS200_SPI, dat)

#endif

//-------------------------------------------------------------------------------------------------------------------
// @brief       写命令 内部调用
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void ips200_write_index (uint8 dat)
{
    IPS200_CS(1);
    IPS200_CS(0);
    IPS200_DC(0);
    ips200_write_8bit_data(dat);
    IPS200_DC(1);
    IPS200_CS(1);
    IPS200_CS(0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置显示区域 内部调用
// @param       x1              起始x轴坐标
// @param       y1              起始y轴坐标
// @param       x2              结束x轴坐标
// @param       y2              结束y轴坐标
// @return      void
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void ips200_set_region (uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
    //zf_assert(x1 < ips200_x_max);
    //zf_assert(y1 < ips200_y_max);
    //zf_assert(x2 < ips200_x_max);
    //zf_assert(y2 < ips200_y_max);


	ips200_write_index(0x2a);                                               // 列地址设置
	ips200_write_16bit_data(x1);
	ips200_write_16bit_data(x2);
	ips200_write_index(0x2b);                                               // 行地址设置
	ips200_write_16bit_data(y1);
	ips200_write_16bit_data(y2);
	ips200_write_index(0x2c);                                               // 储存器写

}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶清屏函数
// @param       color           颜色格式 RGB565 或者可以使用 zf_common_font.h 内常用颜色宏定义
// @return      void
// Sample usage:                ips200_clear(YELLOW);
//-------------------------------------------------------------------------------------------------------------------
void ips200_clear (uint16 color)
{
    uint16 i, j;
    ips200_set_region(0, 0, ips200_x_max - 1, ips200_y_max - 1);
	
	for(i = 0; i < ips200_x_max; i ++)
    {
        for (j = 0; j < ips200_y_max; j ++)
        {
            ips200_write_16bit_data(color);              
        }
    }

}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置显示方向 这个函数只有在初始化屏幕之前调用才生效
// @param       dir             显示方向  参照 zf_device_ips200.h 内 ips200_dir_enum 枚举体定义
// @return      void
// Sample usage:                ips200_set_dir(IPS200_CROSSWISE);
//-------------------------------------------------------------------------------------------------------------------
void ips200_set_dir (ips200_dir_enum dir)
{
    ips200_display_dir = dir;
//    if(dir < 2)
//    {
//        ips200_x_max = IPS_WIGHT;
//        ips200_y_max = IPS_LEN;
//    }
//    else
//    {
//        ips200_x_max = IPS_LEN;
//        ips200_y_max = IPS_WIGHT;
//    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       设置显示颜色
// @param       pen             颜色格式 RGB565 或者可以使用 zf_common_font.h 内常用颜色宏定义
// @param       bgcolor         颜色格式 RGB565 或者可以使用 zf_common_font.h 内常用颜色宏定义
// @return      void
// Sample usage:                ips200_set_color(WHITE,BLACK);
//-------------------------------------------------------------------------------------------------------------------
void ips200_set_color (uint16 pen, uint16 bgcolor)
{
    ips200_pencolor = pen;
    ips200_bgcolor = bgcolor;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶画点
// @param       x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
// @param       dat             需要显示的颜色
// @return      void
// Sample usage:                ips200_draw_point(0,0,RED);                     // 坐标 0,0 画一个红色的点
//-------------------------------------------------------------------------------------------------------------------
void ips200_draw_point (uint16 x,uint16 y,uint16 color)
{
    //zf_assert(x < ips200_x_max);
    //zf_assert(y < ips200_y_max);

    ips200_set_region(x,y,x,y);
    ips200_write_16bit_data(color);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示字符
// @param       x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
// @param       dat             需要显示的字符
// @return      void
// Sample usage:                ips200_show_char(0,0,'x');                      // 坐标 0,0 写一个字符 x
//-------------------------------------------------------------------------------------------------------------------
void ips200_show_char (uint16 x,uint16 y,const char dat)
{
    //zf_assert(x < ips200_x_max);
    //zf_assert(y < ips200_y_max);

    uint8 i,j;
    uint8 temp;

    for(i=0; i<16; i++)
    {
        ips200_set_region(x,y+i,x+7,y+i);
        temp = tft_ascii[dat-32][i];                                            //减 32 因为是取模是从空格开始取得 空格在 ascii 中序号是 32
        for(j=0; j<8; j++)
        {
            if(temp&0x01)   
                ips200_write_16bit_data(ips200_pencolor);
            else
                ips200_write_16bit_data(ips200_bgcolor);
            temp>>=1;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示字符串
// @param       x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
// @param       dat             需要显示的字符串
// @return      void
// Sample usage:                ips200_show_string(0,0,"seekfree");
//-------------------------------------------------------------------------------------------------------------------
void ips200_show_string (uint16 x,uint16 y,const char dat[])
{
    //zf_assert(x < ips200_x_max);
    //zf_assert(y < ips200_y_max);
    
    uint16 j = 0;
    while(dat[j] != '\0')
    {
        ips200_show_char(x+8*j,y,dat[j]);
        j++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      液晶显示8位有符号
// @param       x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
//  @param      dat             需要显示的变量，数据类型int8
//  @return     void
//  Sample usage:               ips200_show_int8(0,0,x);                        // x为 int8 类型
//-------------------------------------------------------------------------------------------------------------------
void ips200_show_int8(uint16 x,uint16 y,int8 dat)
{
    //zf_assert(x < ips200_x_max);
    //zf_assert(y < ips200_y_max);

    uint8 a[3];
    uint8 i;
    if(dat<0)
    {
        ips200_show_char(x,y,'-');
        dat = -dat;
    }
    else
        ips200_show_char(x,y,' ');
    
    a[0] = dat/100;
    a[1] = dat/10%10;
    a[2] = dat%10;
    i = 0;
    while(i<3)
    {
        ips200_show_char(x+(8*(i+1)),y,(uint8)('0' + a[i]));
        i++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示8位无符号
// @param       x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
// @param       dat             需要显示的变量 数据类型uint8
// @return      void
// Sample usage:                ips200_show_uint8(0,0,x);                       // x 为 uint8 类型
//-------------------------------------------------------------------------------------------------------------------
void ips200_show_uint8(uint16 x,uint16 y,uint8 dat)
{
    //zf_assert(x < ips200_x_max);
    //zf_assert(y < ips200_y_max);

    uint8 a[3];
    uint8 i;
    
    a[0] = dat/100;
    a[1] = dat/10%10;
    a[2] = dat%10;
    i = 0;
    while(i<3)
    {
        ips200_show_char(x+(8*i),y,(uint8)('0' + a[i]));
        i++;
    }
    
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示16位有符号
// @param       x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
// @param       dat             需要显示的变量 数据类型int16
// @return      void
// Sample usage:                ips200_show_int16(0,0,x);                       // x 为 int16 类型
//-------------------------------------------------------------------------------------------------------------------
void ips200_show_int16(uint16 x,uint16 y,int16 dat)
{
    //zf_assert(x < ips200_x_max);
    //zf_assert(y < ips200_y_max);

    uint8 a[5];
    uint8 i;

    if(dat<0)
    {
        ips200_show_char(x,y,'-');
        dat = -dat;
    }
    else    ips200_show_char(x,y,' ');

    a[0] = dat/10000;
    a[1] = dat/1000%10;
    a[2] = dat/100%10;
    a[3] = dat/10%10;
    a[4] = dat%10;
    
    i = 0;
    while(i<5)
    {
        ips200_show_char(x+(8*(i+1)),y,(uint8)('0' + a[i]));
        i++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示16位无符号
// @param       x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
// @param       dat             需要显示的变量，数据类型uint16
// @return      void
// Sample usage:                ips200_show_uint16(0,0,x);                      // x 为 uint16 类型
//-------------------------------------------------------------------------------------------------------------------
void ips200_show_uint16(uint16 x,uint16 y,uint16 dat)
{
    //zf_assert(x < ips200_x_max);
    //zf_assert(y < ips200_y_max);

    uint8 a[5];
    uint8 i;

    a[0] = dat/10000;
    a[1] = dat/1000%10;
    a[2] = dat/100%10;
    a[3] = dat/10%10;
    a[4] = dat%10;
    
    i = 0;
    while(i<5)
    {
        ips200_show_char(x+(8*i),y,(uint8)('0' + a[i]));
        i++;
    }
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       液晶显示浮点数(去除整数部分无效的0)
// @param       x               坐标x方向的起点 参数范围 [0, ips200_x_max-1]
// @param       y               坐标y方向的起点 参数范围 [0, ips200_y_max-1]
// @param       dat             需要显示的变量，数据类型float或double
// @param       num             整数位显示长度   最高10位
// @param       pointnum        小数位显示长度   最高6位
// @return      void
// Sample usage:                ips200_showfloat(0,0,x,2,3);                    // 显示浮点数   整数显示2位   小数显示三位
// @note                        特别注意当发现小数部分显示的值与你写入的值不一样的时候，
//                              可能是由于浮点数精度丢失问题导致的，这并不是显示函数的问题，
//                              有关问题的详情，请自行百度学习   浮点数精度丢失问题。
//                              负数会显示一个 ‘-’号   正数显示一个空格
//-------------------------------------------------------------------------------------------------------------------
void ips200_show_float(uint16 x,uint16 y,double dat,uint8 num,uint8 pointnum)
{
    //zf_assert(x < ips200_x_max);
    //zf_assert(y < ips200_y_max);

    uint8   length;
    int8    buff[34];
    int8    start,end,point;

    if(6<pointnum)  pointnum = 6;
    if(10<num)      num = 10;
        
    if(0>dat)   length = zf_sprintf( &buff[0],(const int8 *)"%f",dat);        // 负数
    else
    {
        length = zf_sprintf( &buff[1],(const int8 *)"%f",dat);
        length++;
    }
    point = length - 7;                                                         // 计算小数点位置
    start = point - num - 1;                                                    // 计算起始位
    end = point + pointnum + 1;                                                 // 计算结束位
    while(0>start)                                                              // 整数位不够  末尾应该填充空格
    {
        buff[end] = ' ';
        end++;
        start++;
    }
    
    if(0>dat)   buff[start] = '-';
    else        buff[start] = ' ';
    
    buff[end] = '\0';

    ips200_show_string(x, y, (const char *)buff);                               // 显示数字
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       1.14寸 IPS液晶初始化
// @return      void
// Sample usage:                ips200_init();
//-------------------------------------------------------------------------------------------------------------------
void ips200_init (void)
{
#if IPS200_USE_SOFT_SPI
    soft_spi_init(&ips200_spi, 0, IPS200_SOFT_SPI_DELAY, IPS200_SCL_PIN, IPS200_SDA_PIN, SOFT_SPI_PIN_NULL, SOFT_SPI_PIN_NULL);
#else
    //zf_assert(IPS200_SPI != (IPS200_SCL_PIN&0xF000>>12));
    //zf_assert(IPS200_SPI != (IPS200_SDA_PIN&0xF000>>12));
    spi_init(IPS200_SPI, SPI_MODE0, IPS200_SPI_SPEED, IPS200_SCL_PIN, IPS200_SDA_PIN, SPI_MISO_NULL, SPI_CS_NULL);
#endif

//    gpio_init(IPS200_DC_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
//    gpio_init(IPS200_RST_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
//    gpio_init(IPS200_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
//    gpio_init(IPS200_BLK_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    ips200_set_dir(ips200_display_dir);
    ips200_set_color(ips200_pencolor, ips200_bgcolor);

    IPS200_RST(0);
    system_delay_ms(200);

    IPS200_RST(1);
    system_delay_ms(100);

    ips200_write_index(0x36);
    system_delay_ms(100);
    if      (ips200_display_dir==0) ips200_write_8bit_data(0x00);
    else if (ips200_display_dir==1) ips200_write_8bit_data(0xC0);
    else if (ips200_display_dir==2) ips200_write_8bit_data(0x70);
    else                            ips200_write_8bit_data(0xA0);

    ips200_write_index(0x3A);
    ips200_write_8bit_data(0x05);

    ips200_write_index(0xB2);
    ips200_write_8bit_data(0x0C);
    ips200_write_8bit_data(0x0C);
    ips200_write_8bit_data(0x00);
    ips200_write_8bit_data(0x33);
    ips200_write_8bit_data(0x33);

    ips200_write_index(0xB7);
    ips200_write_8bit_data(0x35);

    ips200_write_index(0xBB);
    ips200_write_8bit_data(0x37);

    ips200_write_index(0xC0);
    ips200_write_8bit_data(0x2C);

    ips200_write_index(0xC2);
    ips200_write_8bit_data(0x01);

    ips200_write_index(0xC3);
    ips200_write_8bit_data(0x12);

    ips200_write_index(0xC4);
    ips200_write_8bit_data(0x20);

    ips200_write_index(0xC6);
    ips200_write_8bit_data(0x0F);

    ips200_write_index(0xD0);
    ips200_write_8bit_data(0xA4);
    ips200_write_8bit_data(0xA1);

    ips200_write_index(0xE0);
    ips200_write_8bit_data(0xD0);
    ips200_write_8bit_data(0x04);
    ips200_write_8bit_data(0x0D);
    ips200_write_8bit_data(0x11);
    ips200_write_8bit_data(0x13);
    ips200_write_8bit_data(0x2B);
    ips200_write_8bit_data(0x3F);
    ips200_write_8bit_data(0x54);
    ips200_write_8bit_data(0x4C);
    ips200_write_8bit_data(0x18);
    ips200_write_8bit_data(0x0D);
    ips200_write_8bit_data(0x0B);
    ips200_write_8bit_data(0x1F);
    ips200_write_8bit_data(0x23);

    ips200_write_index(0xE1);
    ips200_write_8bit_data(0xD0);
    ips200_write_8bit_data(0x04);
    ips200_write_8bit_data(0x0C);
    ips200_write_8bit_data(0x11);
    ips200_write_8bit_data(0x13);
    ips200_write_8bit_data(0x2C);
    ips200_write_8bit_data(0x3F);
    ips200_write_8bit_data(0x44);
    ips200_write_8bit_data(0x51);
    ips200_write_8bit_data(0x2F);
    ips200_write_8bit_data(0x1F);
    ips200_write_8bit_data(0x1F);
    ips200_write_8bit_data(0x20);
    ips200_write_8bit_data(0x23);

    ips200_write_index(0x21);

    ips200_write_index(0x11);
    system_delay_ms(120);

    ips200_write_index(0x29);

    ips200_clear(ips200_bgcolor);
}
