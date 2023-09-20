/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    net.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    19/09 2023 11:58
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        19/09 2023      create the file
 * 
 *     last modified: 19/09 2023 11:58
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/select.h>

#include "log.h"

#include "net.h"

#define _CLIENT_FD_MAX      (4)
#define _EXIT_FLAG          'Q'

#define _LISTEN_THREAD_NAME "hy_log_listen"

typedef struct {
    net_save_config_s   save_c;

    hy_s32_t            is_exit;
    pthread_t           id;

    hy_s32_t            pfd[2];

    hy_s32_t            listen_fd;

    hy_u32_t            client_fd_cnt;
    hy_s32_t            client_fd[_CLIENT_FD_MAX];
} net_s;

static void *_listen_loop_cb(void *args)
{
    net_s *handle = args;
    struct sockaddr_un client_addr;
    hy_s32_t new_fd = -1;
    socklen_t addr_len = sizeof(struct sockaddr_un);
    fd_set read_fs;
    hy_u32_t cnt;
    hy_u8_t buf;

    pthread_setname_np(handle->id, _LISTEN_THREAD_NAME);

    while (!handle->is_exit) {
        FD_ZERO(&read_fs);
        FD_SET(handle->listen_fd, &read_fs);
        FD_SET(handle->pfd[0], &read_fs);

        if (select(FD_SETSIZE, &read_fs, NULL, NULL, NULL) < 0) {
            log_e("select failed \n");
            break;
        }

        if (FD_ISSET(handle->listen_fd, &read_fs)) {
            new_fd = accept(handle->listen_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (-1 == new_fd) {
                log_e("unix accept failed \n");
                break;
            }

            if (handle->client_fd_cnt == _CLIENT_FD_MAX) {
                close(new_fd);
                continue;
            }
            handle->client_fd_cnt++;

            cnt = 0;
            while (1) {
                if (handle->client_fd[cnt] == -1) {
                    handle->client_fd[cnt] = new_fd;
                    break;
                }
                cnt++;
            }

            log_i("new_fd: %d \n", new_fd);
        } else if (FD_ISSET(handle->pfd[0], &read_fs)) {
            log_e("pipe read %c \n", buf);

            read(handle->pfd[0], &buf, sizeof(buf));
            if (buf == _EXIT_FLAG) {
                break;
            }
        }
    }

    log_e("exit listen_loop_cb \n");
    return NULL;
}

static void _listen_fd_destroy(net_s *handle)
{
    if (handle->listen_fd) {
        close(handle->listen_fd);
    }
}

static hy_s32_t _listen_fd_create(net_s *handle, net_config_s *net_c)
{
    hy_s32_t ret;
    struct sockaddr_in addr;
    hy_s32_t flag = 1;

    do {
        handle->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (handle->listen_fd < 0) {
            log_e("socket failed, listen_fd: %d \n", handle->listen_fd);
            break;
        }

        ret = setsockopt(handle->listen_fd, SOL_SOCKET, SO_REUSEADDR, &flag,sizeof(flag));
        if (ret == -1) {
            log_e("setsockopt failed \n");
            break;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family         = AF_INET;
        addr.sin_port           = htons(net_c->port);
        addr.sin_addr.s_addr    = INADDR_ANY;
        ret = bind(handle->listen_fd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            log_e("bind failed \n");
            break;
        }

        ret = listen(handle->listen_fd, 16);
        if (ret != 0) {
            log_e("listen failed \n");
            break;
        }

        log_i("listen_fd: %d \n", handle->listen_fd);
        return 0;
    } while(0);

    return -1;
}

hy_s32_t net_write(void *context, char *buf, hy_u32_t len)
{
    net_s *handle = context;
    hy_s32_t ret;

    for (hy_u32_t i = 0; i < _CLIENT_FD_MAX; i++) {
        if (handle->client_fd[i] > 0) {
_FILE_WRITE_AGAIN:
            ret = write(handle->client_fd[i], buf, len);
            if (ret < 0 && (EINTR == errno || EAGAIN == errno || EWOULDBLOCK == errno)) {
                log_e("try again \n");
                goto _FILE_WRITE_AGAIN;
            } else if (ret == -1) {
                log_e("opposite fd close, fd: %d \n", handle->client_fd[i]);

                close(handle->client_fd[i]);
                handle->client_fd[i] = -1;
                handle->client_fd_cnt--;
            }
        }
    }

    return 0;
}

void net_destroy(void **handle_pp)
{
    net_s *handle = *handle_pp;

    if (!handle_pp || !*handle_pp) {
        log_e("the param is NULL \n");
        return;
    }

    char buf = _EXIT_FLAG;
    write(handle->pfd[1], &buf, sizeof(buf));

    close(handle->pfd[1]);
    close(handle->pfd[0]);

    _listen_fd_destroy(handle);

    log_i("net destroy, handle: %p \n", handle);
    free(handle);
    *handle_pp = NULL;
}

void *net_create(net_config_s *net_c)
{
    net_s *handle = NULL;

    do {
        handle = calloc(1, sizeof(net_s));
        if (!handle) {
            log_e("calloc failed \n");
            break;
        }

        for (hy_s32_t i = 0; i < _CLIENT_FD_MAX; i++) {
            handle->client_fd[i] = -1;
        }

        memcpy(&handle->save_c, &net_c->save_c, sizeof(net_save_config_s));

        if (0 != pipe(handle->pfd)) {
            log_e("pipe failed \n");
            break;
        }

        if (0 != _listen_fd_create(handle, net_c)) {
            log_e("_listen_fd_create failed \n");
            break;
        }

        if (0 != pthread_create(&handle->id, NULL, _listen_loop_cb, handle)) {
            log_e("pthread_create failed \n");
            break;
        }

        log_i("net create, handle: %p \n", handle);
        return handle;
    } while(0);

    log_e("net create failed \n");
    net_destroy((void **)handle);
    return NULL;
}
