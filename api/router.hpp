#pragma once

#include "http_types.hpp"
#include "../endpoints/IController.hpp"
#include <cstdint>
#include <string_view>

#ifndef REST_API_MAX_ROUTES
#define REST_API_MAX_ROUTES 4
#endif

struct Route {
    std::string_view uri;
    IController* controller;
};

class Router {
public:
    static Router& instance();

    Router() = default;
    Router(const Router&) = delete;
    Router& operator=(const Router&) = delete;

    void register_controller(std::string_view path, IController* controller);

    void dispatch(const Request& req, Response& res) const;

private:
    Route _routes[REST_API_MAX_ROUTES]{};
    uint8_t _route_count = 0;
};
