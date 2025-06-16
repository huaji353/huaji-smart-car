#include "zf_common_headfile.h"

/* 全局变量定义 */
imu_data_t imu_data;                           /* IMU数据 */
Quaternion q = {1.0f, 0.0f, 0.0f, 0.0f};       /* 四元数，初始化为单位四元数 */
EulerAngle euler = {0.0f, 0.0f, 0.0f};         /* 欧拉角 */
float encoder_file[1000];
uint16 encoder_ins_num = 0;      /*ins总计数*/
uint8 ins_go_flag = 0;          /*ins发车位*/
uint8 encoder_ins_memory_flag=0; /*惯性导航存储标志位*/
#ifdef text_open
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU数据显示函数，用于在IPS屏幕上显示IMU的加速度和陀螺仪数据
// 参数说明     void
// 返回参数     void
// 使用示例     imu660ra_text();
//-------------------------------------------------------------------------------------------------------------------
void imu660ra_text(void)
{   
    /* 获取IMU660RA数据 */
    imu660ra_get_acc();
    imu660ra_get_gyro();
    /* 在IPS显示器上显示加速度数据 */
    ips200_show_string(0, 16, "acc_x: ");
    ips200_show_int16(60, 16, imu660ra_acc_x);
    
    ips200_show_string(0, 32, "acc_y: ");
    ips200_show_int16(60, 32, imu660ra_acc_y);
    
    ips200_show_string(0, 48, "acc_z: ");
    ips200_show_int16(60, 48, imu660ra_acc_z);
    
    /* 在IPS显示器上显示陀螺仪数据 */
    ips200_show_string(0, 70, "IMU660RA Gyroscope:");
    ips200_show_string(0, 86, "gyro_x: ");
    ips200_show_int16(60, 86, imu660ra_gyro_x);
    
    ips200_show_string(0, 102, "gyro_y: ");
    ips200_show_int16(60, 102, imu660ra_gyro_y);
    
    ips200_show_string(0, 118, "gyro_z: ");
    ips200_show_int16(60, 118, imu660ra_gyro_z);
    
    /* 短暂延时，使显示更稳定 */
    system_delay_ms(100);
}
#endif

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     四元数初始化函数，用于初始化四元数和陀螺仪偏置
// 参数说明     void
// 返回参数     void
// 使用示例     quaternion_init();
//-------------------------------------------------------------------------------------------------------------------
void quaternion_init(void) 
{
    /* 初始化为单位四元数 */
    q.w = 1.0f;
    q.x = 0.0f;
    q.y = 0.0f;
    q.z = 0.0f;
    
    /* 初始化陀螺仪偏置 */
    imu_data.gyro_x = 0.0f;
    imu_data.gyro_y = 0.0f;
    imu_data.gyro_z = 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     角速度数据低通滤波函数，用于滤除小幅度噪声
// 参数说明     cy: 输入的角速度值
// 返回参数     float: 滤波后的角速度值
// 使用示例     filtered_gyro = IMU_lvbo(raw_gyro);
//-------------------------------------------------------------------------------------------------------------------
float IMU_lvbo(float cy)
{
    if (cy < 0.15f && cy > -0.15f)
    {
        cy = 0.0f;
    }
    return cy;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     陀螺仪校准函数，用于计算陀螺仪的零偏值
// 参数说明     samples: 采样点数
// 返回参数     void
// 使用示例     gyro_calibrate(200);
//-------------------------------------------------------------------------------------------------------------------
void gyro_calibrate(uint16 samples) 
{
    uint16 i;
    
    /* 清除校准前的数据 */
    imu_data.gyro_x = 0.0f;
    imu_data.gyro_y = 0.0f;
    imu_data.gyro_z = 0.0f;
    
    /* 清除屏幕，提示校准开始 */
    ips200_clear(RGB565_WHITE);
    ips200_show_string(0, 0, "imu gyro calibrate...");
    ips200_show_string(0, 20, "please keep still");
    
    /* 收集多个样本求平均值 */
    for(i = 0; i < samples; i++) 
    {
        imu660ra_get_gyro();
        
        imu_data.gyro_x += (float)imu660ra_gyro_x;
        imu_data.gyro_y += (float)imu660ra_gyro_y;
        imu_data.gyro_z += (float)imu660ra_gyro_z;
        
        /* 更新进度 */
        if(i % 10 == 0) 
        {
            sprintf(txt, "progress: %d%%", (int)(i * 100 / samples));
            ips200_show_string(0, 40, txt);
        }
        
        system_delay_ms(5);
    }
    
    /* 计算偏置 */
    imu_data.gyro_x = imu_data.gyro_x / (float)samples;
    imu_data.gyro_y = imu_data.gyro_y / (float)samples;
    imu_data.gyro_z = imu_data.gyro_z / (float)samples;
    
    /* 显示校准结果 */
    sprintf(txt, "bias_x: %.2f", imu_data.gyro_x);
    ips200_show_string(0, 60, txt);
    sprintf(txt, "bias_y: %.2f", imu_data.gyro_y);
    ips200_show_string(0, 80, txt);
    sprintf(txt, "bias_z: %.2f", imu_data.gyro_z);
    ips200_show_string(0, 100, txt);
    
    ips200_show_string(0, 120, "calibrate done!");
    system_delay_ms(500);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     四元数归一化函数，用于保持四元数的单位长度
// 参数说明     q: 指向待归一化的四元数的指针
// 返回参数     void
// 使用示例     quat_normalize(&q);
//-------------------------------------------------------------------------------------------------------------------
void quat_normalize(Quaternion* q)
{
    float norm = sqrt(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z);
    
    /* 防止除以零 */
    if(norm < 0.0001f)
    {
        q->w = 1.0f;
        q->x = 0.0f;
        q->y = 0.0f;
        q->z = 0.0f;
        return;
    }
    
    q->w = q->w / norm;
    q->x = q->x / norm;
    q->y = q->y / norm;
    q->z = q->z / norm;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     四元数更新函数，用于根据陀螺仪数据更新四元数
// 参数说明     void
// 返回参数     void
// 使用示例     quaternion_update();
//-------------------------------------------------------------------------------------------------------------------
void quaternion_update(void) 
{
    float gx, gy, gz;
    float qDot1, qDot2, qDot3, qDot4;
    
    /* 读取IMU传感器数据 */
    imu660ra_get_gyro();
    
    /* 处理陀螺仪数据，减去偏置并转换为弧度/秒 */
    gx = DEG_TO_RAD(imu660ra_gyro_transition((float)imu660ra_gyro_x - imu_data.gyro_x));
    gy = DEG_TO_RAD(imu660ra_gyro_transition((float)imu660ra_gyro_y - imu_data.gyro_y));
    gz = DEG_TO_RAD(imu660ra_gyro_transition((float)imu660ra_gyro_z - imu_data.gyro_z));

    /* 应用低通滤波 */
    gx = IMU_lvbo(gx);
    gy = IMU_lvbo(gy);
    gz = IMU_lvbo(gz);
    
    /* 基于四元数微分方程进行积分更新 */
    /* q_dot = 0.5 * q ⊗ ω，其中ω为角速度四元数[0,gx,gy,gz] */
    qDot1 = 0.5f * (-q.x * gx - q.y * gy - q.z * gz);
    qDot2 = 0.5f * (q.w * gx + q.y * gz - q.z * gy);
    qDot3 = 0.5f * (q.w * gy - q.x * gz + q.z * gx);
    qDot4 = 0.5f * (q.w * gz + q.x * gy - q.y * gx);
    
    /* 使用欧拉积分法更新四元数 */
    q.w = q.w + qDot1 * SAMPLE_FREQ;
    q.x = q.x + qDot2 * SAMPLE_FREQ;
    q.y = q.y + qDot3 * SAMPLE_FREQ;
    q.z = q.z + qDot4 * SAMPLE_FREQ;
    
    /* 四元数归一化 */
    quat_normalize(&q);
    
    /* 计算欧拉角 */
    euler = quaternion_to_euler(q);
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     四元数转欧拉角函数，仅计算偏航角
// 参数说明     q: 输入的四元数
// 返回参数     EulerAngle: 计算得到的欧拉角（仅偏航角有效）
// 使用示例     EulerAngle angles = quaternion_to_euler(current_q);
//-------------------------------------------------------------------------------------------------------------------
EulerAngle quaternion_to_euler(Quaternion q) 
{
    float siny_cosp, cosy_cosp;
    
    /* 初始化其他角度为0 */
    euler.roll = 0.0f;
    euler.pitch = 0.0f;
    
    /* 计算偏航角 (Yaw) */
    siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    euler.yaw = atan2(siny_cosp, cosy_cosp);
    
    /* 转换为角度 */
    euler.yaw = euler.yaw * 180.0f / PI;
    
    return euler;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     四元数显示函数，用于在IPS屏幕上显示四元数和欧拉角
// 参数说明     void
// 返回参数     void
// 使用示例     quaternion_display();
//-------------------------------------------------------------------------------------------------------------------
void quaternion_display(void) 
{
    /* 显示四元数 */
    sprintf(txt, "Q.w: %.4f", q.w);
    ips200_show_string(0, 0, txt);
    sprintf(txt, "Q.x: %.4f", q.x);
    ips200_show_string(0, 16, txt);
    sprintf(txt, "Q.y: %.4f", q.y);
    ips200_show_string(0, 32, txt);
    sprintf(txt, "Q.z: %.4f", q.z);
    ips200_show_string(0, 48, txt);
    
    /* 显示欧拉角 */
    sprintf(txt, "Yaw  : %.2f", euler.yaw);
    ips200_show_string(0, 112, txt);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU中断服务函数，用于处理IMU数据更新
// 参数说明     void
// 返回参数     void
// 使用示例     由定时器中断自动调用
//-------------------------------------------------------------------------------------------------------------------
void imu_isr(void)
{
    /* 更新四元数 */
    quaternion_update();
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     简易惯性导航存入
// 参数说明     void
// 返回参数     void
// 使用示例     encoder_ins_infile();
//-------------------------------------------------------------------------------------------------------------------
void encoder_ins_infile(void) 
{
    static uint8 encoder_ins_last_memory_flag=0;
    encoder_integral_flag=1;
    if(!encoder_ins_memory_flag){encoder_ins_last_memory_flag=0;return;}//存储开启标志
    if(!encoder_ins_last_memory_flag){encoder_ins_num=0;encoder_ins_last_memory_flag=1;}//上一次比对判断是否第一次进入存储并归位
    if(encoder_ins_num==0){encoder_file[encoder_ins_num]=euler.yaw;encoder_ins_num++;}//第一次存储直接记录当前角度
    if(encoder_ins_num==1000)return;
    if(encoder_integral_1+encoder_integral_2<speed_threshold)return;//脉冲计数
    encoder_integral_1=0;
    encoder_integral_2=0;
    // encoder_del=encoder_data_dir_1-encoder_data_dir_2;
    encoder_file[encoder_ins_num]=euler.yaw;
    encoder_ins_num++;
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     简易惯性导航实现
// 参数说明     void
// 返回参数     void
// 使用示例     encoder_ins();
//-------------------------------------------------------------------------------------------------------------------
void encoder_ins(void)
{
    static uint8 frist_in_flag;//第一次进入标志位
    static uint16 encoder_ins_num_record;
    if(!ins_go_flag){frist_in_flag=0;return;}
    if(!frist_in_flag){encoder_ins_num_record=encoder_ins_num;encoder_ins_num=0;frist_in_flag=1;}//记录惯性存储的总数，并归位计数
    target_speed=ins_speed;//总控速度
    target_angle=encoder_file[encoder_ins_num];//总控角度
    if(encoder_integral_1+encoder_integral_2>=speed_threshold){//计数更新
        encoder_integral_1=0;
        encoder_integral_2=0;
        encoder_ins_num++;
    }
    if(encoder_ins_num>=encoder_ins_num_record){ins_go_flag=0;target_speed=0;return;}//停车
}