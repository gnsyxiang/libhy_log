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

#include "printf_my.h"

#include "hy_log.h"

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

    *ret += snprintf(context.buf + *ret, sizeof(context.buf) - *ret, "%s[%s]", color[level][1], color[level][0]);
}

static inline void _output_reset_color(HyLogLevel_e level, hy_s32_t *ret)
{
    *ret += snprintf(context.buf + *ret, sizeof(context.buf) - *ret, "%s", PRINT_ATTR_RESET);
}

void HyLogWrite(HyLogAddiInfo_s *addi_info, const char *fmt, ...)
{
    if (context.save_c.level < addi_info->level) {
        return;
    }

    hy_s32_t ret = 0;

    memset(context.buf, '\0', sizeof(context.buf));

    _output_set_color(addi_info->level, &ret);

    ret += snprintf(context.buf + ret, sizeof(context.buf) - ret,
                    "[%s:%d][%s] ", addi_info->file, addi_info->line, addi_info->func); 

    va_list args;
    va_start(args, fmt);
    ret += vsnprintf(context.buf + ret, sizeof(context.buf) - ret, fmt, args);
    va_end(args);

    ret += snprintf(context.buf + ret, sizeof(context.buf) - ret, "\r"); 

    _output_reset_color(addi_info->level, &ret);

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
