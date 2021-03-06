#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/time.h>
//필요하면 header file 추가 가능


#define SUFFLE_NUM	10000	// 이 값은 마음대로 수정할 수 있음

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// 필요한 함수가 있으면 더 추가할 수 있음

//
// argv[1]: 레코드 파일명
// argv[2]: 레코드 파일에 저장되어 있는 전체 레코드 수
//
int main(int argc, char **argv)
{
	struct timeval start, end;
	int *read_order_list;
	int num_of_records;
	if (argc < 3) exit(1);
	sscanf(argv[2], "%d", &num_of_records);
	gettimeofday(&start, NULL);
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) exit(1);
	read_order_list = (int *)calloc(num_of_records, sizeof(int));
	
	for (int i = 0; i < num_of_records; i++) {
		read_order_list[i] = i;
	}
	// 이 함수를 실행하면 'read_order_list' 배열에 읽어야 할 레코드 번호들이 순서대로 나열되어 저장됨
    //'num_of_records'는 레코드 파일에 저장되어 있는 전체 레코드의 수를 의미함
	GenRecordSequence(read_order_list, num_of_records);

	char buf[250];
	for (int i = 0; i < num_of_records; i++) {
		lseek(fd, read_order_list[i]*250, SEEK_SET);
		read(fd, buf, 250);
	}

	// 'read_order_list'를 이용하여 표준 입력으로 받은 레코드 파일로부터 레코드를 random 하게 읽어들이고,
    // 이때 걸리는 시간을 측정하는 코드 구현함
	gettimeofday(&end, NULL);
	free(read_order_list);
	close(fd);
	unsigned long times = ((unsigned long)end.tv_sec * 1000000 + (unsigned long)end.tv_usec) - ((unsigned long)start.tv_sec * 1000000 + (unsigned long)start.tv_usec);
	printf("%lu us\n", times);
	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
