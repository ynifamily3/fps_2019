// 주의사항
// 1. blkmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blkmap.h에 정의되어 있지 않을 경우 본인이 만들어서 사용하면 됨

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"
// 필요한 경우 헤더 파일을 추가하시오.


extern FILE *devicefp;

// [logical block number] = [physical block number] 순서
int mapping_table[DATABLKS_PER_DEVICE];

// free block의 physical block number입니다.
int free_block_position;

int dd_read(int ppn, char *pagebuf);
int dd_write(int ppn, char *pagebuf);
int dd_erase(int pbn);

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// main()함수에서 반드시 먼저 호출이 되어야 한다.
//
void ftl_open()
{
	//
	// address mapping table 생성 및 초기화 등을 진행
    // mapping table에서 lbn과 pbn의 수는 blkmap.h에 정의되어 있는 DATABLKS_PER_DEVICE
    // 수와 같아야 하겠지요? 나머지 free block 하나는 overwrite 시에 사용하면 됩니다.
	// pbn 초기화의 경우, 첫 번째 write가 발생하기 전을 가정하므로 예를 들면, -1로 설정을
    // 하고, 그 이후 필요할 때마다 block을 하나씩 할당을 해 주면 됩니다. 어떤 순서대로 할당하는지는
    // 각자 알아서 판단하면 되는데, free block들을 어떻게 유지 관리할 지는 스스로 생각해 보기
    // 바랍니다.
	int i;
	free_block_position = DATABLKS_PER_DEVICE; // 제일 마지막 위치에 프리 블록을 둔다.
	for (i = 0; i < DATABLKS_PER_DEVICE; i++) {
		mapping_table[i] = i; // lbn-pbn을 순서대로 채움
	}

	return;
}

//
// file system이 ftl_write()를 호출하면 FTL은 flash memory에서 주어진 lsn과 관련있는
// 최신의 데이터(512B)를 읽어서 sectorbuf가 가리키는 곳에 저장한다.
// 이 함수를 호출하기 전에 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 한다.
// 즉, 이 함수에서 메모리를 할당받으면 안된다.
//
void ftl_read(int lsn, char *sectorbuf)
{
	// ftl_read면 lsn에 있는 데이터를 pbn과 관련하여 해당 블록을 간다음 읽기.
	char pagebuf[PAGE_SIZE]; // 섹터 사이즈 + 스페어 사이즈까지 있음.
	int lbn, offset, pbn, ppn; // lsn가지고 계산하는 것들.
	lbn = lsn / PAGES_PER_BLOCK;
	offset = lsn % PAGES_PER_BLOCK;
	pbn = mapping_table[lbn];
	ppn = pbn * PAGES_PER_BLOCK + offset;

	if (pbn < 0) {
		fprintf(stderr, "error reading data in position %d\n", lsn);
		return;
	}
	if (dd_read(ppn, pagebuf) < 0) {
		fprintf(stderr, "internal Error : error reading data in position %d\n", lsn);
		return;
	}

	// 리턴될 섹터는 초기화 상태여야 한다... (문자열 찌꺼기 제거)
	memset(sectorbuf, 0x00, SECTOR_SIZE);
	//printf("읽을때 ppn : %d page buf : %s\n",ppn, pagebuf);
	strncpy(sectorbuf, pagebuf, SECTOR_SIZE);

	return;
}

//
// file system이 ftl_write()를 호출하면 FTL은 flash memory에 sectorbuf가 가리키는 512B
// 데이터를 저장한다. 당연히 flash memory의 어떤 주소에 저장할 것인지는 block mapping 기법을
// 따라야한다.
//
void ftl_write(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	int i;
	int lbn = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int pbn = mapping_table[lbn]; // dest physical block...
	int ppn = pbn * PAGES_PER_BLOCK + offset;

	if (lsn < 0 || lsn > DATABLKS_PER_DEVICE * PAGES_PER_BLOCK - 1) {
		// lsn은  0~ 블록 개수 * 페이지 개수 - 1 넘버까지 허용 
		fprintf(stderr, "Invalid logical sector number\n");
		exit(1);
	}

	//  그 페이지의 내용을 estimate해 본다.
	if (dd_read(ppn, pagebuf) < 0) {
		fprintf(stderr, "spare checking Error ppn : %d\n", ppn);
		exit(1);
	}
	// spare 영역의 값이 -1이면 이 섹터엔 데이터가 들어간 적이 없다.
	if (pagebuf[SECTOR_SIZE] == -1) {
		// sectorbuf에 있는 문자열을 pagebuf에 복사
		memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
		// 0으로 변경 (이미 쓰여졌다는 표시를 넣음)
		memset(pagebuf+SECTOR_SIZE, 0x00, SPARE_SIZE);
		if (dd_write(ppn, pagebuf) < 0) {
			fprintf(stderr, "dd_write error\n");
			return;
		}
	} else {
		// 이미 있다면 free block에 몽땅 복사후 dd_erase 하고. free block 갱신
		// dd_read에 읽은 정보가 있다. 일단 정보를 수정하고 복사.
		//memcpy(pagebuf, sectorbuf, SECTOR_SIZE); // 일단 정보는 수정해줌
		//memset(pagebuf+SECTOR_SIZE, 0x00, SPARE_SIZE); // 일단 정보는 수정해줌

		// free block에 복사하기 (pbn을 free로)
		for (i = 0; i < PAGES_PER_BLOCK; i++) {
			//memcpy(pagebuf)
			//memcpy(pagebuf, (pbn + i), PAGE_SIZE);
			if (i != offset) {
				// 기존 자료 복사
				dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf); // 해당 PPN pagebuf에 저장
				//printf("기존 자료 복사 %s를 프리 메모리로\n", pagebuf);
				dd_write(free_block_position * PAGES_PER_BLOCK + i, pagebuf); // pagebuf에 있는걸 파일에 씀
			} else {
				// 인플레이스 업데이트 발생
				// 정보 수정 시뮬레이션
				//printf("새로운 자료 복사 ppn : [%d] 에 %s\n", free_block_position * PAGES_PER_BLOCK + i, sectorbuf);
				memcpy(pagebuf, sectorbuf, SECTOR_SIZE); // 수정할 정보를 pagebuf에 저장
				memset(pagebuf+SECTOR_SIZE, 0x00, SPARE_SIZE); // dirty bit on
				dd_write(free_block_position * PAGES_PER_BLOCK + i, pagebuf);
			}
		} 

		dd_erase(pbn); // 새로운 프리 블록 포지션 지우기
		mapping_table[lbn] = free_block_position;
		free_block_position = pbn; // 지워진 곳을 프리 블록으로 함.
		// 매핑 테이블 변경
		// erase 함.
	}
	return;
}
