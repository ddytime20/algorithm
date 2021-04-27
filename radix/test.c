#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "radix_tree.h"


#define RADIX_TREE_TEST_NUM 200U

static void get_rand(unsigned long *pdata, int num)
{
	int i;
	srand((unsigned int)time(NULL));
	for (i=0;i<num;i++) {
		*(pdata+i) = ((unsigned long)rand());
		printf("rand i=%lu\n",*(pdata+i));
	}
}

int main(int argc, char *argv[])
{
	void *pData;
	unsigned long data[RADIX_TREE_TEST_NUM];
	int i;
	struct radix_tree_root root ={
		.height = 0,
		.rnode = NULL
	};
	
	get_rand(&data[0], RADIX_TREE_TEST_NUM);
	
	for (i=0;i<RADIX_TREE_TEST_NUM;i++) {
		radix_tree_insert(&root, data[i], &data[i]);
	}
	
	for (i=0;i<RADIX_TREE_TEST_NUM;i++) {
		pData = radix_tree_lookup(&root, data[i]);
		if (pData)
			printf("lookup data %lu\n",*(unsigned long *)pData);
		else
			printf("get null\n");
	}
	
	for (i=0;i<RADIX_TREE_TEST_NUM;i++) {
		pData = radix_tree_delete(&root, data[i]);
		if (pData)
			printf("delete data %lu\n",*(unsigned long *)pData);
		else
			printf("get null\n");
	}
	
	return 0;
}