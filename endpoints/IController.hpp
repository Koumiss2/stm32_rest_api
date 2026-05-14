#pragma once
#include "http_types.hpp"

class IController {
public:
    virtual ~IController() = default;
    virtual void handle(const Request& req, Response& res) = 0;

    virtual const char* get_path() const = 0;
    virtual HttpMethod get_method() const = 0;
};
