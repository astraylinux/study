/*
	test of libupdate
 */
#include "libupdate.h"

#define HTTP_CHAR_IS_UNRESERVED(c) (http_uri_table[(unsigned char)(c)])

const char http_uri_table[256] =
{
    /* 0 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 0, 0, 0, 0, 0, 0,
    /* 64 */
    0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 1, 0,
    /* 128 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    /* 192 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
};

void http_url_encode(const unsigned char *uri, char *ret_str, bool space_as_plus)
{
    char c;
	char buf[4] = {'\0'};

	unsigned index;
	unsigned target_index = 0;
	for(index = 0; uri[index] != '\0'; index++){
		c = uri[index];

        if (HTTP_CHAR_IS_UNRESERVED(c)){
            ret_str[target_index] = c;
			ret_str[target_index + 1] = '\0';
			target_index += 1;
		}else if (c == ' ' && space_as_plus){
			ret_str[target_index] = '+';
			ret_str[target_index + 1] = '\0';
			target_index += 1;
		}else{
            sprintf(buf, "%%%02x", (unsigned char)c);
			strcat(ret_str, buf);
			target_index += 3;
		}
	}
}

bool read_from_file(bson_t *root, char const *data_file, unsigned const limit){
    char buff[BUFF_SIZE];

    FILE *fp = fopen(data_file, "r");
    if(fp == NULL){
        printf("Can't open file.\n");
        exit(1);
    }

    unsigned i = 0;
    char *piece;
    char itoa_buff[20];
    unsigned index;
    unsigned count = 0;
    const char *delim = "\t";

    bson_t item;
    for(; i < limit; i++){
        fgets(buff, BUFF_SIZE, fp);
        index = 0;

        sprintf(itoa_buff, "%d", count);
        bson_append_document_begin(root, itoa_buff, strlen(itoa_buff), &item);

        piece = strtok(buff, delim);
        BSON_APPEND_INT32(&item, "_id", atoi(piece));
        while(piece = strtok(NULL, delim)){
            index += 1;
            if( index == 1)
                BSON_APPEND_UTF8(&item, "name", piece);
        }

        bson_append_document_end(root, &item);
        count += 1;
    }
    return true;
}

int main (int argc, char *argv[]){
	char *password = "$123";
	char * mg_url = "mongodb://test:%s@localhost/?authSource=admin";
	char ret_str[64] = {'\0'};
	char uri[512] = {'\0'};
	http_url_encode(password, ret_str, true);
	snprintf(uri, sizeof(uri), mg_url, ret_str);
	printf("uri: %s\n", uri);

	bson_t root;
	bson_init(&root);

	void *updater = new_mongo_updater();
	char *test_db = "test";
	char *test_collection = "test";
	init_mongo(updater, uri, test_db, test_collection, 3);


	const char * DATA_FILE = "./test.data";
	const unsigned LINE_LIMIT = 8;

	read_from_file(&root, DATA_FILE, LINE_LIMIT);
	char * json_str = bson_as_json(&root, NULL);
	printf("key count in main: %d\n", bson_count_keys(&root));

	update2mongo(updater, json_str);
	update2mongo(updater, json_str);


	destroy_mongo(updater);
	destroy_mongo_updater(updater);

	printf("ok\n");
}

