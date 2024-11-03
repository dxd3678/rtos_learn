__asm void PendSV_Handler(void)
{
    IMPORT stack_p

	/* step1:将 R4-R11 压入栈 buffer */
    LDR R0, =stack_p	// 加载标号的首地址，R0 = &stack_p
    LDR R0, [R0]		// R0 = *stack_p = stack.stack_ptr
    LDR R0, [R0]		// R0 = *(*stack_p) = *(stack.stack_ptr) = stack_buf[128]
	
	LDR R4, =4			// 验证压栈效果
	LDR R5, =5
	LDR R11, =11

    STMDB R0!, {R4-R11} // stack_buf[128] = R11、R0 = R0--; stack_buf[127] = R10、R0 = R0--; ...

	/* step2: 更新栈顶指针 */
    LDR R1, =stack_p	// 加载变量的首地址 
    LDR R1, [R1]		// R0 = *stack_p = stack.stack_ptr
    STR R0, [R1]		// 相当于给 stack.stack_ptr 进行赋值, stack.stack_ptr = R0
	
	/* step3: 从栈当中拿出 R4-R11 复制到对应寄存器  */
	ADD R4, R4, #1
	ADD R5, R5, #1
	LDMIA	R0!, {R4-R11}
	
	/* step4: 更新栈顶指针 */
	LDR R1, =stack_p
	LDR R1, [R1]
	STR R0, [R1]
	
	/* step5: 返回函数 */
	BX LR
}
