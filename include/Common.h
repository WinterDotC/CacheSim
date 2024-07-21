#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include "Cache.h"
#include "LRUCache.h"

// 配置选项
extern int SetNumLog2;                // 组的数量
extern int SetNum;
extern int WayNumLog2;                // 每组的CacheLine数量
extern int WayNum;
extern int CacheLineSizeLog2;         // CacheLine的大小
extern int CacheLineSize;

extern int MissPenalty;               // 缺失代价
extern int HitTime;                   // 命中时间

// 统计量
extern uint64_t HitCnt;               // Cache命中次数
extern uint64_t RequestCnt;           // Cache接收到的事务请求次数

// Cache
extern Cache cache;

uint32_t extract(uint32_t data, int h, int l);

#endif