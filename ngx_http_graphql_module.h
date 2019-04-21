#ifndef _NGX_HTTP_GRAPHQL_MODULE_H
#define _NGX_HTTP_GRAPHQL_MODULE_H

#include <ngx_config.h>
#include <ngx_http.h>
#include <ngx_core.h>

typedef struct ngx_http_graphql_loc_conf_s ngx_http_graphql_loc_conf_t;
struct ngx_http_graphql_loc_conf_s {
    ngx_str_t doc;
};

#endif
