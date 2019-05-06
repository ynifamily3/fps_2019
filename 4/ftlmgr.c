// 주의사항
// 1. blkmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blkmap.h에 정의되어 있지 않을 경우 본인이 만들어서 사용하면 됨

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"
// 필요한 경우 헤더 파일을 추가

extern FILE *devicefp;
extern int dd_read(int ppn, char *pagebuf);
extern int dd_write(int ppn, char *pagebuf);
extern int dd_erase(int pbn);

int mapping_table[DATABLKS_PER_DEVICE];
int free_block_position;

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// main()함수에서 반드시 먼저 호출이 되어야 한다.
//

// lsn에 해당하는 page를 찾아 pagebuf에 채운다. 버퍼가 있다면 역순으로 읽어 버퍼 내용을 꺼낸다.

// spare의 값을 읽어준다. 기본적으로는 -1이다.
int get_spare_data(const char *pagebuf)
{
	return *(int *)(pagebuf + SECTOR_SIZE);
}

int get_spare_data_by_ppn(int ppn)
{
	char pagebuf[PAGE_SIZE];
	dd_read(ppn, pagebuf);
	return get_spare_data(pagebuf);
}

int get_ppn_by_lsn(int lsn)
{
	char pagebuf[PAGE_SIZE];
	int i, lbn, pbn, offset, ppn, spare_data;
	lbn = lsn / NONBUF_PAGES_PER_BLOCK; // lsn 으로부터 lbn 계산하기
	offset = lsn % NONBUF_PAGES_PER_BLOCK; // lsn 으로부터 offset 계산하기
	if (mapping_table[lbn] == -1) mapping_table[lbn] = lbn; // mapping table이 설정되지 않았을 경우 설정
	pbn = mapping_table[lbn];
	ppn = pbn * PAGES_PER_BLOCK + offset;
	
	for (i = PAGES_PER_BLOCK - 1; i >= NONBUF_PAGES_PER_BLOCK; i--) {
		dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf);
		spare_data = get_spare_data(pagebuf);
		if (spare_data == lsn) return pbn * PAGES_PER_BLOCK + i;
	}
	return ppn;
}

// lsn에 해당하는 페이지를 찾아서 pagebuf에 채운다.
void get_page(int lsn, char *pagebuf)
{
	int spare_data; // 버퍼 검사용
	int i, lbn, pbn, offset, ppn;
	lbn = lsn / NONBUF_PAGES_PER_BLOCK; // lsn 으로부터 lbn 계산하기
	offset = lsn % NONBUF_PAGES_PER_BLOCK; // lsn 으로부터 offset 계산하기
	if (mapping_table[lbn] == -1) mapping_table[lbn] = lbn; // mapping table이 설정되지 않았을 경우 설정
	pbn = mapping_table[lbn];
	ppn = pbn * PAGES_PER_BLOCK + offset;

	// buffer영역을 역순으로 돌면서 계산한다.
	for (i = PAGES_PER_BLOCK - 1; i >= NONBUF_PAGES_PER_BLOCK; i--) {
		dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf);
		spare_data = get_spare_data(pagebuf);
		if (spare_data == lsn) return;
	}

	// buffer 에서 찾지 못한 것은 해당 ppn에서 찾아 본다.
	dd_read(ppn, pagebuf);
}

void ftl_open()
{
	int i;
	for (i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		mapping_table[i] = -1; // lbn-pbn 매핑을 -1로 초기화
	}

	free_block_position = DATABLKS_PER_DEVICE; // 제일 마지막 위치에 프리 블록을 둔다.

	return;
}

void ftl_read(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	get_page(lsn, pagebuf);
	memcpy(sectorbuf, pagebuf, SECTOR_SIZE);
}

void ftl_write(int lsn, char *sectorbuf)
{
	int i;
	char pagebuf[PAGE_SIZE];
	
	// 데이터를 버퍼에 기록하기
	memcpy(pagebuf, sectorbuf, SECTOR_SIZE); // data 기록
	*(int *)(pagebuf + SECTOR_SIZE) = lsn; // spare 기록

	// 해당 offset이 비어 있으면 그 쪽에 기록하기
	if (get_spare_data_by_ppn(get_ppn_by_lsn(lsn)) == -1) {
		dd_write(get_ppn_by_lsn(lsn), pagebuf); // flash memory에 기록
		return;
	}

	// 버퍼를 '순차적으로' 조사하여 spare가 -1인 곳을 찾아내 기록하기
	for (i = NONBUF_PAGES_PER_BLOCK; i < PAGES_PER_BLOCK; i++) {
		int spare_ppn = mapping_table[lsn / NONBUF_PAGES_PER_BLOCK] * PAGES_PER_BLOCK + i;
		if (get_spare_data_by_ppn(spare_ppn) == -1) {
			dd_write(spare_ppn, pagebuf); // flash memory에 기록
			return;
		}
	}
	// 버퍼에 남은 공간이 없다면, 새 프리 블록으로 이주하기
	// i  ) 프리블럭 위치에 쓰려는 lsn에 있는 데이터를 우선 쓴다.
	// ii ) 버퍼를 역순으로 읽어 해당 lsn이 있으면 그걸 기록한다., 그 다음에 기존 페이지를 씀
	// iii) 프리 블럭 이주
	int lbn = lsn / NONBUF_PAGES_PER_BLOCK;
	if (mapping_table[lbn] == -1) mapping_table[lbn] = lbn; // mapping table이 설정되지 않았을 경우 설정
	int pbn = mapping_table[lbn];
	int spare_data, spare_offset;

	int free_block_ppn = free_block_position * PAGES_PER_BLOCK + (lsn % NONBUF_PAGES_PER_BLOCK);
	dd_write(free_block_ppn, pagebuf); // 해당 내용을 쓴다. 이미 썼으므로 나중에 pagebuf재활용에 문제 없다..

	// buffer영역을 역순으로 돌면서 계산한다.
	for (i = PAGES_PER_BLOCK - 1; i >= NONBUF_PAGES_PER_BLOCK; i--) {
		dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf);
		spare_data = get_spare_data(pagebuf);
		// spare data 의 나머지가 offset이다.. 
		spare_offset = spare_data % NONBUF_PAGES_PER_BLOCK;

		// freeblock 위치 + offset에 이미 데이터가 있는지 조사하여 없으면 ddwrite한다. 있으면 무시
		if (get_spare_data_by_ppn(free_block_position * PAGES_PER_BLOCK + spare_offset) == -1) {
			// ddwrite
			dd_write(free_block_position * PAGES_PER_BLOCK + spare_offset, pagebuf);
		}
	}

	// NON-buffer 까지 순서대로 돌면서 빈 부분을 복사해옴.
	for (i = 0; i < NONBUF_PAGES_PER_BLOCK; i++) {
		dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf);
		if (get_spare_data_by_ppn(free_block_position * PAGES_PER_BLOCK + i) == -1) { // free block 해당 위치가 비어 있으면
			dd_write(free_block_position * PAGES_PER_BLOCK + i, pagebuf);
		}
	}
	// dd-erase후 free block 이주
	dd_erase(pbn);
	mapping_table[lbn] = free_block_position;
	free_block_position = pbn;
}
