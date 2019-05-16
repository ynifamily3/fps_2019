#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_FILE_NAME    "student.dat"
#define INDEX_FILE_NAME     "student.idx"
#define MAX_RECORD_SIZE     127 //including 7 delimeters

typedef struct _Student
{
	char id[11];		//학번 10
	char name[21];		//이름 20
	char dept[20];		//학과 19
	char year[2];		//학년 1
	char addr[31];	    //주소 30
    char phone[16];		//전화번호 15
	char email[26];		//이메일 주소 25
} Student;

// ID, NAME,   DEPT, YEAR, ADDR, PHONE, EMAIL 순서대로 주어져야 하며, 필드값은 영문자와 숫자로만 구성되어 있다고 가정한다. 이 명령어를 수행하고 난 후의 출력은 없다.
#endif
