/*********************************************************************************************************************
* AI8051U Opensourec Library ����AI8051U ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�??
*
* ���ļ���STC ��Դ���һ����??
*
* AI8051U ��Դ�� ���������??
* �����Ը���������������ᷢ����?? GPL��GNU General Public License���� GNUͨ�ù�������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����??/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı��?
* ����û���������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ�?? GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ��?? GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ����֤Э�� ������������Ϊ���İ汾
* ��������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ����֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������??
*
* �ļ�����          
* ��˾����          �ɶ���ɿƼ����޹��?
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MDK FOR C251
* ����ƽ̨          AI8051U
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����           ��ע
* 2024-08-01        ��W            first version
********************************************************************************************************************/

#ifndef __HEADFILE_H_
#define __HEADFILE_H_


#pragma warning disable = 115

#include "isr.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include "intrins.h"
//------STC32G SDK��
#include "AI8051u_32bit.h"
#include "zf_common_typedef.h"
#include "zf_common_clock.h"
#include "zf_common_fifo.h"
#include "zf_common_debug.h"
#include "zf_common_interrupt.h"
#include "zf_common_font.h"
#include "zf_common_function.h"

//------��ɿƼ���Ƭ����������ͷ�ļ�??
#include "zf_driver_uart.h"
#include "zf_driver_gpio.h"
#include "zf_driver_adc.h"
#include "zf_driver_spi.h"
#include "zf_driver_timer.h"
#include "zf_driver_pwm.h"

#include "zf_driver_exti.h"
#include "zf_driver_delay.h"
#include "zf_driver_eeprom.h"
#include "zf_driver_pit.h"
#include "zf_driver_encoder.h"

//------��ɿƼ���Ʒ����ͷ�ļ�??
#include "zf_device_type.h"

#include "zf_device_imu660ra.h"

#include "zf_device_ips200.h"

#include "zf_device_ble6a20.h"


//------��ɿƼ���Ʒ������ļ�
#include "seekfree_assistant.h"

#include "seekfree_assistant_interface.h"
//---�û���
#include "command.h"
#include "menu.h"
#include "key.h"
#include "IMU.h"
// #include "game.h"
// #include "snake.h"
#include "contrl.h"
#include "set_task.h"
#include "image.h"
#endif