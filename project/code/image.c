#include "zf_common_headfile.h  "

//将yaw和编码器结合绘制出路径图并显示在ips200屏幕上
void image_path(void)
{
   // 静态变量存储上一个点的坐标
   static uint16 last_x = 160;  // 初始为屏幕中心x坐标
   static uint16 last_y = 120;  // 初始为屏幕中心y坐标
   static uint8 first_point = 1;  // 第一个点标志
   static uint8 i;  // 循环变量
   static int16 last_encoder = 0; // 存储上一次的编码器值
   static uint16 color = RGB565_RED;  // 画线颜色
   
   // 局部变量声明（移至函数开头）
   int16 xdata current_encoder;
   float xdata speed;
   float xdata scaling_factor;
   float xdata angle_rad;
   uint16 xdata new_x;
   uint16 xdata new_y;
   int16 xdata dx;
   int16 xdata dy;
   int16 xdata steps;
   float xdata x_inc;
   float xdata y_inc;
   float xdata x;
   float xdata y;
   
   // 获取yaw和编码器数据
   current_encoder = encoder_data_dir_1;  // 从编码器获取数据
   
   // 显示当前yaw和编码器值
//    ips200_show_string(10, 10, "Path Visualization");
//    ips200_show_string(10, 30, "Yaw:");
//    ips200_show_float(50, 30, euler.yaw, 2, 2);
//    ips200_show_string(10, 50, "Encoder:");
//    ips200_show_int16(80, 50, current_encoder);
   
   // 如果按下清除按钮，重置路径
   if(button1)
   {
       ips200_clear(RGB565_WHITE);
       first_point = 1;
       last_x = 160;
       last_y = 120;
       last_encoder = current_encoder;
       
       // 显示当前信息
//        ips200_show_string(10, 10, "Path Visualization");
//        ips200_show_string(10, 30, "Yaw:");
//        ips200_show_float(50, 30, euler.yaw, 3, 2);
//        ips200_show_string(10, 50, "Encoder:");
//        ips200_show_int16(80, 50, current_encoder);
//        ips200_show_string(10, 70, "Path Reset!");
       system_delay_ms(300);
//        ips200_show_string(10, 70, "           ");
       
       // 在中心点绘制起点标记
       ips200_draw_point(last_x, last_y, RGB565_BLUE);
       ips200_draw_point(last_x-1, last_y, RGB565_BLUE);
       ips200_draw_point(last_x+1, last_y, RGB565_BLUE);
       ips200_draw_point(last_x, last_y-1, RGB565_BLUE);
       ips200_draw_point(last_x, last_y+1, RGB565_BLUE);
       return;
   }
   
   // 计算速度（编码器值代表速度）
   speed = current_encoder;
   
   // 速度缩放因子
   scaling_factor = 0.02f;  // 调整值使位移适合显示
   
   // 如果速度太小且不是第一个点，无需绘制
   if(fabs(speed) < 1.0f && !first_point)return;
   
   // 计算位移方向和新坐标
   angle_rad = euler.yaw * PI / 180.0f;  // 转换为弧度
   
   // 计算新的坐标点（速度在yaw方向上的分量）
   new_x = last_x + (int16)(speed * scaling_factor * cos(angle_rad));
   new_y = last_y - (int16)(speed * scaling_factor * sin(angle_rad));  // 减法因为屏幕y轴向下
   
   // 边界检查，确保坐标在屏幕范围内
		new_x=new_x > 319 ? 319:
					new_x < 1   ? 1:
					new_x;
		new_y=new_y > 239 ? 239:
					new_y < 1   ? 1:
					new_y;
   
   // 根据速度更改颜色
		color=fabs(speed)>50.0f?RGB565_RED:
					fabs(speed)>20.0f?RGB565_YELLOW:
					RGB565_GREEN;
   
   // 如果是第一个点，只画点
   if(first_point)
   {
       // 在初始点绘制一个特殊标记
       ips200_draw_point(new_x, new_y, RGB565_BLUE);
       ips200_draw_point(new_x-1, new_y, RGB565_BLUE);
       ips200_draw_point(new_x+1, new_y, RGB565_BLUE);
       ips200_draw_point(new_x, new_y-1, RGB565_BLUE);
       ips200_draw_point(new_x, new_y+1, RGB565_BLUE);
       first_point = 0;
   }
   else
   {
       // 使用Bresenham算法绘制线段
       dx = new_x - last_x;
       dy = new_y - last_y;
       steps = fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy);
       
       if(steps > 0)
       {
           x_inc = (float)dx / steps;
           y_inc = (float)dy / steps;
           x = last_x;
           y = last_y;
           
           for(i = 0; i <= steps; i++)
           {
               ips200_draw_point((uint16)x, (uint16)y, color);
               x += x_inc;
               y += y_inc;
           }
       }
       else
       {
           // 即使steps为0，也画一个点（速度很小但不为零）
           ips200_draw_point(new_x, new_y, color);
       }
   }
   
   // 更新状态变量
   last_x = new_x;
   last_y = new_y;
   last_encoder = current_encoder;
   
   // 显示当前坐标位置和速度
//    ips200_show_string(160, 10, "Pos:");
//    ips200_show_int16(190, 10, new_x);
//    ips200_show_string(250, 10, ",");
//    ips200_show_int16(260, 10, new_y);
//    ips200_show_string(160, 30, "Speed:");
//    ips200_show_int16(220, 30, speed);
}