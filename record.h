#ifndef _RECORD_H_
#define _RECORD_H_

//student record size = 250 bytes
typedef struct _Student
{
	char id[11];		//학번: 10 bytes
	char name[31];		//이름: 30 bytes
	char address[71];	//주소: 70 bytes
	char univ[31];		//학교: 30 bytes
	char dept[41];		//학과: 40 bytes
	char others[71];	//그외: 70 bytes
} Student;

#endif
