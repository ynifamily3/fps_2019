#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "record.h"
//필요하면 header file 추가 가능

//
// argv[1]: 레코드 파일 students.dat에 저장되는 전체 레코드의 수
//
int main(int argc, char **argv)
{
    int number_of_records = -1;
    if (argc < 2) exit(1);
    sscanf(argv[1], "%d", &number_of_records);
    
    if (number_of_records < 0) exit(1);
    int fd = creat("students.dat", 0666);
    if (fd < 0) exit(1);

    Student stdnt;

    for (int i = 0; i < number_of_records; i++) {
        sprintf(stdnt.id, "%d", 1234567890); // id를 씀 
        sprintf(stdnt.name, "%s", "JKChoiASDFJKChoiASDFJKChoiASD$"); // name를씀 
        sprintf(stdnt.address, "%s", "Sando-dongSando-dongSando-dongSando-dongSando-dongSando-dongAAAAAAAAA*"); // address를씀
        sprintf(stdnt.univ, "%s", "UnivSoongsil-UnivSoongsil-UniV"); // univ를씀
        sprintf(stdnt.dept, "%s", "Computer ScienceComputer ScienceComputeR"); // dept를씀
        sprintf(stdnt.others, "%s", "123456789012345678901234567890123456789012345678901234567890123456789^"); // others를씀
        write(fd, (void *)&stdnt.id, 10);
        write(fd, (void *)&stdnt.name, 30);
        write(fd, (void *)&stdnt.address, 70);
        write(fd, (void *)&stdnt.univ, 30);
        write(fd, (void *)&stdnt.dept, 40);
        write(fd, (void *)&stdnt.others, 70);
    }
    close(fd);
    // 250바이트 학생 레코드를 students.dat 파일에 하나씩 저장하는 코드를 구현함
    // 학생 레코드의 데이터는 임의로 생성해도 무방함
    
	return 0;
}
