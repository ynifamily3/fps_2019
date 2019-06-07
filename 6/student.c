#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "student.h"

//
// 학생 레코드 파일로부터  레코드 번호에 해당하는 레코드를 읽어 레코드 버퍼에 저장한다.
//
void readStudentRec(FILE *fp, char *recordbuf, int rn)
{
	fseek(fp, rn * STUDENT_RECORD_SIZE, SEEK_SET);
	fread(recordbuf, SID_FIELD_SIZE, 1, fp); // 학번 필드만 가져옴
}

//
// Hash file로부터 rn의 레코드 번호에 해당하는 레코드를 읽어 레코드 버퍼에 저장한다.
//
void readHashRec(FILE *fp, char *recordbuf, int rn)
{
	fseek(fp, 4 + rn * HASH_RECORD_SIZE, SEEK_SET);
	fread(recordbuf, SID_FIELD_SIZE, 1, fp);
}

//
// Hash file에 rn의 레코드 번호에 해당하는 위치에 레코드 버퍼의 레코드를 저장한다.
//
void writeHashRec(FILE *fp, const char *recordbuf, int rn)
{
	static int ttl = 0;
	fseek(fp, 4 + rn * HASH_RECORD_SIZE, SEEK_SET);
	fwrite(recordbuf, SID_FIELD_SIZE, 1, fp);
	fwrite(&ttl, sizeof(ttl), 1, fp);
	ttl++;
}

//
// n의 크기를 갖는 hash file에서 주어진 학번 키값을 hashing하여 주소값(레코드 번호)를 리턴한다.
//
int hashFunction(const char *sid, int n)
{
	// 마지막 2자리를 취한다.
	int len = (int)strlen(sid);
	return ((int)sid[len - 2] * (int)sid[len - 1]) % n;
}

//
// n의 크기를 갖는 hash file을 생성한다.
// Hash file은 fixed length record 방식으로 저장되며, 레코드의 크기는 14 바이트이다.
// (student.h 참조)
//
void makeHashfile(int n)
{
	FILE *record_fp, *hash_fp;
	int header = n;
	char idbuf[11] = {0, };
	char tmpid_buf[11] = {0, };
	int records, i, j;
	int hash_value;
	char dummy = 0;
	record_fp = fopen(RECORD_FILE_NAME, "rb");
	hash_fp = fopen(HASH_FILE_NAME, "wb+");
	setbuf(record_fp, NULL);
	setbuf(hash_fp, NULL);
	fseek(hash_fp, 0, SEEK_SET);
	fwrite(&header, sizeof(header), 1, hash_fp);
	for (i = 0; i < HASH_RECORD_SIZE * n; i++) {
		fwrite(&dummy, sizeof(char), 1, hash_fp);
	}
	fseek(record_fp, 0, SEEK_END);
	records = (int)ftell(record_fp) / STUDENT_RECORD_SIZE;
	fseek(record_fp, 0, SEEK_SET);
	for (i = 0; i < records; ++i) {
		readStudentRec(record_fp, idbuf, i);
		hash_value = hashFunction(idbuf, n);
		for (j = 0; j < n; j++) {
			readHashRec(hash_fp, tmpid_buf, (hash_value + j) % n);
			if (tmpid_buf[0] != '*' && tmpid_buf[0] != '\0') continue;
			writeHashRec(hash_fp, idbuf, (hash_value + j) % n);
			break;
		}
	}
	fclose(record_fp);
	fclose(hash_fp);
}

//
// 주어진 학번 키값을 hash file에서 검색한다.
// 그 결과는 주어진 학번 키값이 존재하는 hash file에서의 주소(레코드 번호)와 search length이다.
// 검색한 hash file에서의 주소는 rn에 저장하며, 이때 hash file에 주어진 학번 키값이
// 존재하지 않으면 rn에 -1을 저장한다. (search()는 delete()에서도 활용할 수 있음)
// search length는 함수의 리턴값이며, 검색 결과에 상관없이 search length는 항상 계산되어야 한다.
//
int search(const char *sid, int *rn)
{
	FILE *hash_fp;
	int search_length = 0;
	int success = 0;
	int hash_table_size, i;
	char tmpid_buf[11] = {0, };
	hash_fp = fopen(HASH_FILE_NAME, "rb");
	fread(&hash_table_size, sizeof(int), 1, hash_fp);
	*rn = hashFunction(sid, hash_table_size);
	for (i = 0; i < hash_table_size; i++) {
		++search_length;
		fseek(hash_fp, sizeof(hash_table_size) +  HASH_RECORD_SIZE * ((*rn + i) % hash_table_size), SEEK_SET);
		fread((void *)tmpid_buf, sizeof(char), SID_FIELD_SIZE, hash_fp);
		if (tmpid_buf[0] != '*' && strcmp(sid, tmpid_buf) == 0) {
			success = 1;
			break;
		}
		if (tmpid_buf[0] == '\0') break; // 아예 NULL인 경우를 만나면 ...
	}
	if (!success) *rn = -1;
	else *rn = (*rn + i) % hash_table_size;
	fclose(hash_fp);
	return search_length;
}

//
// Hash file에서 주어진 학번 키값과 일치하는 레코드를 찾은 후 해당 레코드를 삭제 처리한다.
// 이때 학생 레코드 파일에서 레코드 삭제는 필요하지 않다.
//
void delete(const char *sid)
{
	FILE *hash_fp;
	int rn;
	search(sid, &rn);
	if (rn == -1) {
		// printf("삭제할 레코드가 없다.\n");
	} else {
		// rn 으로 가서 *표시
		hash_fp = fopen(HASH_FILE_NAME, "rb+");
		fseek(hash_fp, sizeof(int) + HASH_RECORD_SIZE * rn, SEEK_SET);
		fwrite("*", sizeof(char), 1, hash_fp);
		// printf("삭제됨\n");
		fclose(hash_fp);
	}
}

//
// rn은 hash file에서의 레코드 번호를, sl은 search length를 의미한다.
//
void printSearchResult(int rn, int sl)
{
	printf("%d %d\n", rn, sl);
}

int main(int argc, char *argv[])
{
	// 학생레코드파일은 student.h에 정의되어 있는 STUDENT_FILE_NAME을, 
	// hash file은 HASH_FILE_NAME을 사용한다.

	// 검색 기능을 수행할 때 출력은 반드시 주어진 printSearchResult() 함수를 사용한다.
	if (argc != 3) {
		fprintf(stderr, "usage : \n%s -c <hashfile_size>\n%s -s <sid>\n%s -d <sid>\n", argv[0], argv[0], argv[0]);
		return 1;
	}
	if (strcmp("-c", argv[1]) == 0) {
		int hSize = atoi(argv[2]);
		if (hSize <= 0) {
			fprintf(stderr, "%s must be positive number.\n", argv[2]);
			return 1;
		}
		makeHashfile(hSize);
	} else if (strcmp("-s", argv[1]) == 0) {
		int rn;
		int sl = search(argv[2], &rn);
		printSearchResult(rn, sl);
	} else if (strcmp("-d", argv[1]) == 0) {
		delete(argv[2]);
	} else {
		fprintf(stderr, "usage : \n%s -c <hashfile_size>\n%s -s <sid>\n%s -d <sid>\n", argv[0], argv[0], argv[0]);
		return 1;
	}
	return 0;
}
