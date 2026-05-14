#pragma once
#include "IController.hpp"

class StatusController : public IController {
public:
    void handle(const Request& req, Response& res) override;
    const char* get_path() const override { return "/status"; }
    HttpMethod get_method() const override { return HttpMethod::GET; }
};
