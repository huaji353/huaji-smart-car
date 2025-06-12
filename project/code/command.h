#ifndef __COMMAND_H__
#define __COMMAND_H__

#define ADC_CHANNEL1            (ADC_CH9_P01)
#define ADC_CHANNEL2            (ADC_CH0_P10)
#define ADC_CHANNEL3            (ADC_CH14_P06)
#define ADC_CHANNEL4            (ADC_CH13_P05)
#define ADC_CHANNEL5            (ADC_CH7_P17)
#define ADC_CHANNEL6            (ADC_CH4_P14)

#define PIT_CH                          (TIM1_PIT )                             // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
//#define PIT_PRIORITY                    (TIM1_IRQn)                           TIM1的中断优先级默认最低，不可修改，具体看手册。

#define ENCODER_DIR_1                 	(TIM0_ENCOEDER)                         // 正交编码器对应使用的编码器接口 这里使用QTIMER1的ENCOEDER1
#define ENCODER_DIR_DIR_1              	(IO_P35)            				 	// DIR 对应的引脚
#define ENCODER_DIR_PULSE_1            	(TIM0_ENCOEDER_P34)            			// PULSE 对应的引脚

#define ENCODER_DIR_2                 	(TIM3_ENCOEDER)                         // 带方向编码器对应使用的编码器接口 这里使用QTIMER1的ENCOEDER2
#define ENCODER_DIR_DIR_2           	(IO_P13)             					// DIR 对应的引脚
#define ENCODER_DIR_PULSE_2       		(TIM3_ENCOEDER_P04)            			// PULSE 对应的引脚

#define HALL_PIN               (IO_P24 )
//预处理flag
#define VCC_ALARM_FLAG                (0)//是否开启VCC报警 
#define image_open                    (0)//是否开启图片路径测试功能
#define text_open                     (1)//是否开启测试功能

extern char txt[32];
extern float vcc;
extern int16 encoder_data_dir_1;
extern int16 encoder_data_dir_2;
extern uint8 encoder_integral_flag;
extern uint32 encoder_integral_1;
extern uint32 encoder_integral_2;
extern float inductance_horizontal_1,inductance_vertical_2,inductance_vertical_3,inductance_horizontal_4;
extern float speed_L,speed_R;

void uart_printf(uart_index_enum uart_n,const char* format,...);
void seekfree_assistant_oscilloscope_init(void);
void seekfree_assistant_oscilloscope_code(void);
void adc_main_read(void);
void to_init(void);
void encoder_init(void);
void pit_handler (void);
void buzzer_time(uint16 time);
void ips_text(void);
void encoder_text(void);
void encoder_integrate(void);
void adc_text(void);
void buzzer_time(uint16 time);
void display_vcc(void);
void eeprom_init_code(void);
void eeprom_write(void);
void eeprom_erase(void);
void eeprom_write_inductance_max_min(void);
void uart_printf_test(void);
#endif