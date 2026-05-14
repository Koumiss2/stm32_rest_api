#pragma once
#include "IController.hpp"

class StatusController : public IController {
public:
    void get(const Request& req, Response& res) override;
    void post(const Request& req, Response& res) override;
};
