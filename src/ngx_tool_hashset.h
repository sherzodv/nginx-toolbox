
#ifndef _NGX_TOOL_HASHSET_H_INCLUDED_
#define _NGX_TOOL_HASHSET_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_list.h>
#include <ngx_string.h>
#include "ngx_tool_rbtree.h"

/*
 * Intrusive hashset
 *
 * Uses sizeof(void *) + sizeof(ngx_uint_t) for each unused bucket.
 * If only a single ref stored in a bucket, no chain is created.
 * If more then one refs are chained, uses rbtree as a chain.
 *
 * ngx_hashset_t stores only references to ngx_str_t:
 *
 * 		ngx_str_t str1 = ngx_string("hello");
 * 		node->str = str1;
 *
 * !!!
 * Take into account intrusive nature of ngx_tool_hashset_t:
 * all pointers to u_char of (ngx_str_t) instances stored
 * in a set should remain valid during the whole lifetime
 * of the set.
 *
 * TODO: Optimize elements memory alignemnt, as in ngx_hash.c
 * TODO: Optimize cacheline alignment of elements, as in ngx_hash.c
 *
 */

typedef struct {
	size_t nelts;
	union {
		ngx_str_node_t		*as_strnode;
		ngx_list_t			*as_list;
		ngx_tool_rbtree_t	*as_rbtree;
	} bucket;
} ngx_tool_hashset_elt_t;

typedef struct {
	ngx_tool_hashset_elt_t	**buckets;
	ngx_uint_t				size;
	ngx_pool_t				*pool;
} ngx_tool_hashset_t;

ngx_int_t
ngx_tool_hashset_init(ngx_tool_hashset_t * hashset, ngx_pool_t * pool,
	ngx_uint_t size);

ngx_tool_hashset_t *
ngx_tool_hashset_create(ngx_pool_t * pool, ngx_uint_t size);

ngx_int_t
ngx_tool_hashset_insert(ngx_tool_hashset_t * hashset, ngx_str_t * val);

ngx_str_node_t *
ngx_tool_hashset_lookup(ngx_tool_hashset_t * hashset, ngx_str_t * val);

#endif

