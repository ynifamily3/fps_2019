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
int dd_read(int ppn, char *pagebuf);

//
// 이 함수는 file system의 역할을 수행한다고 생각하면 되고,
// file system이 flash memory로부터 512B씩 데이터를 저장하거나 데이터를 읽어
// 오기 위해서는 본인이 구현한 FTL의 ftl_write()와 ftl_read()를 호출하면 됨
//
int main(int argc, char *argv[])
{
  char *blockbuf;
  char sectorbuf[SECTOR_SIZE];
  int lsn, i;

  devicefp = fopen("flashmemory", "w+b");
  if (devicefp == NULL) {
    printf("file open error\n");
    exit(1);
  }

  //
  // flash memory의 모든 바이트를 '0xff'로 초기화한다.
  //
  blockbuf = (char *)malloc(BLOCK_SIZE);
  memset(blockbuf, 0xFF, BLOCK_SIZE);

  for (i = 0; i < BLOCKS_PER_DEVICE; i++) {
    fwrite(blockbuf, BLOCK_SIZE, 1, devicefp);
  }

  free(blockbuf);

  ftl_open(); // ftl_read(), ftl_write() 호출하기 전에 이 함수를 반드시 호출해야
              // 함

  //
  // ftl_write() 및 ftl_read() 테스트를 위한 코드를 자유자재로 만드세요
  //

  system("clear");
  const char cowsay[] = " ____________ \n"
                        "< FTL Tester >\n"
                        " ------------ \n"
                        "        \\   ^__^\n"
                        "         \\  (oo)\\_______\n"
                        "            (__)\\       )\\/\\\n"
                        "                ||----w |\n"
                        "                ||     ||\n";
  printf("%s\n", cowsay);
  printf("type \"help\" to see all commands\n");

  while (1) {
    char command[1024];
    printf("command> ");
    fgets(command, 1024, stdin);
    command[strlen(command) - 1] = '\0';
    if (strcmp(command, "exit") == 0 | strcmp(command, "x") == 0) {
      printf("\nbye\n\n");
      break;
    }
    else if (strcmp(command, "print") == 0 || strcmp(command, "p") == 0) {
      char pagebuf[PAGE_SIZE];

      for (int i = 0; i < BLOCKS_PER_DEVICE; ++i) {
        printf("blk %3d    ", i);
        for (int j = 0; j < PAGES_PER_BLOCK; ++j) {
          memset(pagebuf, 0xFF, sizeof(pagebuf));
          memset(sectorbuf, 0xFF, sizeof(sectorbuf));
          dd_read(i * PAGES_PER_BLOCK + j, pagebuf);
          _Bool clear = 1;
          for (int k = 0; k < PAGE_SIZE; ++k) {
            if (pagebuf[k] != -1) {
              clear = FALSE;
              break;
            }
          }
          if (clear) {
            strcpy(sectorbuf, "NULL");
          }
          else {
            strncpy(sectorbuf, pagebuf, SECTOR_SIZE);
          }
          if (j >= NONBUF_PAGES_PER_BLOCK) {
            printf("\033[0;33m");
          }
          printf("[ %10s ]", sectorbuf);
          if (j >= NONBUF_PAGES_PER_BLOCK) {
            printf("\033[0m");
          }
          printf(" ");
        }
        printf("\n");
      }
      printf("\n");
    }
    else if (strcmp(command, "write") == 0 || strcmp(command, "w") == 0) {
      char line[1024];
      int lsn = 0;
      while (1) {
        printf("lsn> ");
        fgets(line, 1024, stdin);
        lsn = atoi(line);
        if (lsn < 0 || lsn >= NONBUF_PAGES_PER_BLOCK * DATABLKS_PER_DEVICE) {
          printf("\"%d\" is invalid lsn.\n", lsn);
        }
        else
          break;
      }
      memset(sectorbuf, 0xFF, sizeof(sectorbuf));
      printf("data> ");
      fgets(sectorbuf, SECTOR_SIZE, stdin);
      sectorbuf[strlen(sectorbuf) - 1] = '\0';
      ftl_write(lsn, sectorbuf);
      printf("%s\n\n", sectorbuf);
    }
    else if (strcmp(command, "read") == 0 || strcmp(command, "r") == 0) {
      char line[1024];
      int lsn = 0;
      while (1) {
        printf("lsn> ");
        fgets(line, 1024, stdin);
        lsn = atoi(line);
        if (lsn < 0 || lsn >= NONBUF_PAGES_PER_BLOCK * DATABLKS_PER_DEVICE) {
          printf("\"%d\" is invalid lsn.\n", lsn);
        }
        else
          break;
      }
      memset(sectorbuf, 0xFF, sizeof(sectorbuf));
      ftl_read(lsn, sectorbuf);
      printf("%s\n\n", sectorbuf);
    }
    else if (strcmp(command, "clear") == 0 || strcmp(command, "c") == 0) {
      system("clear");
    }
    else if (strcmp(command, "help") == 0 || strcmp(command, "h") == 0) {
      printf("\033[0;32m도움!\033[0m\n\n");
      printf("\033[0;34m[w]rite\033[0m: write sector.\n");
      printf("\033[0;34m[r]ead\033[0m:  read sector.\n");
      printf("\033[0;34m[p]rint\033[0m: write print all datablocks.\n");
      printf("\033[0;34m[h]elp\033[0m:  print all commands.\n");
      printf("\033[0;34m[c]lear\033[0m: clear screen.\n");
      printf("\033[0;34me[x]it\033[0m:  exit program.\n\n");
    }
    else {
      printf("\"%s\" is invalid command!\n\n", command);
    }
  }

  fclose(devicefp);

  return 0;
}