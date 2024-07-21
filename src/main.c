#include "LRUCache.h"
#include "Common.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

char binITraceFileName[50];
FILE *binITraceFile;

// 矩阵
void (*cache_init)(void) = NULL;
void (*cache_access)(uint32_t) = NULL;

int main(int argc, char *argv[]){
  // 处理选项
  int opt = 0;
  while((opt = getopt(argc, argv, "f:s::w::c::r::h")) != -1) {
    switch(opt) {
      case 'f': {
        sprintf(binITraceFileName,"%s", optarg);
        break;
      }
      case 's': {
        SetNumLog2 = atoi(optarg);
        break;
      }
      case 'w': {
        WayNumLog2 = atoi(optarg);
        break;
      }
      case 'c': {
        CacheLineSizeLog2 = atoi(optarg);
        break;
      }
      case 'r': {
        if(strcmp(optarg,"lru")==0){
          cache_init = LRUCache_init;
          cache_access = LRUCache_access;
        }
        break;
      }
      default: {
        printf("Unknown opt %c!",opt);
        printf("Useage:\n");
        printf("-f : Set the filename of the bin file.\n");
        printf("-s : Set the log_2(SetNum), default is 4.\n");
        printf("-w : Set the log_2(WayNum), default is 0.\n");
        printf("-c : Set the log_2(CacheLineSize), default is 2.\n");
        printf("-h : Show the useage.\n");
        exit(0);
      }
    }
  }
  if(binITraceFileName == NULL) {
    printf("Please use -f to set the bin file!\n");
    exit(0);
  }
  if(cache_init == NULL) {
    printf("Replacement Algorithm not Found!\n");
    exit(0);
  }

  cache_init();
  printf("Cache Init!\n");
  // 读文件
  binITraceFile = fopen(binITraceFileName, "rb");
  uint32_t addr = 0;
  int num = 0;
  while((num=(fread(&addr, sizeof(uint32_t), 1, binITraceFile))) != 0) {
    cache_access(addr);
  }
  printf("Hit: %"PRIu64"\n", HitCnt);
  printf("Requst:%"PRIu64"\n",RequestCnt);
  printf("Hitrate:%f\n",(HitCnt*100.0)/RequestCnt);
  exit(0);
}