#include "http_server.hpp"
#include "router.hpp"
#include "StatusController.hpp"
#include <cstring>
#include <cstdio>
#include <string>

static StatusController status_controller;

HttpServer::HttpServer(uint16_t port) : _port(port), _listen_fd(-1) {}

HttpServer::~HttpServer() {
    if (_listen_fd >= 0) {
        lwip_close(_listen_fd);
    }
}

bool HttpServer::init() {
    struct sockaddr_in serv_addr;

    _listen_fd = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_fd < 0) return false;

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

        // Используем глобальный роутер для обработки запроса
        Router::instance().dispatch(req, res);

        send_response(client_fd, res);
    }
}

Request HttpServer::parse_request(char* buffer, size_t size) {
    Request req;
    req.method = HttpMethod::UNKNOWN;
    
    std::string_view full_data(buffer, size);
    
    // Находим начало тела (после \r\n\r\n)
    size_t header_end = full_data.find("\r\n\r\n");
    if (header_end != std::string_view::npos) {
        req.body = full_data.substr(header_end + 4);
    }

    char* method_str = strtok(buffer, " ");
    if (method_str) {
        if (strcmp(method_str, "GET") == 0) req.method = HttpMethod::GET;
        else if (strcmp(method_str, "POST") == 0) req.method = HttpMethod::POST;
        else if (strcmp(method_str, "PUT") == 0) req.method = HttpMethod::PUT;
        else if (strcmp(method_str, "DELETE") == 0) req.method = HttpMethod::DELETE;
    }

    char* uri_str = strtok(nullptr, " ");
    if (uri_str) {
        req.uri = uri_str;
    }

    return req;
}

void HttpServer::send_response(int client_fd, const Response& res) {
    char hdr[256];
    int code = static_cast<int>(res.status);
    const char* status_text = (res.status == HttpStatus::OK) ? "OK" : "Not Found";

    int n = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        code, status_text, (int)res.body.size());

    lwip_send(client_fd, hdr, n, 0);
    lwip_send(client_fd, res.body.data(), res.body.size(), 0);
}

// Реализация задачи FreeRTOS
void http_server_task(void *argument) {
    (void)argument;
    
    // Регистрируем контроллеры с указанием пути
    Router::instance().register_controller("/status", &status_controller);
    
    HttpServer server(80);
    server.run();
}
