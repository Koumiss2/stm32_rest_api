#include "router.hpp"

namespace {
Router g_router;
}

Router& Router::instance() {
    return g_router;
}

void Router::register_controller(std::string_view path, IController* controller) {
    if (!controller || _route_count >= REST_API_MAX_ROUTES) {
        return;
    }

    _routes[_route_count++] = {path, controller};
}

void Router::dispatch(const Request& req, Response& res) const {
    for (uint8_t i = 0; i < _route_count; ++i) {
        const auto& route = _routes[i];
        if (route.uri == req.uri) {
            switch (req.method) {
                case HttpMethod::GET:
                    route.controller->get(req, res);
                    return;
                case HttpMethod::POST:
                    route.controller->post(req, res);
                    return;
                case HttpMethod::PUT:
                    route.controller->put(req, res);
                    return;
                case HttpMethod::DELETE:
                    route.controller->del(req, res);
                    return;
                default:
                    break;
            }
            return;
        }
    }

    res.status = HttpStatus::NOT_FOUND;
    res.body = "{\"error\":\"route not found\"}";
}
