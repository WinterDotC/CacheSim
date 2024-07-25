#ifndef __DSE_H__
#define __DSE_H__

// 设计空间探索 Design Space Exploration
#include <stdint.h>

// 设计报告
typedef struct {
  uint64_t RequestCnt;    // 总事务数量
  uint64_t HitCnt;        // Cache命中次数
  uint64_t MissCnt;       // 未命中次数
  uint64_t HitCost;       // 命中代价
  uint64_t MissCost;      // 未命中代价
  double Area;            // 面积
  double Freq;            // 频率
  double Score;           // 分数
} Report;

// 一个设计（解）
typedef struct {
  int SetNumLog2;         // CacheSet 大小
  int WayNumLog2;         // Way 大小
  int CacheLineSizeLog2;  // CacheLine的大小
  int rp;                 // 替换策略
  Report report;          // 设计对应的设计报告
} Design;


// 评估设计
void eval(Design *design);
// 统计设计空间数量
int countDesignNum();
// 生成设计空间
void generateDesignSpace();

// 最佳设计
extern Design bestDesign;

// 设计空间
extern Design *designs;

// 设计空间总数
extern int DesignNum;

// 设计空间探索时的一些可选配置
#define CONFIG_PRUNING 1

#endif