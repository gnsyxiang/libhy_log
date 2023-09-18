/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    format_cb.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2022 10:21
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2022      create the file
 * 
 *     last modified: 28/04 2022 10:21
 */
#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <string.h>

#include "hy_printf.h"
#include "log.h"

#include "format_cb.h"

static char *format_color[][2] = {
    {"F", PRINT_FONT_RED},
    {"E", PRINT_FONT_RED},
    {"W", PRINT_FONT_YEL},
    {"I", ""},
    {"D", PRINT_FONT_GRE},
    {"T", ""},
};

static int32_t _format_color_cb(dynamic_array_s *dynamic_array,
                                HyLogAddiInfo_s *addi_info)
{
    char buf[8] = {0};
    int32_t ret = snprintf(buf, sizeof(buf), "%s", format_color[addi_info->level][1]);
    return dynamic_array_write(dynamic_array, buf, ret);
}

static int32_t _format_level_str_cb(dynamic_array_s *dynamic_array,
                                    HyLogAddiInfo_s *addi_info)
{
    char buf[4] = {0};
    int32_t ret = snprintf(buf, sizeof(buf), "[%s]", format_color[addi_info->level][0]);
    return dynamic_array_write(dynamic_array, buf, ret);
}

static int32_t _format_time_cb(dynamic_array_s *dynamic_array,
                               HyLogAddiInfo_s *addi_info)
{
    char buf[32] = {0};
    int32_t ret = log_time(buf, sizeof(buf));
    return dynamic_array_write(dynamic_array, buf, ret);
}

static int32_t _format_pid_id_cb(dynamic_array_s *dynamic_array,
                                 HyLogAddiInfo_s *addi_info)
{
    char buf[64] = {0};
    int32_t ret = 0;
    char name[16] = {0};

    pthread_getname_np(addi_info->tid, name, sizeof(name));
    ret = snprintf(buf, sizeof(buf), "[%ld-0x%lx(%s)]",
                   addi_info->pid, addi_info->tid, name);

    return dynamic_array_write(dynamic_array, buf, ret);
}

static int32_t _format_func_line_cb(dynamic_array_s *dynamic_array,
                                    HyLogAddiInfo_s *addi_info)
{
    char buf[128] = {0};
    int32_t ret = 0;

    if (addi_info->err_str) {
        ret += snprintf(buf + ret, sizeof(buf) - ret, "[%s:%"PRId32"]",
                        addi_info->func, addi_info->line);
        ret += snprintf(buf + ret, sizeof(buf) - ret, "[errno: %d, err: %s] ",
                        errno, addi_info->err_str);
    } else {
        ret += snprintf(buf + ret, sizeof(buf) - ret, "[%s:%"PRId32"] ",
                        addi_info->func, addi_info->line);
    }

    return dynamic_array_write(dynamic_array, buf, ret);
}

static int32_t _format_usr_msg_cb(dynamic_array_s *dynamic_array,
                                  HyLogAddiInfo_s *addi_info)
{
    return dynamic_array_write_vprintf(dynamic_array, addi_info->fmt,
                                       addi_info->str_args);
}

static int32_t _format_color_reset_cb(dynamic_array_s *dynamic_array,
                                      HyLogAddiInfo_s *addi_info)
{
    return dynamic_array_write(dynamic_array, PRINT_ATTR_RESET,
                               strlen(PRINT_ATTR_RESET));
}

void format_cb_register(format_cb_t **format_cb_pp,
                        uint32_t *format_cb_cnt, uint32_t format)
{
    if (!format_cb_pp || !format_cb_cnt) {
        log_e("the param is NULL \n");
        return;
    }

    uint32_t cnt;
    format_cb_t *format_cb;
    struct {
        HyLogOutputFormat_e     format;
        format_cb_t             format_cb;
    } _format_cb_array[] = {
        {HY_LOG_OUTPUT_FORMAT_COLOR,            _format_color_cb},
        {HY_LOG_OUTPUT_FORMAT_LEVEL_INFO,       _format_level_str_cb},
        {HY_LOG_OUTPUT_FORMAT_TIME,             _format_time_cb},
        {HY_LOG_OUTPUT_FORMAT_PID_ID,           _format_pid_id_cb},
        {HY_LOG_OUTPUT_FORMAT_FUNC_LINE,        _format_func_line_cb},
        {HY_LOG_OUTPUT_FORMAT_USR_MSG,          _format_usr_msg_cb},
        {HY_LOG_OUTPUT_FORMAT_COLOR_RESET,      _format_color_reset_cb},
    };

    cnt = LOG_ARRAY_CNT(_format_cb_array);

    format_cb = calloc(cnt, sizeof(format_cb_t));
    if (!format_cb) {
        log_e("calloc failed \n");
        return;
    }

    for (size_t i = 0; i < cnt; i++) {
        if (_format_cb_array[i].format == (format & (0x1U << i))) {
            format_cb[i] = _format_cb_array[i].format_cb;
        }
    }

    *format_cb_pp   = format_cb;
    *format_cb_cnt  = cnt;
}
