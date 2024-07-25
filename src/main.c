#include "LRUCache.h"
#include "Common.h"
#include "DSE.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>

bool DSEFlag = false;               // 是否进行设计空间探索
int TotalSizeLog2 = 6;              // 如果进行设计空间探索，所有CacheLine的总大小
int ThreadNum = 1;

char binITraceFileName[50];
FILE *binITraceFile;

// 全局的ITrace数组
uint32_t *trace;
uint32_t traceLen;

// 多线程相关
pthread_mutex_t mutex;
int curDesignID;        // 目前需要评估的设计编号

void parseArg(int argc, char *argv[]) {
  // 处理选项
  int opt = 0;
  while((opt = getopt(argc, argv, "f:s::w::c::r::E::T::j::h")) != -1) {
    switch(opt) {
      case 'f': {
        sprintf(binITraceFileName,"%s", optarg);
        break;
      }
      case 's': {
        SetNumLog2 = atoi(optarg);
        break;
      }
      case 'w': {
        WayNumLog2 = atoi(optarg);
        break;
      }
      case 'c': {
        CacheLineSizeLog2 = atoi(optarg);
        break;
      }
      case 'r': {
        if(strcmp(optarg,"lru")==0){
          rp = RP_LRU;
        }
        break;
      }
      case 'E': {
        DSEFlag = true;
        break;
      }
      case 'T': {
        TotalSizeLog2 = atoi(optarg);
        break;
      }
      case 'j': {
        ThreadNum = atoi(optarg);
        break;
      }
      default: {
        printf("Unknown opt %c!",opt);
        printf("Useage:\n");
        printf("-f : Set the filename of the bin file.\n");
        printf("-s : Set the log_2(SetNum), default is 4.\n");
        printf("-w : Set the log_2(WayNum), default is 0.\n");
        printf("-c : Set the log_2(CacheLineSize), default is 2.\n");
        printf("-r : Set the Replacement Police, default is 'lsu'.\n");
        printf("-E : Start Design Space Exploration.\n");
        printf("-T : Set the log_2(total size) of cache, default is 6.\n");
        printf("-j : Set the num of the thread, default is 1 (Max is 16).\n");
        printf("-h : Show the useage.\n");
        exit(0);
      }
    }
  }
}

void init_sys() {
  // 初始化系统
#ifdef CONFIG_PRUNING
  bestDesign.report.MissCnt = 0xFFFFFFFFFFFFFFFF;
#endif
  // 验证文件是否存在
  if(binITraceFileName == NULL) {
    printf("Please use -f to set the bin file!\n");
    exit(0);
  }
  // 打开文件
  binITraceFile = fopen(binITraceFileName, "rb");
  // 查看文件大小
  fseek(binITraceFile, 0, SEEK_END);
  traceLen = ftell(binITraceFile);
  fseek(binITraceFile, 0, SEEK_SET);
  // 
  if( (traceLen & 0x3) != 0 ) {
    printf("The binfile is not Aligned!\n");
    exit(0);
  }
  traceLen = traceLen >> 2;
  trace = (uint32_t *)malloc(sizeof(uint32_t)*traceLen);
  fread(trace, sizeof(uint32_t), traceLen, binITraceFile);
  printf("Sys init!\n");
}

void *worker(void *arg) {
  // 工人函数
  bool endflag = false;
  int DesignID = 0;
  while(true){
    // 取任务
    pthread_mutex_lock(&mutex);
    if(curDesignID >= DesignNum) endflag = true;
    else {
      DesignID = curDesignID;
      curDesignID++;
    }
    pthread_mutex_unlock(&mutex);
    if(endflag) break;    // 所有设计空间均已得到探索
    // 探索设计空间
    eval(&(designs[DesignID]));
  }
}

int main(int argc, char *argv[]){
  // 处理参数
  parseArg(argc, argv);
  // 初始化系统
  init_sys();
  if(DSEFlag) { // 进行设计空间探索
    if(TotalSizeLog2 < 2) {
      printf("TotalSizeLog2 should large than 2!\n");
      exit(0);
    }
    // 控制长度
    DesignNum = countDesignNum();
    designs = (Design *)malloc(sizeof(Design)*DesignNum);
    // 生成设计空间
    generateDesignSpace();
    FILE *reportfile = fopen("CacheSimReport.txt","w");
    if(ThreadNum==1){   // 单线程
      for(int i = 0; i < DesignNum; i++){
        eval(&(designs[i]));
        // 输出评估结果
        // printf("Design: %d %d %d\n",designs[i].SetNumLog2, designs[i].WayNumLog2, designs[i].CacheLineSizeLog2);
        // printf("HitCnt:%"PRIu64"\n",designs[i].report.HitCnt);
        // printf("RequestCnt:%"PRIu64"\n",designs[i].report.RequestCnt);
        // printf("HitRate:%lf\n",designs[i].report.Score);
        fprintf(reportfile, "%d, %d, %d, %lf\n",
          designs[i].SetNumLog2,
          designs[i].WayNumLog2,
          designs[i].CacheLineSizeLog2,
          designs[i].report.Score);
        if(designs[i].report.Score > bestDesign.report.Score) {
          bestDesign = designs[i];
        }
      }
    }
    else {              // 多线程
      // 初始化目前需要评估的设计编号
      curDesignID = 0;
      // 初始化互斥锁
      pthread_mutex_init(&mutex, NULL);
      pthread_t threads[16];
      int ret;
      for(int i = 0; i < ThreadNum; i++) {
        ret = pthread_create(&threads[i], NULL, worker, NULL);
      }
      // 同步
      for(int i = 0; i < ThreadNum; i++) {
        ret = pthread_join(threads[i], NULL);
      }
      // 释放互斥锁
      pthread_mutex_destroy(&mutex);
      // 多线程评估完毕
      for(int i = 0; i < DesignNum; i++){
        // 输出评估结果
        // printf("Design: %d %d %d\n",designs[i].SetNumLog2, designs[i].WayNumLog2, designs[i].CacheLineSizeLog2);
        // printf("HitCnt:%"PRIu64"\n",designs[i].report.HitCnt);
        // printf("RequestCnt:%"PRIu64"\n",designs[i].report.RequestCnt);
        // printf("HitRate:%lf\n",designs[i].report.Score);
        fprintf(reportfile, "%d, %d, %d, %lf\n",
          designs[i].SetNumLog2,
          designs[i].WayNumLog2,
          designs[i].CacheLineSizeLog2,
          designs[i].report.Score);
        if(designs[i].report.Score > bestDesign.report.Score) {
          bestDesign = designs[i];
        }
      }
    }
    // 写入最优版本
    fprintf(reportfile, "Best Design:\n");
    fprintf(reportfile, "SetNumLog2:%d\n", bestDesign.SetNumLog2);
    fprintf(reportfile, "WayNumLog2:%d\n", bestDesign.WayNumLog2);
    fprintf(reportfile, "CacheLineSizeLog2:%d\n", bestDesign.CacheLineSizeLog2);
    fprintf(reportfile, "HitCnt:%"PRIu64"\n", bestDesign.report.HitCnt);
    fprintf(reportfile, "RequestCnt:%"PRIu64"\n", bestDesign.report.RequestCnt);
    fprintf(reportfile, "HitRate:%lf\n", bestDesign.report.Score);
    fclose(reportfile);
  }
  else {        // 评估一个单独的设计
    // 生成一个设计
    Design design;
    design.SetNumLog2 = SetNumLog2;
    design.WayNumLog2 = WayNumLog2;
    design.CacheLineSizeLog2 = CacheLineSizeLog2;
    design.rp = RP_LRU;
    design.report.RequestCnt = 0;
    design.report.HitCnt = 0;
    // 评估设计
    eval(&design);
    // 输出评估结果
    printf("HitCnt:%"PRIu64"\n",design.report.HitCnt);
    printf("RequestCnt:%"PRIu64"\n",design.report.RequestCnt);
    printf("HitRate:%lf\n",design.report.Score);
  }
  return 0;
}