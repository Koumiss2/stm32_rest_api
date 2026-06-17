#pragma once

#include <cstdint>
#include "../api/http_types.hpp"
#include "lwip/sockets.h"

#ifndef HTTP_RECV_BUF_SZ
#define HTTP_RECV_BUF_SZ 384
#endif

class HttpServer {
public:
    HttpServer(uint16_t port = 80);
    ~HttpServer();

    void run();

private:
    uint16_t _port;
    int _listen_fd;

    bool init();
    
    void handle_client(int client_fd);   
    void send_response(int client_fd, const Response& res);
    Request parse_request(char* buffer, size_t size);
};

#ifdef __cplusplus
extern "C" {
#endif
// Функция-обертка для запуска в задаче FreeRTOS
void http_server_task(void *argument);
#ifdef __cplusplus
}
#endif
