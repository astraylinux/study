/*
	mongo update modular.
 */
#include "libupdate.h"

//get the accurate time into microsecond.
double gettime(){
    struct timeval st_ns;
    gettimeofday(&st_ns, NULL);
    return (double)st_ns.tv_sec + (double)st_ns.tv_usec/10000000;
}

//print bson as json string
void print_bson(bson_t item){
	char * json_str = bson_as_json(&item, NULL);
	printf("%s\n", json_str);
	bson_free(json_str);
}

//if you needn't mongo output info, call, mongoc_log_set_handler(log_handler)
static void log_handler (mongoc_log_level_t  log_level,\
	const char* log_domain, const char* message, void* user_data)
{
}


//new a mongo_updater
void *new_mongo_updater(){
	mongo_updater *updater = malloc(sizeof(mongo_updater));
	unsigned index;
	for(index = 0; index < THREAD_NUM; index++){
		updater->client_pool[index] = NULL;
		updater->collection_pool[index] = NULL;
	}
	updater->thread_num = 0;
	return (void*) updater;
}

void destroy_mongo_updater(void *v_updater){
	free((mongo_updater *)v_updater);
}

//destroy mongo client
void destroy_mongo(void *v_updater)
{
	mongo_updater *updater = (mongo_updater *)v_updater;
	mongoc_client_t **client_pool = updater->client_pool;
	mongoc_collection_t **collection_pool = updater->collection_pool;
	unsigned index;
	for(index = 0; index < THREAD_NUM ;index++){
		if (collection_pool[index] != NULL){
			mongoc_collection_destroy(collection_pool[index]);
			collection_pool[index] = NULL;
		}
		if (client_pool[index] != NULL){
			mongoc_client_destroy(client_pool[index]);
			client_pool[index] = NULL;
		}
	}
}

//init mongo client, collection and  appoint the thread number.
bool init_mongo(void *v_updater, const char *mongo_url, const char *database,\
	const char *collection, const unsigned thread_num)
{
	mongo_updater *updater = (mongo_updater *)v_updater;
	mongoc_client_t **client_pool = updater->client_pool;
	mongoc_collection_t **collection_pool = updater->collection_pool;
	updater->thread_num = thread_num;

	if(thread_num > THREAD_NUM){
		fprintf(stderr, "Too many threads: %d/%d.\n", thread_num, THREAD_NUM);
		return false;
	}

	unsigned index;
	mongoc_init();
	mongoc_log_set_handler(log_handler, NULL);

	//connect to mongo
	for(index = 0; index < thread_num; index++){
		mongoc_client_t *client = mongoc_client_new(mongo_url);

		if(! client){
			fprintf(stderr, "Connect %d to mongo failed!\n", index);
			destroy_mongo(v_updater);
			return false;
		}

		client_pool[index] = client;
		collection_pool[index] = mongoc_client_get_collection(\
				client_pool[index], database, collection);
	}
	return true;
}



//Main function of update to mongo. Called in every thread.
bool into_mongo(bson_t *root[], mongoc_collection_t *collection){
	bson_iter_t id_iter;
	bson_t *query;
	bson_error_t error;

	unsigned index = 0;
	while(root[index] != NULL){
		bson_t *item = root[index];

		bson_iter_init(&id_iter, item);
		bson_iter_find(&id_iter, "_id");

		uint32_t id = bson_iter_value(&id_iter)->value.v_int32;
		query = BCON_NEW("_id", BCON_INT32(id));

		if(!mongoc_collection_update(collection, MONGOC_UPDATE_UPSERT, query, \
				item, NULL, &error)){
			fprintf(stderr, "%s\n", error.message);
		}
		bson_destroy(query);
		bson_destroy(root[index]);
		root[index] = NULL;
		index++;
	}

	//printf("End index in into_mongo: %d\n", index);
	return true;
}

//import thread with INTO_PACKAGE
//Because of mongo client is thread unsafe object.
//So we must give each thread a unique client or collection.
void into_mongo_thread(void *ptr){
	INTO_PACKAGE *package = (INTO_PACKAGE *)ptr;
	into_mongo(package->rows, package->collection);
}

//split the root bson, so that we can input difference data for each threads.
bool split_bson(bson_t *root, bson_t *data_pool[][MAX_ROW], const unsigned thread_num){
	bson_iter_t iter;
	bson_error_t error;
	bson_t *query;
	if(!bson_iter_init(&iter, root))
		return false;

	unsigned index = 0;
	while(bson_iter_next(&iter)){
		if(BSON_ITER_HOLDS_DOCUMENT(&iter)){
			const bson_value_t *row = bson_iter_value(&iter);
			if(! row->value_type == BSON_TYPE_DOCUMENT){
				fprintf(stderr, "%d value type error.", index);
				continue;
			}

			uint8_t *doc_data = row->value.v_doc.data;
			uint32_t doc_len = row->value.v_doc.data_len;

			data_pool[index % thread_num][index / thread_num] =\
				bson_new_from_data(doc_data, doc_len);

			index ++;
		}
	}
}


//main function to upset data to mongo.
//v_updater: is the mongo_updater pointer, make it as 'void *', when it
//	reference by python or other language, define data type will
//	be convenient.
//json_str: json string.
int update2mongo(void *v_updater, const char *json_str)
{
	//use mongo_updater struct to avoid connect to server every time.
	mongo_updater *updater = (mongo_updater *)v_updater;

	double start = gettime();
	unsigned thread_num = updater->thread_num;
	unsigned index;
	int start_ret;
	uint32_t row_count = 0;

	bson_t *root;
	bson_t *data_pool[THREAD_NUM][MAX_ROW] = {NULL};
	bson_error_t error;
	pthread_t thread_pool[THREAD_NUM];
	INTO_PACKAGE package[THREAD_NUM];

	//covert json string to bson struction
	root = bson_new_from_json(json_str, -1, &error);
	if(! root){
		fprintf(stderr, "init bson from json_str Error: %s\n", error.message);
		goto update2mongo_fail;
	}

	row_count = bson_count_keys(root);
	split_bson(root, data_pool, thread_num);
	for(index = 0; index < thread_num; index++){
		package[index].rows = data_pool[index];
		package[index].collection = updater->collection_pool[index];
	}

	for(index = 0; index < thread_num; index++){
        start_ret = pthread_create(&thread_pool[index], NULL, \
                (void *)into_mongo_thread, (void *)&package[index]);
        if(start_ret != 0)
            fprintf(stderr, "Thread %d create failed. => %d\n", index, start_ret);
	}

    int ret;
    for(index = 0; index < thread_num; index++){
        if((ret = pthread_join(thread_pool[index], NULL)) != 0)
            fprintf(stderr, "join thread %d failed: %d\n", index, ret);
    }

	double spend = gettime() - start;
	printf("C model. spend: %lf speed: %d\n", spend, (int)(row_count / spend));

update2mongo_fail:
	//destroy root bson and thread data array.
	bson_destroy(root);
	unsigned index_q;
	for(index = 0; index < thread_num; index++){
		for(index_q = 0; index_q < MAX_ROW; index_q++){
			if(data_pool[index][index_q] != NULL)
				bson_destroy(data_pool[index][index_q]);
		}
	}
	return -1;
}

