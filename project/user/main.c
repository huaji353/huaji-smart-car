/*********************************************************************************************************************
* AI8051U Opensourec Library 即（AI8051U 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是STC 开源库的一部分
*
* AI8051U 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MDK FOR C251
* 适用平台          AI8051U
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者           备注
* 2024-08-01        大W            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"

void main()
{
    clock_init(SYSTEM_CLOCK_40M);				// 务必保留
		debug_init();								// 务必保留

    to_init();

    // 此处编写用户代码 例如外设初始化代码等
    // 陀螺仪校准（收集200个样本）


    while(1)
		{
		// 此处编写需要循环执行的代码
    task_start(0);
    menu_run();                         // 运行菜单系统
    task_end(1);
    // 显示实时VCC值
    task_start(1);
    task_delay(10);
    display_vcc();
    encoder_integrate();
    //检测霍尔
    // task_if(!gpio_get_level(HALL_PIN))
    // {
    //     buzzer_time(100);
    // }

    task_end(1);
    //开启逐飞传输
    task_start(2);
    task_delay(5);
    seekfree_assistant_oscilloscope_code();
    task_end(1);

    task_start(3);//按键判断
    button_entry();
    task_delay(1);
    task_end(1);
    //读取电感值
    task_start(4);
    adc_main_read();
    // inductance_normalization_max();
    inductance_difference_ratio_sum();
    task_delay(5);
    task_end(1);
    }
}


