#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blkmap.h"

FILE *devicefp;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);

//
// 이 함수는 file system의 역할을 수행한다고 생각하면 되고,
// file system이 flash memory로부터 512B씩 데이터를 저장하거나 데이터를 읽어 오기 위해서는
// 본인이 구현한 FTL의 ftl_write()와 ftl_read()를 호출하면 됨
//
int main(int argc, char *argv[])
{
	char *blockbuf;
    char sectorbuf[SECTOR_SIZE];
	char outputbuf[SECTOR_SIZE];
	int lsn, i;

    devicefp = fopen("flashmemory", "w+b");
	if(devicefp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}
	   
    //
    // flash memory의 모든 바이트를 '0xff'로 초기화한다.
    // 
    blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, devicefp);
	}

	free(blockbuf);

	ftl_open();    // ftl_read(), ftl_write() 호출하기 전에 이 함수를 반드시 호출해야 함

	//
	// ftl_write() 및 ftl_read() 테스트를 위한 코드를 자유자재로 만드세요
	//

	strncpy(sectorbuf, "data1", SECTOR_SIZE);
	ftl_write(0, sectorbuf);
	ftl_read(0, outputbuf);
	printf("%s\n", outputbuf);
	strncpy(sectorbuf, "data2", SECTOR_SIZE);
	ftl_write(0, sectorbuf);
	ftl_read(0, outputbuf);
	printf("%s\n", outputbuf);
	strncpy(sectorbuf, "data3", SECTOR_SIZE);
	ftl_write(1, sectorbuf);
	ftl_read(1, outputbuf);
	printf("%s\n", outputbuf);
	strncpy(sectorbuf, "data4", SECTOR_SIZE);
	ftl_write(1, sectorbuf);
	ftl_read(1, outputbuf);
	printf("%s\n", outputbuf);

	strncpy(sectorbuf, "ㅗ", SECTOR_SIZE);
	ftl_write(1, sectorbuf);
	ftl_read(1, outputbuf);
	printf("%s\n", outputbuf);

	fclose(devicefp);

	return 0;
}
