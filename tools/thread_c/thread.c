/*
学习linux下c语言的多线程操作.

创建线程 pthread_create
线程创建函数包含四个变量，分别为： 1. 一个线程变量名，被创建线程的标识 2. 线程的属性指针，缺省为NULL即可 3. 被创建线程的程序代码 4. 程序代码的参数 For example： - pthread_t thrd1; - pthread_attr_t attr; - void thread_function(void argument); - char *some_argument;

pthread_create(&thrd1, NULL, (void *)&thread_function, (void *) &some_argument);

结束线程 pthread_exit
线程结束调用实例：pthread_exit(void *retval); //retval用于存放线程结束的退出状态

线程等待 pthread_join
pthread_create调用成功以后，新线程和老线程谁先执行，谁后执行用户是不知道的，这一块取决与操作系统对线程的调度，如果我们需要等待指定线程结束，需要使用pthread_join函数，这个函数实际上类似与多进程编程中的waitpid。 举个例子，以下假设 A 线程调用 pthread_join 试图去操作B线程，该函数将A线程阻塞，直到B线程退出，当B线程退出以后，A线程会收集B线程的返回码。 该函数包含两个参数：

pthread_t th //th是要等待结束的线程的标识
void **thread_return //指针thread_return指向的位置存放的是终止线程的返回状态。
调用实例：pthread_join(thrd1, NULL);
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define THREAD_NUM 2

void one_thread_execute(void *ptr);

int main(int argc, char **argv){
	pthread_t threads[THREAD_NUM];
	int start_ret[THREAD_NUM];
	void *retval[THREAD_NUM];

	unsigned int index = 0;
	for(index = 0; index < THREAD_NUM; index++){
		start_ret[THREAD_NUM] = pthread_create(&threads[index], NULL, \
				(void *)&one_thread_execute, (void *)&index);
		if(start_ret != 0)
			printf("Thread %d create failed. => %d\n", index, start_ret[index]);
		else
			printf("Thread %d create succeed.\n", index);
		usleep(0.1 * 1000);
	}

	pthread_join(threads[THREAD_NUM-1], &retval[THREAD_NUM-1]);
	printf("thread -1 retval: %d", *(int*)retval[THREAD_NUM-1]);
}


void one_thread_execute(void *ptr){
	int num = *(int *)ptr;
	unsigned int index = 0;
	for(index = 0; index < 5; index++){
		usleep(1 * 1000);
		printf("Thread %d report.\n", num);
	}
}
