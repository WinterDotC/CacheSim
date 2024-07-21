#include "Common.h"
#include <stdio.h>

// 配置选项
int SetNumLog2;                // 组的数量
int SetNum;
int WayNumLog2;                // 每组的CacheLine数量
int WayNum;
int CacheLineSizeLog2;         // CacheLine的大小
int CacheLineSize;

int MissPenalty;               // 缺失代价
int HitTime;                   // 命中时间

// 统计量
uint64_t HitCnt;               // Cache命中次数
uint64_t RequestCnt;           // Cache接收到的事务请求次数

// Cache
Cache cache;

/*
 * 从32位的数据中截取中间几位并返回
 */
uint32_t extract(uint32_t data, int h, int l) {
  // 先制作一个长度为 (h-l+1) 的全1
  int len = h - l + 1;
  uint32_t mask = 0xffffffff;
  mask = mask >> (32-len);
  mask = mask << l;
  return (mask&data)>>l;
}