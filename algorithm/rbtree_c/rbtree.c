/*
	red black tree.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"

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

static void rb_rotate_left(rb_node_t *node, rb_node_t **root){
/*左旋节点成为他的右子节点的左子，并且右子节点的左子成左旋节点的新右子
 */
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
		*root = right;
	}

	//左旋节点的父节点为原右节点
	node->parent = right;
}


static void rb_rotate_right(rb_node_t *node, rb_node_t **root){
/*右旋节点成为他的左子节点的右子，并且左子节点的右子成左旋节点的新左子
 */
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
		*root = left;
	}

	node->parent = left;
}


static rb_node_t* rb_locate_util(rb_key_t key, rb_node_t *root, rb_node_t **save,\
		rb_func_t rb_func){
/*查找定位节点
 */
	rb_node_t *node = root;
	rb_node_t *parent = NULL;
	rb_bool ret;

	while(node){
		parent = node;
		ret = rb_func.compare(node->key, key);
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


rb_node_t* rb_search(rb_key_t key, rb_node_t* root, rb_func_t rb_func){
/* 不保存父节点，单纯的返回
 */
	return rb_locate_util(key, root, NULL, rb_func);
}


rb_state_t rb_insert(rb_key_t key, rb_data_t data, rb_node_t **root, \
		rb_func_t rb_func){
/* 插入新节点，并调用调整函数，使树保持性质
 */
	rb_node_t *parent = NULL;
	rb_node_t *node;

	if(node = rb_locate_util(key, *root, &parent, rb_func))
		return EXISTED;

	node = rb_new_node(key, data);
	//从locate返回的父节点，直接赋值给他
	node->parent = parent;
	node->left = node->right = NULL;
	node->color = RED;

	//如果有父节点的话，令新节点成为父节点的子节点
	if(parent){
		rb_bool ret = rb_func.compare(parent->key, key);
		if(ret > 0)
			parent->left = node;
		else
			parent->right = node;
	}else{
		*root = node;
	}

	return rb_insert_rebalance(node, root);
}


rb_state_t rb_erase(rb_key_t key, rb_node_t **root, rb_func_t rb_func){
/** 删除节点, 如果删除的是黑色节点，刚需要重新调整树
 */
	rb_node_t *child;
	rb_node_t *parent;
	rb_node_t *old;
	rb_node_t *left;
	rb_node_t *node;
	rb_color_t color;

	//找到节点，没有的话返回错误
	if( !(node = rb_locate_util(key, *root, NULL, rb_func))){
		fprintf(stderr, "key is not exist!\n");
		return ERROR;
	}

	old = node;

	if( node->left && node->right ){  //两个子节点都在的情况
		node = node->right;
		//遍历取右子树的最小值
		while((left = node->left) != NULL)
			node = left;

		child = node->right;
		parent = node->parent;
		color = node->color;

		//左子已经确认不存在了，看右子还在不在
		//如果右子存在的话，右子要代替这个节点的位置(右子大于节点)
		if(child)
			child->parent = parent;

		//处理替代节点的父节点(接上它的子节点)
		if(parent){
			if(parent->left == node)
				parent->left = child;
			else
				parent->right = child;
		}else{
			*root =  child;
		}

		if(node->parent == old)
			parent = node;

		//执行替换
		node->parent = old->parent;
		node->color = old->color;
		node->right = old->right;
		node->left = old->left;

		if(old->parent){
			if(old->parent->left == old)
				old->parent->left = node;
			else
				old->parent->right = node;
		}else{
			*root = node;
		}

		//由于选的是右边最小值，所原节点的左子一定在，不用判断
		//而右子有可能是空的(右子只有一个，用来替换了)
		old->left->parent = node;
		if(old->right)
			old->right->parent = node;
	}else{
		//非两子都在的情况,唯一子节点直接代替，没有则为空
		child = node->left? node->left: node->right;

		parent = node->parent;
		color = node->color;

		if(child)
			child->parent = parent;

		if(parent){
			if(parent->left == node)
				parent->left = child;
			else
				parent->right = child;
		}else{
			*root = child;
		}
	}

	rb_func.free(old);
	//删除节点为红色时，性质不变，黑色则要调整
	if(color == BLACK)
		return rb_erase_rebalance(child, parent, root);

	return SUCCEED;
}

static rb_state_t rb_insert_rebalance(rb_node_t *node, rb_node_t **root){
/* 插入节点后，重新平衡红黑树.有三种情况需要处理
 * 情况1: 父节点为红色, 叔叔节点为红色
 * 情况2: 父节点是红（连续两红）,叔叔节点是黑，且当前节点是其父的右子
 * 情况3: 叔叔节点是黑色，节点为其父的左子
 */
	rb_node_t *parent;
	rb_node_t *gparent;
	rb_node_t *uncle;
	rb_node_t *tmp;

	while((parent = node->parent) && parent->color == RED){
		gparent = parent->parent;

		if(parent == gparent->left){
			//父节点为祖节点的左子
			uncle = gparent->right;

			if(uncle && uncle->color == RED){
				//情况1： 父节点为红色, 叔叔节点为红色
				//由于新节点也是红色,性质五不符(红节点的子节点只能是黑)
				uncle->color = BLACK;
				parent->color = BLACK; //去掉连续红
				gparent->color = RED;
				//这时如果祖节点的父节点是红色，循环过后进入情况2，否则就完成了
				node = gparent;
			}else{
				//情况2: 父节点是红（连续两红）,叔叔节点是黑，且当前节点是其父的右子
				if(parent->right == node){
					rb_rotate_left(parent, root);
					//左旋位置关系变了，定位的指针要换
					tmp = parent;
					parent = node;
					node = tmp;
				}
				//情况2处理完后，两个红色节点的位置互换了，但还是两红
				//情况3：叔叔节点是黑色，节点为其父的左子
				parent->color = BLACK;
				gparent->color = RED;
				rb_rotate_right(gparent, root);
			}
		}else{
			//父节点为祖节点的右子的情况, 情况与上面相同，左右对调
			uncle = gparent->left;
			if(uncle && uncle->color == RED){
				uncle->color = BLACK;
				parent->color = BLACK;
				gparent->color = RED;
				node = gparent;
			}else{
				if(parent->left == node){
					rb_rotate_right(parent, root);
					tmp = parent;
					parent = node;
					node = tmp;
				}
				parent->color = BLACK;
				gparent->color = RED;
				rb_rotate_left(gparent, root);
			}
		}
	}
	(*root)->color = BLACK;
	return SUCCEED;
}


static rb_state_t rb_erase_rebalance(rb_node_t *node, rb_node_t *parent, rb_node_t **root){
/* 删除节点后，调整树，使其保持原有的性质
 * 删除后如果替换节点是红，直接染黑就可以了
 * 如果替换节点是黑色，且删除的是根节点，就什么都不做
 * 其他共有四种情况需要调整，当前节点是指删除节点后，子节点接上来后，原位置上的节点
 * 情况1: 当前节点的兄弟节点是红色（且父节点和兄弟节点的子节点都为黑）
 * 情况2: 当前节点的兄弟节点是黑色，兄弟节点的两个子节点都为黑色
 * 情况3: 当前节点的兄弟节点是黑色，兄弟节点的左子是红色，右子是黑色
 * 情况4: 当前节点的兄弟节点是黑色，兄弟节点的右子为红色（左子任意）
 */
	rb_node_t *brother;
	rb_node_t *b_left;   //兄弟节点的左子
	rb_node_t *b_right;	//兄弟节点的右子

	while((!node || node->color == BLACK) && node != *root){
		if(parent->left == node){
			//删除节点是其父节点的左子的情况
			brother = parent->right;
			if(brother->color == RED){
				//情况1：删除节点的兄弟是红色
				brother->color = BLACK;
				parent->color = RED;
				rb_rotate_left(parent, root);
				brother = parent->right; //经常左旋，删除节点的兄弟换了
			}
			if((!brother->left || brother->left->color == BLACK) && \
				(!brother->right || brother->right->color == BLACK)){
				//情况2：删除节点的兄弟是黑色，且兄弟节点的两个子节点都是黑色
				//由于都是黑色，要在兄弟节点和兄弟节点的子节点上去掉一层黑色
				brother->color = RED;
				node = parent;
				parent = node->parent; //parent做为新节点
			}else{
				//情况3：当前节点的兄弟节点是黑色，且兄弟节点的左子是红，右子是黑
				if(!brother->right || brother->right->color == BLACK){
					if(b_left = brother->left)
						b_left->color = BLACK;
					brother->color = RED;
					rb_rotate_right(brother, root);
					brother = parent->right; //情况三处理完进入情况4
				}
				//情况4：当前节点的兄弟节点是黑色
				brother->color = parent->color;
				parent->color = BLACK;
				if(brother->right)
					brother->right->color = BLACK;
				rb_rotate_left(parent, root);
				node = *root;
				break;
			}
		}else{
			//删除节点是其父节点的右子的情况，处理与上面类似
			brother = parent->left;
			if(brother->color == RED){
				brother->color = BLACK;
				parent->color = RED;
				rb_rotate_right(parent, root);
				brother = parent->left;
			}
			if((!brother->left || brother->left->color == BLACK) &&
				(!brother->right || brother->right->color == BLACK)){
				brother->color = RED;
				node = parent;
				parent = node->parent;
			}else{
				if(!brother->left || brother->left->color == BLACK){
					if(b_right = brother->right)
						b_right->color = BLACK;
					brother->color = RED;
					rb_rotate_left(brother, root);
					brother = parent->left;
				}
				brother->color = parent->color;
				parent->color = BLACK;
				if(brother->left)
					brother->left->color = BLACK;
				rb_rotate_right(parent, root);
				node = *root;
				break;
			}
		}
	}

	if(node)
		node->color = BLACK; //根节点必需是黑的
	return SUCCEED;
}

void rb_ergodic(rb_node_t* node, rb_func_t rb_func){
/* 遍历红黑树，从小到大
 */
	if(!node->left && !node->right){
		rb_func.print(node);
		return;
	}

	if(node->left)
		rb_ergodic(node->left, rb_func);

	rb_func.print(node);

	if(node->right)
		rb_ergodic(node->right, rb_func);
}

