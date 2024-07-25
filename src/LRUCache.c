#include "LRUCache.h"
#include "Common.h"
#include "stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


/*
 * 采用LRU替换算法的Cache初始化函数
 * 注意CacheLineSize要大于2
 */
void LRUCache_init(Cache *cache, int SetNumLog2, int WayNumLog2, int CacheLineSizeLog2) {
  // 初始化相关参数
  int SetNum = 1 << SetNumLog2;
  int WayNum = 1 << WayNumLog2;
  int CacheLineSize = 1 << CacheLineSizeLog2;
  // 初始化cache
  cache->sets = (CacheSet *)malloc(SetNum*sizeof(CacheSet));
  for(int i = 0; i < SetNum; i++) {
    cache->sets[i].ways = (Metadata *)malloc(WayNum*sizeof(Metadata));
    for(int j = 0; j < WayNum; j++) {
      cache->sets[i].ways[j].tag = 0;
      cache->sets[i].ways[j].valid = false;
      // LRUcache需要初始化count
      cache->sets[i].ways[j].other = (uint8_t *)malloc(sizeof(uint8_t));
      cache->sets[i].ways[j].other[0] = 0;
    }
  }
}

/*
 * 采用LRU替换算法的Cache访问函数
 */
void LRUCache_access(Cache *cache, uint32_t addr, uint64_t *RequestCnt, uint64_t *HitCnt, int SetNumLog2, int WayNumLog2, int CacheLineSizeLog2) {
  // printf("addr:%08x\n",addr);
  // printf("SetNumLog2:%d\n",SetNumLog2);
  // printf("WayNumLog2:%d\n",WayNumLog2);
  // printf("CacheLineSizeLog2:%d\n",CacheLineSizeLog2);
  int SetNum = 1 << SetNumLog2;
  int WayNum = 1 << WayNumLog2;
  int CacheLineSize = 1 << CacheLineSizeLog2;
  // 总事务请求次数增加
  (*RequestCnt)++;
  if((addr >= 0x30000000) && (addr <= 0x40000000)) return;
  // 划分域
  uint32_t tag    = extract(addr, 31                                , CacheLineSizeLog2 + SetNumLog2);
  uint32_t setidx = extract(addr, CacheLineSizeLog2 + SetNumLog2 - 1, CacheLineSizeLog2             );
  uint32_t offset = extract(addr, CacheLineSizeLog2 - 1             , 0                             );
  // 判断是否命中
  bool hit = false;
  uint32_t wayidx = 0;
  for(int i = 0; i < WayNum; i++) {
    if( (cache->sets[setidx].ways[i].tag==tag) && cache->sets[setidx].ways[i].valid ) {
      hit = true;
      wayidx = i;
      break;
    }
  }
  // 没命中替换
  if(!hit) {
    // 寻找count最大者 或 第一个不是valid的
    wayidx = 0;
    for(int i = 0; i < WayNum; i++) {
      if( cache->sets[setidx].ways[i].other[0] > cache->sets[setidx].ways[wayidx].other[0] ) wayidx = i;
      else if(!cache->sets[setidx].ways[i].valid) {
        wayidx = i;
        break;
      }
    }
    // 替换
    // printf("wayidx:%d",wayidx);
    cache->sets[setidx].ways[wayidx].tag = tag;
    cache->sets[setidx].ways[wayidx].valid = true;
    cache->sets[setidx].ways[wayidx].other[0] = 0;
  }
  else (*HitCnt)++;
  // 所有同组的count增加
  for(int i = 0; i < WayNum; i++){
    if(wayidx != i) {
      if(cache->sets[setidx].ways[i].other[0] < LRUCOUNTMAX) cache->sets[setidx].ways[i].other[0]++;
      else if(cache->sets[setidx].ways[i].other[0] > LRUCOUNTMAX) {
        printf("Error!\n");
        exit(0);
      }
    }
  }
  if(!hit) {
    if(cache->sets[setidx].ways[wayidx].other[0]!=0) exit(0);
  }
}