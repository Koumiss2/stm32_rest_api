#include "router.hpp"

Router& Router::instance() {
    static Router inst;
    return inst;
}

void Router::get(std::string_view uri, HandlerFunc handler) {
    _routes.push_back({HttpMethod::GET, uri, handler});
}

void Router::post(std::string_view uri, HandlerFunc handler) {
    _routes.push_back({HttpMethod::POST, uri, handler});
}

void Router::put(std::string_view uri, HandlerFunc handler) {
    _routes.push_back({HttpMethod::PUT, uri, handler});
}

void Router::del(std::string_view uri, HandlerFunc handler) {
    _routes.push_back({HttpMethod::DELETE, uri, handler});
}

void Router::register_controller(IController* controller) {
    if (!controller) return;
    _routes.push_back({
        controller->get_method(),
        controller->get_path(),
        [controller](const Request& req, Response& res) {
            controller->handle(req, res);
        }
    });
}

void Router::dispatch(const Request& req, Response& res) const {
    for (const auto& route : _routes) {
        if (route.method == req.method && route.uri == req.uri) {
            route.handler(req, res);
            return;
        }
    }

    res.status = HttpStatus::NOT_FOUND;
    res.body = "{\"error\":\"route not found\"}";
}
