/*
锁有一个很明显的缺点，那就是它只有两种状态：锁定与不锁定。

信号量本质上是一个非负数的整数计数器，它也被用来控制对公共资源的访问。当公共资源增加的时候，调用信号量增加函数sem_post()对其进行增加，当公共资源减少的时候，调用函数sem_wait()来减少信号量。其实，我们是可以把锁当作一个0-1信号量的。

它们是在/usr/include/semaphore.h中进行定义的，信号量的数据结构为sem_t, 本质上，它是一个long型整数


相关函数
	在使用semaphore之前，我们需要先引入头文件#include <semaphore.h>

	初始化信号量： int sem_init(sem_t *sem, int pshared, unsigned int value);
		成功返回0，失败返回-1
		参数
		sem：指向信号量结构的一个指针
		pshared： 不是0的时候，该信号量在进程间共享，否则只能为当前进程的所有线程们共享
		value：信号量的初始值

	信号量减1操作，当sem=0的时候该函数会堵塞 int sem_wait(sem_t *sem);
		成功返回0，失败返回-1
		参数
		sem：指向信号量的一个指针
	信号量加1操作 int sem_post(sem_t *sem);
		参数与返回同上
		销毁信号量 int sem_destroy(sem_t *sem);
		参数与返回同上

因为信号量机制的存在，所以代码在handle_data的时候，如果sem_wait(&sem)时，sem为0,那么代码会堵塞在sem_wait上面，从而避免了在stack中访问错误的index而使整个程序崩溃。
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define THREAD_NUM 4
#define MAXSIZE 10

void one_thread_with_post(void *ptr);
void one_thread_with_wait(void *ptr);
void run_test(void *func);

int stack[MAXSIZE];
sem_t sem;

int main(int argc, char **argv){
	pthread_t threads[THREAD_NUM];
	int thread_num[THREAD_NUM];
	int start_ret[THREAD_NUM];
	void *retval[THREAD_NUM];
	void *func;
	unsigned int index;

	sem_init(&sem, 0, 0);

	for(index = 0; index < THREAD_NUM; index++)
		thread_num[index] = index;

	for(index = 0; index < THREAD_NUM; index++){
		if(index == 0)
			func = (void *)&one_thread_with_post;
		else
			func = (void *)&one_thread_with_wait;
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

	sem_destroy(&sem);
	return 0;
}

//信号量加1的处理，生产者
void one_thread_with_post(void* ptr){
	int index;
	for(index = 0; index < MAXSIZE; index++){
		stack[index] = index;
		sem_post(&sem);
	}
}

//信号量减1的处理，消费者
void one_thread_with_wait(void* ptr){
	int index;
	int thread_num = *(int *)ptr;
	for(index = 0; index < MAXSIZE; index++){
		sem_wait(&sem);
		printf("%d wait: %d\n", thread_num, stack[index]);
	}
}

