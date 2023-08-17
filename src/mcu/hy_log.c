/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_log.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/08 2023 13:53
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/08 2023      create the file
 * 
 *     last modified: 17/08 2023 13:53
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "hy_log.h"

/*设置输出前景色*/
#define PRINT_FONT_BLA      "\033[30m"      //黑色
#define PRINT_FONT_RED      "\033[31m"      //红色
#define PRINT_FONT_GRE      "\033[32m"      //绿色
#define PRINT_FONT_YEL      "\033[33m"      //黄色
#define PRINT_FONT_BLU      "\033[34m"      //蓝色
#define PRINT_FONT_PUR      "\033[35m"      //紫色
#define PRINT_FONT_CYA      "\033[36m"      //青色
#define PRINT_FONT_WHI      "\033[37m"      //白色

/*设置输出背景色*/ 
#define PRINT_BACK_BLA      "\033[40m"      //黑色
#define PRINT_BACK_RED      "\033[41m"      //红色
#define PRINT_BACK_GRE      "\033[42m"      //绿色
#define PRINT_BACK_YEL      "\033[43m"      //黄色
#define PRINT_BACK_BLU      "\033[44m"      //蓝色
#define PRINT_BACK_PUR      "\033[45m"      //紫色
#define PRINT_BACK_CYA      "\033[46m"      //青色
#define PRINT_BACK_WHI      "\033[47m"      //白色

/*输出属性设置*/
#define PRINT_ATTR_RESET    "\033[0m"       //重新设置属性到缺省设置 
#define PRINT_ATTR_BOL      "\033[1m"       //设置粗体 
#define PRINT_ATTR_LIG      "\033[2m"       //设置一半亮度(模拟彩色显示器的颜色) 
#define PRINT_ATTR_LIN      "\033[4m"       //设置下划线(模拟彩色显示器的颜色) 
#define PRINT_ATTR_GLI      "\033[5m"       //设置闪烁 
#define PRINT_ATTR_REV      "\033[7m"       //设置反向图象 
#define PRINT_ATTR_THI      "\033[22m"      //设置一般密度 
#define PRINT_ATTR_ULIN     "\033[24m"      //关闭下划线 
#define PRINT_ATTR_UGLI     "\033[25m"      //关闭闪烁 
#define PRINT_ATTR_UREV     "\033[27m"      //关闭反向图象

// printf("\033[字背景颜色;字体颜色m字符串\033[0m" );

typedef struct {
    HyLogSaveConfig_s   save_c;

    char                buf[1024 * 2];
} _log_context_s;

static _log_context_s context;

void HyLogLevelSet(HyLogLevel_e level)
{
}

HyLogLevel_e HyLogLevelGet(void)
{
    return context.save_c.level;
}

static inline void _output_set_color(HyLogLevel_e level, hy_s32_t *ret)
{
    char *color[][2] = {
        {"F", PRINT_FONT_RED},
        {"E", PRINT_FONT_RED},
        {"W", PRINT_FONT_YEL},
        {"I", ""},
        {"D", PRINT_FONT_BLU},
        {"T", PRINT_FONT_PUR},
    };

    *ret += snprintf(context.buf + *ret, sizeof(context.buf) - *ret,
                     "%s[%s]", color[level][1], color[level][0]);
}

static inline void _output_reset_color(HyLogLevel_e level, hy_s32_t *ret)
{
    *ret += snprintf(context.buf + *ret, sizeof(context.buf) - *ret,
                     "%s", PRINT_ATTR_RESET);
}

void HyLogWrite(HyLogLevel_e level, const char *file, const char *func,
                hy_u32_t line, char *fmt, ...)
{
    if (context.save_c.level < level) {
        return;
    }

    hy_s32_t ret = 0;

    memset(context.buf, '\0', sizeof(context.buf));

    _output_set_color(level, &ret);

    char *short_file = strrchr(file, '/');

    ret += snprintf(context.buf + ret, sizeof(context.buf) - ret,
                    "[%s:%d][%s] ", short_file + 1, line, func); 

    va_list args;
    va_start(args, fmt);
    ret += vsnprintf(context.buf + ret, sizeof(context.buf) - ret, fmt, args);
    va_end(args);

    ret += snprintf(context.buf + ret, sizeof(context.buf) - ret, "\r"); 

    _output_reset_color(level, &ret);

    printf("%s", (char *)context.buf);
}

void HyLogDeInit(void)
{

}

hy_s32_t HyLogInit(HyLogConfig_s *log_c)
{
    if (!log_c) {
        printf("the param is NULL \n");
        return -1;
    }

    memset(&context, 0, sizeof(context));
    memcpy(&context.save_c, &log_c->save_c, sizeof(context.save_c));

    return 0;
}
