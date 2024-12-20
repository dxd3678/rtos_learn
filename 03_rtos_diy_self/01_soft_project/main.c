#include <stdint.h>
#include <stdio.h>
#include "tiny_os.h"


// 任务定义
TASK task1;
TASK task2;
TASK idle_task;

/* 每一个单元都是 4 字节，因为其存放的都是 R0-R15 这些寄存器的值，他们就是 4 字节 */
uint32_t task1_stack[1024];
uint32_t task2_stack[1024];
uint32_t idle_task_stack[1024];


int32_t task1_flag = 0;
void task1_handler(void *param)
{
	/* 在第一个任务初始化的时候调用，避免任务还没初始化好，从而导致中断触发 */
	systick_init(10);
	while (1)
	{
		task1_flag = 1;
		task_delay(10);
		task1_flag = 0;
		task_delay(10);
	}
}

int32_t task2_flag = 0;
void task2_handler(void *param)
{
	while (1)
	{
		task2_flag = 1;
		task_delay(10);
		task2_flag = 0;
		task_delay(10);
	}
}

void idle_task_handler(void *param)
{
	while (1)
	{
	}
}

/**
 * @brief 任务初始化函数：1.设置任务栈顶指针；2.伪造初始化现场；
 * @param  task    [in]
 * @param  entry_func [in]
 * @param  param   [in]
 * @param  stack   [in]
 * @param  stack_size [in]
 */
void task_init(TASK *task, void (*entry_func)(void *param), void *param, uint32_t *stack, uint32_t stack_size)
{
	/* 设置任务的栈顶指针 */
	task->stack_top = stack + stack_size;
	task->stack_size = stack_size;

	/* 伪造初始化现场, 硬件自动加载 */
	*(--task->stack_top) = (uint32_t)(1 << 24);		// xPSR, Thumb 模式
	*(--task->stack_top) = (uint32_t)entry_func;	// PC 指针，任务入口函数
	*(--task->stack_top) = (uint32_t)0x14;
	*(--task->stack_top) = (uint32_t)0x12;
	*(--task->stack_top) = (uint32_t)0x3;
	*(--task->stack_top) = (uint32_t)0x2;
	*(--task->stack_top) = (uint32_t)0x1;
	*(--task->stack_top) = (uint32_t)param;			// R0-R3 一般用于存放形参，

	/* 软件自己加载 */
	*(--task->stack_top) = (uint32_t)0x11;
	*(--task->stack_top) = (uint32_t)0x10;
	*(--task->stack_top) = (uint32_t)0x9;
	*(--task->stack_top) = (uint32_t)0x8;
	*(--task->stack_top) = (uint32_t)0x7;
	*(--task->stack_top) = (uint32_t)0x6;
	*(--task->stack_top) = (uint32_t)0x5;
	*(--task->stack_top) = (uint32_t)0x4;
}

int main(void)
{
	int32_t task1_param = 0x1234;
	int32_t task2_param = 0x5678;
	/* 初始化任务 */
	task_init(&task1, task1_handler, &task1_param, task1_stack, sizeof(task1_stack) / sizeof(uint32_t));
	task_init(&task2, task2_handler, &task2_param, task2_stack, sizeof(task2_stack) / sizeof(uint32_t));
	task_init(&idle_task, idle_task_handler, NULL, idle_task_stack, sizeof(idle_task_stack) / sizeof(uint32_t));

	/* 切换到第一个任务运行, 理论上来说，这个函数并不会返回，会不断地的调度任务 */
	task_first_run();

	return 0;
}
