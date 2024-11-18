/**
 * @file tiny_os.h
 * @brief 
 * @author Leo ()
 * @version 1.0
 * @date 2024-11-04
 * 
 * @copyright Copyright (c) 2024  Leo 版权所有，禁止用于商业用途
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2024-11-04 <td>1.0     <td>Leo     <td>内容
 * </table>
 */

#ifndef __TINY_OS_H__
#define __TINY_OS_H__
#include <stdint.h>

typedef struct
{
    uint32_t *stack_top;  // 栈顶指针
    uint32_t stack_size;  // 栈大小
    uint32_t delay_ticks; // 延迟时间
} TASK;

void task_first_run(void);
void task_switch(void);

int32_t systick_init(uint32_t period_ms);

void task_delay(uint32_t delay);

#endif // __TINY_OS_H__
