#ifndef __LRUCACHE_H__
#define __LRUCACHE_H__

#include <stdint.h>
#include "Cache.h"

#define LRUCOUNTMAX 16
// 初始化 LRU Cache
void LRUCache_init(Cache *cache, int SetNumLog2, int WayNumLog2, int CacheLineSizeLog2);

// 访问地址 addr
void LRUCache_access(Cache *cache, uint32_t addr, uint64_t *RequestCnt, uint64_t *HitCnt, int SetNumLog2, int WayNumLog2, int CacheLineSizeLog2);
#endif