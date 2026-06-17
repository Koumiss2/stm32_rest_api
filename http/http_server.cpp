#include "http_server.hpp"
#include "../api/rest_api.hpp"
#include "../api/router.hpp"
#include "cmsis_os2.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include <cstring>

extern "C" {
extern struct netif gnetif;
}

namespace {
bool wait_for_network_ready(uint32_t timeout_ms) {
    constexpr uint32_t step_ms = 250;
    uint32_t waited_ms = 0;

    while (waited_ms < timeout_ms) {
        if (netif_is_up(&gnetif) &&
            netif_is_link_up(&gnetif) &&
            !ip4_addr_isany_val(*netif_ip4_addr(&gnetif))) {
            return true;
        }

        osDelay(step_ms);
        waited_ms += step_ms;
    }

    return false;
}

void append_str(char* dst, size_t& pos, size_t cap, const char* src) {
    while (*src != '\0' && pos < cap) {
        dst[pos++] = *src++;
    }
}

void append_uint(char* dst, size_t& pos, size_t cap, uint32_t value) {
    char tmp[10];
    size_t len = 0;

    do {
        tmp[len++] = static_cast<char>('0' + (value % 10));
        value /= 10;
    } while (value > 0 && len < sizeof(tmp));

    while (len > 0 && pos < cap) {
        dst[pos++] = tmp[--len];
    }
}
}

HttpServer::HttpServer(uint16_t port) : _port(port), _listen_fd(-1) {}

HttpServer::~HttpServer() {
    if (_listen_fd >= 0) {
        lwip_close(_listen_fd);
    }
}

bool HttpServer::init() {
    struct sockaddr_in serv_addr;

    _listen_fd = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_fd < 0) {
        return false;
    }

    int enable = 1;
    lwip_setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(_port);

    if (lwip_bind(_listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        lwip_close(_listen_fd);
        _listen_fd = -1;
        return false;
    }

    if (lwip_listen(_listen_fd, 5) < 0) {
        lwip_close(_listen_fd);
        _listen_fd = -1;
        return false;
    }

    return true;
}

void HttpServer::run() {
    if (!init()) return;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = lwip_accept(_listen_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd >= 0) {
            handle_client(client_fd);
            lwip_close(client_fd);
        }
    }
}

void HttpServer::handle_client(int client_fd) {
    char buffer[HTTP_RECV_BUF_SZ];
    int n = lwip_recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (n > 0) {
        buffer[n] = '\0';
        Request req = parse_request(buffer, n);
        Response res;

        Router::instance().dispatch(req, res);

        send_response(client_fd, res);
    }
}

Request HttpServer::parse_request(char* buffer, size_t size) {
    Request req;
    req.method = HttpMethod::UNKNOWN;
    
    std::string_view full_data(buffer, size);
    
    size_t header_end = full_data.find("\r\n\r\n");
    if (header_end != std::string_view::npos) {
        req.body = full_data.substr(header_end + 4);
    }

    size_t method_end = full_data.find(' ');
    if (method_end == std::string_view::npos) {
        return req;
    }

    std::string_view method = full_data.substr(0, method_end);
    if (method == "GET") req.method = HttpMethod::GET;
    else if (method == "POST") req.method = HttpMethod::POST;
    else if (method == "PUT") req.method = HttpMethod::PUT;
    else if (method == "DELETE") req.method = HttpMethod::DELETE;

    size_t uri_start = method_end + 1;
    size_t uri_end = full_data.find(' ', uri_start);
    if (uri_end != std::string_view::npos) {
        req.uri = full_data.substr(uri_start, uri_end - uri_start);
    }

    return req;
}

void HttpServer::send_response(int client_fd, const Response& res) {
    char hdr[128];
    size_t pos = 0;
    int code = static_cast<int>(res.status);
    const char* status_text = (res.status == HttpStatus::OK) ? "OK" : "Not Found";

    append_str(hdr, pos, sizeof(hdr), "HTTP/1.1 ");
    append_uint(hdr, pos, sizeof(hdr), static_cast<uint32_t>(code));
    append_str(hdr, pos, sizeof(hdr), " ");
    append_str(hdr, pos, sizeof(hdr), status_text);
    append_str(hdr, pos, sizeof(hdr), "\r\nContent-Type: application/json\r\nContent-Length: ");
    append_uint(hdr, pos, sizeof(hdr), static_cast<uint32_t>(res.body.size()));
    append_str(hdr, pos, sizeof(hdr), "\r\nConnection: close\r\n\r\n");

    lwip_send(client_fd, hdr, pos, 0);
    lwip_send(client_fd, res.body.data(), res.body.size(), 0);
}

void http_server_task(void *argument) {
    (void)argument;

    if (!wait_for_network_ready(10000)) {
        osThreadExit();
    }

    rest_api_register_default_routes();
    
    HttpServer server(80);
    server.run();

    osThreadExit();
}

bool rest_api_start_server_task(void) {
    osThreadAttr_t http_task_attr{};
    http_task_attr.name = "http_server";
    http_task_attr.stack_size = 1024 * 2;
    http_task_attr.priority = (osPriority_t) osPriorityNormal;

    return osThreadNew(http_server_task, nullptr, &http_task_attr) != nullptr;
}
