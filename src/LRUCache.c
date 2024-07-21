#include "LRUCache.h"
#include "Common.h"
#include "stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


/*
 * 采用LRU替换算法的Cache初始化函数
 */
void LRUCache_init() {
  // 初始化相关参数
  SetNum = 1 << SetNumLog2;
  WayNum = 1 << WayNumLog2;
  CacheLineSize = 1 << CacheLineSizeLog2;
  // 初始化cache
  cache.sets = (CacheSet *)malloc(SetNum*sizeof(CacheSet));
  for(int i = 0; i < SetNum; i++) {
    cache.sets[i].ways = (Metadata *)malloc(WayNum*sizeof(Metadata));
    for(int j = 0; j < WayNum; j++) {
      cache.sets[i].ways[j].tag = 0;
      cache.sets[i].ways[j].valid = false;
      // LRUcache需要初始化count
      cache.sets[i].ways[j].other = (uint8_t *)malloc(sizeof(uint8_t));
      cache.sets[i].ways[j].other[0] = 0;
    }
  }
}

/*
 * 采用LRU替换算法的Cache访问函数
 */
void LRUCache_access(uint32_t addr) {
  // 总事务请求次数增加
  if((addr >= 0x30000000) && (addr <= 0x40000000)) return;
  RequestCnt++;
  // 划分域
  uint32_t tag    = extract(addr, 31                                , CacheLineSizeLog2 + SetNumLog2);
  uint32_t setidx = extract(addr, CacheLineSizeLog2 + SetNumLog2 - 1, CacheLineSizeLog2             );
  uint32_t offset = extract(addr, CacheLineSizeLog2 - 1             , 0                             );
  // 判断是否命中
  bool hit = false;
  uint32_t wayidx = 0;
  for(int i = 0; i < WayNum; i++) {
    if( (cache.sets[setidx].ways[i].tag==tag) && cache.sets[setidx].ways[i].valid ) {
      hit = true;
      wayidx = i;
      break;
    }
  }
  // 没命中替换
  if(!hit) {
    // 寻找count最大者
    wayidx = 0;
    for(int i = 0; i < WayNum; i++) {
      if( cache.sets[setidx].ways[i].other[0] > cache.sets[setidx].ways[wayidx].other[0] ) wayidx = i;
    }
    // 替换
    cache.sets[setidx].ways[wayidx].tag = tag;
    cache.sets[setidx].ways[wayidx].valid = true;
    cache.sets[setidx].ways[wayidx].other[0] = 0;
  }
  else HitCnt++;
  // 所有同组的count增加
  for(int i = 0; i < WayNum; i++){
    if(wayidx != i) {
      if(cache.sets[setidx].ways[i].other[0] < LRUCOUNTMAX) cache.sets[setidx].ways[wayidx].other[0]++;
    }
  }
}