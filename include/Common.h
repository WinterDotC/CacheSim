#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include "Cache.h"
#include "LRUCache.h"
#include <stdio.h>

// 配置选项
extern int SetNumLog2;                // 组的数量
extern int WayNumLog2;                // 每组的CacheLine数量
extern int CacheLineSizeLog2;         // CacheLine的大小
extern int TotalSizeLog2;             // Cache的总大小
extern int rp;                        // 全局的替换策略

extern int MissPenalty;               // 缺失代价
extern int HitTime;                   // 命中时间

// 统计量
extern uint64_t HitCnt;               // Cache命中次数
extern uint64_t RequestCnt;           // Cache接收到的事务请求次数

// 全局的ITrace数组
extern uint32_t *trace;
extern uint32_t traceLen;

// 全局ITrace文件
extern FILE *binITraceFile;

#define RP_LRU 0

uint32_t extract(uint32_t data, int h, int l);

#endif