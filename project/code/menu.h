#ifndef __MENU_H__
#define __MENU_H__

#include "zf_common_typedef.h"

// 菜单最大层数
#define MENU_MAX_LEVEL 4
// 每层菜单最大项数
#define MENU_MAX_ITEM 8
// 菜单项名称最大长度
#define MENU_NAME_MAX_LEN 20
// 子菜单页索引
#define SETTING_MENU_INDEX 1
#define PID_MENU_INDEX 2
#define ANGLE_PID_MENU_INDEX 4
#define ANGLE_SPEED_PID_MENU_INDEX 5
#define LEFT_MOTOR_PID_MENU_INDEX 6
#define RIGHT_MOTOR_PID_MENU_INDEX 7
#define INFO_MENU_INDEX 3
#define TEST_MENU_INDEX 8
// #define GAME_MENU_INDEX 8  // 添加游戏菜单索引

// 菜单项结构
typedef struct {
    char name[MENU_NAME_MAX_LEN];    // 菜单项名称
    void (*function)(void);          // 菜单项对应功能函数
    uint8 has_child;                 // 是否有子菜单
    uint8 child_index;               // 子菜单在菜单数组中的索引位置
} menu_item_t;

typedef struct {
    float kp;
    float ki;
    float kd;
} pid_t;

// 菜单页结构
typedef struct {
    char title[MENU_NAME_MAX_LEN];       // 菜单页标题
    menu_item_t items[MENU_MAX_ITEM];    // 菜单项数组
    uint8 item_count;                    // 菜单项数量
} menu_page_t;

// 菜单状态结构
typedef struct {
    uint8 current_level;                  // 当前菜单层级
    uint8 current_page[MENU_MAX_LEVEL];   // 各层级当前显示的菜单页索引
    uint8 current_index[MENU_MAX_LEVEL];  // 各层级当前选中的菜单项索引
} menu_state_t;

// 全局菜单状态
extern menu_state_t menu_state;
extern uint8 seekfree_flag;
extern uint8 inductance_max_min_flag;
// 菜单初始化
void menu_init(void);

// 菜单显示
void menu_display(void);

// 菜单处理按键输入
void menu_process_buttons(void);

// 菜单循环执行函数（放在主循环中调用）
void menu_run(void);

// 示例功能函数声明
void function_dummy(void);
void function_speed_setting(void);
void function_pid_setting(void);
void function_sensor_calibration(void);

// 新增PID功能函数声明
void function_angle_pid_setting(void);
void function_angle_speed_pid_setting(void);
void function_motor_pid_setting(void);
void function_reset_pid(void);

// 测试功能函数声明
void function_key_test(void);
void function_adc_test(void);
void function_encoder_test(void);
void function_ips_test(void);
void function_buzzer_test(void);
void function_uart_printf_test(void);

#endif