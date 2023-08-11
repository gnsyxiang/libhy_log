/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    process_handle_data.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2022 08:38
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2022      create the file
 * 
 *     last modified: 28/04 2022 08:38
 */
#include <stdio.h>
#include <unistd.h>

#include "log_private.h"
#include "config.h"

#include "process_handle_data.h"

struct process_handle_data_s {
    int32_t                     is_exit;
    pthread_t                   id;

    log_fifo_s                  *fifo;
    pthread_mutex_t             mutex;
    pthread_cond_t              cond;

    process_handle_data_cb_t    cb;
    void                        *args;
};

int32_t process_handle_data_write(process_handle_data_s *handle,
                                  const void *buf, uint32_t len)
{
    assert(handle);
    assert(buf);

    int32_t ret = 0;

    pthread_mutex_lock(&handle->mutex);
    ret = log_fifo_write(handle->fifo, buf, len);
    pthread_mutex_unlock(&handle->mutex);

    pthread_cond_signal(&handle->cond);

    return ret;
}

static void *_process_handle_data_thread_cb(void *args)
{
#define _ITEM_LEN_MAX   (4 * 1024)
    process_handle_data_s *handle = args;
    int32_t ret = 0;

#ifdef HAVE_PTHREAD_SETNAME_NP
    pthread_setname_np(handle->id, "hy_log_loop");
#endif

    char *buf = calloc(1, _ITEM_LEN_MAX);
    if (!buf) {
        log_e("calloc failed \n");
        return NULL;
    }

    while (!handle->is_exit) {
        pthread_mutex_lock(&handle->mutex);
        if (LOG_FIFO_IS_EMPTY(handle->fifo)) {
            pthread_cond_wait(&handle->cond, &handle->mutex);
        }

        memset(buf, '\0', _ITEM_LEN_MAX);
        ret = log_fifo_read(handle->fifo, buf, _ITEM_LEN_MAX);

        pthread_mutex_unlock(&handle->mutex);

        if (ret > 0 && handle->cb) {
            handle->cb(buf, ret, handle->args);
        }
    }

    if (buf) {
        free(buf);
    }

    return NULL;
}

void process_handle_data_destroy(process_handle_data_s **handle_pp)
{
    if (!handle_pp || !*handle_pp) {
        log_e("the param is NULL \n");
        return;
    }
    process_handle_data_s *handle = *handle_pp;

    while (!LOG_FIFO_IS_EMPTY(handle->fifo)) {
        usleep(10 * 1000);
    }
    handle->is_exit = 1;
    pthread_cond_signal(&handle->cond);
    usleep(10 * 1000);
    pthread_join(handle->id, NULL);

    pthread_mutex_destroy(&handle->mutex);
    pthread_cond_destroy(&handle->cond);

    log_fifo_destroy(&handle->fifo);

    log_i("process handle data handle: %p destroy \n", handle);
    free(handle);
    *handle_pp = NULL;
}

process_handle_data_s *process_handle_data_create(const char *name,
                                                  uint32_t fifo_len,
                                                  process_handle_data_cb_t cb,
                                                  void *args)
{
    if (!name || fifo_len <= 0 || !cb) {
        log_e("the param is NULL \n");
        return NULL;
    }

    process_handle_data_s *handle = NULL;
    do {
        handle = calloc(1, sizeof(*handle));
        if (!handle) {
            log_e("calloc failed \n");
            break;
        }
        handle->cb     = cb;
        handle->args   = args;

        if (0 != pthread_mutex_init(&handle->mutex, NULL)) {
            log_e("pthread_mutex_init failed \n");
            break;
        }

        if (0 != pthread_cond_init(&handle->cond, NULL)) {
            log_e("pthread_cond_init failed \n");
            break;
        }

        handle->fifo = log_fifo_create(fifo_len);
        if (!handle->fifo) {
            log_i("fifo_create failed \n");
            break;
        }

        if (0 != pthread_create(&handle->id, NULL,
                                _process_handle_data_thread_cb, handle)) {
            log_e("pthread_create failed \n");
            break;
        }

#ifdef HAVE_PTHREAD_SETNAME_NP
        pthread_setname_np(handle->id, name);
#endif

        log_i("process handle data handle: %p create \n", handle);
        return handle;
    } while (0);

    log_e("process handle data handle: %p create failed \n", handle);
    process_handle_data_destroy(&handle);
    return handle;
}
