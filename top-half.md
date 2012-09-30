# The top half - hardware interrupt and the bottom half - software interrupt


During execution of most of hardware interrupt handler, interrupts are disable on local cpu to reduce likelihood of race condition.
That is reason that to divide the work of interrupt handler to
1. top half, which runs very fast with interrupt disable
2. bottom half, which do the remain work and can be preempt-able.




During several routine operations, the kernel checks whether any bottom halves are scheduled for execution. If any are waiting, the kernel runs the function do_softirq/invoke_softirq, to execute them.
The checks are performed during
1. exit_irq in do_IRQ: because the top half - hardware interrupt handler usually delegate some work to bottom half - software interrupt handler. So by calling software interrupt handler at exit of hardware interrupt handler, the latency is minimum.


fastcall unsigned int do_IRQ(struct pt_regs * regs) {
irq_enter( );
…
irq_exit( );
return 1;
}


void irq_exit(void) {
...
sub_preempt_count(IRQ_EXIT_OFFSET);
if (!in_interrupt( ) && local_softirq_pending( ))
invoke_softirq( );
preempt_enable_no_resched( );
}
1. return from interrupt and exception including system call
2. schedule
