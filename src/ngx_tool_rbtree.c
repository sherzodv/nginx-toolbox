
#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_tool_rbtree.h"

void
ngx_tool_rbtree_init(ngx_tool_rbtree_t * rbt, ngx_pool_t * pool)
{
	ngx_rbtree_init(&rbt->rbtree, &rbt->sentinel, ngx_str_rbtree_insert_value);
	rbt->pool = pool;
}

ngx_tool_rbtree_t *
ngx_tool_rbtree_create(ngx_pool_t * pool)
{
	ngx_tool_rbtree_t * rbt = ngx_palloc(pool, sizeof(ngx_tool_rbtree_t));
	if (rbt == NULL) {
		return NULL;
	}

	ngx_tool_rbtree_init(rbt, pool);

	return rbt;
}

ngx_int_t
ngx_tool_rbtree_insert(ngx_tool_rbtree_t * rbt, ngx_str_t * str)
{
	ngx_str_node_t * node =
		ngx_palloc(rbt->pool, sizeof(ngx_str_node_t));
	if (node == NULL) {
		return NGX_ERROR;
	}
	/* Be intrusive, store only reference */
	node->str = *str;
	node->node.key = ngx_crc32_long(str->data, str->len);
	ngx_rbtree_insert(&rbt->rbtree, &node->node);
	return NGX_OK;
}

ngx_str_node_t *
ngx_tool_rbtree_lookup(ngx_tool_rbtree_t * rbt, ngx_str_t * name)
{
	return ngx_str_rbtree_lookup(&rbt->rbtree, name,
			ngx_crc32_long(name->data, name->len));
}

