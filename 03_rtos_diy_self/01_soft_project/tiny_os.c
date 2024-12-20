/**
 * @file tiny_os.c
 * @brief 
 * @author Leo ()
 * @version 1.0
 * @date 2024-11-06
 * 
 * @copyright Copyright (c) 2024  Leo 版权所有，禁止用于商业用途
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2024-11-06 <td>1.0     <td>Leo     <td>内容
 * </table>
 */
#include <stdio.h>
#include "tiny_os.h"
#include "ARMCM3.h"

/* 中断控制寄存器定义 */
#define NVIC_INT_CTRL_REG    0xE000ED04
#define PENDSV_INT_ENABLE    0x10000000

/* pendsv handle 优先级定义 */
#define NVIC_SYS_HAND_PRI    0xE000ED20
#define NVIC_PENDSV_SET      0x00FF0000

/* 寄存器写入操作 */
#define MEM32_SET(addr, val) (*(volatile uint32_t *)(addr) = (val))


/**
 * @brief 主要工作在 pendsv 当作运行
 */
void task_first_run(void)
{
    /* psp 指针初始化为 0，用于在 pendSV 当中来判断是不是第一个任务 */
    __set_PSP(0);
    /* 设置 pendsv 中断优先级 */
    MEM32_SET(NVIC_SYS_HAND_PRI, NVIC_PENDSV_SET);
    /* 触发 pendsv 中断 */
    MEM32_SET(NVIC_INT_CTRL_REG, PENDSV_INT_ENABLE);
}

/**
 * @brief 触发 pendsv 中断，进行任务调度
 */
void task_switch(void)
{
    /* 触发 pendsv 中断 */
    MEM32_SET(NVIC_INT_CTRL_REG, PENDSV_INT_ENABLE);
}

__asm void PendSV_Handler(void)
{
    /* 获取当前任务指针、与下一个要执行任务的指针 */
    IMPORT current_task
    IMPORT next_task

    /* 加载PSP指针，用于判断是不是第一个任务 */
    MRS R0, PSP
    /* PSP 为0，则跳转到 PendSV_Handler_nosave 运行 */
    CBZ R0, PendSV_Handler_nosave
	
    /* PSP 不为0，保存当前任务的上下文 */
	STMDB R0!, {R4-R11}
	/* 更新当前任务的堆栈指针 */
    LDR R1, = current_task
    LDR R1, [R1]
    STR R0, [R1]

PendSV_Handler_nosave
    /* current_task = next_task */
        /* 加载 current_task 的地址*/
    LDR R0, =current_task
        /* 加载 next_task 的地址*/
    LDR R1, =next_task
        /* 拿出next_task的地址里面的值，TASK *next_task = &task1; -----> 对应于 task1 的首地址 */
    LDR R2, [R1]
        /* 将task1的首地址，写入到 current_task 当中 */
    STR R2, [R0]

    /* 加载 task1 的首地址当中的值，对应于栈顶指针 */
    LDR R0, [R2]
    /* 将软件的上下文恢复, LDMIA 指令会将栈顶指针也进行移动 */
    LDMIA R0!, {R4-R11}
	
    /* 更新堆栈指针 */
	MSR PSP, R0
	/* 标识中断返回之后，使用的是 PSP 指针，而不是 MSP 指针 */
	ORR LR, LR, #0x04
	/* 这里的中断返回：其实就是触发中断异常退出序列，硬件会加载PC指针的值 */
    BX LR
}

/* todo: 应用任务申明 */
extern TASK task1;
extern TASK task2;
extern TASK idle_task;

/* 标识当前运行的任务 */ 
TASK *current_task = NULL;
/* 标识下一个要运行的任务 */
TASK *next_task = &task1;
/* 任务数组，类似于任务链表，便于访问管理 */
TASK *task_table[2] = {&task1, &task2};

TASK *p_idle_task = &idle_task;

/**
 * @brief 任务调度函数：选择下一个要运行的任务。（很简单的调度器）
 */
void task_sched(void)
{
    if (current_task == p_idle_task)
    {
        /* 遍历任务数组, 找到延时结束的任务 */
        for (int i = 0; i < 2; i++)
        {
            if (task_table[i]->delay_ticks == 0)
            {
                next_task = task_table[i];
                break;
            }
        }
    }
    /* task 1 */
    else if (current_task == task_table[0])
    {
        /* 如果 task 2 的延时结束，则选择 task 2 */
        if (task_table[1]->delay_ticks == 0)
        {
            next_task = task_table[1];
        }
        /* task2 延时触发，task1 延时也触发，则选择 idle task */
        else if (task_table[0]->delay_ticks != 0)
        {
            next_task = p_idle_task;
        }
        else
        {
            /* task2 延时触发，task1 延时还没触发，next_task 无需变化 */
        }
    }
    else if (current_task == task_table[1])
    {
        /* 如果 task 1 的延时结束，则选择 task 1 */
        if (task_table[0]->delay_ticks == 0)
        {
            next_task = task_table[0];
        }
        /* task1 延时触发，task2 延时也触发，则选择 idle task */
        else if (task_table[1]->delay_ticks != 0)
        {
            next_task = p_idle_task;
        }
        else
        {
            /* task1 延时触发，task2 延时还没触发，next_task 无需变化 */
        }
    }

    task_switch();
}

/**
 * @brief 初始化 systick 定时器
 */
int32_t systick_init(uint32_t period_ms)
{
    SysTick->LOAD = period_ms * SystemCoreClock / 1000 - 1;
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    SysTick->VAL = 0;
    // 设置 systick 定时器时钟源为 CPU 时钟，并使能定时器，并使能中断
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk 
                    | SysTick_CTRL_ENABLE_Msk 
                    | SysTick_CTRL_TICKINT_Msk;
	
	return 0;
}

/**
 * @brief systick 定时器中断处理函数
 */
void SysTick_Handler(void)
{
    /* 遍历任务数组，判断任务是否到期 */
    for (int i = 0; i < 2; i++)
    {
        if (task_table[i]->delay_ticks > 0)
        {
            task_table[i]->delay_ticks--;
        }
    }

    /* 调用调度函数 */
    task_sched();
}

void task_delay(uint32_t delay)
{
    current_task->delay_ticks = delay;

    /* 主动进行任务调度 */
    task_sched();
}
