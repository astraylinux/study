#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>

#include <bcon.h>
#include <bson.h>
#include <mongoc.h>

#define THREAD_NUM 6
#define BUFF_SIZE 16*1024
#define MAX_ROW 64*1024

//This struction include data array and mongo client.collection for
//each thread.(use the struct pointer.)
typedef struct {
	bson_t **rows;
	mongoc_collection_t *collection;
}INTO_PACKAGE;

//Save this struction to avoid connect to server every time.
typedef struct _updater{
	mongoc_client_t *client_pool[THREAD_NUM];
	mongoc_collection_t *collection_pool[THREAD_NUM];
	unsigned thread_num;
}mongo_updater;

//get the accurate time into microsecond.
double gettime();

//print bson as json string
void print_bson(bson_t item);

//new a mongo_updater
void *new_mongo_updater();

void destroy_mongo_updater(void *v_updater);

//destroy mongo client
void destroy_mongo(void *v_updater);

//init mongo client, collection and  appoint the thread number.
bool init_mongo(void *v_updater, const char *mongo_url, const char *database,\
	const char *collection, const unsigned thread_num);

//main function to upset data to mongo.
//v_updater: is the mongo_updater pointer, make it as 'void *', when it
//	reference by python or other language, define data type will
//	be convenient.
//json_str: json string.
int update2mongo(void *v_updater, const char *json_str);
