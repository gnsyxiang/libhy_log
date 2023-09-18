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
#include <string.h>

#include "log.h"
#include "hy_printf.h"

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
        return -1; // 最大分配的空间已经有数据，无需插入了
    }

    if (handle->len + increment < handle->max_len) {
        extend_len = HY_MEM_ALIGN4_UP(handle->len + increment + 1);
    }

    if (extend_len > handle->max_len) {
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

static inline uint32_t _write_data(dynamic_array_s *handle,
                                   const void *buf, uint32_t len)
{
    memcpy(handle->buf + handle->write_pos, buf, len);
    handle->write_pos += len;
    handle->cur_len   += len;

    return len;
}

// 3 for "...", 2 for "\n\0", 1数组从0开始
#define _trunc_len(_handle) (_handle->len - 3 - 2 - 1)

int32_t dynamic_array_write(dynamic_array_s *handle, const void *buf, uint32_t len)
{
    assert(handle);
    assert(buf);

    char *ptr = NULL;
    int32_t ret = 0;
    int32_t free_len;

    do {
        ptr = handle->buf + handle->write_pos;
        free_len = handle->len - handle->cur_len;

        // 直接写入数据
        ret = snprintf(ptr, free_len - 1, "%s", (char *)buf); // -1数组从0开始
        if (ret < free_len - 1) {
            handle->cur_len   += ret;
            handle->write_pos += ret;
            break;
        } else {
            ret = _dynamic_array_extend(handle, ret - free_len);
            if (-1 == ret) {
                // 确保颜色复位插入
                if (len == strlen(PRINT_ATTR_RESET)) {
                    if (free_len > (int32_t)strlen(PRINT_ATTR_RESET)) {
                        _write_data(handle, buf, len);
                        ret = len;
                    } else {
                        handle->write_pos = _trunc_len(handle) - len;
                        handle->cur_len   = _trunc_len(handle) - len;
                        ret = 0;

                        log_e("truncated data \n");
                        ret += _write_data(handle, "...", 3);
                        ret += _write_data(handle, buf, len);
                        ret += _write_data(handle, "\n\0", 2);
                    }
                }
                break;
            } else if (1 == ret) {
                free_len = _trunc_len(handle) - handle->cur_len;
                ret = 0;

                ret = snprintf(ptr, free_len, "%s", (char *)buf);
                ret = (ret < free_len) ? ret : free_len;
                handle->write_pos += ret;
                handle->cur_len   += ret;

                log_e("truncated data \n");
                ret += _write_data(handle, "...", 3);
                ret += _write_data(handle, "\n\0", 2);
                break;
            }
        }
    } while(1);

    return len;
}

int32_t dynamic_array_write_vprintf(dynamic_array_s *handle,
                                    const char *format, va_list *args)
{
    assert(handle);
    assert(format);
    assert(args);

    int32_t free_len;
    int32_t ret = 0;
    char *ptr = NULL;

    do {
        ptr = handle->buf + handle->write_pos;
        free_len = handle->len - handle->cur_len;

        // FIXME: vsnprintf中args参数只能使用一次，使用之后就会清空
        // 所以这里的动态扩展一定会有问题

        // 直接写入数据
        ret = vsnprintf(ptr, free_len - 1, format, *args); // -1数组从0开始
        if (ret < 0) {
            log_e("vsnprintf failed \n");
            ret = -1;
            break;
        } else {
            // 写入数据成功
            if (ret < free_len - 1) {
                handle->cur_len   += ret;
                handle->write_pos += ret;
                break;
            } else {
                // 数据不够，扩大空间
                ret = _dynamic_array_extend(handle, ret - free_len);
                if (-1 == ret) {
                    break;
                } else if (1 == ret) {
                    free_len = _trunc_len(handle) - handle->cur_len;
                    ret = 0;

                    ret = vsnprintf(ptr, free_len, format, *args);
                    ret = (ret < free_len) ? ret : free_len;
                    handle->write_pos += ret;
                    handle->cur_len   += ret;

                    log_e("truncated data \n");
                    ret += _write_data(handle, "...", 3);
                    ret += _write_data(handle, "\n\0", 2);

                    break;
                }
            }
        }
    } while(1);

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
