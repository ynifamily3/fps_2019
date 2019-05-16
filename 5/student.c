#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "student.h"

//
// 학생 레코드 파일에 레코드를 저장하기 전에 구조체 Student에 저장되어 있는 데이터를 레코드 형태로 만든다.
// 
void pack(char *recordbuf, const Student *s)
{
	const char *del = "|";
	strncpy(recordbuf, s->id, sizeof(s->id) - 1);
	strcat(recordbuf, del);
	strncat(recordbuf, s->name, sizeof(s->name) - 1);
	strcat(recordbuf, del);
	strncat(recordbuf, s->dept, sizeof(s->dept) - 1);
	strcat(recordbuf, del);
	strncat(recordbuf, s->year, sizeof(s->year) - 1);
	strcat(recordbuf, del);
	strncat(recordbuf, s->addr, sizeof(s->addr) - 1);
	strcat(recordbuf, del);
	strncat(recordbuf, s->phone, sizeof(s->phone) - 1);
	strcat(recordbuf, del);
	strncat(recordbuf, s->email, sizeof(s->email) - 1);
	strcat(recordbuf, del);
}


// 
// 학생 레코드 파일로부터 레코드를 읽어 온 후 구조체 Student로 변환한다.
//
void unpack(const char *recordbuf, Student *s)
{
	int i;
	char *ptr1, *ptr2;
	ptr1 = ptr2 = (char *)recordbuf;
	for (i = 0; i < 7; i++) {
		while (1) {
			if (*ptr2 == '|') {
				*ptr2 = '\0';
				switch (i) {
					case 0:
					strcpy(s->id, ptr1);
					break;
					case 1:
					strcpy(s->name, ptr1);
					break;
					case 2:
					strcpy(s->dept, ptr1);
					break;
					case 3:
					strcpy(s->year, ptr1);
					break;
					case 4:
					strcpy(s->addr, ptr1);
					break;
					case 5:
					strcpy(s->phone, ptr1);
					break;
					case 6:
					strcpy(s->email, ptr1);
					break;
				}
				ptr1 = ++ptr2;
				break;
			}
			++ptr2;
		}
	}
}

//
// 학생 레코드 파일로부터  레코드 번호에 해당하는 레코드를 읽어서 레코드 버퍼에 저장한다.
//
void readRecord(FILE *fp, char *recordbuf, int rn);

//
//알고리즘: 저장
//학생 레코드 파일에서 삭제 레코드의 존재 여부를 검사한 후 삭제 레코드가 존재하면 이 공간에
//새로운 레코드를 저장하며, 만약 삭제 레코드가 존재하지 않거나 조건에 부합하는 삭제 레코드가
//존재하지 않으면  파일의 맨마지막에 저장한다.
//
void add(FILE *fp, const Student *s)
{

}

//
//알고리즘: 검색
//학생 레코드 파일에서 sequential search 기법으로 주어진 학번 키값고 일치하는 레코드를 찾는다.
//출력은 만드시 주어진 print 함수를 사용한다.
//검색 레코드가 존재하면 레코드 번호 rn을, 그렇지 않으면 -1을 리턴한다.
//
int search(FILE *fp, const char *keyval);

//
//알고리즘: 삭제
//학생 파일에서 주어진 학번 키값과 일치하는 레코드를 찾은 후 해당 레코드를 삭제 처리한다.
//
void delete(FILE *fp, const char *keyval);

void printRecord(const Student *s, int n);


int main(int argc, char *argv[])
{
	FILE *fp;  // 학생 레코드 파일의 파일 포인터
	Student student_data;
	char recordbuf[MAX_RECORD_SIZE + 1];
	char *usage = "Usage :\n%s -a <학번> <이름> <전공> <학년> <주소> <연락처> <이메일>\n%s -s <학번>\n%s -d <학번>\n";
	if (argc != 3 && argc != 9) {
		fprintf(stderr, usage, argv[0], argv[0], argv[0]);
		return 1;
	}
	strncpy(student_data.id, (const char *)argv[2], (size_t)(sizeof(student_data.id) - 1));
	strncpy(student_data.name, (const char *)argv[3], (size_t)(sizeof(student_data.name) - 1));
	strncpy(student_data.dept, (const char *)argv[4], (size_t)(sizeof(student_data.dept) - 1));
	strncpy(student_data.year, (const char *)argv[5], (size_t)(sizeof(student_data.year) - 1));
	strncpy(student_data.addr, (const char *)argv[6], (size_t)(sizeof(student_data.addr) - 1));
	strncpy(student_data.phone, (const char *)argv[7], (size_t)(sizeof(student_data.phone) - 1));
	strncpy(student_data.email, (const char *)argv[8], (size_t)(sizeof(student_data.email) - 1));

	// 저장	“20101234” “Gildong Hong” “Computer Science” “3” “Dongjak-Gu,Seoul” “010-828-0567” “gdhong@ssu.ac.kr”
	if (argc == 9 && argv[1][0] == '-' && argv[1][1] == 'a' && argv[1][2] =='\0') {
		// 두 파일 중 하나라도 없다면 둘 다 새로 만듦
		if (access(RECORD_FILE_NAME, R_OK) || access(INDEX_FILE_NAME, R_OK)) {
			// printf("새로 만드는 상황\n");
			fp = fopen(INDEX_FILE_NAME, "wb+");
			short int data = 1; // 레코드 수 기록 (2b)
			fwrite((const void *)&data, sizeof(short int), (size_t)1, fp);
			data = 0; // 레코드 인덱스 기록 (최초 0)
			fwrite((const void *)&data, sizeof(short int), (size_t)1, fp);
			fclose(fp);
			fp = fopen(RECORD_FILE_NAME, "wb+");
			pack(recordbuf, &student_data);
			data = -1;
			fwrite((void *)&data, sizeof(short int), (size_t)1, fp); // header -1로 설정
			fwrite((void *)recordbuf, strlen(recordbuf), 1, fp);
			fclose(fp);
		} else {
			// 둘 다 파일이 있음.
			// 헤더를 검사하여 헤더가 -1 이면 그냥 뒤에 추가하는 액션만으로 충분하고, idx파일을 갱신하고, dat 파일에 append한다.
			short int data;
			short int flastoffset;
			fp = fopen(RECORD_FILE_NAME, "rb+");
			fread((void *)&data, sizeof(short int), (size_t)1, fp);
			if (data == -1) {
				fseek(fp, 0, SEEK_END);
				flastoffset = (short int)ftell(fp);
				pack(recordbuf, &student_data);
				fwrite((const void *)recordbuf, strlen(recordbuf), 1, fp); // append data
				fclose(fp);
				fp = fopen(INDEX_FILE_NAME, "rb+");
				fread((void *)&data, sizeof(short int), (size_t)1, fp);
				rewind(fp);
				++data;
				fwrite((const void *)&data, sizeof(short int), (size_t)1, fp);
				fseek(fp, 0, SEEK_END);
				fwrite((const void *)&flastoffset, sizeof(short int), (size_t)1, fp);
				fclose(fp);
			} else {
				printf("삭제된 레코드 있음\n");
			}
		}
		return 0;
	}
	else if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 'd' && argv[1][2] =='\0') {

		return 0;
	}
	else if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 's' && argv[1][2] =='\0') {

		return 0;
	}
	else {
		fprintf(stderr, usage, argv[0], argv[0], argv[0]);
		return 1;
	}
	// 저장 argc 9개
	/*
	a.out –a 
	*/
	// 삭제 argc 3개
	// 검색 argc 3개
	

	return 0;
}

void printRecord(const Student *s, int n)
{
	int i;

	for(i=0; i<n; i++)
	{
		printf("%s|%s|%s|%s|%s|%s|%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}