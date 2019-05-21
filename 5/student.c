#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "student.h"

FILE *idx_fp;
char *usage = "Usage :\n%s -a <학번> <이름> <전공> <학년> <주소> <연락처> <이메일>\n%s -s <학번>\n%s -d <학번>\n";

//
// 학생 레코드 파일에 레코드를 저장하기 전에 구조체 STUDENT에 저장되어 있는 데이터를 레코드 형태로 만든다.
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
// 학생 레코드 파일로부터 레코드를 읽어 온 후 구조체 STUDENT로 변환한다.
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
void readRecord(FILE *fp, char *recordbuf, int rn)
{
	// rn은 byte offset
	// idx_fp 로부터 rn으로부터의 위치를 읽어서, 그 바이트 오프셋으로부터 길이를 구해 
	short int records = get_idx_number_of_record();
	short int length; // diff with 
	if (rn >= records) {
		fprintf(stderr, "Invalid rn : %d\trecords : %hd\n", rn, length);
		return;
	}
}

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
//학생 레코드 파일에서 sequential search 기법으로 주어진 학번 키값과 일치하는 레코드를 찾는다.
//출력은 만드시 주어진 print 함수를 사용한다.
//검색 레코드가 존재하면 레코드 번호 rn을, 그렇지 않으면 -1을 리턴한다.
//
int search(FILE *fp, const char *keyval)
{
	short rn = -1;

	return rn;
}

//
//알고리즘: 삭제
//학생 파일에서 주어진 학번 키값과 일치하는 레코드를 찾은 후 해당 레코드를 삭제 처리한다.
//
void delete(FILE *fp, const char *keyval)
{

}

short int get_idx_number_of_record(void)
{
	short int ret;
	rewind(idx_fp);
	fread((void *)&ret, sizeof(short int), (size_t)1, idx_fp);
	return ret;
}

void set_idx_number_of_record(short int val)
{
	rewind(idx_fp);
	fwrite((const void *)&val, sizeof(short int), (size_t)1, idx_fp);
}

// idx에 새로운 값을 append한다.
void append_idx_new_byte_offset(short int new_byte_offset)
{
	set_idx_number_of_record(get_idx_number_of_record() + 1); // increase 레코드 개수
	fseek(idx_fp, 0, SEEK_END);
	fwrite((const void *)&new_byte_offset, sizeof(short int), (size_t)1, idx_fp);
}

// record File의 header 값을 지정한다. header는 최근에 삭제된 것.
void set_header_idx(FILE *fp, short int header_val)
{
	rewind(fp);
	fwrite((const void *)&header_val, sizeof(short int), (size_t)1, fp);	
}

short int get_header_idx(FILE *fp)
{
	short int ret;
	rewind(fp);
	fread((void *)&ret, sizeof(short int), (size_t)1, fp);
	return ret;
}

void set_student_struct_by_args(Student *student_data, const char **argv)
{
	strncpy(student_data->id, (const char *)argv[2], (size_t)(sizeof(student_data->id) - 1));
	strncpy(student_data->name, (const char *)argv[3], (size_t)(sizeof(student_data->name) - 1));
	strncpy(student_data->dept, (const char *)argv[4], (size_t)(sizeof(student_data->dept) - 1));
	strncpy(student_data->year, (const char *)argv[5], (size_t)(sizeof(student_data->year) - 1));
	strncpy(student_data->addr, (const char *)argv[6], (size_t)(sizeof(student_data->addr) - 1));
	strncpy(student_data->phone, (const char *)argv[7], (size_t)(sizeof(student_data->phone) - 1));
	strncpy(student_data->email, (const char *)argv[8], (size_t)(sizeof(student_data->email) - 1));
}


int main(int argc, char *argv[])
{
	FILE *fp;  // 학생 레코드 파일의 파일 포인터
	Student student_data;
	char record_buf[MAX_RECORD_SIZE + 1];

	// student.idx와 student.dat 두 파일 중 하나라도 없으면 모두 새로 만든다.
	if (access(RECORD_FILE_NAME, F_OK) || access(INDEX_FILE_NAME, F_OK)) {
		idx_fp = fopen(INDEX_FILE_NAME, "wb+");
		fp = fopen(RECORD_FILE_NAME, "wb+");
		set_idx_number_of_record(0); // 처음엔 0개의 레코드가 있으므로
		set_header_idx(fp, -1); // 최근 삭제된 위치. 지금은 삭제된 레코드가 없으므로 -1
		// header -1로 설정
	} else {
		idx_fp = fopen(INDEX_FILE_NAME, "rb+");
		fp = fopen(RECORD_FILE_NAME, "rb+");
	}

	if (argc != 3 && argc != 9) {
		fprintf(stderr, usage, argv[0], argv[0], argv[0]);
		return 1;
	}

	if (argc == 9 && strcmp("-a", argv[1]) == 0) {
		// arg로부터 얻어온다.
		set_student_struct_by_args(&student_data, argv);
		// 삭제 레코드가 존재하지 않는다면 append 한다.
		if (get_header_idx(fp) == -1) {
			// just append this
		}
	}
	else if (argc == 3 && strcmp("-s", argv[1]) == 0) {

	}
	else if (argc == 3 && strcmp("-d", argv[1]) == 0) {

	}
	else {
		fprintf(stderr, usage, argv[0], argv[0], argv[0]);
		return 1;
	}

	fclose(idx_fp);
	fclose(fp);
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
