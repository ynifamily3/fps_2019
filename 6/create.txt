#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "student.h"

void putRC(char *chr, int times)
{
    char *ptr = chr + strlen(chr);
    while (times--) {
        if (random() % 3 == 0)
            *ptr = 'A' + (random() % 26);
        else {
            if (random() % 2)
                *ptr = 'a' + (random() % 26);
            else
                *ptr = '0' + (random() % 10);
        }
        ptr++;
    }
}


// student.dat 파일을 만든다.

int main(void)
{
    int fd, i, cnt;
    char packbuf[STUDENT_RECORD_SIZE] = {0, };
    char random_letter = 'A' + (random() % 26);
    char *ptr = packbuf;
    srand(time(NULL));
    fd = open(RECORD_FILE_NAME, O_CREAT | O_WRONLY | O_TRUNC);
    setbuf(stdout, NULL);
    printf("만들 개수 입력 : ");
    scanf("%d", &cnt);
    for (i = 0; i < cnt; ++i) {
        printf("학번 입력 : ");
        scanf("%10s", ptr);
        ptr = &packbuf[10];
        putRC(ptr, 20);
        ptr = &packbuf[30];
        putRC(ptr, 30);
        ptr = &packbuf[60];
        putRC(ptr, 1);
        ptr = &packbuf[61];
        putRC(ptr, 19);
        ptr = &packbuf[80];
        putRC(ptr, 15);
        ptr = &packbuf[95];
        putRC(ptr, 25);
        ptr = &packbuf[120];
        write(fd, packbuf, STUDENT_RECORD_SIZE);
        memset(packbuf, 0, STUDENT_RECORD_SIZE);
        ptr = packbuf;
    }
    close(fd);
    
    return 0;
}
