#pragma once
#include "HttpTypes.hpp"
#include <cstdint>

class IController;

class Router {
public:
    static Router& instance();

    void register_controller(const char* prefix, IController* ctrl);
    void handle(uint8_t* raw_request, uint16_t len);

private:
    Router() = default;

    static uint8_t method_from_str(const char* m, uint8_t len);
    static HttpMethod detect_method(const char* buf, uint16_t len);
    static const char* skip_uri_path(const char* p);
};
