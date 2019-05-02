#ifndef _NGX_HTTP_GRAPHQL_SCHEMA_FIELD_H
#define _NGX_HTTP_GRAPHQL_SCHEMA_FIELD_H

#include <ngx_core.h>

enum ngx_http_graphql_schema_field_type {
    ngx_http_graphql_schema_field_type_string,
    ngx_http_graphql_schema_field_type_int,
    ngx_http_graphql_schema_field_type_float
};

enum ngx_http_graphql_schema_field_syntax_status {
    ngx_http_graphql_schema_field_syntax_status_name,
    ngx_http_graphql_schema_field_syntax_status_want_meta,
    ngx_http_graphql_schema_field_syntax_status_meta
};

enum ngx_http_graphql_schema_field_syntax_meta_status {
    ngx_http_graphql_schema_field_syntax_meta_status_first_want_kv,
    ngx_http_graphql_schema_field_syntax_meta_status_want_kv,
    ngx_http_graphql_schema_field_syntax_meta_status_kv_finish,
    ngx_http_graphql_schema_field_syntax_meta_status_type,
    ngx_http_graphql_schema_field_syntax_meta_status_type_value,
    ngx_http_graphql_schema_field_syntax_meta_status_sql_field,
    ngx_http_graphql_schema_field_syntax_meta_status_sql_field_value
};

typedef struct ngx_http_graphql_schema_field_s ngx_http_graphql_schema_field_t;
struct ngx_http_graphql_schema_field_s {
    enum ngx_http_graphql_schema_field_type type;
    ngx_str_t graphql_field_name;
    ngx_str_t sql_field_name;
};

const char * ngx_http_graphql_schema_field_parse(ngx_list_part_t ** list_part,
                                                 ngx_uint_t * nth,
                                                 ngx_list_t * fields);

#endif
