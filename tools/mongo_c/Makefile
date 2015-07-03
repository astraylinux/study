#libs
CUR_PATH = $(shell pwd)
MONGO_LIBS = $(shell pkg-config --cflags --libs libmongoc-1.0)
BSON_LIBS = $(shell pkg-config --cflags --libs libbson-1.0)

#include
INCLUDE = /usr/include/:/usr/local/include/libmongoc-1.0/:/usr/local/include/libbson-1.0/

#compiler
CC = gcc

all: libupdate.c bson.c libupdate.h test.c
	$(CC) -fPIC -shared libupdate.c libupdate.h -o libupdate.so -I \
		$(INCLUDE) $(MONGO_LIBS) -lpthread

	$(CC) bson.c -o bson.bin -I $(INCLUDE) $(BSON_LIBS)

	$(CC) test.c -o test.bin -I $(INCLUDE) $(MONGO_LIBS)\
		-L$(CUR_PATH) -lupdate

clean:
	rm libupdate.so bson.bin test.bin
