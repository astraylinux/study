/*
	red black tree.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void* rb_key_t;
typedef void* rb_data_t;
typedef char rb_bool;

typedef enum color_t{
	RED = 0,
	BLACK = 1,
}color_t;

typedef struct rb_node_t{
	struct rb_node_t *parent;
	struct rb_node_t *left;
	struct rb_node_t *right;
	rb_key_t key;
	rb_data_t data;
	color_t color;
}rb_node_t;


typedef rb_bool (*rb_compare_func_t)(rb_key_t, rb_key_t);

rb_node_t* rb_insert(rb_key_t key, rb_data_t data, rb_node_t* root);
rb_node_t* rb_search(rb_key_t key, rb_node_t* root);
rb_node_t* rb_erase(rb_key_t key, rb_node_t* root);

static rb_node_t* rb_insert_rebalance(rb_node_t *node, rb_node_t *root);
static rb_node_t* rb_erase_rebalance(rb_node_t *node, rb_node_t *parent, rb_node_t *root);


static rb_node_t* rb_new_node(rb_key_t key, rb_data_t data){
	rb_node_t *node = (rb_node_t*)malloc(sizeof(struct rb_node_t));

	if(!node){
		fprintf(stderr, "new node malloc error!\n");
		exit(-1);
	}

	node->key = key;
	node->data = data;
	return node;
}

/*
 * 左旋节点成为他的右子节点的左子，并且右子节点的左子成左旋节点的新右子
 */
static void rb_rotate_left(rb_node_t *node, rb_node_t *root){
	rb_node_t* right = node->right; //保存下节点的右子

	//右子的左子成为左旋节点的新右子
	if(node->right = right->left)
		right->left->parent = node;
	right->left = node;

	//用原右节点顶替左旋节点的位置（左旋节点的父节点的处理）
	if(right->parent = node->parent){
		if(node == node->parent->right)
			node->parent->right = right;
		else
			node->parent->left = right;
	}else{
		root = right;
	}

	//左旋节点的父节点为原右节点
	node->parent = right;
}

/*
 *  右旋节点成为他的左子节点的右子，并且左子节点的右子成左旋节点的新左子
 */
static rb_node_t* rb_rotate_right(rb_node_t* node, rb_node_t* root){
	rb_node_t* left = node->left;

	if(node->left = left->right)
		left->right->parent = node;
	left->right = node;

	if(left->parent = node->parent){
		if(node == node->parent->right)
			node->parent->right = left;
		else
			node->parent->left = left;
	}else{
		root = left;
	}

	node->parent = left;
}

/*
 * 查找定位节点
 */
static rb_node_t* rb_locate_util(rb_key_t key, rb_node_t *root, rb_node_t **save,\
		rb_compare_func_t rb_compare_func){
	rb_node_t *node = root;
	rb_node_t *parent = NULL;
	rb_bool ret;

	while(node){
		parent = node;
		ret = rb_compare_func(node->key, key);
		if(ret > 0)
			node = node->left;
		else if (ret < 0)
			node = node->right;
		else
			return node;
	}

	//没有匹配到节点的，返回最后定位的点，这个节点可以成为新节点的父节点
	if(save)
		*save = parent;
	return NULL;
}

/*
 * 插入新节点
 */

