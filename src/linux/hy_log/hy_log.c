/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_log.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    29/10 2021 20:30
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        29/10 2021      create the file
 * 
 *     last modified: 29/10 2021 20:30
 */
#include <assert.h>
#include <stdio.h>

#include "format_cb.h"
#include "log.h"
#include "dynamic_array.h"
#include "process_single.h"
#include "thread_specific_data.h"

#include "hy_log.h"

#define _DYNAMIC_ARRAY_MIN_LEN  (4 * 1024)
#define _DYNAMIC_ARRAY_MAX_LEN  (10 * 1024)

typedef struct {
    HyLogSaveConfig_s   save_c;

    format_cb_t         *format_cb;
    uint32_t            format_cb_cnt;

    process_single_s    *write_h;
} _log_context_s;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int32_t _is_init = 0;
static _log_context_s _log_ctx;

HyLogLevel_e HyLogLevelGet(void)
{
    return _log_ctx.save_c.level;
}

void HyLogLevelSet(HyLogLevel_e level)
{
    _log_ctx.save_c.level = level;
}

void HyLogWrite(HyLogAddiInfo_s *addi_info, const char *fmt, ...)
{
    dynamic_array_s *dynamic_array;
    log_write_info_s log_write_info;
    va_list args;

    assert(_log_ctx.write_h);

    dynamic_array = thread_specific_data_fetch();
    assert(dynamic_array);

    va_start(args, fmt);
    addi_info->fmt                  = fmt;
    addi_info->str_args             = &args;
    log_write_info.format_cb        = _log_ctx.format_cb;
    log_write_info.format_cb_cnt    = _log_ctx.format_cb_cnt;
    log_write_info.dynamic_array    = dynamic_array;
    log_write_info.addi_info        = addi_info;
    process_single_write(_log_ctx.write_h, &log_write_info);
    va_end(args);
}

static void _thread_specific_data_reset_cb(void *handle)
{
    DYNAMIC_ARRAY_RESET((dynamic_array_s *)handle);
}

static void _thread_specific_data_destroy_cb(void *handle)
{
    if (!handle) {
        log_e("the param is error \n");
        return ;
    }

    dynamic_array_destroy((dynamic_array_s **)&handle);
}

static void *_thread_specific_data_create_cb(void)
{
    return dynamic_array_create(_DYNAMIC_ARRAY_MIN_LEN, _DYNAMIC_ARRAY_MAX_LEN);
}

void HyLogDeInit(void)
{
    process_single_destroy(&_log_ctx.write_h);

    thread_specific_data_destroy();

    free(_log_ctx.format_cb);

    log_i("log context: %p destroy \n", _log_ctx);
}

int32_t HyLogInit(HyLogConfig_s *log_c)
{
    if (!log_c) {
        log_e("the param is error \n");
        return -1;
    }

    pthread_mutex_lock(&lock);
    if (_is_init) {
        log_e("The logging system has been initialized \n");
        pthread_mutex_unlock(&lock);
        return -1;
    }
    _is_init = 1;
    pthread_mutex_unlock(&lock);

    HyLogSaveConfig_s *save_c = &log_c->save_c;

    do {
        memset(&_log_ctx, '\0', sizeof(_log_ctx));
        memcpy(&_log_ctx.save_c, &log_c->save_c, sizeof(_log_ctx.save_c));

        format_cb_register(&_log_ctx.format_cb,
                           &_log_ctx.format_cb_cnt, save_c->output_format);

        if (0 != thread_specific_data_create(_thread_specific_data_create_cb,
                                             _thread_specific_data_destroy_cb,
                                             _thread_specific_data_reset_cb)) {
            log_e("thread_specific_data_create failed \n");
            break;
        }

        _log_ctx.write_h = process_single_create(log_c->fifo_len);
        if (!_log_ctx.write_h) {
            log_e("create write handle failed \n");
            break;
        }

        log_i("log context: %p create \n", &_log_ctx);
        return 0;
    } while (0);

    log_e("log context: %p create failed \n", &_log_ctx);
    HyLogDeInit();
    return -1;
}
