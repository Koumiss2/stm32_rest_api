#pragma once
#include "HttpTypes.hpp"
#include <cstdint>
#include <cstddef>

struct netconn;

class IController;

class Router {
public:
    static Router& instance();
    void add_route(const char* prefix, IController* ctrl);
    void handle(uint8_t* raw, uint16_t len, struct netconn* conn);

private:
    Router() : routes_{}, route_count_(0) {}

    HttpMethod detect_method(const char* buf);
    bool match_prefix(const char* uri, size_t uri_len, const char* prefix);
    void send_response(struct netconn* conn, const Response& res);

    struct Route {
        const char*   prefix;
        IController*  ctrl;
    };

    Route routes_[8];
    uint8_t route_count_;
};
