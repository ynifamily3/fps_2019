#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "student.h"

char *usage = "Usage :\n%s -a <학번> <이름> <전공> <학년> <주소> <연락처> <이메일>\n%s -s <학번>\n%s -d <학번>\n";

short int get_idx_number_of_record(FILE *idx_fp);
void set_idx_number_of_record(FILE *idx_fp, short int val);
short int get_header_idx(FILE *fp);
void append_idx_new_byte_offset(FILE *idx_fp, short int new_byte_offset);
void printRecord(const STUDENT *s);
short int get_byte_offset_by_rn(FILE *idx_fp, int rn);
short int get_length_by_rn(FILE *idx_fp, FILE *fp, int rn);
void set_header_idx(FILE *fp, short int header_val);
void set_student_struct_by_args(STUDENT *student_data, char **argv);

//
// 학생 레코드 파일에 레코드를 저장하기 전에 구조체 STUDENT에 저장되어 있는 데이터를 레코드 형태로 만든다.
// 
void pack(char *recordbuf, const STUDENT *s)
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
void unpack(const char *recordbuf, STUDENT *s)
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
// 학생 레코드 파일로부터 레코드 번호에 해당하는 레코드를 읽어서 레코드 버퍼에 저장한다.
//
void readRecord(FILE *idx_fp, FILE *fp, char *recordbuf, int rn)
{
	// rn은 레코드의 상대 위치 [0 ~ records)
	// idx_fp 로부터 rn으로부터의 위치를 읽어서, 그 바이트 오프셋으로부터 길이를 구해 
	short int records = get_idx_number_of_record(idx_fp);
	short int byte_offset;
	short int length; // diff
	
	byte_offset = get_byte_offset_by_rn(idx_fp, rn);
	length = get_length_by_rn(idx_fp, fp, rn);

	// 실제 레코드 파일의 오프셋은 record file header가 2바이트를 처음에 차지하고 있으므로 그만큼 더해준다.
	byte_offset += sizeof(short int);

	// 레코드 파일에서 데이터를 읽어온다.
	fseek(fp, byte_offset, SEEK_SET);
	fread((void *)recordbuf, (size_t)length, (size_t)1, fp);
}

//
//알고리즘: 저장
//학생 레코드 파일에서 삭제 레코드의 존재 여부를 검사한 후 삭제 레코드가 존재하면 이 공간에
//새로운 레코드를 저장하며, 만약 삭제 레코드가 존재하지 않거나 조건에 부합하는 삭제 레코드가
//존재하지 않으면  파일의 맨마지막에 저장한다.
//
void add(FILE *idx_fp, FILE *fp, const STUDENT *s)
{
		char record_buf[MAX_RECORD_SIZE + 1];
		short int record_size;
		short int previous_header_data = get_header_idx(fp);
		// 파일에 넣기 좋게 packing
		pack(record_buf, s);
		record_size = (short int)strlen(record_buf);
		// 삭제된 레코드가 존재하지 않는다면 append 한다.
		if (previous_header_data == -1) {
			// just append this
			// idx에 새로운 값을 append한다.
			// 허상 byte offset 2가 빠짐
			append_idx_new_byte_offset(idx_fp, ftell(fp) - sizeof(short int));
			fwrite((const void *)record_buf, strlen(record_buf), (size_t)1, fp);
		} else {
			short int curr = previous_header_data;
			short int length;
			// 헤더가 가리키는 곳을 찾아가서 용량이 충분한지 묻는다.
			do {
				// 헤더 길이(2) + del mark길이(1) 만큼 이동하여 다음에 찾을 노드를 탐색
				fseek(fp, curr + sizeof(short int) + sizeof(char), SEEK_SET);
				fread((void *)&curr, sizeof(short int), 1, fp); // 다음에 찾을 노드
				fread((void *)&length, sizeof(short int), 1, fp); // length
			} while (length > record_size && curr != -1);
			// 삭제된 레코드 모두 자리가 충분하지 않은 경우 그냥 append
			if (curr == -1 && length > record_size) {
				append_idx_new_byte_offset(idx_fp, ftell(fp) - sizeof(short int));
				fwrite((const void *)record_buf, strlen(record_buf), (size_t)1, fp);
			} else {
				// 들어갈 자리가 있으므로 해당 자리에 update
				// 우선, 헤더를 업데이트 해야 되는데 현재 위치를 업데이트한다.
				// *[off][len]
				fseek(fp, - sizeof(short int) - sizeof(short int), SEEK_CUR);
				short int tmpPos = ftell(fp) - sizeof(char); // *마크도 빼준다.
				fread((void *)&curr, sizeof(short int), 1, fp); // 헤더에 쓸 값
				set_header_idx(fp, curr);
				// 그리고 해당 위치에 쓴다.
				fseek(fp, tmpPos, SEEK_SET);
				fwrite((const void *)record_buf, strlen(record_buf), (size_t)1, fp);
			}
		}
}

//
//알고리즘: 검색
//학생 레코드 파일에서 sequential search 기법으로 주어진 학번 키값과 일치하는 레코드를 찾는다.
//출력은 만드시 주어진 print 함수를 사용한다.
//검색 레코드가 존재하면 레코드 번호 rn을, 그렇지 않으면 -1을 리턴한다.
//
int search(FILE *idx_fp, FILE *fp, const char *keyval)
{
	STUDENT student_data;
	char record_buf[MAX_RECORD_SIZE + 1];
	short int numbers = get_idx_number_of_record(idx_fp);
	for (int i = 0; i < numbers; i++) {
		readRecord(idx_fp, fp, record_buf, i);
		if (record_buf[0] == '*') continue; // bypass deletion mark
		unpack(record_buf, &student_data);
		if (strcmp(keyval, student_data.id) == 0)
			return i;
	}
	return -1;
}

// rn은 몇 offset 인지 (허상 offset) 리턴한다.
short int get_byte_offset_by_rn(FILE *idx_fp, int rn)
{
	short int records = get_idx_number_of_record(idx_fp);
	short int byte_offset;
	// 범위를 초과할 경우
	if (rn >= records) {
		fprintf(stderr, "Invalid rn : %d\trecords : %hd\n", rn, records);
		exit(1);
	}
	// index 파일에서 record 파일의 byte offset을 가져온다.
	fseek(idx_fp, sizeof(short int) * (rn + 1), SEEK_SET);
	fread((void *)&byte_offset, sizeof(short int), (size_t)1, idx_fp);
	return byte_offset;
}

// rn에 해당하는 레코드는 몇 바이트를 차지하는지 리턴한다.
short int get_length_by_rn(FILE *idx_fp, FILE *fp, int rn)
{
	// 레코드 length 구하기.
	// 파일의 endpoint라면 file length 가 필요하다. 아니라면 다음 레코드의 위치와의 차이이다.
	short int records = get_idx_number_of_record(idx_fp);
	short int length;
	short int byte_offset = get_byte_offset_by_rn(idx_fp, rn);
	if (rn == records - 1) {
		fseek(fp, 0, SEEK_END);
		length = (short int)ftell(fp) - byte_offset;
	} else {
		fseek(idx_fp, sizeof(short int) * (rn + 2), SEEK_SET);
		fread((void *)&length, sizeof(short int), (size_t)1, idx_fp);
		length -= byte_offset;
	}
	return length;
}

//
//알고리즘: 삭제
//학생 파일에서 주어진 학번 키값과 일치하는 레코드를 찾은 후 해당 레코드를 삭제 처리한다.
//
void delete(FILE *idx_fp, FILE *fp, const char *keyval)
{
	short int rn, byte_offset, length;
	short int previous_deleted_offset = get_header_idx(fp);
	// 해당 레코드가 있나 검사해 보기
	if ((rn = search(idx_fp, fp, keyval)) < 0)
		return;
	byte_offset = get_byte_offset_by_rn(idx_fp, rn);
	length = get_length_by_rn(idx_fp, fp, rn);
	// 삭제 처리하기 (허상 오프셋을 고려한다.)
	fseek(fp, byte_offset + sizeof(short int), SEEK_SET);
	fwrite((const void *)"*", (size_t)(sizeof (char)), (size_t)1, fp);
	// 이전에 삭제된 바이트 오프셋을 저장
	fwrite((const void *)&previous_deleted_offset, sizeof(short int), (size_t)1, fp);
	// 자기자신의 size
	fwrite((const void *)&length, sizeof(short int), (size_t)1, fp);

	// header 수정 (삭제한 지금의 offset으로)
	set_header_idx(fp, byte_offset);
}

// 인덱스 파일에서 레코드 개수를 얻어온다. (삭제된 레코드가 차지하는 공간 포함)
short int get_idx_number_of_record(FILE *idx_fp)
{
	short int ret;
	rewind(idx_fp);
	fread((void *)&ret, sizeof(short int), (size_t)1, idx_fp);
	return ret;
}

void set_idx_number_of_record(FILE *idx_fp, short int val)
{
	rewind(idx_fp);
	fwrite((const void *)&val, sizeof(short int), (size_t)1, idx_fp);
}

// idx에 새로운 값을 append한다.
void append_idx_new_byte_offset(FILE *idx_fp, short int new_byte_offset)
{
	set_idx_number_of_record(idx_fp, get_idx_number_of_record(idx_fp) + 1); // increase 레코드 개수
	fseek(idx_fp, 0, SEEK_END);
	fwrite((const void *)&new_byte_offset, sizeof(short int), (size_t)1, idx_fp);
}

// record File의 header 값을 지정한다. header는 최근에 삭제된 byte offset (허상)
void set_header_idx(FILE *fp, short int header_val)
{
	rewind(fp);
	fwrite((const void *)&header_val, sizeof(short int), (size_t)1, fp);	
}

// record file의 header 값을 얻어온다. header는 최근에 삭제된 byte offset (허상)
short int get_header_idx(FILE *fp)
{
	short int ret;
	rewind(fp);
	fread((void *)&ret, sizeof(short int), (size_t)1, fp);
	return ret;
}

void set_student_struct_by_args(STUDENT *student_data, char **argv)
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
	FILE *idx_fp; // 학생 레코드 인덱스 파일의 파일 포인터
	FILE *fp;  // 학생 레코드 파일의 파일 포인터
	STUDENT student_data;
	char record_buf[MAX_RECORD_SIZE + 1];

	// student.idx와 student.dat 두 파일 중 하나라도 없으면 모두 새로 만든다.
	if (access(RECORD_FILE_NAME, F_OK) || access(INDEX_FILE_NAME, F_OK)) {
		idx_fp = fopen(INDEX_FILE_NAME, "wb+");
		fp = fopen(RECORD_FILE_NAME, "wb+");
		set_idx_number_of_record(idx_fp, 0); // 처음엔 0개의 레코드가 있으므로
		set_header_idx(fp, -1); // 최근 삭제된 위치. 지금은 삭제된 레코드가 없으므로 -1
		// header -1로 설정
	} else {
		idx_fp = fopen(INDEX_FILE_NAME, "rb+");
		fp = fopen(RECORD_FILE_NAME, "rb+");
	}

	if (argc != 3 && argc != 9) {
		fprintf(stderr, usage, argv[0], argv[0], argv[0]);
		fclose(idx_fp);
		fclose(fp);
		return 1;
	}

	if (argc == 9 && strcmp("-a", argv[1]) == 0) {
		// arg로부터 얻어온 정보를 구조체에 넣는다.
		set_student_struct_by_args(&student_data, argv);
		// 알고리즘 : 저장
		add(idx_fp, fp, &student_data);
	}
	else if (argc == 3 && strcmp("-s", argv[1]) == 0) {
		short int index = (short int)search(idx_fp, fp, argv[2]);
		if (index >= 0) {
			readRecord(idx_fp, fp, record_buf, index);
			unpack(record_buf, &student_data);
			printRecord(&student_data);
		}
	}
	else if (argc == 3 && strcmp("-d", argv[1]) == 0) {
		delete(idx_fp, fp, argv[2]);
	}
	else {
		fprintf(stderr, usage, argv[0], argv[0], argv[0]);
		return 1;
	}

	fclose(idx_fp);
	fclose(fp);
	return 0;
}

void printRecord(const STUDENT *s)
{
	printf("%s|%s|%s|%s|%s|%s|%s\n", s->id, s->name, s->dept, s->year, s->addr, s->phone, s->email);
}
