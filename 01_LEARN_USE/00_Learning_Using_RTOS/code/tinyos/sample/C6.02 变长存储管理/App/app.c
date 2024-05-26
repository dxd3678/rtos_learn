/**
 * @brief tOS应用示例
 * @details
 * @author 01课堂 李述铜 http://01ketang.cc
 * @date 2017-06-01
 * @version 1.0
 * @copyright 版权所有，禁止用于商业用途
 */
#include "tinyOS.h"
#include "app.h"
#include "hal.h"
#include <stdlib.h>


static tTask task1;                     // 任务1结构
static tTask task2;                     // 任务2结构
static tTask task3;                     // 任务3结构
static tTask task4;                     // 任务4结构

static tTaskStack task1Env[TASK1_ENV_SIZE];     // 任务1的堆栈空间
static tTaskStack task2Env[TASK2_ENV_SIZE];     // 任务2的堆栈空间
static tTaskStack task3Env[TASK3_ENV_SIZE];     // 任务3的堆栈空间
static tTaskStack task4Env[TASK4_ENV_SIZE];     // 任务4的堆栈空间

int task1Flag;           // 用于指示任务运行状态的标志变量
int task2Flag;           // 用于指示任务运行状态的标志变量
int task3Flag;           // 用于指示任务运行状态的标志变量
int task4Flag;           // 用于指示任务运行状态的标志变量

typedef struct __Packet {
    uint32_t size;
    uint8_t buffer[100];
} Packet;

Packet packetItems[20];
tMemBlock packetMemBlock;

void *packetBuffer[20];
tMbox packetMbox;

tMutex heapMutex;

/**
 * 任务的运行代码
 * @param param 任务初始运行参数
 */
void task1Entry (void *param) {
    Packet *packet;
    int i = 0;

    for (;;) {
				tMutexWait(&heapMutex, 0);
				packet = (Packet *)malloc(sizeof(Packet));
				tMutexNotify(&heapMutex);
			
			//tMemBlockWait(&packetMemBlock, (void **) &packet, 0);

        packet->size = i++;

        tMboxNotify(&packetMbox, packet, tMBOXSendNormal);
			
				tTaskDelay(1);
    }
}

/**
 * 任务的运行代码
 * @param param 任务初始运行参数
 */
void task2Entry (void *param) {
    Packet *packet;
    for (;;) {
        tMboxWait(&packetMbox, (void **) &packet, 0);

        xprintf("packet size:%d\n", packet->size);

        //tMemBlockNotify(&packetMemBlock, packet);
				tMutexWait(&heapMutex, 0);
				free(packet);
				tMutexNotify(&heapMutex);

    }
}

/**
 * 任务的运行代码
 * @param param 任务初始运行参数
 */
void task3Entry (void *param) {
    for (;;) {
        task3Flag = 1;
        tTaskDelay(1);
        task3Flag = 0;
        tTaskDelay(1);
    }
}


/**
 * 任务的运行代码
 * @param param 任务初始运行参数
 */
void task4Entry (void *param) {
    for (;;) {
        task4Flag = 1;
        tTaskDelay(1);
        task4Flag = 0;
        tTaskDelay(1);
    }
}

/**
 * App的初始化
 */
void tInitApp (void) {
    halInit();

    tMboxInit(&packetMbox, packetBuffer, 20);
    tMemBlockInit(&packetMemBlock, packetItems, sizeof(Packet), 20);

		tMutexInit(&heapMutex);
	
    tTaskInit(&task1, task1Entry, (void *) 0x0, TASK1_PRIO, task1Env, sizeof(task1Env));
    tTaskInit(&task2, task2Entry, (void *) 0x0, TASK2_PRIO, task2Env, sizeof(task2Env));
    tTaskInit(&task3, task3Entry, (void *) 0x0, TASK3_PRIO, task3Env, sizeof(task3Env));
    tTaskInit(&task4, task4Entry, (void *) 0x0, TASK4_PRIO, task4Env, sizeof(task4Env));
}

