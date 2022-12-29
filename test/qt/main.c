#include "stdlib.h"
#include "stdio.h"
#include "scunit.h"
#include "csma.h"
#include "tzrandom.h"
#include "tztime.h"

static uint64_t gNow = 0;

static uint64_t getTime(void);
static void setTime(uint64_t time);
static void rxFrame(int num);

static void test_case0(void);
static void test_case1(void);

int main(void) {
    ScunitLoad(printf);
    ScunitAddSuite("test:csma");

    TZTimeLoad(getTime);

    //test_case0();
    test_case1();

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
    CsmaLoad(0x12, 1250);

    printf("-------------------->case1:测试开始\n");

    printf("-------------------->信道干净测试\n");
    int nextTime = CsmaGetNextSendTime();
    int lastTime = nextTime;
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("-------------------->信道10%忙碌测试\n");
    rxFrame(80);
    nextTime = CsmaGetNextSendTime();
    lastTime = nextTime;
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("-------------------->信道20%忙碌测试\n");
    setTime(nextTime + 10000);
    rxFrame(160);
    nextTime = CsmaGetNextSendTime();
    lastTime = nextTime;
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("-------------------->信道30%忙碌测试\n");
    setTime(nextTime + 10000);
    rxFrame(240);
    nextTime = CsmaGetNextSendTime();
    lastTime = nextTime;
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("-------------------->信道50%忙碌测试\n");
    rxFrame(400);
    nextTime = CsmaGetNextSendTime();
    lastTime = nextTime;
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("-------------------->信道100%忙碌测试\n");
    rxFrame(1000);
    nextTime = CsmaGetNextSendTime();
    lastTime = nextTime;
    for (int i = 0; i < 100; i++)
    {
        nextTime = CsmaGetNextSendTime();
        printf("%d.%03d-%d ", nextTime / 1000, nextTime % 1000, (nextTime - lastTime) / 1000);
        lastTime = nextTime;
        setTime(nextTime);
        CsmaSendEnd();
    }
    printf("\n");

    printf("-------------------->case1:测试结束\n");
}

static void rxFrame(int num) {
    for (int i = 0; i < num; i++) {
        CsmaReceiveOther();
    }
}
