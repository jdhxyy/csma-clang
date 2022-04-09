#include "stdlib.h"
#include "stdio.h"
#include "scunit.h"
#include "csma.h"
#include "tzrandom.h"

static void test_case0(void);
static void test_case1(void);
static void test_case2(void);

int main(void) {
    ScunitLoad(printf);
    ScunitAddSuite("test:csma");
    test_case0();
    test_case1();
    test_case2();

    return 0;
}

static void test_case0(void)
{
    printf("-------------------->case0:随机数测试开始\n");

    for (uint32_t i = 1; i < 10; i++)
    {
        TZRandomSetSeed((int)i);
        printf("seed: %d max: %d\n", i, 6250);
        for (uint32_t j = 0; j < 100; j++)
        {
            printf("%d\t", TZRandomGetRand(1, 6250));
        }
        printf("\n");
    }

    TZRandomSetSeed(10000);
    printf("seed: %d max: %d\n", 10000, 31250);
    for (uint32_t j = 0; j < 100; j++)
    {
        printf("%d\t", TZRandomGetRand(1, 31250));
    }
    printf("\n");

    TZRandomSetSeed(31232);
    printf("seed: %d max: %d\n", 31232, 62500);
    for (uint32_t j = 0; j < 100; j++)
    {
        printf("%d\t", TZRandomGetRand(1, 62500));
    }
    printf("\n");

    TZRandomSetSeed(65522);
    printf("seed: %d max: %d\n", 31232, 640000);
    for (uint32_t j = 0; j < 100; j++)
    {
        printf("%d\t", TZRandomGetRand(1, 640000));
    }
    printf("\n");

    TZRandomSetSeed(123);
    printf("seed: %d max: 0x%x\n", 31232, 0x12345678);
    uint32_t odd = 0;
    uint32_t even = 0;
    for (uint32_t j = 0; j < 1000000; j++)
    {
        if (TZRandomGetRand(1, 0x12345678) % 2 == 0)
        {
            even++;
        }
        else
        {
            odd++;
        }
    }
    printf("奇数数量:%d 偶数数量:%d\n", odd, even);

    printf("-------------------->case0:随机数测试结束\n");
}

static void test_case1(void)
{
    printf("-------------------->case1:无应答模式测试开始\n");

    printf("---------->CSMA_HIGH_PACKET_LOSS_RATE max: 6250\n");
    CsmaConfigNoAckMode(0x12, 625, CSMA_HIGH_PACKET_LOSS_RATE);
    int nextTime = 0;
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaCalcNextSendTime(nextTime);
        printf("%d\t", nextTime);
        nextTime += 1000;
    }
    printf("\n");

    printf("---------->CSMA_MIDDLE_PACKET_LOSS_RATE max: 31250\n");
    CsmaConfigNoAckMode(0x12, 625, CSMA_MIDDLE_PACKET_LOSS_RATE);
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaCalcNextSendTime(nextTime);
        printf("%d\t", nextTime);
        nextTime += 1000;
    }
    printf("\n");

    printf("---------->CSMA_LOW_PACKET_LOSS_RATE max: 62500\n");
    CsmaConfigNoAckMode(0x12, 625, CSMA_LOW_PACKET_LOSS_RATE);
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaCalcNextSendTime(nextTime);
        printf("%d\t", nextTime);
        nextTime += 1000;
    }
    printf("\n");

    printf("---------->高丢包率仿真实场景测试\n");
    CsmaConfigNoAckMode(0x12, 625, CSMA_HIGH_PACKET_LOSS_RATE);
    for (int i = 1; i < 10; i++)
    {
        printf("接收到其他帧\n");
        CsmaReceiveOther((uint64_t)i * 10);
        printf("计算:%d\n", CsmaCalcNextSendTime((uint64_t)i * 10));
    }

    printf("-------------------->case1:无应答模式测试结束\n");
}

static void test_case2(void)
{
    printf("-------------------->case1:应答模式测试开始\n");

    CsmaConfigAckMode(0x12, 625);
    int nextTime = 0;
    nextTime = CsmaCalcNextSendTime(nextTime);
    printf("send time: %d\n", nextTime);

    printf("---------->CsmaSendSuccess max: 1250\n");
    for (int i = 0; i < 100; i++)
    {
        CsmaSendSuccess();
        nextTime = CsmaCalcNextSendTime(nextTime);
        printf("%d\t", nextTime);
    }
    printf("\n");

    printf("---------->CsmaSendFailure\n");
    for (int i = 0; i < 100; i++)
    {
        CsmaSendFailure();
        nextTime = CsmaCalcNextSendTime(nextTime);
        printf("%d\t", nextTime);
    }
    printf("\n");

    printf("---------->rand test\n");
    for (int i = 0; i < 100; i++)
    {
        if (TZRandomGetRand(1, 100) % 2 == 0)
        {
            CsmaSendSuccess();
            nextTime = CsmaCalcNextSendTime(nextTime);
            printf("s:%d\t", nextTime);
        }
        else
        {
            CsmaSendFailure();
            nextTime = CsmaCalcNextSendTime(nextTime);
            printf("f:%d\t", nextTime);
        }
    }
    printf("\n");

    printf("-------------------->case1:应答模式测试结束\n");
}


