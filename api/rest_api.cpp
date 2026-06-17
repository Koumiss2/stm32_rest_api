#include "rest_api.hpp"
#include "router.hpp"
#include "../endpoints/StatusController.hpp"

namespace {
StatusController g_status_controller;
bool g_routes_registered = false;
}

void rest_api_register_default_routes(void) {
    if (g_routes_registered) {
        return;
    }

    Router::instance().register_controller("/status", &g_status_controller);
    g_routes_registered = true;
}
