#pragma once
#include "http_types.hpp"

class IController {
public:
    virtual ~IController() = default;
    
    virtual void get(const Request& req, Response& res) { res.status = HttpStatus::METHOD_NOT_ALLOWED; }
    virtual void post(const Request& req, Response& res) { res.status = HttpStatus::METHOD_NOT_ALLOWED; }
    virtual void put(const Request& req, Response& res) { res.status = HttpStatus::METHOD_NOT_ALLOWED; }
    virtual void del(const Request& req, Response& res) { res.status = HttpStatus::METHOD_NOT_ALLOWED; }
};
