
#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_tool_hashset.h"

ngx_int_t
ngx_tool_hashset_init(ngx_tool_hashset_t * hashset,
		ngx_pool_t * pool, ngx_uint_t size)
{
	hashset->buckets = ngx_palloc(pool, sizeof(ngx_tool_hashset_elt_t *) * size);
	if (hashset->buckets == NULL) {
		return NGX_ERROR;
	}

	ngx_memzero(hashset->buckets, sizeof(ngx_tool_hashset_elt_t *) * size);

	hashset->size = size;
	hashset->pool = pool;

	return NGX_OK;
}

ngx_tool_hashset_t *
ngx_tool_hashset_create(ngx_pool_t * pool, ngx_uint_t size)
{
	ngx_tool_hashset_t * hashset = ngx_palloc(pool, sizeof(ngx_tool_hashset_t));
	if (hashset == NULL) {
		return NULL;
	}

	if (ngx_tool_hashset_init(hashset, pool, size) != NGX_OK) {
		return NULL;
	}

	return hashset;
}

ngx_int_t
ngx_tool_hashset_chain_start(ngx_tool_hashset_t * hashset,
	ngx_tool_hashset_elt_t * elt, ngx_str_t * val)
{
	ngx_tool_rbtree_t * rbt = ngx_tool_rbtree_create(hashset->pool);
	if (rbt == NULL) {
		return NGX_ERROR;
	}

	elt->bucket.as_rbtree = rbt;
	elt->nelts++;

	return ngx_tool_rbtree_insert(rbt, val);
}

ngx_int_t
ngx_tool_hashset_insert(ngx_tool_hashset_t * hashset, ngx_str_t * val)
{
	ngx_uint_t key = ngx_hash_key(val->data, val->len) % hashset->size;
	ngx_tool_hashset_elt_t * elt = hashset->buckets[key];

	if (elt == NULL) {
		elt = ngx_palloc(hashset->pool, sizeof(ngx_tool_hashset_elt_t));
		if (elt == NULL) {
			return NGX_ERROR;
		}

		elt->bucket.as_strnode = ngx_palloc(hashset->pool, sizeof(ngx_str_node_t));
		if (elt == NULL) {
			return NGX_ERROR;
		}

		elt->nelts = 1;
		elt->bucket.as_strnode->str = *val;
		hashset->buckets[key] = elt;

		return NGX_OK;
	}

	ngx_uint_t i;

	if (elt->nelts == 1) {

		if (elt->bucket.as_strnode->str.len == val->len) {
			
			/* Chem chert ne shutit? */
			if (elt->bucket.as_strnode->str.data == val->data) {
				return NGX_OK;
			}

			for (i = 0; i < val->len; i++)
				if (val->data[i] != elt->bucket.as_strnode->str.data[i])
					return ngx_tool_hashset_chain_start(hashset, elt, val);
		}

		return NGX_OK;
	}

	return ngx_tool_rbtree_insert(elt->bucket.as_rbtree, val);
}

ngx_str_node_t *
ngx_tool_hashset_lookup(ngx_tool_hashset_t * hashset, ngx_str_t * val)
{
	ngx_uint_t key = ngx_hash_key(val->data, val->len) % hashset->size;
	ngx_tool_hashset_elt_t * elt = hashset->buckets[key];

	if (elt == NULL) {
		return NULL;
	}

	ngx_uint_t i;

	if (elt->nelts == 1) {
		
		if (elt->bucket.as_strnode->str.len == val->len) {

			/* Chem chert ne shutit? */
			if (elt->bucket.as_strnode->str.data == val->data) {
				return elt->bucket.as_strnode;
			}

			for (i = 0; i < val->len; i++)
				if (val->data[i] != elt->bucket.as_strnode->str.data[i])
					return NULL;

			return elt->bucket.as_strnode;
		}
	}

	return ngx_tool_rbtree_lookup(elt->bucket.as_rbtree, val);
}

