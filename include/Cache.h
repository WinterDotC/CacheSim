#ifndef __CACHE_H_ 
#define __CACHE_H_ 

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint32_t  tag;            // 用以比较
  bool      valid;          // 用以判断是否合法
  uint8_t   *other;         // 不同种替换算法所需的域，需要自行开辟大小
} Metadata;

typedef struct {
  Metadata *ways;       // 每个Set是多路的，数量由Way决定
} CacheSet;

typedef struct {
  CacheSet *sets;
} Cache;

#endif 