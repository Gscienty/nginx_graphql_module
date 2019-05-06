#ifndef _NGX_HTTP_GRAPHQL_SCHEMA_H
#define _NGX_HTTP_GRAPHQL_SCHEMA_H

#include <ngx_core.h>

enum ngx_http_graphql_schema_syntax_status {
    ngx_http_graphql_schema_syntax_status_name,
    ngx_http_graphql_schema_syntax_status_want_meta,
    ngx_http_graphql_schema_syntax_status_meta,
    ngx_http_graphql_schema_syntax_status_want_fields,
    ngx_http_graphql_schema_syntax_status_fields
};

enum ngx_http_graphql_schema_syntax_meta_status {
    ngx_http_graphql_schema_syntax_meta_status_first_want_kv,
    ngx_http_graphql_schema_syntax_meta_status_want_kv,
    ngx_http_graphql_schema_syntax_meta_status_kv_finish,
    ngx_http_graphql_schema_syntax_meta_status_sql_field,
    ngx_http_graphql_schema_syntax_meta_status_sql_field_value
};

typedef struct ngx_http_graphql_schema_s ngx_http_graphql_schema_t;
struct ngx_http_graphql_schema_s {
    ngx_str_t graphql_schema_name;
    ngx_str_t graphql_sql_name;
    ngx_list_t schema_fields;
};

void ngx_http_graphql_schema_init(ngx_pool_t * pool, ngx_http_graphql_schema_t * schema);

const char * ngx_http_graphql_schema_parse(ngx_list_part_t ** token_list,
                                           ngx_uint_t * nth,
                                           ngx_list_t * schemas);

#endif
