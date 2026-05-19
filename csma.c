// Copyright 2019-2022 The jdh99 Authors. All rights reserved.
// 载波监听模块
// Authors: jdh99 <jdh821@163.com>
// 使用忙碌度来设计算法

#include "csma.h"
#include "tzrandom.h"
#include "tztime.h"
#include "async.h"

#pragma pack(1)

// tRxWindow 接收窗口
typedef struct {
    uint64_t startTime;
    int num;
} tRxWindow;

#pragma pack()

typedef enum {
    S_FREE,
    S_READY,
    S_SEND
} tState;

// 参数
static CsmaParam gParam = {0};

static uint64_t gTimeRxOther = 0;
static int gSlotNumSecond = 0;
static tState gState = S_FREE;

// 接收窗口
static tRxWindow gOldWindow = {0};
static tRxWindow gNewWindow = {0};

// 忙碌度
static int gBusyRatio = 0;

static uint64_t gLockTime = 0;

// 已计算的下次发送时间
static uint64_t gNextSendTime = 0;

// 静默时间.单位:us
static uint64_t gSlientTime = 0;

static int task(void);

static void calcNextSendTime(uint64_t now);
static uint32_t calcWindowSlotNum(uint64_t now);

// CsmaLoad 模块载入
// param: 参数结构体
bool CsmaLoad(CsmaParam *param) {
    if (param == NULL) {
        return false;
    }

    gParam = *param;
    gSlientTime = gParam.SlotLen * gParam.SlientSlotNum;
    TZRandomSetSeed((int)gParam.Seed);
    gSlotNumSecond = 1000000 / gParam.SlotLen;

    if (AsyncStart(task, ASYNC_NO_WAIT) == false) {
        return false;
    }

    return true;
}

static int task(void) {
    static struct pt pt = {0};
    static uint64_t time = 0;

    PT_BEGIN(&pt);

    PT_WAIT_UNTIL(&pt, gState == S_READY);

    // 超时自动解锁
    time = TZTimeGet();
    if (time - gLockTime > gParam.LockTimeout * 1000) {
        gState = S_FREE;
        PT_EXIT(&pt);
    }

    // 计算是否到发送时间
    if (time > gNextSendTime) {
        gState = S_SEND;
        PT_EXIT(&pt);
    }

    PT_END(&pt);
}

// CsmaLock 锁定
bool CsmaLock(void) {
    if (gState != S_FREE) {
        return false;
    }

    gState = S_READY;
    gLockTime = TZTimeGet();
    calcNextSendTime(gLockTime);
    return true;
}

static void calcNextSendTime(uint64_t now) {
    // 接收到一帧后,需要静默一段时间,避免跟应答帧碰撞
    uint32_t base = 0;
    uint64_t delta = now - gTimeRxOther;
    if (delta < gSlientTime) {
        base = (uint32_t)(gSlientTime - delta);
    }

    uint32_t slotNum = calcWindowSlotNum(now);
    gNextSendTime = now + TZRandomGetRand(base, base + slotNum * gParam.SlotLen);
}

static uint32_t calcWindowSlotNum(uint64_t now) {
    // 计算出繁忙率:b = n / (w / 1000 * slotNum) * 100
    uint32_t w = (now - gOldWindow.startTime) / 1000;
    uint32_t n = gOldWindow.num + gNewWindow.num;
    uint32_t b = 100000 / gSlotNumSecond * n / w;

    if (b >= CSMA_BACKOFF_WINDOW_LEN) {
        b = CSMA_BACKOFF_WINDOW_LEN - 1;
    }
    gBusyRatio = b;
    return (uint32_t)gParam.BackoffWindow[b];
}

// CsmaIsAllowSend 是否允许发送.获取到锁之后才能调用本函数
bool CsmaIsAllowSend(void) {
    switch (gState) {
        case S_FREE: {
            CsmaLock();
            return false;
        }
        case S_READY: {
            return false;
        }
        case S_SEND:
        default: {
            return true;
        }
    }
}

// CsmaUnlock 解锁
void CsmaUnlock(void) {
    if (gState != S_FREE) {
        gState = S_FREE;
    }

    // 默认解锁应该是发送完成,所以需要静默等待发送应答帧,等效于接收到一帧
    gTimeRxOther = TZTimeGet();
    CsmaReceive();
}

// CsmaReceive 接收到其他帧
void CsmaReceive(void) {
    uint64_t now = TZTimeGet();
    if (now - gNewWindow.startTime > gParam.RxWindow) {
        gOldWindow = gNewWindow;
        gNewWindow.startTime = now;
        gNewWindow.num = 0;
    }
    gNewWindow.num++;
    gTimeRxOther = now;

    if (gState == S_READY) {
        calcNextSendTime(now);
    }
}

// CsmaReceiveFail 接收失败
// 如果发送的是有应答帧,接收失败可调用本函数
void CsmaReceiveFail(void) {
    CsmaReceive();
}

// CsmaGetBusyRatio 读取忙碌度.忙碌度取值0-100
int CsmaGetBusyRatio(void) {
    static int window = 0;

    if (window == 0) {
        window = gParam.RxWindow * 2;
    }

    uint64_t now = TZTimeGet();
    if (now - gNewWindow.startTime > window) {
        return 0;
    }
    return gBusyRatio <= 100 ? gBusyRatio : 100;
}
