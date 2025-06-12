#include "zf_common_headfile.h"
#include "game.h"

/*********************************************************************************************************************
* 游戏对象类型
*********************************************************************************************************************/
#define GAME_OBJ_PLAYER        0
#define GAME_OBJ_ENEMY         1
#define GAME_OBJ_BULLET        2

/*********************************************************************************************************************
* 屏幕参数 - 横屏模式
*********************************************************************************************************************/
#define SCREEN_WIDTH           320     // 横屏宽度
#define SCREEN_HEIGHT          240     // 横屏高度
#define GAME_AREA_LEFT         20      // 游戏区域左侧位置

/*********************************************************************************************************************
* 游戏配置参数
*********************************************************************************************************************/
#define MAX_OBJECTS            15
#define PLAYER_SPEED           5       // 玩家速度
#define ENEMY_SPEED            3       // 敌人速度
#define BULLET_SPEED           7       // 子弹速度
#define ENEMY_SPAWN_RATE       15      // 敌人生成率(百分比)
#define SCORE_PER_ENEMY        10      // 每个敌人得分

/*********************************************************************************************************************
* 对象尺寸
*********************************************************************************************************************/
#define PLAYER_WIDTH           14
#define PLAYER_HEIGHT          10
#define ENEMY_WIDTH            10
#define ENEMY_HEIGHT           8
#define BULLET_WIDTH           5
#define BULLET_HEIGHT          2

/*********************************************************************************************************************
* 颜色定义
*********************************************************************************************************************/
#define COLOR_PLAYER           RGB565_RED
#define COLOR_ENEMY            RGB565_BLUE
#define COLOR_BULLET           RGB565_YELLOW
#define COLOR_BG               RGB565_WHITE
#define COLOR_TEXT             RGB565_BLACK

/*********************************************************************************************************************
* 玩家区域清除参数
*********************************************************************************************************************/
#define PLAYER_CLEAR_MARGIN    10       // Reasonable margin around player for clearing
#define BORDER_CLEAR_WIDTH     (ENEMY_WIDTH + ENEMY_SPEED + 2) // Width of the strip to clear at the left border

/*********************************************************************************************************************
* 游戏对象结构体
*********************************************************************************************************************/
typedef struct 
{
    int16 x;                  // 坐标X
    int16 y;                  // 坐标Y
    int16 old_x;              // 旧坐标X，用于局部擦除
    int16 old_y;              // 旧坐标Y，用于局部擦除
    uint8 type;               // 类型
    uint8 width;              // 宽度
    uint8 height;             // 高度
    uint8 active;             // 是否激活
    uint8 moved;              // 是否移动过
} game_object_t;

/*********************************************************************************************************************
* 游戏状态
*********************************************************************************************************************/
typedef struct 
{
    game_object_t objects[MAX_OBJECTS];   // 游戏对象数组
    uint8 obj_count;                      // 对象数量
    uint16 score;                         // 得分
    uint8 game_over;                      // 游戏结束标志
} game_state_t;

// 全局游戏状态
static game_state_t g_game;

// 上一次得分，用于判断是否需要刷新
static uint16 g_last_score = 0;

/*********************************************************************************************************************
* 函数声明
*********************************************************************************************************************/
static void game_init(void);
static void game_reset(void);
static void game_display_start_screen(void);
static void game_display_end_screen(void);
static void game_process_input(void);
static void game_update(void);
static void game_render(void);
static void game_add_player(void);
static void game_add_enemy(void);
static void game_add_bullet(int16 x, int16 y);
static void game_check_collisions(void);
static void game_cleanup_objects(void);
static void game_draw_object(game_object_t *obj, uint16 color);
static void game_update_score(void);
static void game_clear_area(int16 x, int16 y, int16 width, int16 height);
static void game_clear_object(game_object_t *obj);
static void game_clear_player_area(game_object_t *player);

/*********************************************************************************************************************
* 简单绘制一个矩形 - 每2像素画1点，减少50%绘制量
*********************************************************************************************************************/
static void draw_rect(int16 x, int16 y, int16 width, int16 height, uint16 color)
{
    int16 i;
    int16 j;
    int16 x_max;
    int16 y_max;

    // 防止越界
    if (x < 0) 
    {
        x = 0;
    }
    if (y < 0) 
    {
        y = 0;
    }
    
    x_max = x + width;
    y_max = y + height;
    
    if (x_max > SCREEN_WIDTH) 
    {
        x_max = SCREEN_WIDTH;
    }
    if (y_max > SCREEN_HEIGHT) 
    {
        y_max = SCREEN_HEIGHT;
    }
    
    for (i = x; i < x_max; i += 2) 
    {
        for (j = y; j < y_max; j += 2) 
        {
            ips200_draw_point(i, j, color);
        }
    }
}

/*********************************************************************************************************************
* 清除特定区域 - 优化的区域清除函数
*********************************************************************************************************************/
static void game_clear_area(int16 x, int16 y, int16 width, int16 height)
{
    int16 i;
    int16 j;
    int16 x_max = x + width;
    int16 y_max = y + height;
    
    // 防止越界
    if (x < GAME_AREA_LEFT) 
    {
        x = GAME_AREA_LEFT;
    }
    if (y < 0) 
    {
        y = 0;
    }
    if (x_max > SCREEN_WIDTH) 
    {
        x_max = SCREEN_WIDTH;
    }
    if (y_max > SCREEN_HEIGHT) 
    {
        y_max = SCREEN_HEIGHT;
    }
    
    // 更密集地清除每个像素点，确保完全清除
    for (i = x; i < x_max; i++) 
    {
        for (j = y; j < y_max; j++) 
        {
            ips200_draw_point(i, j, COLOR_BG);
        }
    }
}

/*********************************************************************************************************************
* 清除特定对象的旧位置
*********************************************************************************************************************/
static void game_clear_object(game_object_t *obj)
{
    // 确保对象在游戏区域内
    if (obj->old_x < GAME_AREA_LEFT) 
    {
        return;
    }
    if (obj->old_y < 0) 
    {
        return;
    }
    if (obj->old_x >= SCREEN_WIDTH) 
    {
        return;
    }
    if (obj->old_y >= SCREEN_HEIGHT) 
    {
        return;
    }
    
    // 清除对象旧位置的矩形区域
    game_clear_area(obj->old_x, obj->old_y, obj->width, obj->height);
}

/*********************************************************************************************************************
* 清除玩家及其周围区域 - 优化版清除策略
*********************************************************************************************************************/
static void game_clear_player_area(game_object_t *player)
{
    int16 clear_x;
    int16 clear_y;
    int16 clear_width;
    int16 clear_height;
    int16 i;
    int16 j;
    int16 x_max;
    int16 y_max;
    int16 min_x;
    int16 min_y;
    int16 max_x;
    int16 max_y;
    
    // 确保对象是玩家
    if (player->type != GAME_OBJ_PLAYER) 
    {
        return;
    }
    
    // 确定包含旧位置和当前位置的最小包围盒
    min_x = (player->old_x < player->x) ? player->old_x : player->x;
    min_y = (player->old_y < player->y) ? player->old_y : player->y;
    max_x = ((player->old_x + player->width) > (player->x + player->width)) ? (player->old_x + player->width) : (player->x + player->width);
    max_y = ((player->old_y + player->height) > (player->y + player->height)) ? (player->old_y + player->height) : (player->y + player->height);
    
    // 增加边距
    clear_x = min_x - PLAYER_CLEAR_MARGIN;
    clear_y = min_y - PLAYER_CLEAR_MARGIN;
    clear_width = (max_x - min_x) + (PLAYER_CLEAR_MARGIN * 2);
    clear_height = (max_y - min_y) + (PLAYER_CLEAR_MARGIN * 2);
    
    // 防止越界
    if (clear_x < GAME_AREA_LEFT)
    {
        clear_x = GAME_AREA_LEFT;
    }
    if (clear_y < 0)
    {
        clear_y = 0;
    }
    
    // 计算清除区域的最大坐标
    x_max = clear_x + clear_width;
    y_max = clear_y + clear_height;
    
    if (x_max > SCREEN_WIDTH)
    {
        x_max = SCREEN_WIDTH;
    }
    if (y_max > SCREEN_HEIGHT)
    {
        y_max = SCREEN_HEIGHT;
    }
    
    // 逐像素清除区域
    for (i = clear_x; i < x_max; i++) 
    {
        for (j = clear_y; j < y_max; j++) 
        {
            ips200_draw_point(i, j, COLOR_BG);
        }
    }
}

/*********************************************************************************************************************
* 游戏初始化
*********************************************************************************************************************/
static void game_init(void)
{
    uint8 i;
    
    // 初始化游戏状态
    for (i = 0; i < MAX_OBJECTS; i++) 
    {
        g_game.objects[i].active = 0;
    }
    
    g_game.obj_count = 0;
    g_game.score = 0;
    g_game.game_over = 0;
    g_last_score = 0;
    
    // 设置为横屏显示模式
    ips200_set_dir(IPS200_CROSSWISE);
}

/*********************************************************************************************************************
* 重置游戏
*********************************************************************************************************************/
static void game_reset(void)
{
    game_init();
    game_add_player();
}

/*********************************************************************************************************************
* 显示游戏开始界面
*********************************************************************************************************************/
static void game_display_start_screen(void)
{
    // 白色背景
    ips200_clear(COLOR_BG);
    
    // 显示游戏标题
    ips200_show_string(SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 - 20, "HORIZONTAL SHOOTER");
    ips200_show_string(SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 10, "PRESS ANY KEY TO START");
    
    // 显示操作说明
    ips200_show_string(10, SCREEN_HEIGHT - 40, "CONTROLS: UP/DOWN TO MOVE, OK TO SHOOT");
}

/*********************************************************************************************************************
* 显示游戏结束界面
*********************************************************************************************************************/
static void game_display_end_screen(void)
{
    // 白色背景
    ips200_clear(COLOR_BG);
    
    // 显示游戏结束和分数
    ips200_show_string(SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 30, "GAME OVER");
    ips200_show_string(SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2, "SCORE:");
    ips200_show_int16(SCREEN_WIDTH/2 + 30, SCREEN_HEIGHT/2, g_game.score);
    ips200_show_string(SCREEN_WIDTH/2 - 75, SCREEN_HEIGHT/2 + 30, "PRESS ANY KEY TO RETURN");
}

/*********************************************************************************************************************
* 添加玩家飞机 - 横版位置
*********************************************************************************************************************/
static void game_add_player(void)
{
    if (g_game.obj_count >= MAX_OBJECTS) 
    {
        return;
    }
    
    g_game.objects[g_game.obj_count].x = GAME_AREA_LEFT + 20;  // 从左侧开始
    g_game.objects[g_game.obj_count].y = SCREEN_HEIGHT / 2 - PLAYER_HEIGHT / 2;  // 居中
    g_game.objects[g_game.obj_count].old_x = g_game.objects[g_game.obj_count].x;
    g_game.objects[g_game.obj_count].old_y = g_game.objects[g_game.obj_count].y;
    
    g_game.objects[g_game.obj_count].type = GAME_OBJ_PLAYER;
    g_game.objects[g_game.obj_count].width = PLAYER_WIDTH;
    g_game.objects[g_game.obj_count].height = PLAYER_HEIGHT;
    g_game.objects[g_game.obj_count].active = 1;
    g_game.objects[g_game.obj_count].moved = 1; // Mark as moved initially to ensure first render clears area
    
    g_game.obj_count++;
}

/*********************************************************************************************************************
* 添加敌人飞机 - 横版位置
*********************************************************************************************************************/
static void game_add_enemy(void)
{
    uint8 rand_val;
    int16 y_pos;
    
    if (g_game.obj_count >= MAX_OBJECTS) 
    {
        return;
    }
    
    rand_val = rand() % 100;
    if (rand_val >= ENEMY_SPAWN_RATE) 
    {
        return;
    }
    
    // 随机纵向位置
    y_pos = rand() % (SCREEN_HEIGHT - ENEMY_HEIGHT - 10) + 5;
    
    // 敌机从右侧生成
    g_game.objects[g_game.obj_count].x = SCREEN_WIDTH - ENEMY_WIDTH - 5;
    g_game.objects[g_game.obj_count].y = y_pos;
    g_game.objects[g_game.obj_count].old_x = g_game.objects[g_game.obj_count].x;
    g_game.objects[g_game.obj_count].old_y = g_game.objects[g_game.obj_count].y;
    g_game.objects[g_game.obj_count].type = GAME_OBJ_ENEMY;
    g_game.objects[g_game.obj_count].width = ENEMY_WIDTH;
    g_game.objects[g_game.obj_count].height = ENEMY_HEIGHT;
    g_game.objects[g_game.obj_count].active = 1;
    g_game.objects[g_game.obj_count].moved = 1;
    
    g_game.obj_count++;
}

/*********************************************************************************************************************
* 添加子弹 - 横版发射方向
*********************************************************************************************************************/
static void game_add_bullet(int16 x, int16 y)
{
    int16 bullet_y;
    uint8 i;
    static uint8 bullet_count = 0;
    
    if (g_game.obj_count >= MAX_OBJECTS) 
    {
        return;
    }
    
    // 控制子弹数量，避免过多子弹导致性能问题
    bullet_count = 0;
    for (i = 0; i < g_game.obj_count; i++)
    {
        if (g_game.objects[i].type == GAME_OBJ_BULLET && g_game.objects[i].active)
        {
            bullet_count++;
        }
    }
    
    // 最多同时3颗子弹
    if (bullet_count >= 3)
    {
        return;
    }
    
    // 子弹从飞机右侧中央发射
    bullet_y = y + (PLAYER_HEIGHT / 2) - (BULLET_HEIGHT / 2);
    
    g_game.objects[g_game.obj_count].x = x + PLAYER_WIDTH;
    g_game.objects[g_game.obj_count].y = bullet_y;
    g_game.objects[g_game.obj_count].old_x = g_game.objects[g_game.obj_count].x;
    g_game.objects[g_game.obj_count].old_y = g_game.objects[g_game.obj_count].y;
    g_game.objects[g_game.obj_count].type = GAME_OBJ_BULLET;
    g_game.objects[g_game.obj_count].width = BULLET_WIDTH;
    g_game.objects[g_game.obj_count].height = BULLET_HEIGHT;
    g_game.objects[g_game.obj_count].active = 1;
    g_game.objects[g_game.obj_count].moved = 1;
    
    g_game.obj_count++;
}

/*********************************************************************************************************************
* 处理按键输入 - 上下摇杆移动和确定键射击
*********************************************************************************************************************/
static void game_process_input(void)
{
    uint8 i;
    uint8 key_up;
    uint8 key_down;
    uint8 key_fire;              // 确定键
    static uint8 fire_cooldown = 0;
    uint8 player_moved = 0;
    
    // 读取按键状态
    key_up = !gpio_get_level(KEY_4);     // 上摇键
    key_down = !gpio_get_level(KEY_3);   // 下摇键
    key_fire = !gpio_get_level(KEY_5);   // 确定键用于射击
    
    // 寻找玩家飞机
    for (i = 0; i < g_game.obj_count; i++) 
    {
        if (g_game.objects[i].type == GAME_OBJ_PLAYER && g_game.objects[i].active) 
        {
            // 保存旧位置
            g_game.objects[i].old_x = g_game.objects[i].x;
            g_game.objects[i].old_y = g_game.objects[i].y;
            
            // 上移
            if (key_up && g_game.objects[i].y > 5) 
            {
                g_game.objects[i].y -= PLAYER_SPEED;
                player_moved = 1;
            }
            
            // 下移
            if (key_down && g_game.objects[i].y < SCREEN_HEIGHT - PLAYER_HEIGHT - 5) 
            {
                g_game.objects[i].y += PLAYER_SPEED;
                player_moved = 1;
            }
            
            // 如果玩家飞机位置有变化，标记为需要重绘和清除
            if (player_moved) 
            {
                g_game.objects[i].moved = 1;
            }
            
            // 发射子弹 - 使用确定键
            if (key_fire && fire_cooldown == 0) 
            {
                game_add_bullet(g_game.objects[i].x, g_game.objects[i].y);
                fire_cooldown = 3;
            }
            
            break;
        }
    }
    
    // 更新发射冷却时间
    if (fire_cooldown > 0) 
    {
        fire_cooldown--;
    }
}

/*********************************************************************************************************************
* 更新游戏状态 - 横版逻辑
*********************************************************************************************************************/
static void game_update(void)
{
    uint8 i;
    
    // 更新所有对象
    for (i = 0; i < g_game.obj_count; i++) 
    {
        if (!g_game.objects[i].active) 
        {
            continue;
        }
        
        // 保存旧位置
        g_game.objects[i].old_x = g_game.objects[i].x;
        g_game.objects[i].old_y = g_game.objects[i].y;
        
        // 根据对象类型更新位置
        if (g_game.objects[i].type == GAME_OBJ_ENEMY) 
        {
            // 敌机向左移动
            g_game.objects[i].x -= ENEMY_SPEED;
            g_game.objects[i].moved = 1;
            
            // 超出屏幕左侧则清除并停用
            if (g_game.objects[i].x + g_game.objects[i].width < GAME_AREA_LEFT) // Check if fully off-screen
            {
                // Clear before deactivating
                game_clear_object(&g_game.objects[i]); 
                g_game.objects[i].active = 0;
            }
        }
        else if (g_game.objects[i].type == GAME_OBJ_BULLET) 
        {
            // 子弹向右移动
            g_game.objects[i].x += BULLET_SPEED;
            g_game.objects[i].moved = 1;
            
            // 超出屏幕右侧则清除并停用
            if (g_game.objects[i].x > SCREEN_WIDTH) 
            {
                // Clear before deactivating
                game_clear_object(&g_game.objects[i]);
                g_game.objects[i].active = 0;
            }
        }
    }
    
    // 检查碰撞
    game_check_collisions();
    
    // 清理无效对象
    game_cleanup_objects();
    
    // 尝试生成新敌机
    game_add_enemy();
}

/*********************************************************************************************************************
* 检查碰撞
*********************************************************************************************************************/
static void game_check_collisions(void)
{
    uint8 i;
    uint8 j;
    game_object_t *obj1;
    game_object_t *obj2;
    game_object_t *enemy_to_clear = NULL; // Pointer to store enemy to be cleared
    game_object_t *bullet_to_clear = NULL; // Pointer to store bullet to be cleared

    for (i = 0; i < g_game.obj_count; i++) 
    {
        obj1 = &g_game.objects[i];
        enemy_to_clear = NULL; // Reset for each obj1 iteration
        bullet_to_clear = NULL;
        
        if (!obj1->active) 
        {
            continue;
        }
        
        for (j = i + 1; j < g_game.obj_count; j++) 
        {
            obj2 = &g_game.objects[j];
            
            if (!obj2->active) 
            {
                continue;
            }
            
            // 矩形碰撞检测
            if (obj1->x < obj2->x + obj2->width && 
                obj1->x + obj1->width > obj2->x && 
                obj1->y < obj2->y + obj2->height && 
                obj1->y + obj1->height > obj2->y) 
            {
                
                // 子弹击中敌机
                if ((obj1->type == GAME_OBJ_BULLET && obj2->type == GAME_OBJ_ENEMY)) 
                {
                    bullet_to_clear = obj1; // Mark bullet for clearing
                    enemy_to_clear = obj2;  // Mark enemy for clearing
                    g_game.score += SCORE_PER_ENEMY;
                }
                else if ((obj1->type == GAME_OBJ_ENEMY && obj2->type == GAME_OBJ_BULLET))
                {
                    enemy_to_clear = obj1;  // Mark enemy for clearing
                    bullet_to_clear = obj2; // Mark bullet for clearing
                    g_game.score += SCORE_PER_ENEMY;
                }
                // 玩家碰到敌机
                else if ((obj1->type == GAME_OBJ_PLAYER && obj2->type == GAME_OBJ_ENEMY) ||
                         (obj1->type == GAME_OBJ_ENEMY && obj2->type == GAME_OBJ_PLAYER)) 
                {
                    g_game.game_over = 1;
                }
            }
            
            // If an enemy or bullet was marked for clearing, clear and deactivate them now
            if (enemy_to_clear != NULL)
            {
                game_clear_object(enemy_to_clear); // Clear the enemy wreckage
                enemy_to_clear->active = 0;        // Deactivate enemy
                enemy_to_clear = NULL;             // Reset the pointer
            }
             if (bullet_to_clear != NULL)
            {
                game_clear_object(bullet_to_clear); // Clear the bullet
                bullet_to_clear->active = 0;         // Deactivate bullet
                bullet_to_clear = NULL;              // Reset the pointer
            }
        } // End inner loop (j)

        // Clearing logic moved inside inner loop to handle multiple collisions correctly
    } // End outer loop (i)
}

/*********************************************************************************************************************
* 清理无效对象
*********************************************************************************************************************/
static void game_cleanup_objects(void)
{
    uint8 i;
    uint8 active_count;
    
    active_count = 0;
    
    // 压缩数组 - 将活动对象移到前面
    for (i = 0; i < g_game.obj_count; i++) 
    {
        if (g_game.objects[i].active) 
        {
            if (i != active_count) 
            {
                g_game.objects[active_count] = g_game.objects[i];
            }
            active_count++;
        }
    }
    
    g_game.obj_count = active_count;
}

/*********************************************************************************************************************
* 渲染游戏 - 对象级别的清除和绘制，优化性能
*********************************************************************************************************************/
static void game_render(void)
{
    uint8 i;
    static uint8 first_render = 1;
    
    // 更新分数
    game_update_score();
    
    // 首次渲染时清屏并绘制分隔线
    if (first_render) 
    {
        // 完全清屏
        ips200_clear(COLOR_BG);
        
        // 绘制分隔线
        for (i = 0; i < SCREEN_HEIGHT; i += 2) 
        {
            ips200_draw_point(GAME_AREA_LEFT - 1, i, COLOR_TEXT);
        }
        first_render = 0;
    }
    else
    {
        // 逐个清除对象，玩家使用特殊清除方法
        for (i = 0; i < g_game.obj_count; i++) 
        {
            if (g_game.objects[i].active) 
            {
                // 对玩家，如果移动过，则清除旧区域
                if (g_game.objects[i].type == GAME_OBJ_PLAYER && g_game.objects[i].moved)
                {
                    game_clear_player_area(&g_game.objects[i]);
                }
                // 对其他对象，如果移动过，则清除旧区域
                else if (g_game.objects[i].moved) 
                {
                    // Only clear if coordinates actually changed to avoid redundant clears
                     if (g_game.objects[i].x != g_game.objects[i].old_x || 
                         g_game.objects[i].y != g_game.objects[i].old_y)
                     {
                         game_clear_object(&g_game.objects[i]);
                     }
                }
            }
            // IMPORTANT: Do not clear inactive objects here, enemy wreckage/offscreen is cleared elsewhere
        }
        
        // 清除左侧边界足够宽的区域以处理离开屏幕的敌机
        game_clear_area(GAME_AREA_LEFT, 0, BORDER_CLEAR_WIDTH, SCREEN_HEIGHT);
    }
    
    // 绘制所有活动对象
    for (i = 0; i < g_game.obj_count; i++) 
    {
        if (g_game.objects[i].active) 
        {
            // 根据对象类型绘制
            if (g_game.objects[i].type == GAME_OBJ_PLAYER) 
            {
                game_draw_object(&g_game.objects[i], COLOR_PLAYER);
            }
            else if (g_game.objects[i].type == GAME_OBJ_ENEMY) 
            {
                game_draw_object(&g_game.objects[i], COLOR_ENEMY);
            }
            else if (g_game.objects[i].type == GAME_OBJ_BULLET) 
            {
                game_draw_object(&g_game.objects[i], COLOR_BULLET);
            }
            
            // 重置移动标志 AFTER drawing
            g_game.objects[i].moved = 0;
        }
    }
}

/*********************************************************************************************************************
* 绘制游戏对象 - 完整像素填充
*********************************************************************************************************************/
static void game_draw_object(game_object_t *obj, uint16 color)
{
    int16 i;
    int16 j;
    int16 x_max;
    int16 y_max;
    int16 x = obj->x;
    int16 y = obj->y;
    
    // 防止越界
    if (x < GAME_AREA_LEFT) 
    {
        x = GAME_AREA_LEFT;
    }
    if (y < 0) 
    {
        y = 0;
    }
    
    x_max = x + obj->width;
    y_max = y + obj->height;
    
    if (x_max > SCREEN_WIDTH) 
    {
        x_max = SCREEN_WIDTH;
    }
    if (y_max > SCREEN_HEIGHT) 
    {
        y_max = SCREEN_HEIGHT;
    }
    
    // 使用完整填充绘制对象
    for (i = x; i < x_max; i++) 
    {
        for (j = y; j < y_max; j++) 
        {
            ips200_draw_point(i, j, color);
        }
    }
}

/*********************************************************************************************************************
* 更新分数显示 - 左上角
*********************************************************************************************************************/
static void game_update_score(void)
{
    if (g_game.score != g_last_score) 
    {
        // 清除旧分数区域
        draw_rect(10, 5, 90, 15, COLOR_BG);
        
        // 显示新分数
        ips200_show_string(10, 5, "SCORE:");
        ips200_show_int16(60, 5, g_game.score);
        
        // 更新缓存的分数
        g_last_score = g_game.score;
    }
}

/*********************************************************************************************************************
* 游戏主函数
*********************************************************************************************************************/
void function_game(void)
{
    uint8 key_any;
    
    // 初始化游戏
    game_init();
    
    // 显示开始界面
    game_display_start_screen();
    
    // 等待任意按键开始
    while (1) 
    {
        key_any = !gpio_get_level(KEY_1) || !gpio_get_level(KEY_2) || 
                 !gpio_get_level(KEY_3) || !gpio_get_level(KEY_4) || 
                 !gpio_get_level(KEY_5);
        
        if (key_any) 
        {
            break;
        }
        
        system_delay_ms(50);
    }
    
    // 等待按键释放
    while (!gpio_get_level(KEY_1) || !gpio_get_level(KEY_2) || 
           !gpio_get_level(KEY_3) || !gpio_get_level(KEY_4) || 
           !gpio_get_level(KEY_5)) 
    {
        system_delay_ms(10);
    }
    
    // 重置游戏并初始化显示
    game_reset();
    ips200_clear(COLOR_BG);
    
    // 游戏主循环
    while (!g_game.game_over) 
    {
        // 处理输入
        game_process_input();
        
        // 更新游戏状态
        game_update();
        
        // 渲染游戏
        game_render();
        
        // 控制帧率
        system_delay_ms(10);
    }
    
    // 显示游戏结束界面
    game_display_end_screen();
    
    // 等待按键释放
    system_delay_ms(300);
    while (!gpio_get_level(KEY_1) || !gpio_get_level(KEY_2) || 
           !gpio_get_level(KEY_3) || !gpio_get_level(KEY_4) || 
           !gpio_get_level(KEY_5)) 
    {
        system_delay_ms(10);
    }
    
    // 等待任意按键返回
    while (1) 
    {
        key_any = !gpio_get_level(KEY_1) || !gpio_get_level(KEY_2) || 
                 !gpio_get_level(KEY_3) || !gpio_get_level(KEY_4) || 
                 !gpio_get_level(KEY_5);
        
        if (key_any) 
        {
            break;
        }
        
        system_delay_ms(50);
    }
}