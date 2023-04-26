/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_log4cplus.cpp
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 14:39
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 14:39
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <log4cplus/config.hxx>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/socket.h>
#include <log4cplus/loggingmacros.h>

#include "hy_log.h"

using namespace log4cplus;

#define _BUF_LEN_MAX    (1024)

typedef struct {
    HyLogSaveConfig_s   save_c;

    char                *buf;
    Logger              root;
} _log_context_t;

static _log_context_t *_context = nullptr;

HyLogLevel_e HyLogLevelGet(void)
{
    return _context->save_c.level;
}

void HyLogLevelSet(HyLogLevel_e level)
{
    _context->save_c.level = level;
}

// void HyLogWrite(LogLevel level, const char *file,
//         const char *func, uint32_t line, char *fmt, ...)
void HyLogWrite(HyLogAddiInfo_s *addi_info, const char *fmt, ...)
{
    struct {
        int32_t log4cplus_level;
    } level[] = {
        {FATAL_LOG_LEVEL},
        {ERROR_LOG_LEVEL},
        {WARN_LOG_LEVEL},
        {INFO_LOG_LEVEL},
        {DEBUG_LOG_LEVEL},
        {TRACE_LOG_LEVEL},
    };

    if (_context && _context->root.isEnabledFor(level[addi_info->level].log4cplus_level)) {
        memset(_context->buf, '\0', _BUF_LEN_MAX);

        uint32_t len = 0;

        // 加线程锁
        va_list args;
        va_start(args, fmt);
        vsnprintf(_context->buf + len, _BUF_LEN_MAX - len, fmt, args);
        va_end(args);

        _context->root.forcedLog(level[addi_info->level].log4cplus_level, _context->buf, addi_info->file, addi_info->line, addi_info->func);
    }
}

void HyLogHex(const char *name, uint32_t line,
        void *_buf, size_t len, int8_t flag)
{
    if (len <= 0) {
        return;
    }
    uint8_t *buf = (uint8_t *)_buf;

    uint8_t cnt = 0;
    printf("[%s %d]len: %zu \r\n", name, line, len);
    for (size_t i = 0; i < len; i++) {
        if (flag == 1) {
            if (buf[i] == 0x0d || buf[i] == 0x0a || buf[i] < 32 || buf[i] >= 127) {
                printf("%02x[ ]  ", (uint8_t)buf[i]);
            } else {
                printf("%02x[%c]  ", (uint8_t)buf[i], (uint8_t)buf[i]);
            }
        } else {
            printf("%02x ", (uint8_t)buf[i]);
        }
        cnt++;
        if (cnt == 16) {
            cnt = 0;
            printf("\r\n");
        }
    }
    printf("\r\n");
}

void HyLogDeInit(void)
{
    delete []_context->buf;

    delete _context;
}

int32_t HyLogInit(HyLogConfig_s *config)
{
    if (!config) {
        printf("the param is NULL \n");
        return -1;
    }

    do {
        _context = new _log_context_t();
        if (!_context) {
            printf("new _log_context_t faild \n");
            break;
        }

        memcpy(&_context->save_c, &config->save_c, sizeof(config->save_c));

        _context->buf = new char[_BUF_LEN_MAX];
        if (!_context->buf) {
            printf("new char faild \n");
            break;
        }

        helpers::LogLog::getLogLog()->setInternalDebugging(false);
        // @fixme
        // PropertyConfigurator::doConfigure(log_config->config_file);

        _context->root = Logger::getRoot();

        //初始化
        log4cplus::initialize();

        //基本配置
        log4cplus::BasicConfigurator config;
        config.configure();

        return 0;
    } while(0);

    HyLogDeInit();
    return -1;
}
