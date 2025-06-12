#include "zf_common_headfile.h"
//横电感1 竖电感2 竖电感3 横电感4
float inductance_horizontal_1,inductance_vertical_2,inductance_vertical_3,inductance_horizontal_4,inductance_horizontal_middle;
float vcc;
uint8 encoder_integral_flag = 0;
int16 L_encoder = 0;
int16 R_encoder = 0;
int16 encoder_data_dir_1 = 0;
int16 encoder_data_dir_2 = 0;
uint32 encoder_integral_1  = 0;
uint32 encoder_integral_2  = 0;
float speed_L,speed_R;//左右轮目标速度
char txt[32];
uint8 eeprom_PID[]={
//角度环      P高位   P低位  P低2位       I高位  I低位  I低2位        D高位  D低位  D低2位
              0x00,  0x00,  0x00,        0x00,  0x00,  0x00,        0x00,  0x00,  0x00, 
//角速度环    P高位   P低位  P低2位       I高位  I低位  I低2位        D高位  D低位  D低2位
              0x00,  0x00,  0x00,        0x00,  0x00,  0x00,        0x00,  0x00,  0x00,
//左轮电机    P高位   P低位  P低2位       I高位  I低位  I低2位        D高位  D低位  D低2位
              0x00,  0x00,  0x00,        0x00,  0x00,  0x00,        0x00,  0x00,  0x00,
//右轮电机    P高位   P低位  P低2位       I高位  I低位  I低2位        D高位  D低位  D低2位
              0x00,  0x00,  0x00,        0x00,  0x00,  0x00,        0x00,  0x00,  0x00,
//预留参数                                                                               启动计数
              0x00,  0x00,  0x00,        0x00,  0x00,  0x00,        0x00,  0x00,  0x00,  0x00
};//索引说明：
//eeprom_PID[0]~[8]为角度环P,I,D参数
//eeprom_PID[9]~[17]为角速度环P,I,D参数
//eeprom_PID[18]~[26]为左轮电机环P,I,D参数
//eeprom_PID[27]~[35]为右轮电机环P,I,D参数
//eeprom_PID[36]~[44]为预留参数
//eeprom_PID[45]为启动计数
uint8 inductance_max_min[]={
//  横电感1最大值          竖电感2最大值            竖电感3最大值           横电感4最大值
    0x00,0x00,0x00    ,    0x00,0x00,0x00    ,    0x00,0x00,0x00    ,    0x00,0x00,0x00 ,
//  横电感1最小值          竖电感2最小值            竖电感3最小值           横电感4最小值
    0x00,0x00,0x00    ,    0x00,0x00,0x00    ,    0x00,0x00,0x00    ,    0x00,0x00,0x00
};
//time采集
uint32 time_start;
uint32 time_end;
uint32 time_delta;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手示波器初始化函数，用于初始化逐飞助手和无线串口输出
// 参数说明     void
// 返回参数     void
// 使用示例     seekfree_assistant_oscilloscope_init();
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_oscilloscope_init(void)
{
    // 逐飞助手初始化
	seekfree_assistant_init();
	// 设置无线串口输出
	seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_BLE6A20);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手参数设置函数，用于调整PID参数和显示调试信息
// 参数说明     void
// 返回参数     void
// 使用示例     seekfree_assistant_set_parameter();
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_set_parameter(void)
{
    switch((int)seekfree_assistant_parameter[0])//根据通道0的值选择要调的环
    {
        case 0:
            target_speed=seekfree_assistant_parameter[4];
            target_angle=seekfree_assistant_parameter[5];
            break;
        case 1://角度环
            PID_angle_P=seekfree_assistant_parameter[1];
            PID_angle_I=seekfree_assistant_parameter[2];
            PID_angle_D=seekfree_assistant_parameter[3];
            target_speed=seekfree_assistant_parameter[4];
            target_angle=seekfree_assistant_parameter[5];
            PID_angle.kp=PID_angle_P;
            PID_angle.ki=PID_angle_I;
            PID_angle.kd=PID_angle_D;
            break;
        case 2://角速度环
            PID_angle_speed_P=seekfree_assistant_parameter[1];
            PID_angle_speed_I=seekfree_assistant_parameter[2];
            PID_angle_speed_D=seekfree_assistant_parameter[3];
            target_speed=seekfree_assistant_parameter[4];
            target_angle=seekfree_assistant_parameter[5];
            PID_angle_speed.kp=PID_angle_speed_P;
            PID_angle_speed.ki=PID_angle_speed_I;
            PID_angle_speed.kd=PID_angle_speed_D;
            break;
        case 3://左轮速度环
            PID_MOTOR_L_P=seekfree_assistant_parameter[1];
            PID_MOTOR_L_I=seekfree_assistant_parameter[2];
            PID_MOTOR_L_D=seekfree_assistant_parameter[3];
            target_speed=seekfree_assistant_parameter[4];
            target_angle=seekfree_assistant_parameter[5];
            PID_MOTOR_L.kp=PID_MOTOR_L_P;
            PID_MOTOR_L.ki=PID_MOTOR_L_I;
            PID_MOTOR_L.kd=PID_MOTOR_L_D;
            break;
        case 4://右轮速度环
            PID_MOTOR_R_P=seekfree_assistant_parameter[1];
            PID_MOTOR_R_I=seekfree_assistant_parameter[2];
            PID_MOTOR_R_D=seekfree_assistant_parameter[3];
            target_speed=seekfree_assistant_parameter[4];
            target_angle=seekfree_assistant_parameter[5];
            PID_MOTOR_R.kp=PID_MOTOR_R_P;
            PID_MOTOR_R.ki=PID_MOTOR_R_I;
            PID_MOTOR_R.kd=PID_MOTOR_R_D;
            break;
        case 5://保存参数
            eeprom_write();
            seekfree_assistant_parameter[0]=0;
            break;
        default:
            seekfree_assistant_parameter[0]=0;
            break;
    }
    switch((int)seekfree_assistant_parameter[7])//通道7选择显示模式
    {
        case 0:
            uart_printf(UART_4, "angle: %f, speed: %f, motor_L: %f, motor_R: %f\r\n", target_angle, target_speed, motor_output_L, motor_output_R);
            break;
        case 1:
            uart_printf(UART_4, "PID_angle_P: %f, PID_angle_I: %f, PID_angle_D: %f\r\n", PID_angle_P, PID_angle_I, PID_angle_D);
            break;
        case 2:
            uart_printf(UART_4, "PID_angle_speed_P: %f, PID_angle_speed_I: %f, PID_angle_speed_D: %f\r\n", PID_angle_speed_P, PID_angle_speed_I, PID_angle_speed_D);
            break;
        case 3:
            uart_printf(UART_4, "PID_MOTOR_L_P: %f, PID_MOTOR_L_I: %f, PID_MOTOR_L_D: %f\r\n", PID_MOTOR_L_P, PID_MOTOR_L_I, PID_MOTOR_L_D);
            break;
        case 4:
            uart_printf(UART_4, "PID_MOTOR_R_P: %f, PID_MOTOR_R_I: %f, PID_MOTOR_R_D: %f\r\n", PID_MOTOR_R_P, PID_MOTOR_R_I, PID_MOTOR_R_D);
            break;
        case 5://速度环调试修正
            uart_printf(UART_4, "encoder_data_dir_1: %d, encoder_data_dir_2: %d, motor_output_L: %f, motor_output_R: %f ,speed: %f\r\n"
            , encoder_data_dir_1, encoder_data_dir_2, motor_output_L, motor_output_R, target_speed);
            speed_debug();
            break;
        case 6://角度环调试修正
            uart_printf(UART_4, "encoder_data_dir_1: %d, encoder_data_dir_2: %d, motor_output_L: %f, motor_output_R: %f ,angle: %f, yaw: %f, angle_output: %f, angle_speed_output: %f\r\n"
            , encoder_data_dir_1, encoder_data_dir_2, motor_output_L, motor_output_R, target_angle, euler.yaw ,angle_output,angle_speed_output);
            angle_debug();
            break;
        case 7:
            uart_printf(UART_4, "inductance_horizontal_1: %f, inductance_vertical_2: %f, inductance_vertical_3: %f, inductance_horizontal_4: %f\r\n"
            , inductance_horizontal_1, inductance_vertical_2, inductance_vertical_3, inductance_horizontal_4);
            core_trace();
            break;
        default:
            seekfree_assistant_parameter[7]=0;
            break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手示波器数据分析函数，用于处理和分析示波器数据
// 参数说明     void
// 返回参数     void
// 使用示例     seekfree_assistant_oscilloscope_code();
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_oscilloscope_code(void)
{
	uint8 i;
    if(!seekfree_flag)return;
    seekfree_assistant_data_analysis();
    // 通过DEBUG串口输出数据
    printf("receive data : ");
    // 通过DEBUG串口输出参数
    for(i = 0; i < SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT; i++)
    {
        printf("%f ", seekfree_assistant_parameter[i]);
    }
    printf("\r\n");
    seekfree_assistant_set_parameter();
    // seekfree_assistant_oscilloscope_data.dat[0] = x;
    // seekfree_assistant_oscilloscope_data.dat[1] = y;
    // seekfree_assistant_oscilloscope_data.dat[2] = z;
    // seekfree_assistant_oscilloscope_data.dat[3] = w;
    // seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
}

//编码器函数定义区
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     PIT 的周期中断处理函数 这个函数将在 PIT 对应的定时器中断调用 详见 isr.c
// 参数说明     void
// 返回参数     void
// 使用示例     pit_handler();
//-------------------------------------------------------------------------------------------------------------------
void pit_handler (void)
{
    encoder_data_dir_1 = abs(encoder_get_count(ENCODER_DIR_1));                  // 获取编码器计数
    encoder_data_dir_2 = abs(encoder_get_count(ENCODER_DIR_2));              	// 获取编码器计数

    encoder_clear_count(ENCODER_DIR_1);                                		// 清空编码器计数
    encoder_clear_count(ENCODER_DIR_2);                             		// 清空编码器计数
}
void encoder_init(void)
{
    encoder_dir_init(ENCODER_DIR_1, ENCODER_DIR_DIR_1, ENCODER_DIR_PULSE_1);   	// 初始化编码器模块与引脚 带方向增量编码器模式
    encoder_dir_init(ENCODER_DIR_2, ENCODER_DIR_DIR_2, ENCODER_DIR_PULSE_2);    // 初始化编码器模块与引脚 带方向增量编码器模式
                // 设置定时器1中断回调函数
	tim1_irq_handler = pit_handler;
	
    pit_ms_init(PIT_CH, 100);                                                   // 初始化 PIT 为周期中断 100ms 周期
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     编码器积分函数，用于累加编码器计数值
// 参数说明     void
// 返回参数     void
// 使用示例     encoder_integrate();
//-------------------------------------------------------------------------------------------------------------------
void encoder_integrate(void)
{
    if(encoder_integral_flag){
    encoder_integral_1  += encoder_data_dir_1;
    encoder_integral_2  += encoder_data_dir_2;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     编码器数据显示函数，用于在IPS屏幕上显示编码器数据
// 参数说明     void
// 返回参数     void
// 使用示例     encoder_text();
//-------------------------------------------------------------------------------------------------------------------
void encoder_text(void)
{
    sprintf(txt, "encoder1: %5d", encoder_data_dir_1);
    ips200_show_string(0,0,txt);
    sprintf(txt, "encoder2: %5d", encoder_data_dir_2);
    ips200_show_string(0,16,txt);
}

//蜂鸣器函数定义区
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     蜂鸣器控制函数，用于控制蜂鸣器发声时间
// 参数说明     time: 蜂鸣器发声时间(ms)
// 返回参数     void
// 使用示例     buzzer_time(100);
//-------------------------------------------------------------------------------------------------------------------
void buzzer_time(uint16 time)
{
    gpio_set_level(IO_P07, 1);
    system_delay_ms(time);
    gpio_set_level(IO_P07, 0);
    system_delay_ms(time);
}

//霍尔函数定义区
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     霍尔传感器初始化函数，用于初始化霍尔传感器引脚
// 参数说明     void
// 返回参数     void
// 使用示例     hall_init();
//-------------------------------------------------------------------------------------------------------------------
void hall_init(void)
{
    gpio_init(HALL_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     霍尔传感器数据显示函数，用于在IPS屏幕上显示霍尔传感器状态
// 参数说明     void
// 返回参数     void
// 使用示例     hall_text();
//-------------------------------------------------------------------------------------------------------------------
void hall_text(void)
{
    sprintf(txt, "hall: %d", gpio_get_level(HALL_PIN));
    ips200_show_string(0,0,txt);
}   

//ADC函数定义区
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ADC初始化函数，用于初始化所有ADC通道
// 参数说明     void
// 返回参数     void
// 使用示例     adc_main_init();
//-------------------------------------------------------------------------------------------------------------------
void adc_main_init(void)
{
    adc_init(ADC_CHANNEL1, ADC_12BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL2, ADC_12BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL3, ADC_12BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL4, ADC_12BIT);                                           // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL5, ADC_12BIT);                                           // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL6, ADC_12BIT);                                           // 初始化对应 ADC 通道为对应精度
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ADC数据读取函数，用于读取所有电感值和电压值
// 参数说明     void
// 返回参数     void
// 使用示例     adc_main_read();
//-------------------------------------------------------------------------------------------------------------------
void adc_main_read(void)
{
    inductance_horizontal_1= (float)adc_convert(ADC_CHANNEL1);
    inductance_vertical_2= (float)adc_convert(ADC_CHANNEL2);
    inductance_horizontal_middle =(float)adc_convert(ADC_CHANNEL6);
    inductance_vertical_3= (float)adc_convert(ADC_CHANNEL3);
    inductance_horizontal_4= (float)adc_convert(ADC_CHANNEL4);
    vcc= (float)adc_convert(ADC_CHANNEL5);  
    // seekfree_assistant_oscilloscope_code(adc1,adc2,adc3,adc4);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ADC数据显示函数，用于在IPS屏幕上显示ADC数据
// 参数说明     void
// 返回参数     void
// 使用示例     adc_text();
//-------------------------------------------------------------------------------------------------------------------
void adc_text(void)
{
    sprintf(txt, "inductance_horizontal_1: %08.2f", inductance_horizontal_1);
    ips200_show_string(0,0,txt);
    sprintf(txt, "inductance_vertical_2: %08.2f", inductance_vertical_2);
    ips200_show_string(0,16,txt);
    sprintf(txt, "inductance_vertical_3: %08.2f", inductance_vertical_3);
    ips200_show_string(0,32,txt);
    sprintf(txt, "inductance_horizontal_4: %08.2f", inductance_horizontal_4);
    ips200_show_string(0,48,txt);
    sprintf(txt, "inductance_horizontal_m: %08.2f", inductance_horizontal_middle);
    ips200_show_string(0,64,txt); 
    sprintf(txt, "horizontal_diff_ratio_sum: %.2f", horizontal_diff_ratio_sum);
    ips200_show_string(0,80,txt);
    sprintf(txt, "vertical_diff_ratio_sum: %.2f", vertical_diff_ratio_sum);
    ips200_show_string(0,96,txt);
}

//IPS200函数定义区
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IPS显示测试函数，用于测试IPS显示功能
// 参数说明     void
// 返回参数     void
// 使用示例     ips_text();
//-------------------------------------------------------------------------------------------------------------------
void ips_text(void)
{
	ips200_show_string(0,0,"HI,this is a test");
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     串口打印函数，用于通过串口输出格式化字符串
// 参数说明     uart_n: 串口编号
//              format: 格式化字符串
//              ...: 可变参数列表
// 返回参数     void
// 使用示例     uart_printf(UART_4, "test: %d", 123);
//-------------------------------------------------------------------------------------------------------------------
void uart_printf(uart_index_enum uart_n,const char* format,...)
{
  char buffer[256];
  va_list args;
  va_start(args,format);
  vsprintf(buffer,format,args);
  uart_write_string (uart_n, buffer);
  va_end(args);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     串口打印测试函数，用于测试串口通信功能
// 参数说明     void
// 返回参数     void
// 使用示例     uart_printf_test();
//-------------------------------------------------------------------------------------------------------------------
void uart_printf_test(void)
{
    uint8 data_len = 0;
    uart_printf(UART_4, "test, %d\n", 123);
    data_len = ble6a20_read_buffer(txt, 16);
    if(data_len > 0)
    {   
        uart_printf(UART_4, "data_len: %d\n", data_len);
        uart_printf(UART_4, "recv txt: %s\n", txt);
    }
    system_delay_ms(100);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电压显示函数，用于在屏幕上显示当前电压值
// 参数说明     void
// 返回参数     void
// 使用示例     display_vcc();
//-------------------------------------------------------------------------------------------------------------------
void display_vcc(void) {
    // 格式化电压字符串
    vcc= adc_convert(ADC_CHANNEL5)/10.491;
#if VCC_ALARM_FLAG
    if(vcc<11.1&&vcc>1.5){
        buzzer_time(100);
    }
#endif  
    sprintf(txt, "VCC: %.2fV", vcc) ;
    // 在屏幕右上角显示VCC值
    ips200_show_string(150, 10, txt);
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电感值写入EEPROM函数，用于存储电感的最大最小值
// 参数说明     void
// 返回参数     void
// 使用示例     eeprom_write_inductance_max_min();
//-------------------------------------------------------------------------------------------------------------------
void eeprom_write_inductance_max_min(void)
{
    uint32 hex_value;

    hex_value = (uint32)inductance_horizontal_1_max;
    inductance_max_min[0] = (uint8)(hex_value >> 16);
    inductance_max_min[1] = (uint8)(hex_value >> 8);
    inductance_max_min[2] = (uint8)(hex_value);

    hex_value = (uint32)inductance_vertical_2_max;
    inductance_max_min[3] = (uint8)(hex_value >> 16);
    inductance_max_min[4] = (uint8)(hex_value >> 8);
    inductance_max_min[5] = (uint8)(hex_value);
    
    hex_value = (uint32)inductance_vertical_3_max;
    inductance_max_min[6] = (uint8)(hex_value >> 16);
    inductance_max_min[7] = (uint8)(hex_value >> 8);
    inductance_max_min[8] = (uint8)(hex_value);
    
    hex_value = (uint32)inductance_horizontal_4_max;
    inductance_max_min[9] = (uint8)(hex_value >> 16);
    inductance_max_min[10] = (uint8)(hex_value >> 8);
    inductance_max_min[11] = (uint8)(hex_value);
    
    // 存储最小值
    hex_value = (uint32)inductance_horizontal_1_min;
    inductance_max_min[12] = (uint8)(hex_value >> 16);
    inductance_max_min[13] = (uint8)(hex_value >> 8);
    inductance_max_min[14] = (uint8)(hex_value);
    
    hex_value = (uint32)inductance_vertical_2_min;
    inductance_max_min[15] = (uint8)(hex_value >> 16);
    inductance_max_min[16] = (uint8)(hex_value >> 8);
    inductance_max_min[17] = (uint8)(hex_value);
    
    hex_value = (uint32)inductance_vertical_3_min;
    inductance_max_min[18] = (uint8)(hex_value >> 16);
    inductance_max_min[19] = (uint8)(hex_value >> 8);
    inductance_max_min[20] = (uint8)(hex_value);
    
    hex_value = (uint32)inductance_horizontal_4_min;
    inductance_max_min[21] = (uint8)(hex_value >> 16);
    inductance_max_min[22] = (uint8)(hex_value >> 8);
    inductance_max_min[23] = (uint8)(hex_value);
    // 写入EEPROM（从地址eeprom后开始，避开PID参数区域）
    iap_write_buff(sizeof(eeprom_PID)+1, inductance_max_min, sizeof(inductance_max_min));
}

//EEPROM写入函数定义区
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     EEPROM写入函数，用于将PID参数写入EEPROM
// 参数说明     void
// 返回参数     void
// 使用示例     eeprom_write();
//-------------------------------------------------------------------------------------------------------------------
void eeprom_write(void)
{
    uint8 i;
    float temp_value;
    uint32 hex_value;
    
    // 循环存储所有15个PID参数（5组，每组3个参数）
    for(i = 0; i < 15; i++) {
        // 根据索引选择对应的PID参数
        if(i == 0) temp_value = PID_angle_P;
        else if(i == 1) temp_value = PID_angle_I;
        else if(i == 2) temp_value = PID_angle_D;
        else if(i == 3) temp_value = PID_angle_speed_P;
        else if(i == 4) temp_value = PID_angle_speed_I;
        else if(i == 5) temp_value = PID_angle_speed_D;
        else if(i == 6) temp_value = PID_MOTOR_L_P;
        else if(i == 7) temp_value = PID_MOTOR_L_I;
        else if(i == 8) temp_value = PID_MOTOR_L_D;
        else if(i == 9) temp_value = PID_MOTOR_R_P;
        else if(i == 10) temp_value = PID_MOTOR_R_I;
        else if(i == 11) temp_value = PID_MOTOR_R_D;
        // 预留3个参数位置供未来扩展
        else if(i == 12) temp_value = 0.0f;
        else if(i == 13) temp_value = 0.0f;
        else if(i == 14) temp_value = 0.0f;
        
        // 将浮点数转换为32位整数（保留4位小数精度）
        hex_value = (uint32)(temp_value * 100000.0f);
        
        // 存储到EEPROM数组（每个参数占用4字节）
        eeprom_PID[i*3] = (uint8)(hex_value >> 16);        // 最高字节
        eeprom_PID[i*3+1] = (uint8)(hex_value >> 8);       // 次低字节
        eeprom_PID[i*3+2] = (uint8)(hex_value);            // 最低字节
    }
    eeprom_PID[45] = 0x00; // 启动计数，位置变为45 (15*3)

    // 写入所有PID参数
    iap_write_buff(0, eeprom_PID, sizeof(eeprom_PID));
}


//读取电感最大值最小值
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电感值读取函数，用于从EEPROM读取电感的最大最小值
// 参数说明     void
// 返回参数     void
// 使用示例     eeprom_read_inductance_max_min();
//-------------------------------------------------------------------------------------------------------------------
void eeprom_read_inductance_max_min(void)
{
    uint32 hex_value;
    
    // 从EEPROM读取数据（从地址 sizeof(eeprom_PID)+1 开始）
    iap_read_buff(sizeof(eeprom_PID)+1, inductance_max_min, sizeof(inductance_max_min));
    
    // 读取并转换最大值
    hex_value = ((uint32)inductance_max_min[0] << 16) |
                ((uint32)inductance_max_min[1] << 8) |
                ((uint32)inductance_max_min[2]);
    inductance_horizontal_1_max = (float)hex_value;
    
    hex_value = ((uint32)inductance_max_min[3] << 16) |
                ((uint32)inductance_max_min[4] << 8) |
                ((uint32)inductance_max_min[5]);
    inductance_vertical_2_max = (float)hex_value;
    
    hex_value = ((uint32)inductance_max_min[6] << 16) |
                ((uint32)inductance_max_min[7] << 8) |
                ((uint32)inductance_max_min[8]);
    inductance_vertical_3_max = (float)hex_value;
    
    hex_value = ((uint32)inductance_max_min[9] << 16) |
                ((uint32)inductance_max_min[10] << 8) |
                ((uint32)inductance_max_min[11]);
    inductance_horizontal_4_max = (float)hex_value;
    
    // 读取并转换最小值
    hex_value = ((uint32)inductance_max_min[12] << 16) |
                ((uint32)inductance_max_min[13] << 8) |
                ((uint32)inductance_max_min[14]);
    inductance_horizontal_1_min = (float)hex_value;
    
    hex_value = ((uint32)inductance_max_min[15] << 16) |
                ((uint32)inductance_max_min[16] << 8) |
                ((uint32)inductance_max_min[17]);
    inductance_vertical_2_min = (float)hex_value;
    
    hex_value = ((uint32)inductance_max_min[18] << 16) |
                ((uint32)inductance_max_min[19] << 8) |
                ((uint32)inductance_max_min[20]);
    inductance_vertical_3_min = (float)hex_value;
    
    hex_value = ((uint32)inductance_max_min[21] << 16) |
                ((uint32)inductance_max_min[22] << 8) |
                ((uint32)inductance_max_min[23]);
    inductance_horizontal_4_min = (float)hex_value;
}

//擦除eeprom缓冲区
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     EEPROM擦除函数，用于清除EEPROM中的数据
// 参数说明     void
// 返回参数     void
// 使用示例     eeprom_erase();
//-------------------------------------------------------------------------------------------------------------------
void eeprom_erase(void)
{
    // uint8 i;
    iap_erase_page(200);
    // // 擦除PID参数区域
    // for(i = 0; i < 45; i++) {
    //     eeprom_PID[i] = 0x00;
    // }
    // eeprom_PID[45] = 0x00; // 启动计数
    
    // // 擦除电感值区域
    // for(i = 0; i < 24; i++) {
    //     inductance_max_min[i] = 0x00;
    // }
    
    // 写入擦除后的数据
    // iap_write_buff(0, eeprom_PID, sizeof(eeprom_PID));
    // iap_write_buff(sizeof(eeprom_PID)+1, inductance_max_min, sizeof(inductance_max_min));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     EEPROM初始化函数，用于初始化EEPROM并读取存储的参数
// 参数说明     void
// 返回参数     void
// 使用示例     eeprom_init_code();
//-------------------------------------------------------------------------------------------------------------------
void eeprom_init_code(void)
{
    uint8 i;
    float temp_value;
    uint32 hex_value;
    
    iap_init();
    iap_read_buff(0x00, eeprom_PID, sizeof(eeprom_PID));//读取eeprom_PID
    if(eeprom_PID[45] != 0x00)
    {
            // 擦除EEPROM页
        iap_erase_page(200);
        eeprom_write();
        eeprom_write_inductance_max_min(); // 写入电感值
        iap_read_buff(0x00, eeprom_PID, sizeof(eeprom_PID));//读取eeprom_PID

        
    }
                // 读取电感值
    eeprom_read_inductance_max_min();

    
    // 循环将EEPROM中的数据转换回PID参数
    for(i = 0; i < 15; i++) {
        // 从EEPROM中读取32位整数
        hex_value = ((uint32)eeprom_PID[i*3] << 16) |
                   ((uint32)eeprom_PID[i*3+1] << 8) |
                   ((uint32)eeprom_PID[i*3+2]);
        
        // 转换回浮点数（除以100000得到原始值）
        temp_value = (float)hex_value / 100000.0f;
        
        // 根据索引将值赋给对应的PID参数
        if(i == 0) PID_angle_P = temp_value;
        else if(i == 1) PID_angle_I = temp_value;
        else if(i == 2) PID_angle_D = temp_value;
        else if(i == 3) PID_angle_speed_P = temp_value;
        else if(i == 4) PID_angle_speed_I = temp_value;
        else if(i == 5) PID_angle_speed_D = temp_value;
        else if(i == 6) PID_MOTOR_L_P = temp_value;
        else if(i == 7) PID_MOTOR_L_I = temp_value;
        else if(i == 8) PID_MOTOR_L_D = temp_value;
        else if(i == 9) PID_MOTOR_R_P = temp_value;
        else if(i == 10) PID_MOTOR_R_I = temp_value;
        else if(i == 11) PID_MOTOR_R_D = temp_value;
        // 其余参数暂不处理
    }
}

//初始化函数定义区
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     系统初始化函数，用于初始化所有外设和系统参数
// 参数说明     void
// 返回参数     void
// 使用示例     to_init();
//-------------------------------------------------------------------------------------------------------------------
void to_init(void)
{
    ips200_init();
    key_into();                             // 初始化按键
    encoder_init();
    menu_init();                           // 初始化菜单系统
    menu_display();                        // 首次显示菜单   
    gpio_init(IO_P07, GPO, 0, GPO_PUSH_PULL);//初始化蜂鸣器
    gpio_init(HALL_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);;//初始化霍尔传感器
    adc_main_init();//初始化ADC
    while(ble6a20_init())
    {
        system_delay_ms(100);
        printf("wifi init error\n");
    }
    imu660ra_init();
    quaternion_init(); // 初始化四元数
    gyro_calibrate(200);
    //初始化逐飞助手
    seekfree_assistant_oscilloscope_init();
    //初始化eeprom
    eeprom_init_code();
    //初始化串口
    ble6a20_init();
    PidInit();
    control_init();
    //初始化任务

    pit_ms_init(TIM4_PIT, 1);
    tim4_irq_handler=set_task_mode;
    pit_ms_init(TIM11_PIT, 5);
    tim11_irq_handler=imu_isr;

}