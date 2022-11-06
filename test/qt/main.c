#include "stdlib.h"
#include "stdio.h"
#include "scunit.h"
#include "csma.h"
#include "tzrandom.h"
#include "tztime.h"

static uint64_t gNow = 0;

static uint64_t getTime(void);
static void setTime(uint64_t time);

static void test_case0(void);
static void test_case1(void);
static void test_case2(void);

int main(void) {
    ScunitLoad(printf);
    ScunitAddSuite("test:csma");

    TZTimeLoad(getTime);

    //test_case0();
    //test_case1();
    test_case2();

    return 0;
}

static uint64_t getTime(void) {
    uint64_t t = gNow;
    gNow += 1000;
    return t;
}

static void setTime(uint64_t time) {
    gNow = time;
}

static void test_case0(void)
{
    printf("-------------------->case0:随机数测试开始\n");

    TZRandomSetSeed(123);
    printf("seed: %d max: %d\n", 13, 10000000);
    for (uint32_t j = 0; j < 100; j++)
    {
        printf("%d\t", TZRandomGetRand(0, 10000000));
    }
    printf("\n");

    TZRandomSetSeed(12);
    printf("seed: %d max: %d\n", 12, 131500);
    for (uint32_t j = 0; j < 100; j++)
    {
        printf("%d\t", TZRandomGetRand(6500, 131500));
    }
    printf("\n");

    for (uint32_t i = 1; i < 1; i++)
    {
        TZRandomSetSeed((int)i);
        printf("seed: %d max: %d\n", i, 12500);
        for (uint32_t j = 0; j < 100; j++)
        {
            printf("%d\t", TZRandomGetRand(1, 12500));
        }
        printf("\n");
    }

    TZRandomSetSeed(10000);
    printf("seed: %d max: %d\n", 10000, 62500);
    for (uint32_t j = 0; j < 100; j++)
    {
        printf("%d\t", TZRandomGetRand(1, 62500));
    }
    printf("\n");

    TZRandomSetSeed(31232);
    printf("seed: %d max: %d\n", 31232, 125000);
    for (uint32_t j = 0; j < 100; j++)
    {
        printf("%d\t", TZRandomGetRand(1, 125000));
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

    printf("---------->CSMA_HIGH_PACKET_LOSS_RATE max: 12.5\n");
    CsmaConfigNoAckMode(0x12, 1250, CSMA_HIGH_PACKET_LOSS_RATE);
    int nextTime = 0;
    int lastTime = 0;
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("---------->CSMA_MIDDLE_PACKET_LOSS_RATE max: 62.5\n");
    CsmaConfigNoAckMode(0x12, 1250, CSMA_MIDDLE_PACKET_LOSS_RATE);
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("---------->CSMA_LOW_PACKET_LOSS_RATE max: 125\n");
    CsmaConfigNoAckMode(0x12, 1250, CSMA_LOW_PACKET_LOSS_RATE);
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("---------->高丢包率仿真实场景测试\n");
    CsmaConfigNoAckMode(0x12, 1250, CSMA_HIGH_PACKET_LOSS_RATE);
    for (int i = 1; i < 10; i++)
    {
        printf("接收到其他帧\n");
        CsmaReceiveOther();
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d\n", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
    }

    printf("-------------------->case1:无应答模式测试结束\n");
}

static void test_case2(void)
{
    printf("-------------------->case1:应答模式测试开始\n");

    CsmaConfigAckMode(0x12, 1250);
    int nextTime = 0;
    int lastTime = 0;
    nextTime = CsmaGetNextSendTime();
    printf("send time: %d\n", nextTime);
    setTime(nextTime);

    printf("---------->CsmaSendSuccess max: 1250\n");
    for (int i = 0; i < 100; i++)
    {
        CsmaSendSuccess();
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("---------->CsmaSendFailure\n");
    for (int i = 0; i < 100; i++)
    {
        CsmaSendFailure();
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("---------->rand test\n");
    for (int i = 0; i < 100; i++)
    {
        if (TZRandomGetRand(1, 100) % 2 == 0)
        {
            CsmaSendSuccess();
            CsmaSendSuccess();
            nextTime = CsmaGetNextSendTime();
            printf("s:%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
            lastTime = nextTime;
            setTime(nextTime);
            CsmaSendEnd();
        }
        else
        {
            CsmaSendFailure();
            nextTime = CsmaGetNextSendTime();
            printf("f:%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
            lastTime = nextTime;
            setTime(nextTime);
            CsmaSendEnd();
        }
    }
    printf("\n");

    printf("-------------------->case1:应答模式测试结束\n");
}


