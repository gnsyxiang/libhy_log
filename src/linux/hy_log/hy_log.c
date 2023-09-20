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
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "format_cb.h"
#include "dynamic_array.h"
#include "thread_specific.h"
#include "log_fifo.h"
#include "uart.h"
#include "net.h"

#include "hy_log.h"

#define _DYNAMIC_ARRAY_MIN_LEN  (4 * 1024)
#define _DYNAMIC_ARRAY_MAX_LEN  (10 * 1024)

#define _LOGER_NUM              (4)

#define _LOG_THREAD_NAME        "hy_log_loop"

typedef enum {
    LOGER_UART,
    LOGER_NET,

    LOGER_MAX,
} _loger_e; 

typedef struct {
    HyLogSaveConfig_s   save_c;

    format_cb_t         *format_cb;
    hy_u32_t            format_cb_cnt;

    hy_s32_t            is_exit;
    pthread_t           id;

    log_fifo_s          *fifo;
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;

    void                *loger_h[LOGER_MAX];
} _log_context_s;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static hy_s32_t _is_init = 0;
static _log_context_s handle;

static void _thread_specific_reset_cb(void *handle)
{
    DYNAMIC_ARRAY_RESET((dynamic_array_s *)handle);
}

static void _thread_specific_destroy_cb(void **handle_pp)
{
    if (!handle_pp || !*handle_pp) {
        log_e("the param is error \n");
        return ;
    }

    dynamic_array_destroy((dynamic_array_s **)handle_pp);
}

static void *_thread_specific_create_cb(void)
{
    return dynamic_array_create(_DYNAMIC_ARRAY_MIN_LEN, _DYNAMIC_ARRAY_MAX_LEN);
}

static void *_log_loop_cb(void *args)
{
    _log_context_s *handle = args;
    hy_s32_t ret = 0;
    struct {
        hy_u8_t index;
        void *handle;
        hy_s32_t (*write)(void *handle, char *buf, hy_u32_t len);
    } loger_write_arr[] = {
        {LOGER_UART,    handle->loger_h[LOGER_UART],    uart_write},
        {LOGER_NET,     handle->loger_h[LOGER_NET],     net_write},
    };

    pthread_setname_np(handle->id, _LOG_THREAD_NAME);

    char *buf = calloc(1, _DYNAMIC_ARRAY_MIN_LEN);
    if (!buf) {
        log_e("calloc failed \n");
        return NULL;
    }

    while (!handle->is_exit) {
        memset(buf, '\0', _DYNAMIC_ARRAY_MIN_LEN);

        pthread_mutex_lock(&handle->mutex);

        while (LOG_FIFO_IS_EMPTY(handle->fifo)) {

            // pthread_cond_wait可能会出现假唤醒，唤醒之后需要再次判断条件
            pthread_cond_wait(&handle->cond, &handle->mutex);

            if (handle->is_exit) {
                log_i("the thread is exit \n");
                pthread_mutex_unlock(&handle->mutex);

                goto _ERR_LOG_EXIT; 
            }
        }

        ret = log_fifo_read(handle->fifo, buf, _DYNAMIC_ARRAY_MIN_LEN - 1);
        buf[ret] = '\0';

        pthread_mutex_unlock(&handle->mutex);

        for (hy_u32_t i = 0; i < LOG_ARRAY_CNT(loger_write_arr); i++) {
            if (loger_write_arr[i].write) {
                loger_write_arr[i].write(loger_write_arr[i].handle, buf, ret);
            }
        }
    }

_ERR_LOG_EXIT:
    if (buf) {
        free(buf);
    }

    return NULL;
}

void HyLogWrite(HyLogAddiInfo_s *addi_info, const char *fmt, ...)
{
    dynamic_array_s *dynamic_array;
    log_write_info_s log_write_info;
    va_list args;

    dynamic_array = thread_specific_fetch();
    assert(dynamic_array);

    va_start(args, fmt);
    addi_info->fmt                  = fmt;
    addi_info->str_args             = &args;
    log_write_info.format_cb        = handle.format_cb;
    log_write_info.format_cb_cnt    = handle.format_cb_cnt;
    log_write_info.dynamic_array    = dynamic_array;
    log_write_info.addi_info        = addi_info;

    for (hy_u32_t i = 0; i < log_write_info.format_cb_cnt; ++i) {
        if (log_write_info.format_cb[i]) {
            log_write_info.format_cb[i](dynamic_array, addi_info);
        }
    }

    pthread_mutex_lock(&handle.mutex);
    log_fifo_write(handle.fifo, dynamic_array->buf, dynamic_array->cur_len);
    pthread_mutex_unlock(&handle.mutex);

    pthread_cond_signal(&handle.cond);
    va_end(args);
}

HyLogLevel_e HyLogLevelGet(void)
{
    return handle.save_c.level;
}

void HyLogLevelSet(HyLogLevel_e level)
{
    handle.save_c.level = level;
}

void HyLogDeInit(void)
{
    // 等待log全部输出完成
    while (!LOG_FIFO_IS_EMPTY(handle.fifo)) {
        usleep(10 * 1000);
    }

    handle.is_exit = 1;

    pthread_cond_signal(&handle.cond);

    pthread_join(handle.id, NULL);

    pthread_mutex_destroy(&handle.mutex);
    pthread_cond_destroy(&handle.cond);

    thread_specific_destroy();

    struct {
        hy_u8_t index;
        void *handle;
        void (*destroy)(void **handle_pp);
    } loger_destroy_arr[] = {
        {LOGER_UART,    handle.loger_h[LOGER_UART], uart_destroy},
        {LOGER_NET,     handle.loger_h[LOGER_NET],  net_destroy},
    };
    for (hy_u32_t i = 0; i < LOG_ARRAY_CNT(loger_destroy_arr); i++) {
        if (loger_destroy_arr[i].destroy) {
            loger_destroy_arr[i].destroy(&loger_destroy_arr[i].handle);
        }
    }

    log_fifo_destroy(&handle.fifo);

    free(handle.format_cb);

    log_i("log context destroy, handle: %p \n", &handle);
}

hy_s32_t HyLogInit(HyLogConfig_s *log_c)
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
        memset(&handle, '\0', sizeof(handle));
        memcpy(&handle.save_c, &log_c->save_c, sizeof(handle.save_c));

        format_cb_register(&handle.format_cb, &handle.format_cb_cnt,
                           save_c->output_format);

        thread_specific_s thread_specific;
        memset(&thread_specific, 0, sizeof(thread_specific));
        thread_specific.create_cb   = _thread_specific_create_cb;
        thread_specific.destroy_cb  = _thread_specific_destroy_cb;
        thread_specific.reset_cb    = _thread_specific_reset_cb;
        if (0 != thread_specific_create(&thread_specific)) {
            log_e("thread_specific_create failed \n");
            break;
        }

        uart_config_s uart_c;
        memset(&uart_c, 0, sizeof(uart_config_s));
        handle.loger_h[LOGER_UART] = uart_create(&uart_c);
        if (!handle.loger_h[LOGER_UART]) {
            log_e("uart_create failed \n");
            break;
        }

        net_config_s net_c;
        memset(&net_c, 0, sizeof(net_config_s));
        net_c.ip = NULL;
        net_c.port = 50000;
        handle.loger_h[LOGER_NET] = net_create(&net_c);
        if (!handle.loger_h[LOGER_NET]) {
            log_e("net_create failed \n");
            break;
        }

        if (0 != pthread_mutex_init(&handle.mutex, NULL)) {
            log_e("pthread_mutex_init failed \n");
            break;
        }

        if (0 != pthread_cond_init(&handle.cond, NULL)) {
            log_e("pthread_cond_init failed \n");
            break;
        }

        handle.fifo = log_fifo_create(log_c->fifo_len);
        if (!handle.fifo) {
            log_i("fifo_create failed \n");
            break;
        }

        if (0 != pthread_create(&handle.id, NULL, _log_loop_cb, &handle)) {
            log_e("pthread_create failed \n");
            break;
        }

        log_i("log context create, handle: %p \n", &handle);
        return 0;
    } while (0);

    log_e("log context create failed \n");
    HyLogDeInit();
    return -1;
}
