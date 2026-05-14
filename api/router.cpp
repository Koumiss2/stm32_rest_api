#include "router.hpp"
#include "IController.hpp"
#include <cstring>

using HandlerFunc = std::function<void(const Request& req, Response& res)>;

struct Route {
    HttpMethod method;
    std::string_view uri;
    HandlerFunc handler;
};

Router& Router::instance() {
    static Router inst;
    return inst;
}
