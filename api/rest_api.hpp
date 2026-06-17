#pragma once

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void rest_api_register_default_routes(void);
bool rest_api_start_server_task(void);

#ifdef __cplusplus
}
#endif
