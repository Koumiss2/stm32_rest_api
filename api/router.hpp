#pragma once
#include "http_types.hpp"
#include <cstdint>

class IController;

class Router {
public:
    static Router& instance();

private:
    Router() = default;
};
