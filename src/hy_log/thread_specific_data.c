/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    thread_specific_data.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2022 10:47
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2022      create the file
 * 
 *     last modified: 28/04 2022 10:47
 */
#include <stdio.h>

#include "log_private.h"

#include "thread_specific_data.h"

static thread_specific_data_s _TSD_ctx;

static int32_t _thread_specific_data_set(void *handle)
{
    if (!handle) {
        log_e("the param is error \n");
        return -1;
    }

    if (0 != pthread_setspecific(_TSD_ctx.thread_key, handle)) {
        log_e("pthread_setspecific fail \n");
        return -1;
    } else {
        return 0;
    }
}

static void *_thread_specific_data_get(void)
{
    void *handle = NULL;

    handle = pthread_getspecific(_TSD_ctx.thread_key);
    if (!handle) {
        log_i("pthread_getspecific failed \n");
        return NULL;
    } else {
        return handle;
    }
}

static void _thread_specific_data_destroy(void *args)
{
    if (_TSD_ctx.destroy_cb) {
        _TSD_ctx.destroy_cb(args);
    }
}

static void _main_thread_specific_data_destroy(void)
{
    _thread_specific_data_destroy(_thread_specific_data_get());
}

void *thread_specific_data_fetch(void)
{
    void *handle = _thread_specific_data_get();
    if (!handle) {
        if (_TSD_ctx.destroy_cb) {
            handle = _TSD_ctx.create_cb();
            if (!handle) {
                log_e("context.create_cb failed \n");
                return NULL;
            }
        }

        _thread_specific_data_set(handle);
    } else {
        if (_TSD_ctx.reset_cb) {
            _TSD_ctx.reset_cb(handle);
        }
    }

    return handle;
}

void thread_specific_data_destroy(void)
{
    if (0 != atexit(_main_thread_specific_data_destroy)) {
        log_e("atexit fail \n");
    }
}

int32_t thread_specific_data_create(thread_specific_data_create_cb_t create_cb,
                                    thread_specific_data_destroy_cb_t destroy_cb,
                                    thread_specific_data_reset_cb_t reset_cb)
{
    assert(create_cb);
    assert(destroy_cb);

    do {
        memset(&_TSD_ctx, '\0', sizeof(_TSD_ctx));

        _TSD_ctx.create_cb  = create_cb;
        _TSD_ctx.destroy_cb = destroy_cb;
        _TSD_ctx.reset_cb   = reset_cb;

        if (0 != pthread_key_create(&_TSD_ctx.thread_key,
                                    _thread_specific_data_destroy)) {
            log_e("pthread_key_create failed \n");
            break;
        }

        log_i("thread_specific_data_create create, context: %p \n", context);
        return 0;
    } while (0);

    return -1;
}
