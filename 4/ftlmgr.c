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
void ftl_open()
{
	//
	// address mapping table 생성 및 초기화 등을 진행
    // mapping table에서 lbn과 pbn의 수는 blkmap.h에 정의되어 있는 DATABLKS_PER_DEVICE
    // 수와 같아야 하겠지요? 나머지 free block 하나는 overwrite 발생 시에 사용하면 됩니다.
	// pbn 초기화의 경우, 첫 번째 write가 발생하기 전을 가정하므로 예를 들면, -1로 설정을
    // 하고, 그 이후 필요할 때마다 block을 하나씩 할당을 해 주면 됩니다. 어떤 순서대로 할당하는지는
    // 각자 알아서 판단하면 되는데, free block들을 어떻게 유지 관리할 지는 스스로 생각해 보기
    // 바랍니다.
	int i;
	for (i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		mapping_table[i] = -1; // lbn-pbn 매핑을 -1로 초기화
	}

	free_block_position = DATABLKS_PER_DEVICE; // 제일 마지막 위치에 프리 블록을 둔다.

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
	// 처음에는 버퍼에서 거꾸로 읽음 (최신 데이터 찾기)
	// 거기에 없으면 원래 offset에서 읽음
	char pagebuf[PAGE_SIZE]; // 섹터 사이즈 + 스페어 사이즈까지 있음. 플래시 메모리에서 가져온 것.
	int sparebuf; // 버퍼 검사용
	int i, lbn, pbn, offset, ppn; // lsn가지고 계산하는 것들.
	lbn = lsn / NONBUF_PAGES_PER_BLOCK;
	offset = lsn % NONBUF_PAGES_PER_BLOCK;
	pbn = mapping_table[lbn] != -1 ? mapping_table[lbn] : lbn;
	ppn = pbn * PAGES_PER_BLOCK + offset;

	// 여기서 버퍼를 역순으로 읽어 ppn을 갱신한다. (spare 검사)
	/*
	non-buf 0
	non-buf 1
	non-buf 2
	buf 3
	*/
	for (i = PAGES_PER_BLOCK - 1; i >= NONBUF_PAGES_PER_BLOCK; i--)
	{
		// spare를 검사한다. spare에서 (int형 크기)바이트만 사용한다
		if (dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf) < 0) // 버퍼를 검사한다.
		{
			fprintf(stderr, "error dd_read for buffer : %d\n", i);
			exit(1);
		}

		memcpy(sparebuf, pagebuf + SECTOR_SIZE, sizeof(int)); // 버퍼에서 spare만 빼온다.

		if (sparebuf == lsn)
		{
			memset(sectorbuf, 0x00, SECTOR_SIZE); // 리턴될 섹터 버퍼 초기화
			memcpy(sectorbuf, pagebuf, SECTOR_SIZE); // 데이터를 전달해줌
			break;
		}
	}

	if (dd_read(ppn, pagebuf) < 0)
	{
		fprintf(stderr, "error dd_read for lsn : %d\n", lsn);
		exit(1);
	}

	memset(sectorbuf, 0x00, SECTOR_SIZE); // 리턴될 섹터 버퍼 초기화
	memcpy(sectorbuf, pagebuf, SECTOR_SIZE);


	return;
}

//
// file system이 ftl_write()를 호출하면 FTL은 flash memory에 sectorbuf가 가리키는 512B
// 데이터를 저장한다. 당연히 flash memory의 어떤 주소에 저장할 것인지는 
// buffer-based block mapping 기법을 따라야한다.
//
void ftl_write(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	int i;
	int sparebuf; // 버퍼 검사용
	int lbn = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int pbn = mapping_table[lbn]; // dest physical block...
	int ppn = pbn * PAGES_PER_BLOCK + offset;
	// 그 페이지의 내용을 estimate해 본다. (비어 있는지.)
	if (dd_read(ppn, pagebuf) < 0) {
		fprintf(stderr, "spare checking Error ppn : %d\n", ppn);
		exit(1);
	}
	memcpy(sparebuf, pagebuf + SECTOR_SIZE, sizeof(int)); // 버퍼에서 spare만 빼온다.
	// spare 영역의 값이 -1이면 이 섹터엔 데이터가 들어간 적이 없다.
	if (sparebuf == -1) {
		// sectorbuf에 있는 데이터를 pagebuf에 복사
		memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
		// lsn으로 변경 (이미 쓰여졌다는 표시를 넣음)
		sparebuf = lsn;
		*((int *)(pagebuf+SECTOR_SIZE)) = sparebuf;
		if (dd_write(ppn, pagebuf) < 0) {
			fprintf(stderr, "dd_write error\n");
			exit(1);
		}
		return;
	}
	else
	{
		// sparebuf에 다른 값이 있으면 비어있는 버퍼가 나올때까지 찾는다. (순차적으로)
		for (i = NONBUF_PAGES_PER_BLOCK; i < PAGES_PER_BLOCK; i++)
		{
			// spare를 검사한다. spare에서 (int형 크기)바이트만 사용한다
			if (dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf) < 0) // 버퍼를 검사한다.
			{
				fprintf(stderr, "error dd_read for buffer : %d\n", i);
				exit(1);
			}
			memcpy(sparebuf, pagebuf + SECTOR_SIZE, sizeof(int)); // 버퍼에서 spare만 빼온다.
			if (sparebuf == -1)
			{
				// 데이터를 넣음.
				memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
				// 스페어를 lsn으로 변경 (이미 쓰여졌다는 표시를 넣음)
				sparebuf = lsn;
				*((int *)(pagebuf+SECTOR_SIZE)) = sparebuf;
				if (dd_write(pbn * PAGES_PER_BLOCK + i, pagebuf) < 0)
				{
					fprintf(stderr, "dd_write error\n");
					exit(1);
				}
				return; // 버퍼에 썼으므로 함수를 끝냄
			}
		}
		// 여기에 제어가 걸린다는 것은 offset에 데이터가 이미 있고, 버퍼도 꽉 차서 새로운 free block에 써야 한다는 걸 의미한다.
		// 1. 해당하는 free block의 offset 에 데이터를 쓴다.
		// 2. 버퍼를 역순으로 읽어 해당하는 lsn찾고 거기에 우선적으로 복사한다. 이미 값이 들어가 있는 경우엔 pass
		// free블록을 이주 (dd_erase)
		
		// 1. 
		// sectorbuf에 있는 데이터를 pagebuf에 복사
		memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
		// spare data를 lsn으로 기록
		sparebuf = lsn;
		*((int *)(pagebuf+SECTOR_SIZE)) = sparebuf;
		dd_write(free_block_position * PAGES_PER_BLOCK + offset, pagebuf); // 쓰려는 데이터를 쓴다.

		//2. 
		for (i = PAGES_PER_BLOCK - 1; i >= NONBUF_PAGES_PER_BLOCK; i--)
		{
			// 기존 버퍼에 있는 spare를 읽고, 해당 위치에 데이터를 기록한다. 만약 이미 기록된 lsn이면 pass 한다
			
			// 버퍼에서 읽은 spare
			if (dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf) < 0)
			{
				fprintf(stderr, "error dd_read for buffer : %d\n", i);
				exit(1);
			}
			memcpy(sparebuf, pagebuf + SECTOR_SIZE, sizeof(int)); // 버퍼에서 spare만 빼온다.

			// 버퍼에서 읽은 spare기반으로 원래 위치를 찾아가서 데이터가 들어있는지 살펴본다.
			int newoffset = sparebuf % PAGES_PER_BLOCK;
			char pagebuf2[PAGE_SIZE];
			if (dd_read(pbn * PAGES_PER_BLOCK + newoffset, pagebuf2) < 0)
			{
				fprintf(stderr, "error dd_read for buffer : %d\n", i);
				exit(1);
			}
			// 기록을 시도하는 위치에서 읽은 spare (-1이면 씀)
			int newspare;
			memcpy(newspare, pagebuf + SECTOR_SIZE, sizeof(int)); // 버퍼에서 spare만 빼온다.
			if (newspare == -1)
			{
				// 해당하는 내용을 버퍼에 있던 내용으로 씀
				// ********************** 여기에서부터 내용을 추가해야 겠음 ********************************************//
				dd_write(free_block_position * PAGES_PER_BLOCK + offset, pagebuf2); // 쓰려는 데이터를 쓴다. (이 코드가 맞나?)
			}
			else
			{
				// 아무 작업 할 필요 없음.
			}


		}

	}
	return;
}
