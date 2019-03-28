#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");

int init_module(void) {
	struct task_struct *task = &init_task;
	do {
		printk( KERN_INFO "*ps* %s [%d]\n", task->comm, task->pid);
	} while ((task = next_task(task)) != &init_task );
	return 0;
}

void cleanup_module(void) {
	//printk(KERN_INFO "Goodbay user!\n");
}
