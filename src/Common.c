#include "Common.h"
#include <stdio.h>

// 配置选项
int SetNumLog2 = 4;                // 组的数量
int WayNumLog2 = 0;                // 每组的CacheLine数量
int CacheLineSizeLog2 = 2;         // CacheLine的大小
int rp = RP_LRU;                   // 全局替换策略 
/*
 * 从32位的数据中截取中间几位并返回
 */
uint32_t extract(uint32_t data, int h, int l) {
  // 先制作一个长度为 (h-l+1) 的全1
  if(h<l) return 0;
  int len = h - l + 1;
  uint32_t mask = 0xffffffff;
  mask = mask >> (32-len);
  mask = mask << l;
  return (mask&data)>>l;
}