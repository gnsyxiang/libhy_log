/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    log_fifo.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/04 2022 15:44
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/04 2022      create the file
 * 
 *     last modified: 21/04 2022 15:44
 */
#ifndef __LIBHY_LOG_INCLUDE_LOG_FIFO_H_
#define __LIBHY_LOG_INCLUDE_LOG_FIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief 打印FIFO相关信息
 */
typedef enum {
    LOG_FIFO_DUMP_TYPE_ALL,              ///< 打印FIFO全部信息，按照开辟的空间打印
    LOG_FIFO_DUMP_TYPE_CONTENT,          ///< 打印FIFO里面的有效内容

    LOG_FIFO_DUMP_TYPE_MAX = 0xffffffff,
} log_fifo_dump_type_e;

/**
 * @brief fifo上下文
 */
typedef struct {
    uint32_t    len;                    ///< FIFO长度
    char        *buf;                   ///< FIFO数据

    uint32_t    read_pos;               ///< 读位置
    uint32_t    write_pos;              ///< 写位置
} log_fifo_s;

/**
 * @brief 创建fifo模块
 *
 * @param len fifo长度
 *
 * @return 成功返回句柄，失败返回NULL
 */
log_fifo_s *log_fifo_create(uint32_t len);

/**
 * @brief 销毁fifo模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void log_fifo_destroy(log_fifo_s **handle_pp);

/**
 * @brief 向fifo中写入数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 长度
 *
 * @return 成功返回写入的长度，失败返回-1
 */
int32_t log_fifo_write(log_fifo_s *handle, const void *buf, uint32_t len);

/**
 * @brief 从fifo中读取数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 长度
 *
 * @return 成功返回读取的长度，失败返回-1
 */
int32_t log_fifo_read(log_fifo_s *handle, void *buf, uint32_t len);

/**
 * @brief 从fifo中读取数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 长度
 *
 * @return 成功返回读取的长度，失败返回-1
 *
 * @note 该操作不会删除数据
 */
int32_t log_fifo_read_peek(log_fifo_s *handle, void *buf, uint32_t len);

/**
 * @brief 打印fifo中的数据
 *
 * @param handle 句柄
 * @param type 打印的类型
 */
void log_fifo_dump(log_fifo_s *handle, log_fifo_dump_type_e type);

// 删除指定长度的数据
#define LOG_FIFO_READ_DEL(_handle)  (_handle)->read_pos += len

// 复位fifo
#define LOG_FIFO_RESET(_handle)                             \
    do {                                                    \
        HY_MEMSET((_handle)->buf, (_handle)->len);          \
        (_handle)->write_pos = (_handle)->read_pos = 0;     \
    } while (0)

// 空返回1，否则返回0
#define LOG_FIFO_IS_EMPTY(_handle)      ((_handle)->read_pos == (_handle)->write_pos)

// 满返回1，否则返回0
#define LOG_FIFO_IS_FULL(_handle)       ((_handle)->len == LOG_FIFO_USED_LEN(_handle))

// fifo中有效数据的长度
#define LOG_FIFO_USED_LEN(_handle)      ((_handle)->write_pos - (_handle)->read_pos)

// fifo中剩余空间
#define LOG_FIFO_FREE_LEN(_handle)      ((_handle)->len - (LOG_FIFO_USED_LEN(_handle)))

// 读指针位置
#define LOG_FIFO_READ_POS(_handle)      ((_handle)->read_pos & ((_handle)->len - 1))    // 优化 _handle->read_pos % _handle->len

// 写指针位置
#define LOG_FIFO_WRITE_POS(_handle)     ((_handle)->write_pos & ((_handle)->len - 1))   // 优化 _handle->write_pos % _handle->len

#ifdef __cplusplus
}
#endif

#endif

