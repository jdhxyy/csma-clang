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

// 监听到其他帧后静默时间.单位：时隙个数
#define CSMA_SLIENT_SLOT_NUM 10

// 锁定超时时间,超时自动解锁.单位:ms
#define CSMA_LOCK_TIMEOUT 2000

// 接收时间窗口.单位:us
#define CSMA_RX_WINDOW 5000000

// CsmaLoad 模块载入
// seed: 随机数种子
// slotLen: 时隙长度.单位: us
bool CsmaLoad(uint32_t seed, uint32_t slotLen);

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
