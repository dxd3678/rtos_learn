#include <stdint.h>

/* 中断控制寄存器定义 */
#define NVIC_INT_CTRL_REG    0xE000ED04
#define PENDSV_INT_ENABLE    0x10000000

/* pendsv handle 优先级定义 */
#define NVIC_SYS_HAND_PRI    0xE000ED20
#define NVIC_PENDSV_SET      0x00FF0000

/* 寄存器写入操作 */
#define MEM32_SET(addr, val) (*(volatile uint32_t *)(addr) = (val))

/* 栈数据结构抽象 */
typedef struct __stack_data__
{
	uint32_t *stack_ptr;
	uint32_t stack_size;
} STACK_DATA;

uint32_t stack_buf[128];

STACK_DATA stack = 
{
	.stack_ptr = &stack_buf[128],
	.stack_size = sizeof(stack_buf),
};

STACK_DATA *stack_p = &stack;


int main(void)
{
	/* 设置 pendsv 中断优先级 */
	MEM32_SET(NVIC_SYS_HAND_PRI, NVIC_PENDSV_SET);

	/* 触发 pendsv 中断 */
	MEM32_SET(NVIC_INT_CTRL_REG, PENDSV_INT_ENABLE);
	
	while (1);
	return 0;
}
