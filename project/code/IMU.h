#ifndef __IMU_H__
#define __IMU_H__

/* 常量定义 */
#define SAMPLE_FREQ  0.005f                   /* 采样频率 (s) */
#define PI      3.14159265358979323846f      /* 圆周率 */
#define speed_threshold 20000                /* 设定ins采样距离*/
#define ins_speed  2000                      /* 设定ins运行速度*/
#define RAD_TO_DEG(x) ((x) * 180.0f / PI)       /* 弧度转角度 */
#define DEG_TO_RAD(x) ((x) * PI / 180.0f)       /* 角度转弧度 */
/* IMU数据结构体，存储校准后的数据 */
typedef struct
{
    float gyro_x;     /* X轴角速度 */
    float gyro_y;     /* Y轴角速度 */
    float gyro_z;     /* Z轴角速度 */
} imu_data_t;

/* 四元数结构体 */
typedef struct {
    float w;  /* 实部 */
    float x;  /* 虚部i */
    float y;  /* 虚部j */
    float z;  /* 虚部k */
} Quaternion;

/* 欧拉角结构体 */
typedef struct {
    float roll;   /* 横滚角 */
    float pitch;  /* 俯仰角 */
    float yaw;    /* 偏航角 */
} EulerAngle;

/* 外部变量声明 */
extern imu_data_t imu_data;    /* IMU数据 */
extern Quaternion q;           /* 四元数 */
extern EulerAngle euler;       /* 欧拉角 */

/* 函数声明 */
/* 初始化与校准 */
void quaternion_init(void);                   /* 四元数初始化 */
void gyro_calibrate(uint16 samples);         /* 陀螺仪校准 */

/* 姿态更新 */
void quaternion_update(void);                /* 仅使用角速度计更新四元数 */
EulerAngle quaternion_to_euler(Quaternion q); /* 四元数转欧拉角 */
Quaternion get_quaternion(void);             /* 获取当前四元数 */
void quat_normalize(Quaternion* q);          /* 四元数归一化 */

/* 显示与测试 */
void quaternion_display(void);               /* 显示四元数和欧拉角 */
void imu660ra_text(void);                    /* IMU数据显示 */

/* 辅助函数 */
float IMU_lvbo(float cy);                      /* 角速度低通滤波 */
void imu_isr(void);                            /* IMU中断服务函数 */

#endif


