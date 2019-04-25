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

enum ngx_http_graphql_lex_match_status {
    ngx_http_graphql_lex_match_unknow,
    ngx_http_graphql_lex_match_name,
    ngx_http_graphql_lex_match_num,
    ngx_http_graphql_lex_match_str,
    ngx_http_graphql_lex_match_punctuator
};

enum ngx_http_graphql_lex_match_int_status {
    ngx_http_graphql_lex_match_int_negative_sign,
    ngx_http_graphql_lex_match_int_zero,
    ngx_http_graphql_lex_match_int_part_num,
    ngx_http_graphql_lex_match_float_frac,
    ngx_http_graphql_lex_match_float_exp,
    ngx_http_graphql_lex_match_float_exp_sign,
    ngx_http_graphql_lex_match_float_exp_num
};

ngx_list_t * ngx_http_graphql_lex_analysis(ngx_pool_t * pool, ngx_str_t * doc);

#endif
