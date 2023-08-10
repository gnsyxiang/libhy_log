/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    dynamic_array.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    22/04 2022 09:06
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        22/04 2022      create the file
 * 
 *     last modified: 22/04 2022 09:06
 */
#include <stdio.h>

#include "log_private.h"

#include "dynamic_array.h"

#define HY_MEM_ALIGN(len, align)    (((len) + (align) - 1) & ~((align) - 1))
#define HY_MEM_ALIGN4(len)          HY_MEM_ALIGN(len, 4)
#define HY_MEM_ALIGN4_UP(len)       (HY_MEM_ALIGN(len, 4) + HY_MEM_ALIGN4(1))

static int32_t _dynamic_array_extend(dynamic_array_s *handle, uint32_t increment)
{
    assert(handle);

    uint32_t extend_len = 0;
    int32_t ret = 0;
    void *ptr = NULL;

    if (handle->len >= handle->max_len) {
        log_e("can't extend, len: %d, max_len: %d \n", handle->len, handle->max_len);
        return -1;
    }

    if (handle->len + increment <= handle->max_len) {
        extend_len = HY_MEM_ALIGN4_UP(handle->len + increment + 1);
    } else {
        extend_len = handle->max_len;
        ret = 1; // 已经达到分配空间最大值，给被调用函数做截断处理提供条件
        log_e("extend to max len, len: %d, extend_len: %d \n", handle->len, extend_len);
    }

    ptr = realloc(handle->buf, extend_len);
    if (!ptr) {
        log_e("realloc failed \n");
        return -1;
    }

    handle->buf = ptr;
    handle->len = extend_len;

    return ret;
}

int32_t dynamic_array_read(dynamic_array_s *handle, void *buf, uint32_t len)
{
    assert(handle);
    assert(buf);

    if (len > handle->cur_len) {
        len = handle->cur_len;
    }

    memcpy(buf, handle->buf + handle->read_pos, len);
    handle->read_pos += len;
    handle->cur_len  -= len;

    return len;
}

int32_t dynamic_array_write_vprintf(dynamic_array_s *handle,
                                    const char *format, va_list *args)
{
    assert(handle);
    assert(format);
    assert(args);

    int32_t free_len;
    int32_t ret;
    char *ptr = NULL;

    ptr = handle->buf + handle->write_pos;

    free_len = handle->len - handle->cur_len - 1; // -1为'\0'保留

    ret = vsnprintf(ptr, free_len, format, *args);
    if (ret < 0) {
        log_e("vsnprintf failed \n");
        ret = -1;
    } else if (ret >= 0) {
        if (ret < free_len) {
            handle->cur_len   += ret;
            handle->write_pos += ret;
        } else {
            ret = _dynamic_array_extend(handle, ret - (handle->len - handle->cur_len));
            if (-1 == ret) {
                log_i("_dynamic_array_extend failed \n");
            } else if (ret >= 0) {
                // 扩大内存后，重新计算空闲空间
                free_len = handle->len - handle->cur_len - 1; // -1为'\0'保留

                ret = vsnprintf(ptr, free_len, format, *args);
                handle->cur_len += ret;
                handle->write_pos += ret;

                /* @fixme: <22-04-23, uos> 做进一步判断 */
            }
        }
    }

    return ret;
}

int32_t dynamic_array_write(dynamic_array_s *handle, const void *buf, uint32_t len)
{
    #define _write_data_com(_buf, _len)                 \
        do {                                            \
            char *ptr = NULL;                           \
            ptr = handle->buf + handle->write_pos;      \
            memcpy(ptr, _buf, _len);                    \
            handle->cur_len      += _len;               \
            handle->write_pos    += _len;               \
        } while (0)

    assert(handle);
    assert(buf);

    int32_t ret = 0;

    if (handle->len > handle->cur_len && handle->len - handle->cur_len > len) {
        _write_data_com(buf, len);
    } else {
        ret = _dynamic_array_extend(handle, len - (handle->len - handle->cur_len));
        if (-1 == ret) {
            log_i("_dynamic_array_extend failed \n");
            len = -1;
        } else if (0 == ret) {
            _write_data_com(buf, len);
        } else {
            // 3 for "..." 1 for "\0"
            len = handle->len - handle->cur_len - 3 - 1;
            _write_data_com(buf, len);

            log_e("truncated data \n");
            _write_data_com("...", 3);
            len += 3;
        }
    }

    return len;
}

void dynamic_array_destroy(dynamic_array_s **handle_pp)
{
    if (!handle_pp || !*handle_pp) {
        log_e("the param is error \n");
        return;
    }
    dynamic_array_s *handle = *handle_pp;

    log_i("dynamic array handle: %p destroy, buf: %p \n", handle, handle->buf);

    free(handle->buf);
    free(handle);
    *handle_pp = NULL;
}

dynamic_array_s *dynamic_array_create(uint32_t min_len, uint32_t max_len)
{
    if (min_len == 0 || max_len == 0 || min_len > max_len) {
        log_e("the param is error \n");
        return NULL;
    }

    dynamic_array_s *handle = NULL;
    do {
        handle = calloc(1, sizeof(*handle));
        if (!handle) {
            log_e("calloc failed \n");
            break;
        }

        handle->buf =calloc(1, min_len);
        if (!handle->buf) {
            log_e("calloc failed \n");
            break;
        }

        handle->max_len    = max_len;
        handle->min_len    = min_len;
        handle->len        = handle->min_len;
        handle->write_pos  = handle->read_pos = 0;

        log_i("dynamic array handle: %p create, buf: %p \n", handle, handle->buf);
        return handle;
    } while (0);

    log_e("dynamic array handle: %p create failed \n", handle);
    dynamic_array_destroy(&handle);
    return NULL;
}
