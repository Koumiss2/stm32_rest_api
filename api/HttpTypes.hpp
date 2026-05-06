#pragma once
#include <string_view>

enum class HttpMethod { GET, POST, PUT, DELETE, UNKNOWN };

enum class HttpStatus {
    OK          = 200,
    CREATED     = 201,
    BAD_REQUEST = 400,
    NOT_FOUND   = 404,
    INTERNAL    = 500
};

struct Request {
    HttpMethod        method;
    std::string_view uri;
    std::string_view body;
};

struct Response {
    HttpStatus        status = HttpStatus::OK;
    std::string_view body;
};
