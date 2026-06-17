#include "StatusController.hpp"
#define JSMN_STATIC
#include "../jsmn/jsmn.h"
#include <cstring>

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

    jsmn_parser p;
    jsmntok_t tokens[16]; 
    jsmn_init(&p);
    
    // Парсим
    int r = jsmn_parse(&p, req.body.data(), req.body.size(), tokens, 16);
    
    // Если r < 0 — ошибка парсинга
    // Если tokens[0].type != JSMN_OBJECT — мы ожидаем JSON объект {}
    if (r < 0 || tokens[0].type != JSMN_OBJECT) {
        res.status = HttpStatus::BAD_REQUEST;
        res.body = "{\"error\":\"invalid json structure\"}";
        return;
    }

    res.status = HttpStatus::OK;
    res.body = "{\"status\":\"ok\",\"info\":\"POST request processed\"}";
}
