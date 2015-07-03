#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bson.h>


void print_bson(bson_t item, char *msg){
	char * json_str = bson_as_json(&item, NULL);
	printf("%s\n%s\n", json_str, msg);
	bson_free(json_str);
}

int main (int argc, char *argv[])
{
	//start, 创建bson, 和插入子结构.
	bson_t root = BSON_INITIALIZER;
	bson_t item = BSON_INITIALIZER;

	bson_append_document_begin(&root, "item", 4, &item);
	BSON_APPEND_UTF8(&item, "name", "中国");
	bson_append_document_end(&root, &item);

	print_bson(root, "");
	bson_destroy(&root);
	bson_destroy(&item);

	//从字符串转成bson结构
	char* json_string = "[{\"_id\":123,\"title\":\"start\"}, \
				{\"_id\":124, \"title\":\"second\"}]";
	bson_t *bpoint;
	bson_error_t error;
	bpoint = bson_new_from_json(json_string, -1, &error);
	if(bpoint != NULL){
		print_bson(*bpoint, "");
		bson_destroy(bpoint);
	}else{
		printf("Error: %s\n", error.message);
	}
}

