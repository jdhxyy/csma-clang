// Copyright 2019-2022 The jdh99 Authors. All rights reserved.
// 载波监听模块
// Authors: jdh99 <jdh821@163.com>

#ifndef CSMA_H
#define CSMA_H

#include <stdbool.h>
#include <math.h>
#include <stdint.h>

// 不同丢包率级别下的最大监听时隙数
#define CSMA_SLOT_NUM_HIGH_LOSS_RATE 10
#define CSMA_SLOT_NUM_MIDDLE_LOSS_RATE 50
#define CSMA_SLOT_NUM_LOW_LOSS_RATE 100

// 监听到其他帧后静默时间.单位：时隙个数
#define SLIENT_SLOT_NUM 6

// CsmaPacketLossRate 丢包率级别
typedef enum
{
    CSMA_HIGH_PACKET_LOSS_RATE = 0,
    CSMA_MIDDLE_PACKET_LOSS_RATE,
    CSMA_LOW_PACKET_LOSS_RATE
} CsmaPacketLossRate;

// CsmaConfigAckMode 设置有应答模式
// seed: 随机数种子
// slotLength: 时隙长度.单位: us
void CsmaConfigAckMode(uint32_t seed, uint32_t slotLength);

// CsmaConfigNoAckMode 设置无应答模式
// seed: 随机数种子
// slotLength: 时隙长度.单位: us
// lossRate: 期望的丢包率级别
void CsmaConfigNoAckMode(uint32_t seed, uint32_t slotLength, CsmaPacketLossRate lossRate);

// CsmaReceiveOther 接收到其他帧
// 接收到非发给本机的帧需要调用本函数
// 接收时间.单位：us
void CsmaReceiveOther(uint64_t time);

// CsmaSendSuccess 发送成功
// 有应答模式下发送成功后调用本函数
void CsmaSendSuccess(void);

// CsmaSendFailure 发送失败
// 有应答模式下发送失败后调用本函数
void CsmaSendFailure(void);

// CsmaCalcNextSendTime 计算下次发送时间
// timeNow是当前时间.单位:us
uint64_t CsmaCalcNextSendTime(uint64_t timeNow);

// CsmaGetNextSendTime 读取下次发送时间
// 返回的下次发送时间单位:us
uint64_t CsmaGetNextSendTime(void);

// CsmaIsBusy csma模块是否忙碌
// timeNow是当前时间.单位:us
// 忙碌时不应该再次计算下次发送时间
bool CsmaIsBusy(uint64_t timeNow);

#endif
