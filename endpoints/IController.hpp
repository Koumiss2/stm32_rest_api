#include "HttpTypes.hpp"

class IController {
public:
    virtual ~IController() = default;
    virtual void handle(const Request& req, Response& res) = 0;
};
