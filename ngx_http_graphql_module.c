#include "ngx_http_graphql_module.h"
#include "ngx_http_graphql_lex_analyzer.h"

static void * ngx_http_graphql_create_loc_conf(ngx_conf_t * cf);
static char * ngx_http_graphql(ngx_conf_t * cf, void * post, void * data);

static ngx_conf_post_handler_pt ngx_http_graphql_p = ngx_http_graphql;

static ngx_command_t ngx_http_graphql_commands[] = {
    {
        ngx_string("graphql"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_graphql_loc_conf_t, doc),
        &ngx_http_graphql_p
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_graphql_module_ctx = {
    .preconfiguration = NULL,
    .postconfiguration = NULL,
    
    .create_main_conf = NULL,
    .init_main_conf = NULL,

    .create_srv_conf = NULL,
    .merge_srv_conf = NULL,

    .create_loc_conf = ngx_http_graphql_create_loc_conf,
    .merge_loc_conf = NULL
};

ngx_module_t ngx_http_graphql_module = {
    NGX_MODULE_V1,
    &ngx_http_graphql_module_ctx,
    ngx_http_graphql_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

static ngx_list_t * tokens;

static void * ngx_http_graphql_create_loc_conf(ngx_conf_t * cf)
{
    ngx_http_graphql_loc_conf_t * conf;
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_graphql_loc_conf_t));

    if (conf == NULL) {
        return NULL;
    }

    conf->doc.data = NULL;
    conf->doc.len = 0;

    return conf;
}

static ngx_int_t ngx_http_graphql_handler(ngx_http_request_t * req)
{
    ngx_int_t ret = ngx_http_discard_request_body(req);
    if (ret != NGX_OK) {
        return ret;
    }
    ngx_str_t response;

    response.data = ngx_pcalloc(req->pool, 5);
    ngx_sprintf(response.data, "%d", tokens->size);
    response.len = 5;

    req->headers_out.status = NGX_HTTP_OK;
    ngx_str_set(&req->headers_out.content_type, "application/json");
    if (ngx_http_set_content_type(req) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    req->headers_out.content_length_n = response.len;
    ret = ngx_http_send_header(req);
    if (ret == NGX_ERROR || ret > NGX_OK || req->header_only) {
        return ret;
    }

    ngx_buf_t *buf = ngx_create_temp_buf(req->pool, response.len);
    if (buf == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    ngx_memcpy(buf->pos, response.data, response.len);
    buf->last = buf->pos + response.len;
    buf->last_buf = 1;
    ngx_chain_t out;
    out.buf = buf;
    out.next = NULL;

    return ngx_http_output_filter(req, &out);
}

static char * ngx_http_graphql(ngx_conf_t * cf, void * post, void * data)
{
    (void) post;
    ngx_str_t * doc = data;
    ngx_http_core_loc_conf_t * core_loc_fc =
        ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    core_loc_fc->handler = ngx_http_graphql_handler;

    if (doc == NULL || ngx_strcmp(doc->data, "") == 0) {
        return NGX_CONF_ERROR;
    }
    tokens = ngx_http_graphql_lex_analysis(cf->pool, doc);

    if (tokens->size == 0) {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}
