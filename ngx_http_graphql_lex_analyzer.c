#include "ngx_http_graphql_lex_analyzer.h"

static enum ngx_http_graphql_lex_match_status
ngx_http_graphql_lex_analysis_unknow(u_char c)
{
    switch (c) {
    case '_':
    case 'A' ... 'Z':
    case 'a' ... 'z':
        return ngx_http_graphql_lex_match_name;

    case '-':
        return ngx_http_graphql_lex_match_num;

    case '0' ... '9':
        return ngx_http_graphql_lex_match_num;

    case '"':
        return ngx_http_graphql_lex_match_str;

    case ',':
    case '.':
    case '!':
    case '$':
    case '(':
    case ')':
    case ':':
    case '=':
    case '@':
    case '[':
    case ']':
    case '{':
    case '|':
    case '}':
        return ngx_http_graphql_lex_match_punctuator;
    }

    return ngx_http_graphql_lex_match_unknow;
}

ngx_list_t * ngx_http_graphql_lex_analysis(ngx_pool_t * pool, ngx_str_t doc)
{
    enum ngx_http_graphql_lex_match_status lex_status
        = ngx_http_graphql_lex_match_unknow;
    ngx_list_t * list = ngx_list_create(pool, 8, sizeof(ngx_str_t));
    size_t i;
    ngx_str_t * token = NULL;
    enum ngx_http_graphql_lex_match_int_status int_status
        = ngx_http_graphql_lex_match_int_part_num;

    for (i = 0; i < doc.len; i++) {
init:
        switch (lex_status) {
        case ngx_http_graphql_lex_match_unknow:
            lex_status = ngx_http_graphql_lex_analysis_unknow(doc.data[i]);
            if (lex_status != ngx_http_graphql_lex_match_unknow) {
                goto init;
            }
            break;

        case ngx_http_graphql_lex_match_name:
            if (token == NULL) {
                token = ngx_list_push(list);
                token->data = doc.data + i;
                token->len = 1;
            }
            else {
                switch (doc.data[i]) {
                    case '_':
                    case '0' ... '9':
                    case 'A' ... 'Z':
                    case 'a' ... 'z':
                        token->len++;
                        break;
                    default:
                        lex_status = ngx_http_graphql_lex_match_unknow;
                        token = NULL;
                        goto init;
                };
            }
            break;

        case ngx_http_graphql_lex_match_punctuator:
            if (token == NULL) {
                token = ngx_list_push(list);
                token->data = doc.data + i;
                token->len = 1;
                if (doc.data[i] != '.') {
                    lex_status = ngx_http_graphql_lex_match_unknow;
                    token = NULL;
                }
            }
            else if (token->len < 3 && doc.data[i] == '.') {
                token->len++;
            }
            else {
                lex_status = ngx_http_graphql_lex_match_unknow;
                token = NULL;
            }
            break;

        case ngx_http_graphql_lex_match_num:
            if (token == NULL) {
                token = ngx_list_push(list);
                token->data = doc.data + i;
                token->len = 1;
                if (token->data[0] == '-') {
                    int_status = ngx_http_graphql_lex_match_int_negative_sign;
                }
                else if (token->data[0] == '0'){
                    int_status = ngx_http_graphql_lex_match_int_zero;
                }
                else {
                    int_status = ngx_http_graphql_lex_match_int_part_num;
                }
            }
            else switch (int_status) {
            case ngx_http_graphql_lex_match_int_negative_sign:
                token->len++;
                if (doc.data[i] == '0') {
                    int_status = ngx_http_graphql_lex_match_int_zero;
                }

                break;
            default:
                goto init;
            }
            break;
                
        default:
            break;
        }
    }

    return list;
}
