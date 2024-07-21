#ifndef __LRUCACHE_H__
#define __LRUCACHE_H__

#include <stdint.h>

#define LRUCOUNTMAX 16

void LRUCache_init();                   // 初始化 LRU Cache
void LRUCache_access(uint32_t addr);    // 访问地址 addr
#endif