// Copyright 2019-2022 The jdh99 Authors. All rights reserved.
// 载波监听模块
// Authors: jdh99 <jdh821@163.com>

#include "csma.h"
#include "tzrandom.h"

/**
* @brief 最大的幂指数
*/

#define MAX_POWER_INDEX                             10

static bool _is_ack_mode = false;
static uint32_t _slot_length = 0;
static CsmaPacketLossRate _loss_rate = CSMA_HIGH_PACKET_LOSS_RATE;
static uint8_t _power_index = 0;
static uint64_t _time_receive_other = 0;

// 已计算的下次发送时间
static uint64_t _next_send_time = 0;

static void calc_next_send_time(uint64_t time_now);
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
    _power_index = 1;
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
// 接收时间.单位：us
void CsmaReceiveOther(uint64_t time)
{
    _time_receive_other = time;
    if (_next_send_time > time) {
        calc_next_send_time(time);
    }
}

static void calc_next_send_time(uint64_t time_now) {
    uint32_t base = 0;
    // 接收到一帧后,需要静默一段时间
    if (time_now != 0 && _time_receive_other != 0 &&
        _time_receive_other <= time_now &&
        time_now - _time_receive_other < _slot_length * SLIENT_SLOT_NUM)
    {
        uint64_t temp = time_now - _time_receive_other;
        uint64_t slient_time = _slot_length * SLIENT_SLOT_NUM;
        if (slient_time > temp)
        {
            base = (uint32_t)(slient_time - temp);
        }
    }

    uint32_t slot_num = calc_window_slot_num();
    _next_send_time = time_now + TZRandomGetRand(base, base + slot_num * _slot_length);
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

static uint32_t calc_ack_mode_window_slot_num(void)
{
    if (_power_index == 0)
    {
        _power_index = 1;
    }
    if (_power_index > MAX_POWER_INDEX)
    {
        _power_index = MAX_POWER_INDEX;
    }
    return (uint32_t)(powl(2, _power_index));
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

// csma_send_success发送成功
// 有应答模式下发送成功后调用本函数
void CsmaSendSuccess(void)
{
    if (_power_index > 0)
    {
        _power_index--;
    }
}

// csma_send_failure发送失败
// 有应答模式下发送失败后调用本函数
void CsmaSendFailure(void)
{
    if (_power_index < MAX_POWER_INDEX)
    {
        _power_index++;
    }
}

// CsmaCalcNextSendTime 计算下次发送时间
// timeNow是当前时间.单位:us
uint64_t CsmaCalcNextSendTime(uint64_t timeNow) {
    // 如果已经计算了下次发送时间,则不需要再次计算
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

// CsmaIsBusy csma模块是否忙碌
// timeNow是当前时间.单位:us
// 忙碌时不应该再次计算下次发送时间
bool CsmaIsBusy(uint64_t timeNow) {
    return timeNow <= _next_send_time;
}
