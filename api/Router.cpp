#include "Router.hpp"
#include "../endpoints/IController.hpp"
#include <cstring>

// ---------- singleton ----------
Router& Router::instance() {
    static Router inst;
    return inst;
}

// ---------- helpers ----------
static bool str_eq(const char* a, const char* b, size_t len) {
    return len == strlen(b) && strncmp(a, b, len) == 0;
}

static bool match_path(const char* uri, size_t uri_len,
                       const char* prefix, size_t prefix_len,
                       const char** remainder) {
    if (uri_len < prefix_len) return false;
    if (strncmp(uri, prefix, prefix_len) != 0) return false;
    *remainder = uri + prefix_len;
    return true;
}

// ---------- routing ----------
void Router::handle(uint8_t* raw, uint16_t len)
{
    if (len == 0) return;

    // 1) parse method
    HttpMethod m = detect_method(reinterpret_cast<char*>(raw), len);
    if (m == HttpMethod::UNKNOWN) return;

    // 2) skip "METHOD "
    const char* p = reinterpret_cast<char*>(raw);
    while (*p && *p != ' ') p++;          // skip method
    while (*p == ' ') p++;                // skip space

    // 3) URI starts here — find end of URI token
    const char* uri_end = p;
    while (*uri_end && *uri_end != ' ' && *uri_end != '\r') uri_end++;
    size_t uri_len = uri_end - p;

    // 4) extract body (after \r\n\r\n)
    std::string_view body;
    const char* body_start = strstr(p, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
        body = std::string_view(body_start, len - (body_start - reinterpret_cast<char*>(raw)));
    }

    Request req{m, std::string_view(p, uri_len), body};
    Response res;

    // 5) dispatch to controllers
    if (false) {}
#include "router_dispatch.inc"
}

HttpMethod Router::detect_method(const char* buf, uint16_t len)
{
    (void)len;
    if (strncmp(buf, "GET ",   4) == 0) return HttpMethod::GET;
    if (strncmp(buf, "POST ", 5) == 0) return HttpMethod::POST;
    if (strncmp(buf, "PUT ",   4) == 0) return HttpMethod::PUT;
    if (strncmp(buf, "DELETE ", 7) == 0) return HttpMethod::DELETE;
    return HttpMethod::UNKNOWN;
}