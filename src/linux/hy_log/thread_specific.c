/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    thread_specific.c
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
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "thread_specific.h"

static thread_specific_s _TSD_ctx;

static inline hy_s32_t _thread_specific_set(void *handle)
{
    assert(handle);

    return (pthread_setspecific(_TSD_ctx.thread_key, handle) == 0) ? 0 : -1;
}

static inline void *_thread_specific_get(void)
{
    return pthread_getspecific(_TSD_ctx.thread_key);
}

static inline void _thread_specific_destroy(void *args)
{
    if (_TSD_ctx.destroy_cb) {
        _TSD_ctx.destroy_cb(&args);
    }
}

static inline void _main_thread_specific_destroy(void)
{
    _thread_specific_destroy(_thread_specific_get());
}

void *thread_specific_fetch(void)
{
    void *handle = _thread_specific_get();
    if (!handle) {
        if (_TSD_ctx.create_cb) {
            handle = _TSD_ctx.create_cb();
            if (!handle) {
                log_e("create_cb failed \n");
                return NULL;
            }
        }

        _thread_specific_set(handle);
    } else {
        if (_TSD_ctx.reset_cb) {
            _TSD_ctx.reset_cb(handle);
        }
    }

    return handle;
}

void thread_specific_destroy(void)
{
    if (0 != atexit(_main_thread_specific_destroy)) {
        log_e("atexit fail \n");
    }

    log_i("thread specific destroy\n");
}

hy_s32_t thread_specific_create(thread_specific_s *thread_specific_c)
{
    do {
        memset(&_TSD_ctx, '\0', sizeof(_TSD_ctx));
        memcpy(&_TSD_ctx, thread_specific_c, sizeof(thread_specific_s));

        if (0 != pthread_key_create(&_TSD_ctx.thread_key, _thread_specific_destroy)) {
            log_e("pthread_key_create failed \n");
            break;
        }

        log_i("thread specific create \n");
        return 0;
    } while (0);

    log_e("thread specific create failed \n");
    return -1;
}
