// Copyright 2019-2022 The jdh99 Authors. All rights reserved.
// 载波监听模块
// Authors: jdh99 <jdh821@163.com>
// 无应答算法:提供高中低三种丢包率级别.丢包率越低,发送延时越高
// 有应答算法:
// 根据应答情况自动适应三种丢包率.有应答时处于高丢包率模式,应答丢失时逐渐往低丢包率模式靠拢
// 周围考虑到最多有300个点,有1/3的点要发送,保留70%的信道余量,则需要最大退避300个时隙

#include "csma.h"
#include "tzrandom.h"
#include "tztime.h"

static bool _is_ack_mode = false;
static uint32_t _slot_length = 0;
static CsmaPacketLossRate _loss_rate = CSMA_HIGH_PACKET_LOSS_RATE;
static uint8_t _power_index = 0;
static uint64_t _time_receive_other = 0;

// 有应答模式下的时延实习数
#define POWER_INDEX_MAX 10
static int gPowerIndexValue[POWER_INDEX_MAX] = {4, 9, 15, 22, 30, 40, 52, 66, 82, 100};

// 已计算的下次发送时间
static uint64_t _next_send_time = 0;

static void calc_next_send_time(uint64_t now);
static uint32_t calc_window_slot_num(void);
static uint32_t calc_ack_mode_window_slot_num(void);
static uint32_t calc_no_ack_mode_window_slot_num(void);

// CsmaConfigAckMode 设置有应答模式
// seed: 随机数种子
// slotLength: 时隙长度.单位: us
void CsmaConfigAckMode(uint32_t seed, uint32_t slotLength) {
    TZRandomSetSeed((int)seed);
    _is_ack_mode = true;
    _slot_length = slotLength;
    _power_index = 0;
}

// CsmaConfigNoAckMode 设置无应答模式
// seed: 随机数种子
// slotLength: 时隙长度.单位: us
// lossRate: 期望的丢包率级别
void CsmaConfigNoAckMode(uint32_t seed, uint32_t slotLength, CsmaPacketLossRate lossRate) {
    TZRandomSetSeed((int)seed);
    _is_ack_mode = false;
    _slot_length = slotLength;
    _loss_rate = lossRate;
}

// CsmaReceiveOther 接收到其他帧
// 接收到非发给本机的帧需要调用本函数
void CsmaReceiveOther(void) {
    _time_receive_other = TZTimeGet();
    calc_next_send_time(_time_receive_other);
}

static void calc_next_send_time(uint64_t now) {
    static uint64_t slientTime = 0;
    if (slientTime == 0) {
        slientTime = _slot_length * SLIENT_SLOT_NUM;
    }

    // 接收到一帧后,需要静默一段时间,避免跟应答帧碰撞
    uint32_t base = 0;
    uint64_t delta = now - _time_receive_other;
    if (delta < slientTime) {
        base = (uint32_t)(slientTime - delta);
    }

    uint32_t slot_num = calc_window_slot_num();
    _next_send_time = now + TZRandomGetRand(base, base + slot_num * _slot_length);
}

static uint32_t calc_window_slot_num(void)
{
    if (_is_ack_mode)
    {
        return calc_ack_mode_window_slot_num();
    }
    else
    {
        return calc_no_ack_mode_window_slot_num();
    }
}

static uint32_t calc_ack_mode_window_slot_num(void) {
    return gPowerIndexValue[_power_index];
}

static uint32_t calc_no_ack_mode_window_slot_num(void)
{
    uint32_t slot_num = 0;
    switch (_loss_rate)
    {
        case CSMA_HIGH_PACKET_LOSS_RATE:
        {
            slot_num = CSMA_SLOT_NUM_HIGH_LOSS_RATE;
            break;
        }
        case CSMA_MIDDLE_PACKET_LOSS_RATE:
        {
            slot_num = CSMA_SLOT_NUM_MIDDLE_LOSS_RATE;
            break;
        }
        default:
        {
            slot_num = CSMA_SLOT_NUM_LOW_LOSS_RATE;
            break;
        }
    }
    return slot_num;
}

// CsmaSendEnd 发送结束
// 无应答模式下发送结束后调用本函数
void CsmaSendEnd(void) {
    // 发送结束等效于接收到一帧,用于静默等待发送应答帧
    _time_receive_other = TZTimeGet();
    calc_next_send_time(TZTimeGet());
}

// CsmaSendSuccess 发送成功
// 有应答模式下发送成功后调用本函数
void CsmaSendSuccess(void) {
    if (_power_index > 0) {
        _power_index--;
    }
    calc_next_send_time(TZTimeGet());
}

// CsmaSendFailure 发送失败
// 有应答模式下发送失败后调用本函数
void CsmaSendFailure(void) {
    if (_power_index < POWER_INDEX_MAX - 1) {
        _power_index++;
    }
    calc_next_send_time(TZTimeGet());
}

// CsmaCalcNextSendTime 计算下次发送时间
// 返回的下次发送时间单位:us
uint64_t CsmaCalcNextSendTime(void) {
    // 如果已经计算了下次发送时间,则不需要再次计算
    uint64_t timeNow = TZTimeGet();
    if (_next_send_time > timeNow) {
        return _next_send_time;
    }
    calc_next_send_time(timeNow);
    return _next_send_time;
}

// CsmaGetNextSendTime 读取下次发送时间
// 返回的下次发送时间单位:us
uint64_t CsmaGetNextSendTime(void) {
    return _next_send_time;
}

// CsmaIsAllowSend 是否允许发送
bool CsmaIsAllowSend(void) {
    return TZTimeGet() >= _next_send_time;
}
