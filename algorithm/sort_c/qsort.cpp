#include<stdio.h>
#include<iostream>
using namespace std;

// 快速排序
int qsort(int *array, int start, int end){
	if(start >= end)
		return 0;
	int i = start;
	int j = end;
	int k = array[start];
	while(i < j){
		while(i < j && array[j] >= k)j--;
		array[i] = array[j];
		while(i < j && array[i] <= k)i++;
		array[j] = array[i];
	}
	array[i] = k;
	qsort(array, start, i-1);
	qsort(array, j+1, end);
	return 0;
}

int print_array(int* array, int start, int end){
	for(int i=start;i<=end;i++){
		cout<<array[i]<<", ";
	}
	cout<<endl;
}

int main(int argc, char** argv){
	int array[] = {16, 26, 35, 68, 94, 31, 64, 36, 87, 17, 21, 53, 9, 125,\
		136, 14, 18, 39, 77, 88, 99, 65, 24, 30};
	qsort(array, 0, sizeof(array)/sizeof(int)-1);
	print_array(array, 0, sizeof(array)/sizeof(int)-1);
}
