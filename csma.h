// Copyright 2019-2022 The jdh99 Authors. All rights reserved.
// 载波监听模块
// Authors: jdh99 <jdh821@163.com>
// 本模块调用方法:
// 1.调用CsmaLock获取权限
// 2.调用CsmaIsAllowSend等待发送
// 3.发送完成调用CsmaUnlock解锁

#ifndef CSMA_H
#define CSMA_H

#include <stdbool.h>
#include <math.h>
#include <stdint.h>

// 退避时隙窗口长度
#define CSMA_BACKOFF_WINDOW_LEN 31

// BackoffWindow中存放的是避让时隙,对应的是忙碌度,从0%~30%,共31个值
// BackoffWindow中是对应忙碌度的避让时隙,以下是1250us时隙下的避让时隙示例:
// BackoffWindow[CSMA_BACKOFF_WINDOW_LEN] = {2, 
//     4,   9,   14,  19,  25,  32,  39,  47,  56,  67,
//     78,  92,  106, 123, 142, 163, 186, 211, 239, 270,
//     303, 340, 379, 422, 469, 519, 572, 630, 692, 758
// };

// CsmaParam 载入参数
typedef struct {
    // 随机数种子
    uint32_t Seed;
    // 时隙长度.单位: us
    uint32_t SlotLen;
    // 监听到其他帧后静默时间.单位：时隙个数
    uint32_t SlientSlotNum;
    // 锁定超时时间,超时自动解锁.单位:ms
    uint32_t LockTimeout;
    // 接收时间窗口.单位:us
    uint32_t RxWindow;
    // 退避时隙窗口.对应忙碌度0-30%,共31个值
    uint16_t BackoffWindow[CSMA_BACKOFF_WINDOW_LEN];
} CsmaParam;

// CsmaLoad 模块载入
// param: 参数结构体
bool CsmaLoad(CsmaParam *param);

// CsmaRun 模块运行
int CsmaRun(void);

// CsmaLock 锁定
bool CsmaLock(void);

// CsmaIsAllowSend 是否允许发送.获取到锁之后才能调用本函数
bool CsmaIsAllowSend(void);

// CsmaUnlock 解锁
void CsmaUnlock(void);

// CsmaReceive 接收到其他帧
void CsmaReceive(void);

// CsmaReceiveFail 接收失败
// 如果发送的是有应答帧,接收失败可调用本函数
void CsmaReceiveFail(void);

// CsmaGetBusyRatio 读取忙碌度.忙碌度取值0-100
int CsmaGetBusyRatio(void);

#endif
