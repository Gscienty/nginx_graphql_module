#include "ngx_http_graphql_schema_field.h"
#include "ngx_http_graphql_lex_analyzer.h"

const char * ngx_http_graphql_schema_field_parse(ngx_list_part_t ** list_part,
                                                 ngx_uint_t * nth,
                                                 ngx_list_t * fields)
{
    ngx_http_graphql_schema_field_t * field = NULL;
    ngx_http_graphql_lex_token_t * token = NULL;
    enum ngx_http_graphql_schema_field_syntax_status status
        = ngx_http_graphql_schema_field_syntax_status_name;
    enum ngx_http_graphql_schema_field_syntax_meta_status meta_status
        = ngx_http_graphql_schema_field_syntax_meta_status_want_kv;

    if (*list_part == NULL) {
        return NGX_CONF_OK;
    }

    while (*list_part) {
        for (; *nth < (*list_part)->nelts; nth++) {
            token = (ngx_http_graphql_lex_token_t *) (*list_part)->elts;
            switch (status) {
            case ngx_http_graphql_schema_field_syntax_status_name:
                if (token->type != ngx_http_graphql_lex_token_type_name) {
                    return NGX_CONF_ERROR;
                }
                field = ngx_list_push(fields);
                field->graphql_field_name = token->token;
                field->sql_field_name = token->token;
                field->type = ngx_http_graphql_schema_field_type_string;
                status = ngx_http_graphql_schema_field_syntax_status_want_meta;
                break;

            case ngx_http_graphql_schema_field_syntax_status_want_meta:
                if (token->type == ngx_http_graphql_lex_token_punctuator
                    && token->token.data[0] == '(') {
                    status = ngx_http_graphql_schema_field_syntax_status_meta;
                }
                else {
                    goto finish;
                }
                break;

            case ngx_http_graphql_schema_field_syntax_status_meta:
                if (token->type == ngx_http_graphql_lex_token_type_punctuator) {
                    switch (token->token.data[0]) {
                    case ')':
                        if (meta_status == ngx_http_graphql_schema_field_syntax_meta_status_kv_finish
                            || meta_status == ngx_http_graphql_schema_field_syntax_meta_status_first_want_kv) {
                            goto ignore_one_token_finish;
                        }
                        return NGX_CONF_ERROR;
                    case ':':
                        switch (meta_status) {
                        case ngx_http_graphql_schema_field_syntax_meta_status_type:
                            meta_status = ngx_http_graphql_schema_field_syntax_meta_status_type_value;
                            break;
                        case ngx_http_graphql_schema_field_syntax_meta_status_sql_field:
                            meta_status = ngx_http_graphql_schema_field_syntax_meta_status_sql_field_value;
                            break;
                        default:
                            return NGX_CONF_ERROR;
                        }
                        break;
                    case ',':
                        if (meta_status != ngx_http_graphql_schema_field_syntax_meta_status_kv_finish) {
                            return NGX_CONF_ERROR;
                        }
                        meta_status = ngx_http_graphql_schema_field_syntax_meta_status_want_kv;
                        break;
                    }
                }
                else if (token->type == ngx_http_graphql_lex_token_type_name) {
                    switch (meta_status) {
                    case ngx_http_graphql_schema_field_syntax_meta_status_first_want_kv:
                    case ngx_http_graphql_schema_field_syntax_meta_status_want_kv:
                        if (ngx_strcmp(token->token.data, "type") == 0) {
                            meta_status = ngx_http_graphql_schema_field_syntax_meta_status_type;
                        }
                        else if (ngx_strcmp(token->token.data, "sql_field") == 0) {
                            meta_status = ngx_http_graphql_schema_field_syntax_meta_status_sql_field;
                        }
                        else {
                            return NGX_CONF_ERROR;
                        }
                        break;

                    default:
                        return NGX_CONF_ERROR;
                    }
                }
                else if (token->type == ngx_http_graphql_lex_token_type_string_value) {
                    switch (meta_status) {
                    case ngx_http_graphql_schema_field_syntax_meta_status_sql_field_value:
                        field->sql_field_name.data = token->token.data + 1;
                        field->sql_field_name.len = token->token.len - 2;
                        meta_status = ngx_http_graphql_schema_field_syntax_meta_status_kv_finish;
                        break;

                    case ngx_http_graphql_schema_field_syntax_meta_status_type_value:
                        if (ngx_strcmp(token->token.data, "\"string\"") == 0) {
                            field->type = ngx_http_graphql_schema_field_type_string;
                        }
                        else if (ngx_strcmp(token->token.data, "\"int\"") == 0) {
                            field->type = ngx_http_graphql_schema_field_type_int;
                        }
                        else if (ngx_strcmp(token->token.data, "\"float\"") == 0) {
                            field->type = ngx_http_graphql_schema_field_type_float;
                        }
                        else {
                            return NGX_CONF_ERROR;
                        }
                        meta_status = ngx_http_graphql_schema_field_syntax_meta_status_kv_finish;
                        break;
                        
                    default:
                        return NGX_CONF_ERROR;
                    }
                }
                break;
            }
        }
        *nth = 0;
        *list_part = (*list_part)->next;
    }

ignore_one_token_finish:
    (*nth)++;
finish:
    return NGX_CONF_OK;
}
