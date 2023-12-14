/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    log_fifo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/04 2022 15:43
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/04 2022      create the file
 * 
 *     last modified: 21/04 2022 15:43
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "log.h"

#include "log_fifo.h"

#if defined _WIN32 || defined __CYGWIN__
#else
#ifdef __GNUC__
#if __GNUC__ >= 4 || defined(__arm__)
/* Optimization barrier */
/* The "volatile" is due to gcc bugs */
#ifndef HY_BARRIER
#define HY_BARRIER()   __asm__ __volatile__("": : :"memory")    // volatile: 告诉编译barrier()周围的指令不要被优化；
// memory: 告诉编译器汇编代码会使内存里面的值更改，编译器应使用内存里的新值而非寄存器里保存的老值。
#endif

#ifndef HY_SMP_MB
#define HY_SMP_MB()    HY_BARRIER()
#endif

#ifndef HY_SMP_WMB
#define HY_SMP_WMB()   HY_BARRIER()
#endif

#ifndef HY_SMP_RMB
#define HY_SMP_RMB()   HY_BARRIER()
#endif
#else
#error "gcc version too low"
#endif
#endif
#endif

#define _IS_POWER_OF_2(x)       ((x) != 0 && (((x) & ((x) - 1)) == 0))  ///< 判断x是否为2^n，是返回1，否返回0
#define HY_LOG_MIN(x, y)        ((x) < (y) ? (x) : (y))                 ///< 求最小值
#define HY_LOG_MAX(x, y)        ((x) > (y) ? (x) : (y))                 ///< 求最大值

/**
 * @brief 内存屏障
 *
 * 解决两个问题:
 * 1, 解决内存可见性问题
 * 2, 解决cpu重排的问题，cpu优化
 */
#define USE_MB

static void _hex(const void *_buf, hy_u32_t len, hy_s32_t flag)
{
    hy_s32_t cnt = 0;
    const unsigned char *buf = (const unsigned char *)_buf;

    if (len <= 0) {
        return;
    }

    for (hy_u32_t i = 0; i < len; i++) {
        if (flag == 1) {
            if (buf[i] == 0x0d || buf[i] == 0x0a || buf[i] < 32 || buf[i] >= 127) {
                printf("%02x[ ]  ", buf[i]);
            } else {
                printf("%02x[%c]  ", buf[i], buf[i]);
            }
        } else {
            printf("%02x ", buf[i]);
        }

        cnt++;
        if (cnt == 16) {
            cnt = 0;
            printf("\r\n");
        }
    }
    printf("\r\n");
}

static void _dump_content(log_fifo_s *handle)
{
    assert(handle);

    hy_u32_t len_tmp;

    len_tmp = handle->len - LOG_FIFO_READ_POS(handle);
    len_tmp = HY_LOG_MIN(len_tmp, LOG_FIFO_USED_LEN(handle));

    log_i("used len: %u, write_pos: %u, read_pos: %u \n",
            LOG_FIFO_USED_LEN(handle), handle->write_pos, handle->read_pos);

    _hex(handle->buf + LOG_FIFO_READ_POS(handle), len_tmp, 1);
    _hex(handle->buf, LOG_FIFO_USED_LEN(handle) - len_tmp, 1);
}

void fifo_dump(log_fifo_s *handle, log_fifo_dump_type_e type)
{
    assert(handle);

    switch (type) {
        case LOG_FIFO_DUMP_TYPE_ALL:
            printf("[%s:%d] len: %d \n", __func__, __LINE__, handle->len);
            _hex(handle->buf, handle->len, 1);
            break;
        case LOG_FIFO_DUMP_TYPE_CONTENT:
            _dump_content(handle);
            break;
        default:
            log_e("error type: %d \n", type);
    }
}

static hy_s32_t _fifo_read_com(log_fifo_s *handle, void *buf, hy_u32_t len)
{
    hy_u32_t len_tmp = 0;

    if (LOG_FIFO_IS_EMPTY(handle)) {
        return 0;
    }

    len = HY_LOG_MIN(len, LOG_FIFO_USED_LEN(handle));

#ifdef USE_MB
    // 确保其他线程对read_pos的可见性
    HY_SMP_WMB();
#endif

    len_tmp = HY_LOG_MIN(len, handle->len - LOG_FIFO_READ_POS(handle));

    memcpy(buf, handle->buf + LOG_FIFO_READ_POS(handle), len_tmp);
    memcpy(buf + len_tmp, handle->buf, len - len_tmp);

#ifdef USE_MB
    // 确保read_pos不会优化到上面去
    HY_SMP_MB();
#endif

    return len;
}

hy_s32_t log_fifo_read(log_fifo_s *handle, void *buf, hy_u32_t len)
{
    assert(handle);
    assert(buf);

    len = _fifo_read_com(handle, buf, len);
    handle->read_pos += len;

    return len;
}

hy_s32_t log_fifo_read_peek(log_fifo_s *handle, void *buf, hy_u32_t len)
{
    assert(handle);
    assert(buf);

    return _fifo_read_com(handle, buf, len);
}

hy_s32_t log_fifo_write(log_fifo_s *handle, const void *buf, hy_u32_t len)
{
    assert(handle);
    assert(buf);

    hy_u32_t len_tmp = 0;

    if (len > LOG_FIFO_FREE_LEN(handle)) {
        log_e("write failed, len: %u, free_len: %u \n",
              len, LOG_FIFO_FREE_LEN(handle));
        return -1;
    }

#ifdef USE_MB
    // 确保其他线程对write_pos的可见性
    HY_SMP_MB();
#endif

    len_tmp = HY_LOG_MIN(len, handle->len - LOG_FIFO_WRITE_POS(handle));

    memcpy(handle->buf + LOG_FIFO_WRITE_POS(handle), buf, len_tmp);
    memcpy(handle->buf, buf + len_tmp, len - len_tmp);

#ifdef USE_MB
    // 确保write_pos不会优化到上面去
    HY_SMP_WMB();
#endif

    handle->write_pos += len;

    return len;
}

static hy_u32_t _num_to_2n(hy_u32_t num)
{
    hy_u32_t i = 1;
    hy_u32_t num_tmp = num;

    while (num >>= 1) {
        i <<= 1;
    }

    return (i < num_tmp) ? i << 1U : i;
}

void log_fifo_destroy(log_fifo_s **handle_pp)
{
    if (!handle_pp || !*handle_pp) {
        log_e("the param is error \n");
        return ;
    }
    log_fifo_s *handle = *handle_pp;
    log_i("fifo create handle: %p destroy, buf: %p \n",
            handle, handle->buf);

    free(handle->buf);

    free(handle);
    *handle_pp = NULL;
}

log_fifo_s *log_fifo_create(hy_u32_t len)
{
    if (len == 0) {
        log_e("the param is error \n");
        return NULL;
    }

    log_fifo_s *handle = NULL;
    do {
        if (!_IS_POWER_OF_2(len)) {
            log_i("old len: %d \n", len);
            len = _num_to_2n(len);
            log_i("len must be power of 2, new len: %d \n", len);
        }

        handle = calloc(1, sizeof(*handle));
        if (!handle) {
            log_e("calloc failed \n");
            break;
        }

        handle->buf = calloc(1, len);
        if (!handle->buf) {
            log_e("calloc failed \n");
            break;
        }

        handle->len        = len;
        handle->read_pos   = handle->write_pos = 0;

        log_i("fifo create handle: %p create, buf: %p \n",
                handle, handle->buf);
        return handle;
    } while (0);

    log_e("fifo create handle: %p failed \n", handle);
    return NULL;
}
