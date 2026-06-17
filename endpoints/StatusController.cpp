#include "StatusController.hpp"
#define JSMN_HEADER
#include "../jsmn/jsmn.h"

void StatusController::get(const Request& req, Response& res) {
    (void)req;
    res.status = HttpStatus::OK;
    res.body = "{\"status\":\"ok\",\"info\":\"GET request received\"}";
}

void StatusController::post(const Request& req, Response& res) {
    if (req.body.empty()) {
        res.status = HttpStatus::BAD_REQUEST;
        res.body = "{\"error\":\"body is empty\"}";
        return;
    }

    jsmn_parser parser;
    jsmntok_t tokens[8];
    jsmn_init(&parser);

    int parsed = jsmn_parse(&parser, req.body.data(), req.body.size(), tokens, 8);
    if (parsed < 0 || tokens[0].type != JSMN_OBJECT) {
        res.status = HttpStatus::BAD_REQUEST;
        res.body = "{\"error\":\"invalid json structure\"}";
        return;
    }

    res.status = HttpStatus::OK;
    res.body = "{\"status\":\"ok\",\"info\":\"POST request processed\"}";
}
