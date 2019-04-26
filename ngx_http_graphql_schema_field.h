#ifndef _NGX_HTTP_GRAPHQL_SCHEMA_FIELD_H
#define _NGX_HTTP_GRAPHQL_SCHEMA_FIELD_H

#include <ngx_core.h>

enum ngx_http_graphql_schema_field_type {
    ngx_http_graphql_schema_field_type_string,
    ngx_http_graphql_schema_field_type_int,
    ngx_http_graphql_schema_field_type_float
};

typedef struct ngx_http_graphql_schema_field_s ngx_http_graphql_schema_field_t;
struct ngx_http_graphql_schema_field_s {
    enum ngx_http_graphql_schema_field_type type;
    ngx_str_t graphql_field_name;
    ngx_str_t sql_field_name;
};

#endif
