#include "ngx_http_graphql_schema_field.h"
#include "ngx_http_graphql_lex_analyzer.h"

void ngx_http_graphql_schema_field_parse(ngx_list_part_t ** list_part,
                                         ngx_uint_t * nth,
                                         ngx_list_t * fields)
{
    ngx_http_graphql_schema_field_t * field = NULL;
    ngx_http_graphql_lex_token_t * token = NULL;
    enum ngx_http_graphql_schema_field_syntax_status status
        = ngx_http_graphql_schema_field_syntax_status_name;

    if (*list_part == NULL) {
        return;
    }

    field = ngx_list_push(fields);

    while (*list_part) {
        for (; *nth < (*list_part)->nelts; nth++) {
            token = (ngx_http_graphql_lex_token_t *) (*list_part)->elts;
            switch (status) {
            case ngx_http_graphql_schema_field_syntax_status_name:
                field->graphql_field_name = token->token;
                field->sql_field_name = token->token;
                field->type = ngx_http_graphql_schema_field_type_string;
                status = ngx_http_graphql_schema_field_syntax_status_want_meta;
                break;
            case ngx_http_graphql_schema_field_syntax_status_want_meta:
                if (token->type != ngx_http_graphql_lex_token_type_punctuator
                    || token->token.data[0] == '(') {
                    return;
                }
                else {
                    status = ngx_http_graphql_schema_field_syntax_status_meta;
                }
                break;
            case ngx_http_graphql_schema_field_syntax_status_meta:
                if (token->type == ngx_http_graphql_lex_token_type_punctuator
                    && token->token.data[0] == ')') {
                    
                }
                break;
            }
        }
        *nth = 0;
        *list_part = (*list_part)->next;
    }
}
