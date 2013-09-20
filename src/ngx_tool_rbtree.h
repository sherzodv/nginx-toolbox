#ifndef _NGX_TOOL_RBTREE_H_INCLUDED_
#define _NGX_TOOL_RBTREE_H_INCLUDED_

#include <ngx_rbtree.h>
#include <ngx_string.h>

/*
 * Intrusive red-black tree
 * convenience wrappers around ngx_rbtree_t.
 *
 * ngx_string.h doest provide insert function but
 * leaves it to user to choose what actually to store.
 *
 * ngx_tool_rbtree_t stores only references to ngx_str_t:
 *
 * 		ngx_str_t str1 = ngx_string("hello");
 * 		node->str = str1;
 *
 * !!!
 * Take into account intrusive nature of ngx_tool_rbtree_t:
 * all pointers to u_char of (ngx_str_t) instances stored
 * in a tree should remain valid during the whole lifetime
 * of the tree.
 *
 */

typedef struct {
	ngx_rbtree_t		rbtree;
	ngx_rbtree_node_t	sentinel;
	ngx_pool_t			*pool;
} ngx_tool_rbtree_t;

void
ngx_tool_rbtree_init(ngx_tool_rbtree_t * rbt, ngx_pool_t * pool);

ngx_tool_rbtree_t *
ngx_tool_rbtree_create(ngx_pool_t * pool);

ngx_int_t
ngx_tool_rbtree_insert(ngx_tool_rbtree_t * rbt, ngx_str_t * val);

ngx_str_node_t *
ngx_tool_rbtree_lookup(ngx_tool_rbtree_t * rbt, ngx_str_t * val);

#endif

