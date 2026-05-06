#include "lwip/api.h"
#include "string.h"

static void http_server_serve(struct netconn *conn) {
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;

    if (netconn_recv(conn, &inbuf) == ERR_OK) {
        netbuf_data(inbuf, (void**)&buf, &buflen);

        if (strncmp(buf, "GET /api/status", 15) == 0) {
            const char *response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Connection: close\r\n\r\n"
                "{\"device\":\"stm32\",\"status\":\"ok\"}";
            netconn_write(conn, response, strlen(response), NETCONN_COPY);
        } 
        else if (strncmp(buf, "POST /api/config", 16) == 0) {
            const char *response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Connection: close\r\n\r\n"
                "{\"status\":\"updated\"}";
            netconn_write(conn, response, strlen(response), NETCONN_COPY);
        } 
        else {
            const char *response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
            netconn_write(conn, response, strlen(response), NETCONN_COPY);
        }
    }
    netconn_close(conn);
    netbuf_delete(inbuf);
}

static void http_server_thread(void *arg) {
    struct netconn *conn, *newconn;
    err_t err;

    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);

    while (1) {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK) {
            http_server_serve(newconn);
            netconn_delete(newconn);
        }
    }
}