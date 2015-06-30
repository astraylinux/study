/*
在主线程中初始化锁为解锁状态
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);

在编译时初始化锁为解锁状态
锁初始化 pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

访问对象时的加锁操作与解锁操作
加锁 pthread_mutex_lock(&mutex)
释放锁 pthread_mutex_unlock(&mutex)))

锁保护的并不是我们的共享变量（或者说是共享内存），对于共享的内存而言，用户是无法直接对其保护的，因为那是物理内存，无法阻止其他程序的代码访问。事实上，锁之所以对关键区域进行了保护，在本例中，是因为所有线程都遵循了一个规则，那就是在进入关键区域钱加同一把锁，在退出关键区域钱释放同一把锁

我们从上述运行结果中可以看到，加锁是会带来额外的开销的，加锁的代码其运行速度，明显比不加锁的要慢一些，所以，在使用锁的时候，要合理，在不需要对关键区域进行保护的场景下，我们便不要画蛇添足，为其加锁了
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define THREAD_NUM 4

void one_thread_not_mutex(void *ptr);
void one_thread_with_mutex(void *ptr);
void run_test(void *func);

int share_value = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char **argv){
	run_test((void *)&one_thread_not_mutex);
	printf("=====================================\n");
	share_value = 0;
	run_test((void *)&one_thread_with_mutex);
}

//加锁的情况下，每次跑完线程，share_value的值都是一样的
void one_thread_with_mutex(void *ptr){
	int thread_num = *(int *)ptr;
	long i, tmp;
	for(i = 0; i < 10000; i++){
		//加锁
		if(pthread_mutex_lock(&mutex) != 0){
			perror("pthread_mutex_lock\n");
			exit(EXIT_FAILURE);
		}
		tmp = share_value;
		tmp = tmp + 1;
		share_value = tmp;

		//解锁
		if(pthread_mutex_unlock(&mutex) != 0){
			perror("pthread_mutex_unlock\n");
			exit(EXIT_FAILURE);
		}
	}
}

//没有加锁的情况下，每次跑完线程，share_value的值可能都不同
void one_thread_not_mutex(void *ptr){
	long i, tmp;
	for(i = 0; i <= 10000; i++){
		tmp = share_value;
		tmp = tmp + 1;
		share_value = tmp;
	}
}

//创建线程，并等待结束
void run_test(void *func){
	pthread_t threads[THREAD_NUM];
	int thread_num[THREAD_NUM];
	int start_ret[THREAD_NUM];
	void *retval[THREAD_NUM];
	unsigned int index;

	for(index = 0; index < THREAD_NUM; index++)
		thread_num[index] = index;

	for(index = 0; index < THREAD_NUM; index++){
		start_ret[index] = pthread_create(&threads[index], NULL, \
				(void *)func, (void *)&thread_num[index]);
		if(start_ret[index] != 0)
			printf("Thread %d create failed. => %d\n", index, start_ret[index]);
		else
			printf("Thread %d create succeed.\n", index);
	}

	int ret;
	for(index = 0; index < THREAD_NUM; index++){
		if((ret = pthread_join(threads[index], &retval[index])) == 0)
			printf("thread %d retval: %d\n", index, *(int*)&retval[index]);
		else
			printf("join thread %d failed: %d\n", index, ret);
	}
	printf("share value: %d\n", share_value);
}
