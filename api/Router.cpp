#include "Router.hpp"
#include "../endpoints/IController.hpp"
#include "lwip/api.h"
#include <cstring>
#include <cstdio>

// ---------- singleton ----------
Router& Router::instance() {
    static Router inst;
    return inst;
}

void Router::add_route(const char* prefix, IController* ctrl) {
    if (route_count_ < 8) {
        routes_[route_count_].prefix = prefix;
        routes_[route_count_].ctrl   = ctrl;
        route_count_++;
    }
}

// ---------- http parsing ----------
HttpMethod Router::detect_method(const char* buf) {
    if (strncmp(buf, "GET ",    4) == 0) return HttpMethod::GET;
    if (strncmp(buf, "POST ",   5) == 0) return HttpMethod::POST;
    if (strncmp(buf, "PUT ",    4) == 0) return HttpMethod::PUT;
    if (strncmp(buf, "DELETE ", 7) == 0) return HttpMethod::DELETE;
    return HttpMethod::UNKNOWN;
}

bool Router::match_prefix(const char* uri, size_t uri_len, const char* prefix) {
    size_t plen = strlen(prefix);
    return uri_len >= plen && strncmp(uri, prefix, plen) == 0;
}

// ---------- response ----------
static const char* status_phrase(HttpStatus s) {
    switch (s) {
        case HttpStatus::OK:          return "OK";
        case HttpStatus::CREATED:     return "Created";
        case HttpStatus::BAD_REQUEST: return "Bad Request";
        case HttpStatus::NOT_FOUND:   return "Not Found";
        case HttpStatus::INTERNAL:    return "Internal Server Error";
        default:                      return "Unknown";
    }
}

void Router::send_response(struct netconn* conn, const Response& r) {
    const char* phrase = status_phrase(r.status);
    char hdr[128];
    int n = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "\r\n",
        (int)r.status, phrase, (unsigned int)r.body.size());

    netconn_write(conn, hdr, (size_t)n, NETCONN_COPY);
    if (!r.body.empty()) {
        netconn_write(conn, r.body.data(), r.body.size(), NETCONN_COPY);
    }
}

// ---------- routing ----------
void Router::handle(uint8_t* raw, uint16_t len, struct netconn* conn)
{
    if (len == 0) return;

    const char* buf = reinterpret_cast<const char*>(raw);

    HttpMethod m = detect_method(buf);
    if (m == HttpMethod::UNKNOWN) {
        Response r{HttpStatus::BAD_REQUEST, "{\"error\":\"bad request\"}"};
        send_response(conn, r);
        return;
    }

    // Skip "METHOD "
    const char* p = buf;
    while (*p && *p != ' ') p++;
    while (*p == ' ') p++;

    // Extract URI token
    const char* uri_end = p;
    while (*uri_end && *uri_end != ' ' && *uri_end != '\r') uri_end++;
    size_t uri_len = (size_t)(uri_end - p);

    // Extract body (after \r\n\r\n)
    std::string_view body;
    const char* body_start = strstr(p, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
        size_t off = (size_t)(body_start - buf);
        body = std::string_view(body_start,
            (len > off) ? (size_t)(len - (unsigned)off) : 0);
    }

    Request req{m, std::string_view(p, uri_len), body};
    (void)req;
    Response res{HttpStatus::NOT_FOUND, "{\"error\":\"not found\"}"};

    // Try each registered route
    for (uint8_t i = 0; i < route_count_; i++) {
        if (match_prefix(p, uri_len, routes_[i].prefix)) {
            const char* subpath = p + strlen(routes_[i].prefix);
            Request sub_req{m, std::string_view(subpath), body};
            routes_[i].ctrl->handle(sub_req, res);
            break;
        }
    }

    send_response(conn, res);
}