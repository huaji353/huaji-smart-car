#ifndef __CONTRL_H__
#define __CONTRL_H__

#include "zf_common_headfile.h"

// 宏定义
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))

#define PWM_L1               (PWMB_CH1_P50)
#define PWM_L2               (PWMB_CH4_P53)
#define PWM_R1               (PWMB_CH3_P52)
#define PWM_R2               (PWMB_CH2_P51)
// PID控制器参数结构体
typedef struct
{
    float                kp;         // P
    float                ki;         // I
    float                kd;         // D
    float                imax;       // 积分限幅

    float                out_p;      // KP输出
    float                out_i;      // KI输出
    float                out_d;      // KD输出
    float                out;        // pid输出

    float                integrator; // 积分值
    float                last_error; // 上次误差
    float                last_derivative; // 上次误差与上上次误差之差
    unsigned long        last_t;     // 上次时间
}pid_param_t;

// 控制系统变量（外部引用）
extern float target_angle;       // 目标角度
extern float target_speed;       // 目标速度
extern float motor_output_L;     // 左电机输出
extern float motor_output_R;     // 右电机输出
extern float angle_output;       // 角度环输出
extern float angle_speed_output; // 角速度环输出

// PID控制器
extern pid_param_t PID_angle;         // 角度环 采用位置式控制器
extern pid_param_t PID_angle_speed;   // 角速度环 采用增量式控制器
extern pid_param_t PID_MOTOR_L;       // 左轮速度环 采用位置式控制器
extern pid_param_t PID_MOTOR_R;       // 右轮速度环 采用位置式控制器

// PID参数设定
extern float PID_angle_P;
extern float PID_angle_I;
extern float PID_angle_D;
extern float PID_angle_speed_P;
extern float PID_angle_speed_I;
extern float PID_angle_speed_D;
extern float PID_MOTOR_L_P;      // 左轮速度环P参数
extern float PID_MOTOR_L_I;      // 左轮速度环I参数
extern float PID_MOTOR_L_D;      // 左轮速度环D参数
extern float PID_MOTOR_R_P;      // 右轮速度环P参数
extern float PID_MOTOR_R_I;      // 右轮速度环I参数
extern float PID_MOTOR_R_D;      // 右轮速度环D参数

// 电感相关变量
extern float horizontal_diff_ratio_sum;
extern float vertical_diff_ratio_sum;
extern float inductance_horizontal_1_max;
extern float inductance_vertical_2_max; 
extern float inductance_vertical_3_max;
extern float inductance_horizontal_4_max;
extern float inductance_horizontal_1_min;
extern float inductance_vertical_2_min;
extern float inductance_vertical_3_min;
extern float inductance_horizontal_4_min;

// 函数声明
// PID控制相关函数
void PidInit(void);
float constrain_float(float amt, float low, float high);
float PidLocCtrl(pid_param_t * pid, float error);
float PidIncCtrl(pid_param_t * pid, float error);

// 电感处理相关函数
void inductance_judgment(void);
void inductance_normalization_max(void);
void inductance_difference_ratio_sum(void);

// 控制系统相关函数
void speed_control_L(void);      // 左轮速度控制
void speed_control_R(void);      // 右轮速度控制
void speed_control(void);        // 整体速度控制
void angle_control(void);
void angle_speed_control(float target_angular_speed);
void control_update(void);
void control_output(void);
void control_test(void);
void control_init(void);
void core_trace(void);
void speed_debug(void);
void angle_debug(void);
#endif /* __CONTRL_H__ */

