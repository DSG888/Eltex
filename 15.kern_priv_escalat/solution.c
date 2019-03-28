#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");

static struct cred* newcred;

int init_module(void) {
	struct task_struct *task = &init_task;
	do {
		if (!strcmp(task->comm, "brazenapp")) {
			printk( KERN_INFO "*ps* %s [%d] %d\n", task->comm, task->pid, task->cred->uid.val);

			newcred = (struct cred *)__task_cred(task);
		//	newcred->uid = (kuid_t){0};		//real UID of the task
		//	newcred->gid = (kgid_t){0};		//real GID of the task
		//	newcred->euid = (kuid_t){0};	//saved UID of the task
		//	newcred->egid = (kgid_t){0};	//saved GID of the task
		//	newcred->suid = (kuid_t){0};	//effective UID of the task
		//	newcred->sgid = (kgid_t){0};	//effective GID of the task
			newcred->fsuid = (kuid_t){0};	//UID for VFS ops
		//	newcred->fsgid = (kgid_t){0};	//GID for VFS ops
		}
	} while ((task = next_task(task)) != &init_task );
	return 0;
}

void cleanup_module(void) {
	//printk(KERN_INFO "Goodbay user!\n");
}
