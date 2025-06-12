#include "zf_common_headfile.h"
#include "snake.h"

// 游戏区域定义
#define GAME_AREA_START_X  10
#define GAME_AREA_START_Y  40
#define GAME_AREA_WIDTH    180
#define GAME_AREA_HEIGHT   180
#define GRID_SIZE          8

// 颜色定义
#define COLOR_SNAKE    RGB565_GREEN
#define COLOR_FOOD     RGB565_RED
#define COLOR_BORDER   RGB565_BLUE
#define COLOR_BG       RGB565_WHITE
#define COLOR_TEXT     RGB565_BLACK

// 方向定义
#define DIR_UP      0
#define DIR_RIGHT   1
#define DIR_DOWN    2
#define DIR_LEFT    3

// 游戏状态定�?
#define GAME_RUNNING  0
#define GAME_OVER     1
#define GAME_PAUSE    2

// �?��蛇游戏结构体
typedef struct {
    uint8 x[180];     // 蛇身体x坐标数组
    uint8 y[180];     // 蛇身体y坐标数组
    uint8 length;     // 蛇的长度
    uint8 direction;  // 蛇的移动方向
    uint8 food_x;     // 食物x坐标
    uint8 food_y;     // 食物y坐标
    uint16 score;     // 游戏得分
    uint8 status;     // 游戏状�?
} snake_game_t;

static snake_game_t snake_game;
static uint16 game_speed = 150; // 游戏速度(ms)
static uint32 last_time = 0;    // 上�?更新时间
static uint32 game_time = 0;    // 游戏时间计数�?

// 函数声明
static void snake_game_init(void);
static void generate_food(void);
static void draw_border_line(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 color);
static void draw_game_area(void);
static void fill_grid(uint16 x, uint16 y, uint16 color);
static void draw_snake(void);
static void draw_food(void);
static void erase_tail(void);
static void move_snake(void);
static uint8 check_collision(void);
static void show_game_over(void);
static void show_game_info(void);

// 初�?化游�?
static void snake_game_init(void)
{
    uint8 i;
    
    // 初�?化蛇
    snake_game.length = 3;
    snake_game.direction = DIR_RIGHT;
    
    // 初�?蛇身位置 (�?��位置)
    for (i = 0; i < snake_game.length; i++) {
        snake_game.x[i] = GAME_AREA_WIDTH / (2 * GRID_SIZE) - i;
        snake_game.y[i] = GAME_AREA_HEIGHT / (2 * GRID_SIZE);
    }
    
    // 初�?化得�?
    snake_game.score = 0;
    snake_game.status = GAME_RUNNING;
    
    // 生成�?���??�?
    snake_game.food_x = rand() % (GAME_AREA_WIDTH / GRID_SIZE);
    snake_game.food_y = rand() % (GAME_AREA_HEIGHT / GRID_SIZE);
}

// 生成新的食物
static void generate_food(void)
{
    uint8 overlapping;
    uint8 i;
    
    do {
        // 随机生成食物位置
        snake_game.food_x = rand() % (GAME_AREA_WIDTH / GRID_SIZE);
        snake_game.food_y = rand() % (GAME_AREA_HEIGHT / GRID_SIZE);
        
        // 检查是否与蛇身重叠
        overlapping = 0;
        for (i = 0; i < snake_game.length; i++) {
            if (snake_game.x[i] == snake_game.food_x && 
                snake_game.y[i] == snake_game.food_y) {
                overlapping = 1;
                break;
            }
        }
    } while (overlapping);
}

// 绘制边�?�?
static void draw_border_line(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 color)
{
    uint16 i;
    if (x1 == x2) {  // 垂直�?
        for (i = y1; i <= y2; i++) {
            ips200_draw_point(x1, i, color);
        }
    } else if (y1 == y2) {  // 水平�?
        for (i = x1; i <= x2; i++) {
            ips200_draw_point(i, y1, color);
        }
    }
}

// 绘制游戏区域
static void draw_game_area(void)
{
    uint16 i;
    uint16 j;
    
    // 绘制边�?
    draw_border_line(GAME_AREA_START_X - 1, GAME_AREA_START_Y - 1, 
                    GAME_AREA_START_X + GAME_AREA_WIDTH + 1, GAME_AREA_START_Y - 1, COLOR_BORDER);  // 上边�?
    
    draw_border_line(GAME_AREA_START_X - 1, GAME_AREA_START_Y + GAME_AREA_HEIGHT + 1, 
                    GAME_AREA_START_X + GAME_AREA_WIDTH + 1, GAME_AREA_START_Y + GAME_AREA_HEIGHT + 1, COLOR_BORDER);  // 下边�?
    
    draw_border_line(GAME_AREA_START_X - 1, GAME_AREA_START_Y - 1, 
                    GAME_AREA_START_X - 1, GAME_AREA_START_Y + GAME_AREA_HEIGHT + 1, COLOR_BORDER);  // 左边�?
    
    draw_border_line(GAME_AREA_START_X + GAME_AREA_WIDTH + 1, GAME_AREA_START_Y - 1, 
                    GAME_AREA_START_X + GAME_AREA_WIDTH + 1, GAME_AREA_START_Y + GAME_AREA_HEIGHT + 1, COLOR_BORDER);  // 右边�?
    
    // 清空游戏区域
    for (i = GAME_AREA_START_X; i <= GAME_AREA_START_X + GAME_AREA_WIDTH; i++) {
        for (j = GAME_AREA_START_Y; j <= GAME_AREA_START_Y + GAME_AREA_HEIGHT; j++) {
            ips200_draw_point(i, j, COLOR_BG);
        }
    }
}

// �?��一�?���?
static void fill_grid(uint16 x, uint16 y, uint16 color)
{
    uint16 real_x = GAME_AREA_START_X + (x * GRID_SIZE);
    uint16 real_y = GAME_AREA_START_Y + (y * GRID_SIZE);
    uint16 i;
    uint16 j;
    
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            ips200_draw_point(real_x + i, real_y + j, color);
        }
    }
}

// 绘制�?
static void draw_snake(void)
{
    uint8 i;
    
    for (i = 0; i < snake_game.length; i++) {
        fill_grid(snake_game.x[i], snake_game.y[i], COLOR_SNAKE);
    }
}

// 绘制食物
static void draw_food(void)
{
    fill_grid(snake_game.food_x, snake_game.food_y, COLOR_FOOD);
}

// 擦除蛇尾
static void erase_tail(void)
{
    fill_grid(snake_game.x[snake_game.length-1], snake_game.y[snake_game.length-1], COLOR_BG);
}

// 移动�?
static void move_snake(void)
{
    uint8 i;
    uint8 new_x = snake_game.x[0];
    uint8 new_y = snake_game.y[0];
    
    // 根据方向更新头部位置
    switch (snake_game.direction) {
        case DIR_UP:
            new_y--;
            break;
        case DIR_RIGHT:
            new_x++;
            break;
        case DIR_DOWN:
            new_y++;
            break;
        case DIR_LEFT:
            new_x--;
            break;
    }
    
    // 检查是否吃到�?�?
    if (new_x == snake_game.food_x && new_y == snake_game.food_y) {
        // 增加分数
        snake_game.score += 10;
        // 增加蛇的长度
        snake_game.length++;
        // 生成新�?�?
        generate_food();
        // 减少游戏速度 (提高难度)
        if (game_speed > 50) {
            game_speed -= 5;
        }
        
        // 移动蛇身，保持尾部位�?���?
        for (i = snake_game.length - 1; i > 0; i--) {
            snake_game.x[i] = snake_game.x[i-1];
            snake_game.y[i] = snake_game.y[i-1];
        }
    } else {
        // 移动蛇身 (除头部�?)
        for (i = snake_game.length - 1; i > 0; i--) {
            snake_game.x[i] = snake_game.x[i-1];
            snake_game.y[i] = snake_game.y[i-1];
        }
    }
    
    // 更新蛇头位置
    snake_game.x[0] = new_x;
    snake_game.y[0] = new_y;
}

// 检查�?�?
static uint8 check_collision(void)
{
    uint8 i;
    
    // 检查是否撞到边�?
    if (snake_game.x[0] >= GAME_AREA_WIDTH / GRID_SIZE || 
        snake_game.y[0] >= GAME_AREA_HEIGHT / GRID_SIZE ||
        snake_game.x[0] < 0 ||
        snake_game.y[0] < 0) {
        return 1;
    }
    
    // 检查是否撞到自己的�?��
    for (i = 1; i < snake_game.length; i++) {
        if (snake_game.x[0] == snake_game.x[i] && 
            snake_game.y[0] == snake_game.y[i]) {
            return 1;
        }
    }
    
    return 0;
}

// 显示游戏结束
static void show_game_over(void)
{
    // 清除屏幕并显示游戏结束消�?
    ips200_clear(COLOR_BG);
    ips200_show_string(80, 100, "GAME OVER");
    
    // 显示最终得�?
    ips200_show_string(80, 120, "Score:");
    ips200_show_uint16(140, 120, snake_game.score);
    
    // 显示按键提示
    ips200_show_string(60, 160, "Press 5 to restart");
    ips200_show_string(60, 180, "Press 6 to exit");
    
    system_delay_ms(1000); // 防�?按键消抖
}

// 显示游戏界面信息
static void show_game_info(void)
{
    // 显示分数
    ips200_show_string(10, 5, "Score:");
    ips200_show_uint16(60, 5, snake_game.score);
    
    // 显示速度
    ips200_show_string(120, 5, "Speed:");
    ips200_show_uint16(170, 5, 200 - game_speed);
}

// �?��蛇游戏主函数
void function_snake(void)
{
    
    // 初�?化游�?
    ips200_clear(COLOR_BG);
    
    // 显示游戏标�?
    ips200_show_string(50, 120, "SNAKE GAME");
    ips200_show_string(30, 150, "Press any key to start");
    
    // 等待按键开始游�?
    while(1) {
        button_entry();
        if (button1 || button2 || button3 || button4 || button5) {
            break;
        }
        system_delay_ms(10);
    }
    
    // 清除按键状�?
    button1 = 0; button2 = 0; button3 = 0; button4 = 0; button5 = 0;
    
    // 初�?化游�?
    ips200_clear(COLOR_BG);
    snake_game_init();
    draw_game_area();
    draw_food();
    draw_snake();
    
    // 游戏主循�?
    game_time = 0;
    while (1) {
        // 处理按键输入
        button_entry();
        
        // 处理方向控制
        if (button4 && snake_game.direction != DIR_DOWN) {  // 上键
            snake_game.direction = DIR_UP;
        }
        if (button3 && snake_game.direction != DIR_UP) {    // 下键
            snake_game.direction = DIR_DOWN;
        }
        if (button1 && snake_game.direction != DIR_RIGHT) { // 左键
            snake_game.direction = DIR_LEFT;
        }
        if (button2 && snake_game.direction != DIR_LEFT) {  // 右键
            snake_game.direction = DIR_RIGHT;
        }
        
        // 暂停/继续
        if (button5) {
            if (snake_game.status == GAME_RUNNING) {
                snake_game.status = GAME_PAUSE;
                ips200_show_string(80, 120, "PAUSED");
                ips200_show_string(40, 140, "Press 5 to continue");
            } else if (snake_game.status == GAME_PAUSE) {
                snake_game.status = GAME_RUNNING;
                // 擦除暂停信息
                ips200_clear(COLOR_BG);
                draw_game_area();
            }
            button5 = 0;  // 清除按键状�?
        }
        
        // �?��在游戏运行状态下才更�?
        if (snake_game.status == GAME_RUNNING) {
            // 增加游戏时间计数�?
            game_time++;
            
            // 控制游戏速度
            if (game_time >= game_speed / 10) {
                game_time = 0;
                
                // 更新游戏状�?
                erase_tail();  // 擦除蛇尾
                move_snake();  // 移动�?
                
                // 检查�?�?
                if (check_collision()) {
                    snake_game.status = GAME_OVER;
                    show_game_over();
                    
                    // 等待按键选择
                    while (1) {
                        button_entry();
                        if (button5) {  // 重新开�?
                            ips200_clear(COLOR_BG);
                            snake_game_init();
                            draw_game_area();
                            draw_food();
                            draw_snake();
                            button5 = 0;
                            break;
                        }
                        system_delay_ms(10);
                    }
                } else {
                    // 绘制蛇和食物
                    draw_snake();
                    draw_food();
                }
            }
        }
        
        // 显示游戏信息
        show_game_info();
        
        // �?��延时
        system_delay_ms(10);
    }
} 