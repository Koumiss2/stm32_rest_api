#include "StatusController.hpp"
#include "jsmn.h"
#include <cstring>

void StatusController::handle(const Request& req, Response& res) {
    // Example of using jsmn to parse request body if it's not empty
    if (!req.body.empty()) {
        jsmn_parser p;
        jsmntok_t tokens[8];
        jsmn_init(&p);
        
        int r = jsmn_parse(&p, req.body.data(), req.body.size(), tokens, 8);
        
        // In a real scenario, we would check r and tokens to extract data
        (void)r; 
    }

    // Static response for now
    res.status = HttpStatus::OK;
    res.body = "{\"status\":\"ok\",\"free_heap\":65536,\"uptime\":1000}";
}
