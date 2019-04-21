#ifndef _NGX_HTTP_GRAPHQL_LEX_ANALYSIS_H
#define _NGX_HTTP_GRAPHQL_LEX_ANALYSIS_H

#include <ngx_core.h>

enum ngx_http_graphql_lex_token {
    ngx_http_graphql_lex_token_punctuator,
    ngx_http_graphql_lex_token_name,
    ngx_http_graphql_lex_token_int_value,
    ngx_http_graphql_lex_token_float_value,
    ngx_http_graphql_lex_token_string_value
};

void ngx_http_graphql_lex_analysis(ngx_str_t doc);

#endif
