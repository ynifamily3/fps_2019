#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/time.h>
//필요하면 header file 추가 가능

//
// argv[1]: 레코드 파일명
// argv[2]: 레코드 파일에 저장되어 있는 전체 레코드의 수
//
int main(int argc, char **argv)
{
	struct timeval start, end;
	if (argc < 3) exit(1);
	int number_of_records;
	gettimeofday(&start, NULL);
	sscanf(argv[2], "%d", &number_of_records);
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) exit(1);

	char buf[250];

	for (int i = 0; i < number_of_records; i++) {
		// * 250
		read(fd, buf, 250);
	}
	gettimeofday(&end, NULL);

	unsigned long times = ((unsigned long)end.tv_sec * 1000000 + (unsigned long)end.tv_usec) - ((unsigned long)start.tv_sec * 1000000 + (unsigned long)start.tv_usec);
	printf("%lu us\n", times);
	close(fd);

	// 표준입력으로 받은 레코드 파일로부터 전체 레코드를 "순차적"으로 읽어들이고, 이때
	// 걸리는 시간을 측정하는 코드 구현함
	// 파일 i/o 시간 비용 측정 실험이므로 파일로부터 읽어 들인 레코드를 굳이 학생 구조체 변수에
    // 저장할 필요는 없음


	return 0;
}
