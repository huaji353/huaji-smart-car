#include "zf_common_headfile.h"

//===================================================LQ_PID===================================================

pid_param_t PID_angle;         // 角度环 采用位置式控制器
pid_param_t PID_angle_speed;   // 角速度环 采用增量式控制器
pid_param_t PID_MOTOR_L;       // 左轮速度环 采用位置式控制器
pid_param_t PID_MOTOR_R;       // 右轮速度环 采用位置式控制器
// pid_param_t PID_Init;          // 初始化 (移除未使用的变量)

// PID参数设定
float PID_angle_P = 1.8f;      // 角度环P参数
float PID_angle_I = 0.0f;      // 角度环I参数 (通常角度环不需要I)
float PID_angle_D = 0.5f;      // 角度环D参数
float PID_angle_speed_P = 18.0f; // 角速度环P参数
float PID_angle_speed_I = 0.1f; // 角速度环I参数 
float PID_angle_speed_D = 0.1f; // 角速度环D参数
float PID_MOTOR_L_P = 2.23f;     // 左轮速度环P参数
float PID_MOTOR_L_I = 0.0015f;     // 左轮速度环I参数
float PID_MOTOR_L_D = 0.01f;     // 左轮速度环D参数
float PID_MOTOR_R_P = 2.23f;     // 右轮速度环P参数
float PID_MOTOR_R_I = 0.0015f;     // 右轮速度环I参数
float PID_MOTOR_R_D = 0.01f;     // 右轮速度环D参数

// 控制系统变量
float target_angle = 0.0f;     // 目标角度
float target_speed = 0.0f;     // 目标速度
float motor_output_L = 0.0f;   // 左电机输出
float motor_output_R = 0.0f;   // 右电机输出
float angle_output = 0.0f;     // 角度环输出(角速度目标值)
float angle_speed_output = 0.0f; // 角速度环输出

uint32 time_out =0;

/*************************************************************************
 *  函数名称：float constrain_float(float amt, float low, float high)
 *  功能说明：限幅函数
 *  参数说明：
  * @param    amt   ： 参数
  * @param    low   ： 最低值
  * @param    high  ： 最高值
 *  函数返回：无
 *  修改时间：2020年4月1日
 *  备    注：
 *************************************************************************/
float constrain_float(float amt, float low, float high)
{
    return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
}

// pid参数初始化函数
void PidInit(void)
{
    // 角度环PID参数初始化 (位置式PID)
    PID_angle.kp        = PID_angle_P;
    PID_angle.ki        = PID_angle_I;
    PID_angle.kd        = PID_angle_D;
    PID_angle.imax      = 3000;
    PID_angle.out_p     = 0;
    PID_angle.out_i     = 0;
    PID_angle.out_d     = 0;
    PID_angle.out       = 0;
    PID_angle.integrator= 0;
    PID_angle.last_error= 0;
    PID_angle.last_derivative = 0;
    PID_angle.last_t    = 0;
    
    // 角速度环PID参数初始化 (增量式PID)
    PID_angle_speed.kp   = PID_angle_speed_P;
    PID_angle_speed.ki   = PID_angle_speed_I;
    PID_angle_speed.kd   = PID_angle_speed_D;
    PID_angle_speed.imax = 3000;
    PID_angle_speed.out_p= 0;
    PID_angle_speed.out_i= 0;    
    PID_angle_speed.out_d= 0;
    PID_angle_speed.out  = 0;
    PID_angle_speed.integrator = 0;
    PID_angle_speed.last_error = 0;
    PID_angle_speed.last_derivative = 0;
    PID_angle_speed.last_t = 0;
    
    // 左轮速度环PID参数初始化 (位置式PID)
    PID_MOTOR_L.kp        = PID_MOTOR_L_P;
    PID_MOTOR_L.ki        = PID_MOTOR_L_I;
    PID_MOTOR_L.kd        = PID_MOTOR_L_D;
    PID_MOTOR_L.imax      = 3000;
    PID_MOTOR_L.out_p     = 0;
    PID_MOTOR_L.out_i     = 0;
    PID_MOTOR_L.out_d     = 0;    
    PID_MOTOR_L.out       = 0;
    PID_MOTOR_L.integrator= 0;
    PID_MOTOR_L.last_error= 0;
    PID_MOTOR_L.last_derivative = 0;
    PID_MOTOR_L.last_t    = 0;
    
    // 右轮速度环PID参数初始化 (位置式PID)
    PID_MOTOR_R.kp        = PID_MOTOR_R_P;
    PID_MOTOR_R.ki        = PID_MOTOR_R_I;
    PID_MOTOR_R.kd        = PID_MOTOR_R_D;
    PID_MOTOR_R.imax      = 3000;
    PID_MOTOR_R.out_p     = 0;
    PID_MOTOR_R.out_i     = 0;
    PID_MOTOR_R.out_d     = 0;    
    PID_MOTOR_R.out       = 0;
    PID_MOTOR_R.integrator= 0;
    PID_MOTOR_R.last_error= 0;
    PID_MOTOR_R.last_derivative = 0;
    PID_MOTOR_R.last_t    = 0;
}

/*************************************************************************
 *  函数名称：float PidLocCtrl(pid_param_t * pid, float error)
 *  功能说明：pid位置式控制器输出
 *  参数说明：
  * @param    pid     pid参数
  * @param    error   pid输入误差
 *  函数返回：PID输出结果
 *  修改时间：2020年4月1日
 *  备    注：
 *************************************************************************/
float PidLocCtrl(pid_param_t * pid, float error)
{
    /* 累积误差 */
    pid->integrator += error;

    /* 误差限幅 */
    pid->integrator = constrain_float(pid->integrator, -pid->imax, pid->imax);

    pid->out_p = pid->kp * error;
    pid->out_i = pid->ki * pid->integrator;
    pid->out_d = pid->kd * (error - pid->last_error);

    pid->last_error = error;

    pid->out = pid->out_p + pid->out_i + pid->out_d;

    return pid->out;
}

/*************************************************************************
 *  函数名称：float PidIncCtrl(pid_param_t * pid, float error)
 *  功能说明：pid增量式控制器输出
 *  参数说明：
  * @param    pid     pid参数
  * @param    error   pid输入误差
 *  函数返回：PID输出结果   注意输出结果已经包涵了上次结果
 *  修改时间：2020年4月1日
 *  备    注：
 *************************************************************************/
float PidIncCtrl(pid_param_t * pid, float error)
{
    pid->out_p = pid->kp * (error - pid->last_error);
    pid->out_i = pid->ki * error;
    pid->out_d = pid->kd * ((error - pid->last_error) - pid->last_derivative);

    pid->last_derivative = error - pid->last_error;
    pid->last_error = error;

    pid->out += pid->out_p + pid->out_i + pid->out_d;
    return pid->out;
}

//===================================================LQ_PID===================================================
//=======================================电感判断=============================================================
float inductance_horizontal_1_max = 1;
float inductance_vertical_2_max = 1;
float inductance_vertical_3_max = 1;
float inductance_horizontal_4_max = 1;
float inductance_horizontal_1_min = 1000;
float inductance_vertical_2_min = 1000;
float inductance_vertical_3_min = 1000;
float inductance_horizontal_4_min = 1000;
float horizontal_diff_ratio_sum = 0.0f;
float vertical_diff_ratio_sum = 0.0f;
//电感区间判定
void inductance_judgment(void)
{
    inductance_horizontal_1_max = (inductance_horizontal_1 > inductance_horizontal_1_max) ? inductance_horizontal_1 : inductance_horizontal_1_max;
    inductance_vertical_2_max = (inductance_vertical_2 > inductance_vertical_2_max) ? inductance_vertical_2 : inductance_vertical_2_max;
    inductance_vertical_3_max = (inductance_vertical_3 > inductance_vertical_3_max) ? inductance_vertical_3 : inductance_vertical_3_max;
    inductance_horizontal_4_max = (inductance_horizontal_4 > inductance_horizontal_4_max) ? inductance_horizontal_4 : inductance_horizontal_4_max;
    inductance_horizontal_1_min = (inductance_horizontal_1 < inductance_horizontal_1_min) ? inductance_horizontal_1 : inductance_horizontal_1_min;
    inductance_vertical_2_min = (inductance_vertical_2 < inductance_vertical_2_min) ? inductance_vertical_2 : inductance_vertical_2_min;
    inductance_vertical_3_min = (inductance_vertical_3 < inductance_vertical_3_min) ? inductance_vertical_3 : inductance_vertical_3_min;
    inductance_horizontal_4_min = (inductance_horizontal_4 < inductance_horizontal_4_min) ? inductance_horizontal_4 : inductance_horizontal_4_min;
    ips200_show_string(10, 0, "inductance_horizontal_1_max");
    sprintf(txt, "%08.1f", inductance_horizontal_1_max);
    ips200_show_string(10, 1*16, txt);    
    ips200_show_string(10, 2*16, "inductance_horizontal_1_min");
    sprintf(txt, "%08.1f", inductance_horizontal_1_min);
    ips200_show_string(10, 3*16, txt);    
    ips200_show_string(10, 4*16, "inductance_vertical_2_max");
    sprintf(txt, "%08.1f", inductance_vertical_2_max);
    ips200_show_string(10, 5*16, txt);
    ips200_show_string(10, 6*16, "inductance_vertical_2_min");
    sprintf(txt, "%08.1f", inductance_vertical_2_min);
    ips200_show_string(10, 7*16, txt);   
    ips200_show_string(10, 8*16, "inductance_vertical_3_max");
    sprintf(txt, "%08.1f", inductance_vertical_3_max);
    ips200_show_string(10, 9*16, txt);
    ips200_show_string(10, 10*16, "inductance_vertical_3_min");
    sprintf(txt, "%08.1f", inductance_vertical_3_min);
    ips200_show_string(10, 11*16, txt);   
    ips200_show_string(10, 12*16, "inductance_horizontal_4_max");
    sprintf(txt, "%08.1f", inductance_horizontal_4_max);
    ips200_show_string(10, 13*16, txt);
    ips200_show_string(10, 14*16, "inh4_min");
    sprintf(txt, "%08.1f", inductance_horizontal_4_min);
    ips200_show_string(100, 14*16, txt);   
}
//电感归一化
void inductance_normalization_max(void)
{
    //电感归一化
    inductance_horizontal_1 = (inductance_horizontal_1 / inductance_horizontal_1_max )*100;
    inductance_vertical_2   = (inductance_vertical_2   / inductance_vertical_2_max   )*100;
    inductance_vertical_3   = (inductance_vertical_3   / inductance_vertical_3_max   )*100;
    inductance_horizontal_4 = (inductance_horizontal_4 / inductance_horizontal_4_max )*100;
}

//电感差比和
void inductance_difference_ratio_sum(void)
{
    // 计算水平差比和
    float vertical_sum;
    float vertical_diff;
    float horizontal_diff;
    float horizontal_sum;
    horizontal_diff = inductance_horizontal_1 - inductance_horizontal_4; //水平电感差
    horizontal_sum = inductance_horizontal_1 + inductance_horizontal_4; //水平电感和

    // 防止除以零
    if (horizontal_sum > 0) {
        horizontal_diff_ratio_sum = horizontal_diff / horizontal_sum;
    } else {
        horizontal_diff_ratio_sum = 0.0f; // 或者根据需要设置为其他默认值
    }

    // 计算垂直差比和
    vertical_diff = inductance_vertical_2 - inductance_vertical_3;
    vertical_sum = inductance_vertical_2 + inductance_vertical_3;

    // 防止除以零
    if (vertical_sum > 0) {
        vertical_diff_ratio_sum = vertical_diff / vertical_sum;
    } else {
        vertical_diff_ratio_sum = 0.0f; // 或者根据需要设置为其他默认值
    }
}


/*************************************************************************
 *  函数名称：void angle_control(void)
 *  功能说明：角度环控制 - 位置式PID
 *  参数说明：无
 *  函数返回：无
 *************************************************************************/
void angle_control(void)
{
    float angle_error = 0.0f;
    
    // 获取当前角度 (IMU的yaw值)
    float current_angle = euler.yaw;
    
    // 计算角度误差 (-180~180范围内)
    angle_error = target_angle - current_angle;
    
    // 将角度误差限制在-180~180范围内
    angle_error = angle_error > 180.0f ? angle_error - 360.0f 
                : angle_error<-180.0f  ? angle_error + 360.0f:angle_error;
    
    // 角度PID控制计算 (位置式)
    angle_output = PidLocCtrl(&PID_angle, angle_error);
    
    // 限制角速度范围 (±150°/s)
    angle_output = constrain_float(angle_output, -150.0f, 150.0f);
}

/*************************************************************************
 *  函数名称：void angle_speed_control(float target_angular_speed)
 *  功能说明：角速度环控制 - 增量式PID
 *  参数说明：target_angular_speed 目标角速度 (°/s)
 *  函数返回：无
 *************************************************************************/
void angle_speed_control(float target_angular_speed)
{
    float angular_speed_error = 0.0f;
    
    // 获取当前角速度 (IMU的z轴陀螺仪值)
    // 注意：imu963ra_gyro_z是原始值，需要转换为°/s
    float current_angular_speed = imu660ra_gyro_transition((float)imu660ra_gyro_z - imu_data.gyro_z);
    
    // 计算角速度误差
    angular_speed_error = target_angular_speed - current_angular_speed;
    
    // 限制输出范围 (±4000)
    PID_angle_speed.out = constrain_float(PID_angle_speed.out, -4000.0f, 4000.0f);

    // 角速度PID控制计算 (增量式)
    angle_speed_output = PidIncCtrl(&PID_angle_speed, angular_speed_error);
    
    
}

/*************************************************************************
 *  函数名称：void speed_control_L(void)
 *  功能说明：左轮速度闭环控制 - 位置式PID
 *  参数说明：无
 *  函数返回：无
 *************************************************************************/
void speed_control_L(void)
{
    float speed_error = 0.0f;
    
    // 获取左轮当前速度
    float current_speed_L = encoder_data_dir_1;
    
    // 计算左轮速度误差（考虑角速度的影响）
    // 当有转向需求时，内侧车轮应该减速
    float target_speed_L = target_speed;
        
    target_speed_L = target_speed * (1.0f + angle_speed_output / 4000.0f); // 加减速
    
    speed_error = target_speed_L - current_speed_L;
    
    // 速度PID控制计算 (位置式)
    motor_output_L = PidLocCtrl(&PID_MOTOR_L, speed_error);
    
    // 限制输出范围
    motor_output_L = constrain_float(motor_output_L, 1.0f, 5000.0f);
}

/*************************************************************************
 *  函数名称：void speed_control_R(void)
 *  功能说明：右轮速度闭环控制 - 位置式PID
 *  参数说明：无
 *  函数返回：无
 *************************************************************************/
void speed_control_R(void)
{
    float speed_error = 0.0f;
    
    // 获取右轮当前速度
    float current_speed_R = encoder_data_dir_2;
    
    // 计算右轮速度误差（考虑角速度的影响）
    // 当有转向需求时，内侧车轮应该减速
    float target_speed_R = target_speed;
    
    target_speed_R = target_speed * (1.0f - angle_speed_output / 4000.0f); // 加减速
    
    speed_error = target_speed_R - current_speed_R;
    
    // 速度PID控制计算 (位置式)
    motor_output_R = PidLocCtrl(&PID_MOTOR_R, speed_error);
    
    // 限制输出范围
    motor_output_R = constrain_float(motor_output_R, 1.0f, 5000.0f);
}

/*************************************************************************
 *  函数名称：void speed_control(void)
 *  功能说明：整体速度控制 - 分别控制左右轮速度
 *  参数说明：无
 *  函数返回：无
 *************************************************************************/
void speed_control(void)
{
    // 分别控制左右车轮速度
    speed_control_L();
    speed_control_R();
    
    // 角速度补偿（差速转向）
    // 当需要转向时，两轮输出差异将由各自的PID控制器产生
    // 此处可以进行额外的微调
    // if(angle_speed_output != 0) {
    //     // 在各轮速度控制基础上增加角速度差分
    //     float steering_factor = 0.5f; // 转向系数，可根据需要调整
    //     motor_output_L -= angle_speed_output * steering_factor;
    //     motor_output_R += angle_speed_output * steering_factor;
        
    //     // 再次限制输出范围
    //     motor_output_L = constrain_float(motor_output_L, 1.0f, 5000.0f);
    //     motor_output_R = constrain_float(motor_output_R, 1.0f, 5000.0f);
    // }
}

/*************************************************************************
 *  函数名称：void control_update(void)
 *  功能说明：整体控制系统更新函数
 *  参数说明：无
 *  函数返回：无
 *************************************************************************/
void control_update(void)
{
    // 1. 角度环 - 输入：目标角度与当前角度，输出：目标角速度
    angle_control();
    
    // 2. 角速度环 - 输入：目标角速度与当前角速度，输出：角速度偏差
    angle_speed_control(angle_output);
    
    // 3. 速度环 - 输入：目标速度与当前速度，输出：电机控制信号
    speed_control(); // 调用更新后的速度控制函数
}

/*************************************************************************
 *  函数名称：void control_test(void)
 *  功能说明：控制系统测试函数
 *  参数说明：无
 *  函数返回：无
 *************************************************************************/
void control_test(void)
{
//    uint8 exit_flag = 0;
//    
//    ips200_clear(RGB565_WHITE);
//    ips200_show_string(10, 10, "Control System Test");
//    ips200_show_string(10, 30, "Press OK to exit");
//    
//    // 设定测试参数
//    target_angle = 0.0f;  // 保持直线
//    target_speed = 30.0f; // 适当速度
//    
//    while(!exit_flag)
//    {
//        // 更新按键状态
//        button_entry();
//        
//        // 更新控制系统
//        control_update();
//        
//        // 显示当前状态
//        sprintf(txt, "Target Angle: %05.2f", target_angle);
//        ips200_show_string(10, 50, txt);
//        
//        sprintf(txt, "Current Angle: %05.2f", euler.yaw);
//        ips200_show_string(10, 70, txt);
//        
//        sprintf(txt, "Target Speed: %05.2f", target_speed);
//        ips200_show_string(10, 90, txt);
//        
//        sprintf(txt, "L Encoder: %05d", encoder_data_dir_1);
//        ips200_show_string(10, 110, txt);
//        
//        sprintf(txt, "R Encoder: %05d", encoder_data_dir_2);
//        ips200_show_string(10, 130, txt);
//        
//        sprintf(txt, "L Output: %08.2f", motor_output_L);
//        ips200_show_string(10, 150, txt);
//        
//        sprintf(txt, "R Output: %08.2f", motor_output_R);
//        ips200_show_string(10, 170, txt);
//        // 检测退出按键
//        if(button5)
//        {
//            exit_flag = 1;
//            
//        }
//        system_delay_ms(5);
//    }
}
//控制初始化
void control_init(void)
{
    pwm_init(PWM_L1, 4000, 50);
	pwm_init(PWM_L2, 4000, 50);
    pwm_init(PWM_R1, 4000, 50);   
    pwm_init(PWM_R2, 4000, 50);   
}
//控制输出
void control_output(void)
{
    control_update();
    motor_output_L = constrain_float(motor_output_L, 0, 5000);
    motor_output_R = constrain_float(motor_output_R, 0, 5000);
    if(motor_output_L > 0)
    {
      pwm_set_duty(PWM_L1, motor_output_L);
	  pwm_set_duty(PWM_L2, motor_output_L);
        //待定正转
    }
    else
    {
        //待定反转
      pwm_set_duty(PWM_L1, -motor_output_L);
	    pwm_set_duty(PWM_L2, -motor_output_L);
    }
    if(motor_output_R > 0)
    {
        pwm_set_duty(PWM_R1, motor_output_R);
	    pwm_set_duty(PWM_R2, motor_output_R);
        //待定正转
    }
    else
    {
        //待定反转
        pwm_set_duty(PWM_R1, -motor_output_R);
	    pwm_set_duty(PWM_R2, -motor_output_R);
    }
}

//速度环调试修正
void speed_debug(void)
{
    //抑制其他环
    angle_speed_output=0;
    angle_output=0;
    speed_control();
    control_output();
}
//角度环调试修正
void angle_debug(void)
{
    control_update();
    control_output();
}
//核心循迹函数
//暂时注释目标速度
void core_trace(void)
{
    float total_inductance = 0.0f;
    float angle_correction = 0.0f;
    
    // 3. 计算目标角度
    // 根据水平和垂直电感的差比和计算目标偏航角
    // 水平差比和控制主要转向，垂直差比和用于辅助判断
    
    // 水平电感差比和作为主要控制量
    angle_correction = horizontal_diff_ratio_sum  * 90.0f; // 比例因子30.0可根据实际情况调整
    
    // 垂直电感差异用于辅助判断特殊情况
    // 如十字路口、锐角弯等
    if(vertical_diff_ratio_sum > 0.7f || vertical_diff_ratio_sum < -0.7f) {
        // 垂直电感差异很大时，可能是特殊路况
        // 可以进行特殊处理，比如减速或增大转向系数
        angle_correction += vertical_diff_ratio_sum * 90.0f; // 增强转向
        // target_speed = 20.0f; // 降低速度
    } else {
        // 正常路况
        // target_speed = 30.0f; // 正常速度
    }
//=========================================元素判断=======================================
    // 根据电感总量判断是否在赛道上
    total_inductance = inductance_horizontal_1 + inductance_horizontal_4 + 
                            inductance_vertical_2 + inductance_vertical_3;
    
    if(total_inductance < 1.0f) {
        // 电感总量太小，可能偏离赛道
        // 进入紧急处理模式
        time_out++;
        if(time_out > 10000)
        {
            target_speed = 10.0f; // 大幅降低速度
            // 可以根据上一次的转向继续转向，或停车等待
        }
    }
    else
    {
        time_out = 0;
    }
//=========================================控制区=========================================
    // 4. 设置目标角度
    target_angle = euler.yaw - angle_correction; // 当前角度 - 修正角度
    
    // 确保角度在-180到180范围内
    target_angle = (target_angle > 180.0f) ? (target_angle - 360.0f) : 
                  ((target_angle < -180.0f) ? (target_angle + 360.0f) : target_angle);
    

    
}