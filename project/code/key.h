#ifndef _key_h
#define _key_h
extern uint8 button1;
extern uint8 button2;
extern uint8 button3;
extern uint8 button4;
extern uint8 button5;
#ifdef  MENU_USE_RTT
#endif

#define KEY_1               IO_P46                                                       // 定义主板上按键对应引脚 原IO_P45
int  key1_can(void);

#define KEY_2               IO_P15                                                       // 定义主板上按键对应引脚 原IO_P36
int  key2_can(void);

#define KEY_3               IO_P45                                                       // 定义主板上按键对应引脚 原IO_P15
int  key3_can(void);

#define KEY_4               IO_P36                                                       // 定义主板上按键对应引脚 原IO_P46
int  key4_can(void);

#define KEY_5               IO_P37                                                       // 定义主板上按键对应引脚 原IO_P37
int  key5_can(void);

#define LONG_PRESS_TIME	    10	//约为10*20*2=400ms
void key_into(void);
void button_entry(void);
void key_text(void);

#endif








