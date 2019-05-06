#include "ngx_http_graphql_schema.h"
#include "ngx_http_graphql_schema_field.h"
#include "ngx_http_graphql_lex_analyzer.h"

void ngx_http_graphql_schema_init(ngx_pool_t * pool, ngx_http_graphql_schema_t * schema)
{
    schema->graphql_schema_name.data = NULL;
    schema->graphql_schema_name.len = 0;
    schema->graphql_sql_name.data = NULL;
    schema->graphql_sql_name.len = 0;
    ngx_list_init(&schema->schema_fields,
                  pool,
                  8,
                  sizeof(ngx_http_graphql_schema_field_t));
}

const char * ngx_http_graphql_schema_parse(ngx_list_part_t ** token_list,
                                           ngx_uint_t * nth,
                                           ngx_list_t * schemas)
{
    ngx_http_graphql_lex_token_t * token = NULL;
    ngx_http_graphql_schema_t * schema = NULL;
    enum ngx_http_graphql_schema_syntax_status status
        = ngx_http_graphql_schema_syntax_status_name;
    enum ngx_http_graphql_schema_syntax_meta_status
        meta_status = ngx_http_graphql_schema_syntax_meta_status_first_want_kv;
    const char * field_parse_result = NULL;

    if (*token_list == NULL) {
        return NGX_CONF_OK;
    }

    while (*token_list) {
        for (; *nth < (*token_list)->nelts; (*nth)++) {
loop_begin:
            if (*token_list == NULL) {
                goto finish;
            }
            token = (ngx_http_graphql_lex_token_t *) (*token_list)->elts + *nth;
            switch (status) {
            case ngx_http_graphql_schema_syntax_status_name:
                if (token->type != ngx_http_graphql_lex_token_type_name) {
                    return NGX_CONF_ERROR;
                }
                schema = ngx_list_push(schemas);
                ngx_http_graphql_schema_init(schemas->pool, schema);
                schema->graphql_schema_name.data = token->token.data;
                schema->graphql_schema_name.len = token->token.len;
                schema->graphql_sql_name.data = token->token.data;
                schema->graphql_sql_name.len = token->token.len;
                status = ngx_http_graphql_schema_syntax_status_want_meta;
                break;

            case ngx_http_graphql_schema_syntax_status_want_meta:
                if (token->type != ngx_http_graphql_lex_token_type_punctuator) {
                    return NGX_CONF_ERROR;
                }
                if (token->token.data[0] == '(') {
                    status = ngx_http_graphql_schema_syntax_status_meta;
                }
                else if (token->token.data[0] == '{') {
                    status = ngx_http_graphql_schema_syntax_status_fields;
                }
                else {
                    return NGX_CONF_ERROR;
                }
                break;

            case ngx_http_graphql_schema_syntax_status_meta:
                switch (token->type) {
                case ngx_http_graphql_lex_token_type_punctuator:
                    switch (token->token.data[0]) {
                    case ')':
                        if (meta_status != ngx_http_graphql_schema_syntax_meta_status_first_want_kv
                            && meta_status != ngx_http_graphql_schema_syntax_meta_status_kv_finish) {
                            return NGX_CONF_ERROR;
                        }
                        status = ngx_http_graphql_schema_syntax_status_want_fields;
                        break;
                    case ':':
                        switch (meta_status) {
                        case ngx_http_graphql_schema_syntax_meta_status_sql_field:
                            meta_status = ngx_http_graphql_schema_syntax_meta_status_sql_field_value;
                            break;
                        default:
                            return NGX_CONF_ERROR;
                        } /* switch (meta_status) */
                        break;
                    case ',':
                        if (meta_status != ngx_http_graphql_schema_syntax_meta_status_kv_finish) {
                            return NGX_CONF_ERROR;
                        }
                        meta_status = ngx_http_graphql_schema_syntax_meta_status_want_kv;
                        break;
                    } /* switch (token->token.data[0]) */
                    break;

                case ngx_http_graphql_lex_token_type_name:
                    switch (meta_status) {
                    case ngx_http_graphql_schema_syntax_meta_status_first_want_kv:
                    case ngx_http_graphql_schema_syntax_meta_status_want_kv:
                        if (ngx_strncmp(token->token.data, "sql_field", token->token.len) == 0) {
                            meta_status = ngx_http_graphql_schema_syntax_meta_status_sql_field;
                        }
                        else {
                            return NGX_CONF_ERROR;
                        }
                        break;
                    default:
                        return NGX_CONF_ERROR;
                    } /* switch (meta_status) */
                    break;

                case ngx_http_graphql_lex_token_type_string_value:
                    switch (meta_status) {
                    case ngx_http_graphql_schema_syntax_meta_status_sql_field_value:
                        schema->graphql_sql_name.data = token->token.data + 1;
                        schema->graphql_sql_name.len = token->token.len - 2;
                        meta_status = ngx_http_graphql_schema_syntax_meta_status_kv_finish;
                        break;
                    default:
                        return NGX_CONF_ERROR;
                    } /* switch (meta_status) */

                default:
                    return NGX_CONF_ERROR;
                } /* switch (token->type) */
                break;

            case ngx_http_graphql_schema_syntax_status_want_fields:
                if (token->type != ngx_http_graphql_lex_token_type_punctuator
                    || token->token.data[0] != '{') {
                    return NGX_CONF_ERROR;
                }
                status = ngx_http_graphql_schema_syntax_status_fields;
                break;

            case ngx_http_graphql_schema_syntax_status_fields:
                if (token->type == ngx_http_graphql_lex_token_type_punctuator
                    && token->token.data[0] == '}') {
                    goto finish;
                }
                field_parse_result = ngx_http_graphql_schema_field_parse(token_list, nth, &schema->schema_fields);
                if (field_parse_result == NGX_CONF_ERROR) {
                    return NGX_CONF_ERROR;
                }
                
                goto loop_begin;

            default:
                return NGX_CONF_ERROR;
            } /* switch (status) */
        }

        *nth = 0;
        *token_list = (*token_list)->next;
    }

finish:
    (*nth)++;
    return NGX_CONF_OK;
}
