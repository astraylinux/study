/*
	red black tree.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"

rb_bool compare(rb_key_t key_a, rb_key_t key_b){
	int value_a = *(int*)key_a;
	int value_b = *(int*)key_b;
	//printf("compare %d and %d\n", value_a, value_b);
	if(value_a == value_b)
		return 0;
	return (value_a > value_b)? 1: -1;
}

rb_bool rb_free(rb_node_t *node){
	free(node->key);
	if(node->data)
		free(node->data);
	free(node);
}

rb_bool rb_print(rb_node_t* node){
	printf("%d->", *(int*)node->key);
}

int main()
{
    int index, count = 100;
    rb_key_t key;
    rb_node_t *root = NULL;
	rb_node_t *node = NULL;
	rb_func_t rb_func;
	rb_func.compare = &compare;
	rb_func.free = &rb_free;
	rb_func.print = &rb_print;


    srand(time(NULL));
    for (index = 1; index < count; ++index)
    {
		int *tmp = (int*)malloc(sizeof(int));
		int *data = (int*)malloc(sizeof(int));
		//*tmp = rand() % count;
		*tmp = index;
		*data = index;
		key = tmp;

		rb_bool ret = rb_insert(key, data, &root, rb_func);
        if(ret == SUCCEED){
            printf("[index = %d] insert key %d success!\n", index, *(int*)key);
		}else if(ret == EXISTED){
            printf("[index = %d] insert key %d existed!\n", index, *(int*)key);
		}else{
            printf("[index = %d] insert key %d error!\n", index, *(int*)key);
            exit(-1);
        }

        if (node = rb_search(key, root, rb_func))
            printf("[index = %d] search key %d success!\n", index, *(int*)key);
        else{
            printf("[index = %d] search key %d error!\n", index, *(int*)key);
            exit(-1);
        }

        if (!(index % 10)){
            printf("[index = %d] erase key %d!\n", index, *(int*)key);
            if(rb_erase(key, &root, rb_func))
                printf("[index = %d] erase key %d error!\n", index, *(int*)key);
            else
                printf("[index = %d] erase key %d success!\n", index, *(int*)key);
		}
    }

	rb_ergodic(root, rb_func);
    return 0;
}

