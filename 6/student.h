#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_FILE_NAME    "student.dat"
#define HASH_FILE_NAME     "student.hsh"
#define STUDENT_RECORD_SIZE     120
#define HASH_RECORD_SIZE    14      //학번키값(10)+학생레코드번호(4)
#define SID_FIELD_SIZE	10	// 학번 필드의 크기

typedef struct _Student
{
	char id[11];		//학번
	char name[21];		//이름
	char addr[31];	    //주소
	char year[2];		//학년
	char dept[20];		//학과
    char phone[16];		//전화번호
	char email[26];		//이메일 주소
} STUDENT;

#endif
