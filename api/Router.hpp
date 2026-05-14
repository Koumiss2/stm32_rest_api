#pragma once
#include "HttpTypes.hpp"
#include <cstdint>

class IController;

class Router {
public:
    static Router& instance();

private:
    Router() = default;
};
