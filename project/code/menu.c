#include "zf_common_headfile.h"

// 定义示例变量，这些变量可以在菜单功能中使用
int16 speed_value = 0;
uint8 seekfree_flag = 0;
uint8 inductance_max_min_flag = 0;
// 定义菜单
menu_page_t menu_pages[17];

// 菜单状态
menu_state_t menu_state;

// 主菜单页索引

#define MAIN_MENU_INDEX 0


// 声明测试函数
extern void key_text(void);
extern void adc_text(void);
extern void encoder_text(void);
extern void ips_text(void);
extern void uart_printf_test(void);
extern void quaternion_display(void);

// 声明游戏函数
// extern void function_game(void);
// extern void function_snake(void);

// 声明左右轮PID设置函数
void function_left_motor_pid_setting(void);
void function_right_motor_pid_setting(void);

//开启逐飞传输
void seekfree_assistant_open(void)
{
    uint8 exit_flag = 0;
    seekfree_flag = !seekfree_flag;
    // 清屏
    ips200_clear(RGB565_WHITE);
    while (!exit_flag) {
    button_entry();
    ips200_show_string(100, 100, "Seekfree_flag");
    sprintf(txt, "%d", seekfree_flag);
    ips200_show_string(80, 120, txt);
    // 检测退出按键
    if (button5) {
        exit_flag = 1;
    }
    system_delay_ms(1);
    }
}
//开启电感最值判断
void function_inductance_max_min_setting(void)
{
    uint8 exit_flag = 0;
    inductance_max_min_flag = !inductance_max_min_flag;
    // 清屏
    ips200_clear(RGB565_WHITE);
    while (!exit_flag) {
    button_entry();
    inductance_judgment();
    // 检测退出按键
    if (button5) {
        exit_flag = 1;
    }
    system_delay_ms(1);
    }
    eeprom_write_inductance_max_min();
}

#if text_open //测试功能封装函数
// 测试功能封装函数
//开启电感最值显示
void function_inductance_max_min_show(void)
{
    uint8 exit_flag = 0;
    // 清屏
    ips200_clear(RGB565_WHITE);
    
    while (!exit_flag) {
        // 显示电感最大值
        sprintf(txt, "H1_max: %.2f", inductance_horizontal_1_max);
        ips200_show_string(0, 0, txt);
        
        sprintf(txt, "V2_max: %.2f", inductance_vertical_2_max);
        ips200_show_string(0, 16, txt);
        
        sprintf(txt, "V3_max: %.2f", inductance_vertical_3_max);
        ips200_show_string(0, 32, txt);
        
        sprintf(txt, "H4_max: %.2f", inductance_horizontal_4_max);
        ips200_show_string(0, 48, txt);
        
        // 显示电感最小值
        sprintf(txt, "H1_min: %.2f", inductance_horizontal_1_min);
        ips200_show_string(0, 80, txt);
        
        sprintf(txt, "V2_min: %.2f", inductance_vertical_2_min);
        ips200_show_string(0, 96, txt);
        
        sprintf(txt, "V3_min: %.2f", inductance_vertical_3_min);
        ips200_show_string(0, 112, txt);
        
        sprintf(txt, "H4_min: %.2f", inductance_horizontal_4_min);
        ips200_show_string(0, 128, txt);
        
        // 检测按键
        button_entry();
        if (button5) {
            exit_flag = 1;
        }
        
        // 延时刷新
        system_delay_ms(100);
    }
}
void function_key_test(void) {
    uint8 exit_flag = 0;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    while (!exit_flag) {
        button_entry();
        // 调用按键测试函数
        key_text();
        
        // 检测退出按键
        if (button5) {
            exit_flag = 1;
        }
        system_delay_ms(10);
    }
}
void function_imu_test(void) {
    uint8 exit_flag = 0;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    while (!exit_flag) {
        button_entry();
        // 调用IMU测试函数
        quaternion_display();
        // imu660ra_text();
        // 检测退出按键
        if (button5) {
            exit_flag = 1;
        }
        system_delay_ms(5);
    }
}       
void function_uart_printf_test(void) {
    uint8 exit_flag = 0;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);

    while (!exit_flag) {
        button_entry();
        // 调用UART测试函数
        uart_printf_test(); 
        
        // 检测退出按键
        if (button5) {
            exit_flag = 1;
        }   
        system_delay_ms(10);
    }
}


void function_adc_test(void) {
    uint8 exit_flag = 0;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    
    while (!exit_flag) {
        button_entry();
        // 调用ADC测试函数
        adc_main_read();
        // inductance_normalization_max();
        inductance_difference_ratio_sum();
        adc_text();
        
        // 检测退出按键
        if (button5) {
            exit_flag = 1;
        }
        system_delay_ms(10);
    }
}

void function_encoder_test(void) {
    uint8 exit_flag = 0;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    
    while (!exit_flag) {
        button_entry();
        // 调用编码器测试函数
        encoder_text();
        
        // 检测退出按键
        if (button5) {
            exit_flag = 1;
        }
        system_delay_ms(10);
    }
}

void function_ips_test(void) {
    uint8 exit_flag = 0;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    
    while (!exit_flag) {
        button_entry();
        // 调用IPS显示测试函数
        ips_text();
        
        // 检测退出按键
        if (button5) {
            exit_flag = 1;
        }
        system_delay_ms(10);
    }
}
void function_buzzer_test(void) {
    uint8 exit_flag = 0;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    
    while (!exit_flag) {
        button_entry();
        // 调用蜂鸣器测试函数
        buzzer_time(100);
        
        // 检测退出按键 
        if (button5) {
            exit_flag = 1;
        }
        system_delay_ms(10);
    }
}  
// 添加 image_path 测试函数
void function_image_path_test(void) {
    uint8 exit_flag = 0;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    
    while (!exit_flag) {
        button_entry();
        #if image_open
        // 调用路径绘制函数
        image_path();
        // 检测退出按键
        #endif
        if (button5) {
            exit_flag = 1;
        }
        system_delay_ms(5);
    }
}
#endif
// 初始化菜单系统
void menu_init(void) {
    // 初始化菜单状态
    menu_state.current_level = 0;
    menu_state.current_page[0] = MAIN_MENU_INDEX;
    menu_state.current_index[0] = 0;
    
    // 初始化主菜单
    strcpy(menu_pages[MAIN_MENU_INDEX].title, "MAIN MENU");
    menu_pages[MAIN_MENU_INDEX].item_count = 7; // 增加菜单项数
    
    // 设置主菜单项
    strcpy(menu_pages[MAIN_MENU_INDEX].items[0].name, "Settings");
    menu_pages[MAIN_MENU_INDEX].items[0].function = NULL;
    menu_pages[MAIN_MENU_INDEX].items[0].has_child = 1;
    menu_pages[MAIN_MENU_INDEX].items[0].child_index = SETTING_MENU_INDEX;
    
    strcpy(menu_pages[MAIN_MENU_INDEX].items[1].name, "PID Parameters");
    menu_pages[MAIN_MENU_INDEX].items[1].function = NULL;
    menu_pages[MAIN_MENU_INDEX].items[1].has_child = 1;
    menu_pages[MAIN_MENU_INDEX].items[1].child_index = PID_MENU_INDEX;
    
    strcpy(menu_pages[MAIN_MENU_INDEX].items[2].name, "Tests");
    menu_pages[MAIN_MENU_INDEX].items[2].function = NULL;
    menu_pages[MAIN_MENU_INDEX].items[2].has_child = 1;
    menu_pages[MAIN_MENU_INDEX].items[2].child_index = TEST_MENU_INDEX;
    
    strcpy(menu_pages[MAIN_MENU_INDEX].items[3].name, "Calibration");
    menu_pages[MAIN_MENU_INDEX].items[3].function = function_sensor_calibration;
    menu_pages[MAIN_MENU_INDEX].items[3].has_child = 0;
    
     // 添加游戏菜单
    //  strcpy(menu_pages[MAIN_MENU_INDEX].items[4].name, "Games");
    //  menu_pages[MAIN_MENU_INDEX].items[5].function = NULL;
    //  menu_pages[MAIN_MENU_INDEX].items[5].has_child = 1;
    // menu_pages[MAIN_MENU_INDEX].items[5].child_index = GAME_MENU_INDEX;

    strcpy(menu_pages[MAIN_MENU_INDEX].items[4].name, "Seekfree");
    menu_pages[MAIN_MENU_INDEX].items[4].function = seekfree_assistant_open;
    menu_pages[MAIN_MENU_INDEX].items[4].has_child = 0;

    strcpy(menu_pages[MAIN_MENU_INDEX].items[5].name, "Inductance Max Min");
    menu_pages[MAIN_MENU_INDEX].items[5].function = function_inductance_max_min_setting;
    menu_pages[MAIN_MENU_INDEX].items[5].has_child = 0;
    
    // 初始化子菜单
    strcpy(menu_pages[SETTING_MENU_INDEX].title, "SETTINGS");
    menu_pages[SETTING_MENU_INDEX].item_count = 3;
    
    strcpy(menu_pages[SETTING_MENU_INDEX].items[0].name, "Speed");
    menu_pages[SETTING_MENU_INDEX].items[0].function = function_speed_setting;
    menu_pages[SETTING_MENU_INDEX].items[0].has_child = 0;
    
    strcpy(menu_pages[SETTING_MENU_INDEX].items[1].name, "Display Mode");
    menu_pages[SETTING_MENU_INDEX].items[1].function = function_dummy;
    menu_pages[SETTING_MENU_INDEX].items[1].has_child = 0;
    
    strcpy(menu_pages[SETTING_MENU_INDEX].items[2].name, "Back");
    menu_pages[SETTING_MENU_INDEX].items[2].function = NULL;
    menu_pages[SETTING_MENU_INDEX].items[2].has_child = 0;
    
    // 初始化PID子菜单页
    strcpy(menu_pages[PID_MENU_INDEX].title, "PID PARAMETERS");
    menu_pages[PID_MENU_INDEX].item_count = 6;  // 增加菜单项数
    
    strcpy(menu_pages[PID_MENU_INDEX].items[0].name, "Angle PID");
    menu_pages[PID_MENU_INDEX].items[0].function = NULL;
    menu_pages[PID_MENU_INDEX].items[0].has_child = 1;
    menu_pages[PID_MENU_INDEX].items[0].child_index = ANGLE_PID_MENU_INDEX;
    
    strcpy(menu_pages[PID_MENU_INDEX].items[1].name, "Angle Speed PID");
    menu_pages[PID_MENU_INDEX].items[1].function = NULL;
    menu_pages[PID_MENU_INDEX].items[1].has_child = 1;
    menu_pages[PID_MENU_INDEX].items[1].child_index = ANGLE_SPEED_PID_MENU_INDEX;
    
    strcpy(menu_pages[PID_MENU_INDEX].items[2].name, "Left Motor PID");
    menu_pages[PID_MENU_INDEX].items[2].function = NULL;
    menu_pages[PID_MENU_INDEX].items[2].has_child = 1;
    menu_pages[PID_MENU_INDEX].items[2].child_index = LEFT_MOTOR_PID_MENU_INDEX;

    strcpy(menu_pages[PID_MENU_INDEX].items[3].name, "Right Motor PID");
    menu_pages[PID_MENU_INDEX].items[3].function = NULL;
    menu_pages[PID_MENU_INDEX].items[3].has_child = 1;
    menu_pages[PID_MENU_INDEX].items[3].child_index = RIGHT_MOTOR_PID_MENU_INDEX;
    
    // 添加重置PID参数选项
    strcpy(menu_pages[PID_MENU_INDEX].items[4].name, "Reset All PID");
    menu_pages[PID_MENU_INDEX].items[4].function = function_reset_pid;
    menu_pages[PID_MENU_INDEX].items[4].has_child = 0;
    
    strcpy(menu_pages[PID_MENU_INDEX].items[5].name, "Back");
    menu_pages[PID_MENU_INDEX].items[5].function = NULL;
    menu_pages[PID_MENU_INDEX].items[5].has_child = 0;
    
    // 初始化角度PID子菜单页
    strcpy(menu_pages[ANGLE_PID_MENU_INDEX].title, "ANGLE PID");
    menu_pages[ANGLE_PID_MENU_INDEX].item_count = 4;
    
    strcpy(menu_pages[ANGLE_PID_MENU_INDEX].items[0].name, "KP Value");
    menu_pages[ANGLE_PID_MENU_INDEX].items[0].function = function_angle_pid_setting;
    menu_pages[ANGLE_PID_MENU_INDEX].items[0].has_child = 0;
    
    strcpy(menu_pages[ANGLE_PID_MENU_INDEX].items[1].name, "KI Value");
    menu_pages[ANGLE_PID_MENU_INDEX].items[1].function = function_angle_pid_setting;
    menu_pages[ANGLE_PID_MENU_INDEX].items[1].has_child = 0;
    
    strcpy(menu_pages[ANGLE_PID_MENU_INDEX].items[2].name, "KD Value");
    menu_pages[ANGLE_PID_MENU_INDEX].items[2].function = function_angle_pid_setting;
    menu_pages[ANGLE_PID_MENU_INDEX].items[2].has_child = 0;
    
    strcpy(menu_pages[ANGLE_PID_MENU_INDEX].items[3].name, "Back");
    menu_pages[ANGLE_PID_MENU_INDEX].items[3].function = NULL;
    menu_pages[ANGLE_PID_MENU_INDEX].items[3].has_child = 0;
    
    // 初始化角度速度PID子菜单页
    strcpy(menu_pages[ANGLE_SPEED_PID_MENU_INDEX].title, "ANGLE SPEED PID");
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].item_count = 4;
    
    strcpy(menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[0].name, "KP Value");
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[0].function = function_angle_speed_pid_setting;
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[0].has_child = 0;
    
    strcpy(menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[1].name, "KI Value");
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[1].function = function_angle_speed_pid_setting;
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[1].has_child = 0;
    
    strcpy(menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[2].name, "KD Value");
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[2].function = function_angle_speed_pid_setting;
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[2].has_child = 0;
    
    strcpy(menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[3].name, "Back");
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[3].function = NULL;
    menu_pages[ANGLE_SPEED_PID_MENU_INDEX].items[3].has_child = 0;
    
    // 初始化左轮电机PID子菜单页
    strcpy(menu_pages[LEFT_MOTOR_PID_MENU_INDEX].title, "LEFT MOTOR PID");
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].item_count = 4;
    
    strcpy(menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[0].name, "KP Value");
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[0].function = function_left_motor_pid_setting;
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[0].has_child = 0;
    
    strcpy(menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[1].name, "KI Value");
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[1].function = function_left_motor_pid_setting;
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[1].has_child = 0;
    
    strcpy(menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[2].name, "KD Value");
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[2].function = function_left_motor_pid_setting;
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[2].has_child = 0;
    
    strcpy(menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[3].name, "Back");
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[3].function = NULL;
    menu_pages[LEFT_MOTOR_PID_MENU_INDEX].items[3].has_child = 0;
    
    // 初始化右轮电机PID子菜单页
    strcpy(menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].title, "RIGHT MOTOR PID");
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].item_count = 4;
    
    strcpy(menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[0].name, "KP Value");
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[0].function = function_right_motor_pid_setting;
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[0].has_child = 0;
    
    strcpy(menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[1].name, "KI Value");
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[1].function = function_right_motor_pid_setting;
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[1].has_child = 0;
    
    strcpy(menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[2].name, "KD Value");
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[2].function = function_right_motor_pid_setting;
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[2].has_child = 0;
    
    strcpy(menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[3].name, "Back");
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[3].function = NULL;
    menu_pages[RIGHT_MOTOR_PID_MENU_INDEX].items[3].has_child = 0;

    // 初始化测试子菜单
		#if text_open
    strcpy(menu_pages[TEST_MENU_INDEX].title, "TESTS");
    menu_pages[TEST_MENU_INDEX].item_count = 8;  // 增加菜单项数
    
    strcpy(menu_pages[TEST_MENU_INDEX].items[0].name, "Key Test");
    menu_pages[TEST_MENU_INDEX].items[0].function = function_key_test;
    menu_pages[TEST_MENU_INDEX].items[0].has_child = 0;
    
    strcpy(menu_pages[TEST_MENU_INDEX].items[1].name, "ADC Test");
    menu_pages[TEST_MENU_INDEX].items[1].function = function_adc_test;
    menu_pages[TEST_MENU_INDEX].items[1].has_child = 0;
    
    strcpy(menu_pages[TEST_MENU_INDEX].items[2].name, "Encoder Test");
    menu_pages[TEST_MENU_INDEX].items[2].function = function_encoder_test;
    menu_pages[TEST_MENU_INDEX].items[2].has_child = 0;

    strcpy(menu_pages[TEST_MENU_INDEX].items[3].name, "IMU Test");
    menu_pages[TEST_MENU_INDEX].items[3].function = function_imu_test;
    menu_pages[TEST_MENU_INDEX].items[3].has_child = 0;
    
    strcpy(menu_pages[TEST_MENU_INDEX].items[4].name, "Buzzer Test");
    menu_pages[TEST_MENU_INDEX].items[4].function = function_buzzer_test;
    menu_pages[TEST_MENU_INDEX].items[4].has_child = 0;
    
    strcpy(menu_pages[TEST_MENU_INDEX].items[5].name, "UART Test");
    menu_pages[TEST_MENU_INDEX].items[5].function = function_uart_printf_test;
    menu_pages[TEST_MENU_INDEX].items[5].has_child = 0;

    strcpy(menu_pages[TEST_MENU_INDEX].items[6].name, "Path Test");
    menu_pages[TEST_MENU_INDEX].items[6].function = function_image_path_test;
    menu_pages[TEST_MENU_INDEX].items[6].has_child = 0;

    strcpy(menu_pages[TEST_MENU_INDEX].items[7].name, "inductance_max_min");
    menu_pages[TEST_MENU_INDEX].items[7].function = function_inductance_max_min_show;
    menu_pages[TEST_MENU_INDEX].items[7].has_child = 0;
		#endif
    // // 初始化游戏菜单页
    // strcpy(menu_pages[GAME_MENU_INDEX].title, "GAMES");
    // menu_pages[GAME_MENU_INDEX].item_count = 3;
    
    // strcpy(menu_pages[GAME_MENU_INDEX].items[0].name, "Space Shooter");
    // menu_pages[GAME_MENU_INDEX].items[0].function = function_game;
    // menu_pages[GAME_MENU_INDEX].items[0].has_child = 0;
    
    // strcpy(menu_pages[GAME_MENU_INDEX].items[1].name, "Snake Game");
    // menu_pages[GAME_MENU_INDEX].items[1].function = function_snake;
    // menu_pages[GAME_MENU_INDEX].items[1].has_child = 0;
    
    // strcpy(menu_pages[GAME_MENU_INDEX].items[2].name, "Back");
    // menu_pages[GAME_MENU_INDEX].items[2].function = NULL;
    // menu_pages[GAME_MENU_INDEX].items[2].has_child = 0;
}

// 显示当前菜单
void menu_display(void) {
    uint8 i;
    uint8 current_page_index = menu_state.current_page[menu_state.current_level];
    uint8 current_item_index = menu_state.current_index[menu_state.current_level];
    menu_page_t *current_page = &menu_pages[current_page_index];
    uint8 items_per_column = (current_page->item_count + 1) / 2;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    
    // 显示菜单标题
    ips200_show_string(10, 10, current_page->title);
    
    // 绘制分隔线
    for (i = 0; i < 38; i++) {  // 调整为38个"-"以适应320宽度
        ips200_show_string(10 + i * 8, 30, "-");
    }
    
    // 显示菜单项 - 两列布局
    for (i = 0; i < current_page->item_count; i++) {
        uint8 column = i / items_per_column;
        uint8 row = i % items_per_column;
        uint16 x_pos = 10 + (column * 140); // 两列布局
        uint16 y_pos = 50 + row * 30;
        
        if (i == current_item_index) {
            ips200_set_color(RGB565_WHITE, RGB565_BLUE);
            ips200_show_string(x_pos, y_pos, current_page->items[i].name);
            ips200_set_color(RGB565_BLACK, RGB565_WHITE);
        } else {
            ips200_show_string(x_pos, y_pos, current_page->items[i].name);
        }
        
        if (current_page->items[i].has_child) {
            ips200_show_string(x_pos + 120, y_pos, ">");
        }
    }
    
    // 调整操作提示位置
    ips200_show_string(10, 220, "UP/DOWN:Navigate LEFT/RIGHT:Switch");
    ips200_show_string(10, 240, "OK:Select BACK:Return");
}

// 处理菜单按键输入
void menu_process_buttons(void) {
    uint8 current_page_index = menu_state.current_page[menu_state.current_level];
    uint8 current_item_index = menu_state.current_index[menu_state.current_level];
    menu_page_t *current_page = &menu_pages[current_page_index];
    menu_item_t *current_item = &current_page->items[current_item_index];
    uint8 key_pressed = 0;  // 标记是否有按键处理
    uint8 items_per_column = (current_page->item_count + 1) / 2;
    
    // 上移选择
    if (button4) {
        if (current_item_index > 0) {
            menu_state.current_index[menu_state.current_level]--;
            menu_display();
        }
    }
    
    // 下移选择
    if (button3) {
        if (current_item_index < current_page->item_count - 1) {
            menu_state.current_index[menu_state.current_level]++;
            menu_display();
        }
    }
    
    // 左移选择 - 切换到左侧列
    if (button1) {
        if (current_item_index >= items_per_column) {
            menu_state.current_index[menu_state.current_level] -= items_per_column;
            menu_display();
        } else if (menu_state.current_level > 0) {
            // 返回上级菜单
            menu_state.current_level--;
            menu_display();
        }
    }
    
    // 右移选择 - 切换到右侧列
    if (button2) {
        if (current_item_index + items_per_column < current_page->item_count) {
            menu_state.current_index[menu_state.current_level] += items_per_column;
            menu_display();
        }
    }
    
    
    // 确定按钮
    if (button5) {
        // 判断当前选中项是否为"Back"
        if (strcmp(current_item->name, "Back") == 0) {
            // 返回上一级菜单
            if (menu_state.current_level > 0) {
                menu_state.current_level--;
                menu_display();
            }
        } 
        // 如果有子菜单，进入子菜单
        else if (current_item->has_child) {
            menu_state.current_level++;
            menu_state.current_page[menu_state.current_level] = current_item->child_index;
            menu_state.current_index[menu_state.current_level] = 0;
            menu_display();
        }
        // 如果有功能函数，执行功能
        else if (current_item->function != NULL) {
            current_item->function();
            menu_display();
        }
    }
}

// 菜单运行函数，放在主函数中
void menu_run(void) {
    static uint8 last_button_status[5] = {0, 0, 0, 0, 0}; // 记录上一帧按键状态
    static uint8 one_into_flag = 1;
    if(one_into_flag)
    {
        one_into_flag = 0;
        menu_display();
    }
    
    // 检查按键状态是否已经处理过，如果已处理过就跳过
    if (button1 && !last_button_status[0]) {
        last_button_status[0] = 1;
    } else if (!button1 && last_button_status[0]) {
        last_button_status[0] = 0;
    } else if (button1) {
        button1 = 0; // 已经处理过，清除状态
    }
    
    if (button2 && !last_button_status[1]) {
        last_button_status[1] = 1;
    } else if (!button2 && last_button_status[1]) {
        last_button_status[1] = 0;
    } else if (button2) {
        button2 = 0; // 已经处理过，清除状态
    }
    
    if (button3 && !last_button_status[2]) {
        last_button_status[2] = 1;
    } else if (!button3 && last_button_status[2]) {
        last_button_status[2] = 0;
    } else if (button3) {
        button3 = 0; // 已经处理过，清除状态
    }
    
    if (button4 && !last_button_status[3]) {
        last_button_status[3] = 1;
    } else if (!button4 && last_button_status[3]) {
        last_button_status[3] = 0;
    } else if (button4) {
        button4 = 0; // 已经处理过，清除状态
    }
    
    if (button5 && !last_button_status[4]) {
        last_button_status[4] = 1;
    } else if (!button5 && last_button_status[4]) {
        last_button_status[4] = 0;
    } else if (button5) {
        button5 = 0; // 已经处理过，清除状态
    }
    
    // 处理按键动作
    if (button1) {
        // 按左键返回上级菜单
        if (menu_state.current_level > 0) {
            menu_state.current_level--;
            menu_display();
        }
    } else if (button3) {
        // 按下键选择下一菜单项
        if (menu_state.current_index[menu_state.current_level] < 
            menu_pages[menu_state.current_page[menu_state.current_level]].item_count - 1) {
            menu_state.current_index[menu_state.current_level]++;
            menu_display();
        }
    } else if (button4) {
        // 按上键选择上一菜单项
        if (menu_state.current_index[menu_state.current_level] > 0) {
            menu_state.current_index[menu_state.current_level]--;
            menu_display();
        }
    } else if (button5) {
        // 按左键执行当前选中的菜单项
        menu_process_buttons();
    }
}

// ========== 示例功能函数实现 ==========

// 空函数
void function_dummy(void) {
    button5 = 0;
    // 清屏
    ips200_clear(RGB565_WHITE);
    ips200_show_string(10, 100, "Function Called");
    
    // 等待任意按键返回
    while(1) {
        if (button1 || button2 || button3 || button4 || button5) {
            break;
        }
    }
}

// 速度设置功能
void function_speed_setting(void) {
    uint8 exit_flag = 0;
    uint8 i;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    ips200_show_string(10, 10, "SPEED SETTING");
    
    // 绘制分隔线
    for (i = 0; i < 30; i++) {
        ips200_show_string(10 + i * 8, 30, "-");
    }
    
    while (!exit_flag) {
        // 显示当前速度
        ips200_show_string(20, 60, "Current speed: ");
        ips200_show_int16(180, 60, speed_value);
        
        // 显示操作提示
        ips200_show_string(10, 200, "UP/DOWN: +/- value");
        ips200_show_string(10, 230, "LEFT: Return");
        button_entry();
        // 处理按键
        if (button4) {  // 上键增加
            speed_value += 1;
        }
        
        if (button3) {  // 下键减少
            speed_value -= 1;
        }
        
        if (button1) {  // 左键返回
            exit_flag = 1;
        }
    }
}

// 角度PID参数设置功能
void function_angle_pid_setting(void) {
    uint8 exit_flag = 0;
    float *current_value;
    char *param_name;
    uint8 current_page_index = menu_state.current_page[menu_state.current_level];
    uint8 current_item_index = menu_state.current_index[menu_state.current_level];
    uint8 i;
    char value_str[20]; // 用于格式化显示
    uint8 modified = 0; // 标记是否修改了参数
    button5 = 0;
    
    // 获取当前编辑的参数
    if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KP Value") == 0) {
        current_value = &PID_angle_P;
        param_name = "Angle KP";
    } else if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KI Value") == 0) {
        current_value = &PID_angle_I;
        param_name = "Angle KI";
    } else if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KD Value") == 0) {
        current_value = &PID_angle_D;
        param_name = "Angle KD";
    } else {
        return;
    }
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    ips200_show_string(10, 10, "ANGLE PID SETTING");
    
    // 绘制分隔线
    for (i = 0; i < 30; i++) {
        ips200_show_string(10 + i * 8, 30, "-");
    }
    
    while (!exit_flag) {
        // 显示当前参数 - 使用字符串格式化
        ips200_show_string(20, 60, param_name);
        ips200_show_string(20, 90, "Value: ");
        
        // 格式化浮点数显示，避免显示问号
        sprintf(value_str, "%.2f", *current_value);
        ips200_show_string(100, 90, value_str);
        
        // 显示已修改
        if (modified) {
            ips200_show_string(180, 90, "(Modified)");
        }
        
        // 显示操作提示
        ips200_show_string(10, 150, "UP: +0.1  DOWN: -0.1");
        ips200_show_string(10, 170, "RIGHT: +1.0  LEFT: -1.0");
        button_entry();
        // 处理按键
        if (button4) {  // 上键增加
            (*current_value) += 0.1;
            modified = 1;
        }
        
        if (button3) {  // 下键减少
            (*current_value) -= 0.1;
            modified = 1;
        }
        
        if (button2) {  // 右键大幅增加
            (*current_value) += 1.0;
            modified = 1;
        }
        
        if (button1) {  // 左键大幅减少
            (*current_value) -= 1.0;
            modified = 1;
        }
        
        if (button5) {  // 应用并返回
            exit_flag = 1;
            if (modified) {
                // 应用参数到PID控制器
                PidInit();
                // 保存到EEPROM
                eeprom_write();
                
                // 显示保存提示
                ips200_clear(RGB565_WHITE);
                ips200_show_string(50, 120, "Parameters Saved!");
                system_delay_ms(500);
            }
        }
        
        system_delay_ms(50);  // 减少延迟，提高响应速度
    }
}

// 角速度PID参数设置功能
void function_angle_speed_pid_setting(void) {
    uint8 exit_flag = 0;
    float *current_value;
    char *param_name;
    uint8 current_page_index = menu_state.current_page[menu_state.current_level];
    uint8 current_item_index = menu_state.current_index[menu_state.current_level];
    uint8 i;
    char value_str[20]; // 用于格式化显示
    uint8 modified = 0; // 标记是否修改了参数
    button5 = 0;
    
    // 获取当前编辑的参数
    if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KP Value") == 0) {
        current_value = &PID_angle_speed_P;
        param_name = "Angle Speed KP";
    } else if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KI Value") == 0) {
        current_value = &PID_angle_speed_I;
        param_name = "Angle Speed KI";
    } else if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KD Value") == 0) {
        current_value = &PID_angle_speed_D;
        param_name = "Angle Speed KD";
    } else {
        return;
    }
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    ips200_show_string(10, 10, "ANGLE SPEED PID SETTING");
    
    // 绘制分隔线
    for (i = 0; i < 30; i++) {
        ips200_show_string(10 + i * 8, 30, "-");
    }
    
    while (!exit_flag) {
        // 显示当前参数 - 使用字符串格式化
        ips200_show_string(20, 60, param_name);
        ips200_show_string(20, 90, "Value: ");
        
        // 格式化浮点数显示，避免显示问号
        sprintf(value_str, "%.2f", *current_value);
        ips200_show_string(100, 90, value_str);
        
        // 显示已修改
        if (modified) {
            ips200_show_string(180, 90, "(Modified)");
        }
        
        // 显示操作提示
        ips200_show_string(10, 150, "UP: +0.1  DOWN: -0.1");
        ips200_show_string(10, 170, "RIGHT: +1.0  LEFT: -1.0");
        button_entry();
        // 处理按键
        if (button4) {  // 上键增加
            (*current_value) += 0.1;
            modified = 1;
        }
        
        if (button3) {  // 下键减少
            (*current_value) -= 0.1;
            modified = 1;
        }
        
        if (button2) {  // 右键大幅增加
            (*current_value) += 1.0;
            modified = 1;
        }
        
        if (button1) {  // 左键大幅减少
            (*current_value) -= 1.0;
            modified = 1;
        }
        
        if (button5) {  // 应用并返回
            exit_flag = 1;
            if (modified) {
                // 应用参数到PID控制器
                PidInit();
                // 保存到EEPROM
                eeprom_write();
                
                // 显示保存提示
                ips200_clear(RGB565_WHITE);
                ips200_show_string(50, 120, "Parameters Saved!");
                system_delay_ms(500);
            }
        }
        
        system_delay_ms(50);  // 减少延迟，提高响应速度
    }
}

// 左轮电机PID参数设置功能
void function_left_motor_pid_setting(void) {
    uint8 exit_flag = 0;
    float *current_value;
    char *param_name;
    uint8 current_page_index = menu_state.current_page[menu_state.current_level];
    uint8 current_item_index = menu_state.current_index[menu_state.current_level];
    uint8 i;
    char value_str[20]; // 用于格式化显示
    uint8 modified = 0; // 标记是否修改了参数
    button5 = 0;
    
    // 获取当前编辑的参数
    if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KP Value") == 0) {
        current_value = &PID_MOTOR_L_P;
        param_name = "Left Motor KP";
    } else if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KI Value") == 0) {
        current_value = &PID_MOTOR_L_I;
        param_name = "Left Motor KI";
    } else if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KD Value") == 0) {
        current_value = &PID_MOTOR_L_D;
        param_name = "Left Motor KD";
    } else {
        return;
    }
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    
    // 绘制分隔线
    for (i = 0; i < 30; i++) {
        ips200_show_string(10 + i * 8, 30, "-");
    }
    
    while (!exit_flag) {
        // 显示当前参数 - 使用字符串格式化
        ips200_show_string(20, 60, param_name);
        ips200_show_string(20, 90, "Value: ");
        
        // 格式化浮点数显示，避免显示问号
        sprintf(value_str, "%.4f", *current_value);
        ips200_show_string(100, 90, value_str);
        
        // 显示已修改
        if (modified) {
            ips200_show_string(180, 90, "(Modified)");
        }
        
        // 显示操作提示
        ips200_show_string(10, 150, "UP: +0.01  DOWN: -0.01");
        ips200_show_string(10, 170, "RIGHT: +0.1  LEFT: -0.1");
        button_entry();
        // 处理按键
        if (button4) {  // 上键增加
            (*current_value) += 0.01;
            modified = 1;
        }
        
        if (button3) {  // 下键减少
            (*current_value) -= 0.01;
            modified = 1;
        }
        
        if (button2) {  // 右键大幅增加
            (*current_value) += 0.1;
            modified = 1;
        }
        
        if (button1) {  // 左键大幅减少
            (*current_value) -= 0.1;
            modified = 1;
        }
        
        if (button5) {  // 应用并返回
            exit_flag = 1;
            if (modified) {
                // 应用参数到PID控制器
                PidInit();
                // 保存到EEPROM
                eeprom_write();
                
                // 显示保存提示
                ips200_clear(RGB565_WHITE);
                ips200_show_string(50, 120, "Parameters Saved!");
                system_delay_ms(500);
            }
        }
        
        system_delay_ms(50);  // 减少延迟，提高响应速度
    }
}

// 右轮电机PID参数设置功能
void function_right_motor_pid_setting(void) {
    uint8 exit_flag = 0;
    float *current_value;
    char *param_name;
    uint8 current_page_index = menu_state.current_page[menu_state.current_level];
    uint8 current_item_index = menu_state.current_index[menu_state.current_level];
    uint8 i;
    char value_str[20]; // 用于格式化显示
    uint8 modified = 0; // 标记是否修改了参数
    button5 = 0;
    
    // 获取当前编辑的参数
    if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KP Value") == 0) {
        current_value = &PID_MOTOR_R_P;
        param_name = "Right Motor KP";
    } else if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KI Value") == 0) {
        current_value = &PID_MOTOR_R_I;
        param_name = "Right Motor KI";
    } else if (strcmp(menu_pages[current_page_index].items[current_item_index].name, "KD Value") == 0) {
        current_value = &PID_MOTOR_R_D;
        param_name = "Right Motor KD";
    } else {
        return;
    }
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    ips200_show_string(10, 10, "RIGHT MOTOR PID SETTING");
    
    // 绘制分隔线
    for (i = 0; i < 30; i++) {
        ips200_show_string(10 + i * 8, 30, "-");
    }
    
    while (!exit_flag) {
        // 显示当前参数 - 使用字符串格式化
        ips200_show_string(20, 60, param_name);
        ips200_show_string(20, 90, "Value: ");
        
        // 格式化浮点数显示，避免显示问号
        sprintf(value_str, "%.4f", *current_value);
        ips200_show_string(100, 90, value_str);
        
        // 显示已修改
        if (modified) {
            ips200_show_string(180, 90, "(Modified)");
        }
        
        // 显示操作提示
        ips200_show_string(10, 150, "UP: +0.01  DOWN: -0.01");
        ips200_show_string(10, 170, "RIGHT: +0.1  LEFT: -0.1");
        ips200_show_string(10, 190, "OK: Apply & Return");
        button_entry();
        // 处理按键
        if (button4) {  // 上键增加
            (*current_value) += 0.01;
            modified = 1;
        }
        
        if (button3) {  // 下键减少
            (*current_value) -= 0.01;
            modified = 1;
        }
        
        if (button2) {  // 右键大幅增加
            (*current_value) += 0.1;
            modified = 1;
        }
        
        if (button1) {  // 左键大幅减少
            (*current_value) -= 0.1;
            modified = 1;
        }
        
        if (button5) {  // 应用并返回
            exit_flag = 1;
            if (modified) {
                // 应用参数到PID控制器
                PidInit();
                // 保存到EEPROM
                eeprom_write();
                
                // 显示保存提示
                ips200_clear(RGB565_WHITE);
                ips200_show_string(50, 120, "Parameters Saved!");
                system_delay_ms(500);
            }
        }
        
        system_delay_ms(50);  // 减少延迟，提高响应速度
    }
}

// 传感器校准功能
void function_sensor_calibration(void) {
    uint8 i;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    ips200_show_string(10, 10, "SENSOR CALIBRATION");
    
    // 绘制分隔线
    for (i = 0; i < 30; i++) {
        ips200_show_string(10 + i * 8, 30, "-");
    }
    
    ips200_show_string(20, 90, "Please wait...");
    
    // 模拟校准过程
    quaternion_init();
    gyro_calibrate(200);
    
    ips200_show_string(20, 120, "Calibration complete!");
    
    // 等待任意按键返回
    while(1) {
        button_entry();
        if (button1 || button2 || button3 || button4 || button5) {
            break;
        }
    }
}


// 添加重置PID参数功能
void function_reset_pid(void) {
    uint8 exit_flag = 0;
    uint8 confirm = 0;
    uint8 i;
    button5 = 0;
    
    // 清屏
    ips200_clear(RGB565_WHITE);
    ips200_show_string(10, 10, "RESET ALL PID PARAMETERS");
    
    // 绘制分隔线
    for (i = 0; i < 30; i++) {
        ips200_show_string(10 + i * 8, 30, "-");
    }
    
    // 显示警告信息
    ips200_show_string(20, 60, "Warning: This will reset");
    ips200_show_string(20, 90, "all PID parameters!");
    ips200_show_string(20, 120, "Are you sure?");
    
    while (!exit_flag) {
        // 显示选项
        if (confirm) {
            ips200_set_color(RGB565_WHITE, RGB565_RED);
            ips200_show_string(50, 160, "YES");
            ips200_set_color(RGB565_BLACK, RGB565_WHITE);
            ips200_show_string(150, 160, "NO");
        } else {
            ips200_show_string(50, 160, "YES");
            ips200_set_color(RGB565_WHITE, RGB565_BLUE);
            ips200_show_string(150, 160, "NO");
            ips200_set_color(RGB565_BLACK, RGB565_WHITE);
        }
        
        // 显示操作提示
        ips200_show_string(10, 200, "LEFT/RIGHT: Select");
        ips200_show_string(10, 230, "OK: Confirm");
        button_entry();
        // 处理按键
        if (button1 || button2) {  // 左右键切换选项
            confirm = !confirm;
        }
        
        if (button5) {  // 确定
            if (confirm) {
                // 执行重置操作
                eeprom_erase();
                
                // 重置所有PID参数
                // 角度环
                PID_angle_P = 0.0f;
                PID_angle_I = 0.0f;
                PID_angle_D = 0.0f;
                
                // 角速度环
                PID_angle_speed_P = 0.0f;
                PID_angle_speed_I = 0.0f;
                PID_angle_speed_D = 0.0f;
                
                // 左轮电机环
                PID_MOTOR_L_P = 0.0f;
                PID_MOTOR_L_I = 0.0f;
                PID_MOTOR_L_D = 0.0f;
                
                // 右轮电机环
                PID_MOTOR_R_P = 0.0f;
                PID_MOTOR_R_I = 0.0f;
                PID_MOTOR_R_D = 0.0f;
                
                // 应用重置后的参数
                PidInit();
                
                // 显示重置成功
                ips200_clear(RGB565_WHITE);
                ips200_show_string(50, 120, "Reset Successful!");
                system_delay_ms(1000);
            }
            exit_flag = 1;
        }
        
        system_delay_ms(50);
    }
}

