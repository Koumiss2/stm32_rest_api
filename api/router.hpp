#pragma once

#include "http_types.hpp"
#include "../endpoints/IController.hpp"
#include <cstdint>
#include <vector>
#include <string_view>
#include <functional>

using HandlerFunc = std::function<void(const Request& req, Response& res)>;

struct Route {
    HttpMethod method;
    std::string_view uri;
    HandlerFunc handler;
};

class Router {
public:
    static Router& instance();

    Router(const Router&) = delete;
    Router& operator=(const Router&) = delete;

    void get(std::string_view uri, HandlerFunc handler);
    void post(std::string_view uri, HandlerFunc handler);
    void put(std::string_view uri, HandlerFunc handler);
    void del(std::string_view uri, HandlerFunc handler);
    
    void register_controller(std::string_view path, IController* controller);

    void dispatch(const Request& req, Response& res) const;

private:
    Router() = default;
    std::vector<Route> _routes;
};
