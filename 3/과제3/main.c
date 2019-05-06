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
/*


	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, devicefp);
	}

	free(blockbuf);

	ftl_open();    // ftl_read(), ftl_write() 호출하기 전에 이 함수를 반드시 호출해야 함

	//
	// ftl_write() 및 ftl_read() 테스트를 위한 코드를 자유자재로 만드세요
	//
	char num_text[10];
	for(int i = 0; i<(PAGES_PER_BLOCK*DATABLKS_PER_DEVICE);i++){
	    sprintf(num_text,"%d",i);
	    printf("write start %s\n",num_text);
	    ftl_write(i,num_text);
  	}

	   //All Read

    //update test
  ftl_write(1,"updated!");
  //ftl_write(4,"updated!");
  ftl_write(0,"updated!");
  ftl_read(0,blockbuf);
  printf("%s\n",blockbuf);
  ftl_read(1,blockbuf);
  printf("%s\n",blockbuf);
  //ftl_read(4,blockbuf);
  //printf("%s\n",blockbuf);
	
	 //All Read
 for(int i = 0; i <(PAGES_PER_BLOCK*DATABLKS_PER_DEVICE);i++){
    ftl_read(i,blockbuf);
    printf("%s\n",blockbuf);
  }

   //over index text
  //ftl_write(PAGES_PER_BLOCK*DATABLKS_PER_DEVICE,"updated!");
  //ftl_read(PAGES_PER_BLOCK*DATABLKS_PER_DEVICE,"updated!");
  //10 time update in a row
  for(int i = 0; i< 10 ; i++){
    ftl_write(3,"updating 10 times complete");
  }
   ftl_read(3,blockbuf);

	//char *test = (char *)malloc(30);
	//strcpy(test, "naninani!!");
	//printf("123%s",test);
	*/
   // ???
   printf("----------------------------------\n");
   strcpy(blockbuf, "naninani!!");
   printf("123%s\n", blockbuf);
   printf("2%s\n",blockbuf);
	fclose(devicefp);
 //printf("%c\n",0xff);
	return 0;
}
