1.该系统的编译为c51

2.main.c为主函数

3.芯片为ai8051u

# 项目功能模块说明

## contrl.c 和 contrl.h 功能说明

### 结构体
1. **pid_param_t** - PID控制器参数结构体
   - kp, ki, kd - 比例、积分、微分系数
   - imax - 积分限幅值
   - out_p, out_i, out_d - 各部分输出值
   - out - 总输出值
   - integrator - 积分值
   - last_error - 上次误差
   - last_derivative - 上次误差变化量
   - last_t - 上次计算时间

### 全局变量
1. **PID控制器实例**
   - PID_angle - 角度环控制器(位置式)
   - PID_angle_speed - 角速度环控制器(增量式)
   - PID_MOTOR - 电机控制器(位置式)

2. **PID参数**
   - PID_angle_P, PID_angle_I, PID_angle_D - 角度环PID参数
   - PID_angle_speed_P, PID_angle_speed_I, PID_angle_speed_D - 角速度环PID参数
   - PID_MOTOR_P, PID_MOTOR_I, PID_MOTOR_D - 电机PID参数

3. **控制系统变量**
   - target_angle - 目标角度
   - target_speed - 目标速度
   - motor_output_L - 左电机输出
   - motor_output_R - 右电机输出
   - angle_output - 角度环输出
   - angle_speed_output - 角速度环输出

4. **电感系统变量**
   - inductance_horizontal_1_max, inductance_vertical_2_max, inductance_vertical_3_max, inductance_horizontal_4_max - 电感最大值
   - horizontal_diff_ratio_sum, vertical_diff_ratio_sum - 水平和垂直电感差比和

### 功能函数

#### PID控制函数
1. **PidLocCtrl(pid_param_t * pid, float error)** - 位置式PID控制器
   - 根据输入误差计算PID输出
   - 包含积分限幅处理
   - 返回PID控制器输出值

2. **PidIncCtrl(pid_param_t * pid, float error)** - 增量式PID控制器
   - 根据输入误差计算PID增量输出
   - 返回累积后的PID控制器输出值

#### 电感处理函数
1. **inductance_normalization_max(void)** - 电感归一化函数
   - 记录电感最大值
   - 将电感值按最大值归一化为0-100范围

2. **inductance_difference_ratio_sum(void)** - 电感差比和计算函数
   - 计算水平和垂直电感的差比和
   - 差比和用于判断小车相对于轨道的位置偏差

#### 闭环控制函数
1. **angle_control(void)** - 角度环控制函数
   - 基于位置式PID算法
   - 计算当前角度与目标角度误差
   - 输出目标角速度

2. **angle_speed_control(float target_angular_speed)** - 角速度环控制函数
   - 基于增量式PID算法
   - 计算当前角速度与目标角速度误差
   - 输出角速度控制量

3. **speed_control(void)** - 速度环控制函数
   - 基于位置式PID算法
   - 计算当前速度与目标速度误差
   - 结合角速度输出计算左右轮差分控制量

4. **control_update(void)** - 控制系统更新函数
   - 按角度环->角速度环->速度环顺序更新整个控制系统
   - 级联PID控制架构

#### 硬件控制函数
1. **control_init(void)** - 控制系统初始化函数
   - 初始化PWM输出通道
   - 设置PWM频率和初始占空比

2. **control_output(void)** - 电机控制输出函数
   - 限制电机输出范围
   - 根据输出值控制电机正反转
   - 设置PWM占空比

#### 辅助函数
1. **constrain_float(float amt, float low, float high)** - 浮点数限幅函数
   - 限制输入值在指定范围内
   - 用于防止PID输出超出系统可接受范围

2. **PidInit()** - PID参数初始化函数
   - 初始化所有PID控制器的参数
   - 设置默认PID参数值和限幅

3. **control_test(void)** - 控制系统测试函数
   - 在IPS200屏幕上显示控制系统状态
   - 实时显示角度、速度和输出值
   - 通过按键退出测试

4. **core_trace(void)** - 核心循迹函数
   - 实现小车的循迹控制逻辑（小车实际下地两天不到驱动全炸了）

## key.c 和 key.h 功能说明

### 按键定义
1. **button1** - 左摇按键
2. **button2** - 右摇按键
3. **button3** - 下摇按键
4. **button4** - 上摇按键
5. **button5** - 确定按键

### 全局变量
1. **按键状态变量**：每个按键都有对应的状态变量
   - key1_state, key2_state, key3_state, key4_state, key5_state
   - key1_state_last, key2_state_last, key3_state_last, key4_state_last, key5_state_last
   - key1_flag, key2_flag, key3_flag, key4_flag, key5_flag

2. **按键反馈变量**：用于外部访问按键状态
   - button1, button2, button3, button4, button5

### 按键引脚定义
1. **KEY_1** - IO_P46
2. **KEY_2** - IO_P15
3. **KEY_3** - IO_P45
4. **KEY_4** - IO_P36
5. **KEY_5** - IO_P37

### 功能函数

#### 按键检测函数
1. **key1_can()** - 检测按键1的状态变化
   - 返回1表示按键被按下，0表示未按下

2. **key2_can()**, **key3_can()**, **key4_can()**, **key5_can()** - 同样功能的其他按键检测函数

#### 按键处理函数
1. **button_entry()** - 按键输入处理函数
   - 读取所有按键的当前状态
   - 实现按键消抖处理
   - 检测长按动作
   - 设置按键反馈变量

2. **key_into()** - 按键初始化函数
   - 初始化所有按键GPIO为上拉输入模式

3. **key_text()** - 按键测试函数
   - 显示所有按键状态到IPS200屏幕

### 常量
1. **LONG_PRESS_TIME** - 长按时间阈值，默认为10（约400ms）

## IMU.c 功能说明

### 结构体
1. **Quaternion** - 四元数结构体
   - w, x, y, z 四个分量
   - 用于表示三维旋转

2. **EulerAngle** - 欧拉角结构体
   - roll (横滚角)
   - pitch (俯仰角)
   - yaw (偏航角)

3. **imu_data_t** - IMU传感器数据结构体
   - 包含陀螺仪数据

### 全局变量
1. **imu_data** - IMU数据实例
2. **q** - 四元数实例，初始化为单位四元数(1,0,0,0)
3. **euler** - 欧拉角实例，用于存储当前姿态

### 功能函数

#### 初始化与校准
1. **quaternion_init()** - 四元数初始化
   - 将四元数初始化为单位四元数
   - 清零陀螺仪偏置

2. **gyro_calibrate(uint16 samples)** - 陀螺仪校准函数
   - 收集多个样本计算陀螺仪偏置
   - 校准过程显示在屏幕上

#### 数据处理
1. **IMU_lvbo(float cy)** - 角速度数据低通滤波
   - 小于阈值的数据置零

2. **quat_normalize(Quaternion* q)** - 四元数归一化
   - 确保四元数模长为1

3. **quaternion_update()** - 更新四元数
   - 读取IMU传感器数据
   - 使用角速度计更新四元数
   - 计算欧拉角

4. **quaternion_to_euler(Quaternion q)** - 四元数转欧拉角
   - 将四元数转换为欧拉角(ZYX顺序)
   - 返回角度值(度)

5. **get_quaternion()** - 获取当前四元数
   - 返回全局四元数实例

6. **custom_atan2(float y, float x)** - 自定义反正切函数
   - 计算反正切值

#### 中断与测试
1. **imu_isr()** - IMU中断服务函数
   - 更新四元数
   - 可在定时器中断中调用

#### 显示函数
1. **imu660ra_text()** - IMU传感器原始数据显示函数
   - 显示加速度和陀螺仪原始读数

2. **quaternion_display()** - 显示四元数和欧拉角
   - 在IPS200显示屏上显示四元数和欧拉角

3. **quaternion_test()** - 四元数测试函数
   - 调用quaternion_display()并添加延时

## menu.c 功能说明

### 结构体
1. **menu_item_t** - 菜单项结构体
   - name - 菜单项名称
   - function - 菜单项对应的功能函数指针
   - has_child - 是否有子菜单标志
   - child_index - 子菜单索引

2. **menu_page_t** - 菜单页结构体
   - title - 菜单页标题
   - items - 菜单项数组
   - item_count - 菜单项数量

3. **menu_state_t** - 菜单状态结构体
   - current_level - 当前菜单层级
   - current_page - 各层级当前显示的菜单页索引
   - current_index - 各层级当前选中的菜单项索引

### 全局变量
1. **menu_pages** - 菜单页数组，存储所有菜单页
2. **menu_state** - 全局菜单状态

### 菜单索引常量
1. **MAIN_MENU_INDEX** - 主菜单索引
2. **SETTING_MENU_INDEX** - 设置菜单索引
3. **PID_MENU_INDEX** - PID参数菜单索引
4. **TEST_MENU_INDEX** - 测试功能菜单索引
5. **GAME_MENU_INDEX** - 游戏菜单索引

### 功能函数

#### 菜单系统核心函数
1. **menu_init()** - 菜单系统初始化
   - 初始化菜单状态
   - 设置所有菜单页和菜单项

2. **menu_display()** - 显示当前菜单
   - 显示菜单标题和分隔线
   - 显示菜单项，突出显示当前选中项
   - 显示操作提示

3. **menu_process_buttons()** - 处理菜单按键输入
   - 处理上下左右按键导航
   - 处理确定按键选择功能
   - 处理返回上级菜单

4. **menu_run()** - 菜单运行函数
   - 处理按键状态
   - 调用菜单处理函数
   - 在主循环中调用

#### 测试功能函数
1. **function_key_test()** - 按键测试功能
2. **function_adc_test()** - ADC测试功能
3. **function_encoder_test()** - 编码器测试功能
4. **function_imu_test()** - IMU测试功能
5. **function_buzzer_test()** - 蜂鸣器测试功能
6. **function_uart_printf_test()** - UART打印测试功能
7. **function_image_path_test()** - 路径绘制测试功能

#### 设置功能函数
1. **function_speed_setting()** - 速度设置功能
2. **function_angle_pid_setting()** - 角度PID参数设置
3. **function_angle_speed_pid_setting()** - 角速度PID参数设置
4. **function_motor_pid_setting()** - 电机PID参数设置
5. **function_reset_pid()** - 重置所有PID参数
6. **function_sensor_calibration()** - 传感器校准功能
7. **function_system_info()** - 系统信息显示功能

## image.c 功能说明
### 功能函数

1. **image_path()** - 路径绘制函数
   - 结合yaw角度和编码器数据绘制路径
   - 在IPS200屏幕上实时显示运动轨迹
   - 支持路径重置和颜色变化

## 逐飞助手功能说明

### 通信接口
### 参数设置
1. **通道功能**
   - 通道0: 控制模式选择
     - 0: 基础控制模式
     - 1: 角度环参数调节
     - 2: 角速度环参数调节
     - 3: 左轮电机参数调节
     - 4: 右轮电机参数调节
     - 5: 保存参数到EEPROM
   
   - 通道1-3: PID参数调节
     - 通道1: P参数
     - 通道2: I参数
     - 通道3: D参数
   
   - 通道4-5: 系统目标值设置
     - 通道4: 目标速度设置
     - 通道5: 目标角度设置
   
   - 通道7: 显示模式选择
     - 0: 显示基本控制参数
     - 1: 显示角度环PID参数
     - 2: 显示角速度环PID参数
     - 3: 显示左轮电机PID参数
     - 4: 显示右轮电机PID参数
     - 5: 速度环调试数据
     - 6: 角度环调试数据
     - 7: 电感数据显示

### 数据显示
1. **基本控制参数显示**
   - 目标角度
   - 目标速度
   - 左右电机输出值

2. **PID参数显示**
   - 各环路的P、I、D参数值
   - 实时输出值
   - 误差值

3. **调试数据显示**
   - 编码器数据
   - 电机输出值
   - 角度数据
   - 角速度数据
   - 电感数据
## 所有使用方式均放在菜单以及逐飞助手通道中
   -目前发车只能通过上位机拉到对应的通道进行发车，速度和角度都能调（可以寻迹也可以当遥控车玩）

# 调试过程

## 遇到过的问题
   - 在进行陀螺仪测试的时候尝试过对角速度计进行纯积分的形式，但是测试下来总会与预测角度有个倍数的偏差，于是在四元数计算的时候将这个倍数的偏差用一个除法去除了。
   -（考虑到ai8051资源问题就把加速度计去除了，原先的计算中有加速度计进行梯度修正，数值是精确的，要是想纯角速度计算的再准点可以多套几阶龙格库塔）
   - 别的在测试过程中就一路畅通无阻了，除了因为用的王的foc，还有硬件那边可能也出了点问题，foc当时还是测试版，全部爆炸了
   - 因为用的正交编码器，而系统已经没有资源了，于是直接abs绝对值计算了编码器值（反正小车也不用倒退）
   - 当初把eeprom用错了，实际上eeprom没什么页的，直接进位计算导入就好，将所有pid和电感大小数值计算出来以后把数据导进code就不用再管了
## 额外的功能
   - 因为资源问题我将游戏和部分代码进行了移除和删减，很多逐飞原有的库和封装进行了删除，需要的话可以自行导入
# 感谢逐飞以及王的开源
