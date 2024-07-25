#include "DSE.h"
#include <stdint.h>
#include "LRUCache.h"
#include "Cache.h"
#include <stddef.h>
#include "Common.h"


Design bestDesign;    // 最佳设计

Design *designs;      // 设计空间

int DesignNum;        // 设计空间总数

// 评估设计
void eval(Design *design) {
  // 生成一个Cache用于模拟
  Cache cache;
  // 函数指针
  void (*cache_init)(Cache *, int, int, int) = NULL;
  void (*cache_access)(Cache *, uint32_t, uint64_t *, uint64_t *, int , int, int) = NULL;
  // 根据替换策略选择函数
  switch(design->rp){
    case RP_LRU: {
      cache_init = LRUCache_init;
      cache_access = LRUCache_access;
    }
    default: {
      cache_init = LRUCache_init;
      cache_access = LRUCache_access;
    }
  }
  // 初始化Cache
  cache_init(&cache, design->SetNumLog2, design->WayNumLog2, design->CacheLineSizeLog2);
  // 循环读trace并统计Report
  for(int i = 0; i < traceLen; i++) {
    cache_access(&cache, trace[i], &(design->report.RequestCnt), &(design->report.HitCnt), design->SetNumLog2, design->WayNumLog2, design->CacheLineSizeLog2);
#ifdef CONFIG_PRUNING
    // 定义了剪枝
    // 如果设计方案的未命中次数已经大于了最佳方案的未命中次数，则不再继续
    if( (design->report.RequestCnt - design->report.HitCnt) > bestDesign.report.MissCnt ) {
      design->report.Score = 0;
      return;
    }
#endif
  }
  // 完成剩下的统计
  design->report.MissCnt = design->report.RequestCnt - design->report.HitCnt;
  // 计算分数
  design->report.Score = (design->report.HitCnt * 100.0) / design->report.RequestCnt;
}

// 统计设计空间数量
int countDesignNum() {
  int count = 0;
  for(int i = 2; i <= TotalSizeLog2; i++) {
    for(int j = 0; j <= (TotalSizeLog2-i); j++) {
      count++;
    }
  }
  return count;
}

// 生成设计空间
void generateDesignSpace() {
  int count = 0;
  for(int i = 2; i <= TotalSizeLog2; i++) {
    for(int j = 0; j <= (TotalSizeLog2-i); j++) {
      // i, j, (TotalSizeLog2-i-j)
      designs[count].CacheLineSizeLog2 = i;
      designs[count].SetNumLog2 = j;
      designs[count].WayNumLog2 = (TotalSizeLog2-i-j);
      designs[count].rp = RP_LRU;
      designs[count].report.RequestCnt = 0;
      designs[count].report.HitCnt = 0;
      count++;
    }
  }
}