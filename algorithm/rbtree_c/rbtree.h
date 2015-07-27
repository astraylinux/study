/*
	red black tree.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void* rb_key_t;
typedef void* rb_data_t;
typedef char rb_bool;

typedef enum rb_color_t{
	RED = 0,
	BLACK = 1,
}rb_color_t;

typedef enum rb_state_t{
	SUCCEED = 0,
	ERROR = 1,
	EXISTED = 2,
}rb_state_t;

typedef struct rb_node_t{
	struct rb_node_t *parent;
	struct rb_node_t *left;
	struct rb_node_t *right;
	rb_key_t key;
	rb_data_t data;
	rb_color_t color;
}rb_node_t;

typedef rb_bool (*rb_compare_func_t)(rb_key_t, rb_key_t);
typedef rb_bool (*rb_free_func_t)(rb_node_t*);
typedef rb_bool (*rb_print_func_t)(rb_node_t*);

typedef struct rb_func_t{
	rb_compare_func_t compare;
	rb_free_func_t free;
	rb_print_func_t print;
}rb_func_t;

rb_state_t rb_insert(rb_key_t, rb_data_t, rb_node_t** root, rb_func_t);
rb_state_t rb_erase(rb_key_t, rb_node_t** root, rb_func_t);
rb_node_t* rb_search(rb_key_t, rb_node_t* root, rb_func_t);
void rb_ergodic(rb_node_t*, rb_func_t);

static rb_state_t rb_insert_rebalance(rb_node_t *node, rb_node_t **root);
static rb_state_t rb_erase_rebalance(rb_node_t *node, rb_node_t *parent, rb_node_t **root);



