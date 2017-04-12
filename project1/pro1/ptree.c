#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/list.h>
#include <linux/slab.h> //alloction function
#include <linux/uaccess.h>	//KtoU
#include <linux/syscalls.h>

MODULE_LICENSE("Dual BSD/GPL");

#define __NR_hellocall 356

static int(*oldcall)(void);
struct prinfo{
	pid_t parent_pid;
	pid_t pid;
	pid_t first_child_pid;
	pid_t next_sibling_pid;
	long state;
	long uid;
	char comm[64];
};

//transfer task_struct to prinfo

void copyToPrinfo(struct task_struct *task, struct prinfo *kbuf, int *knr)
{
	kbuf[*knr].parent_pid=(task->real_parent==NULL)?0:task->real_parent->pid;
	kbuf[*knr].pid=task->pid;
	kbuf[*knr].state=task->state;
	kbuf[*knr].uid=task->cred->uid;
	get_task_comm(kbuf[*knr].comm,task);
	kbuf[*knr].first_child_pid=(list_empty(&task->children)==1)?0:(list_entry(task->children.next,struct task_struct,sibling)->pid);
	kbuf[*knr].next_sibling_pid=(list_empty(&task->sibling)==1)?0:(list_entry(task->sibling.next,struct task_struct,sibling)->pid);
}	

//dfs function

void dfs(struct task_struct *task, struct prinfo *kbuf,int *knr)
{
	if(task==NULL) return;
	struct task_struct *task_new;
	copyToPrinfo(task,kbuf,knr);
	(*knr)++;
	if(list_empty(&(task->children))!=1)
		list_for_each_entry(task_new,&task->children,sibling)
			dfs(task_new,kbuf,knr);
	return;
}

//copy kbuf from kernel to user

int mySyscall(struct prinfo *buf, int *nr)
{
	int knr=0;
	struct prinfo *kbuf;
	kbuf=kmalloc(100*sizeof(struct prinfo),GFP_KERNEL);
	if (kbuf == NULL) 
	{	printk("Allocate kbuf error!\n");
		return -EFAULT;
	}
	read_lock(&tasklist_lock);
	dfs(&init_task,kbuf,&knr);
	read_unlock(&tasklist_lock);
	if((copy_to_user(nr,&knr,sizeof(int))!=0)||(copy_to_user(buf,kbuf,knr*sizeof(struct prinfo))!=0))
	printk("Copy to user error!\n");
	kfree(kbuf);
	return 0;
}



//No need to change them

static int addsyscall_init(void)
{
	long *syscall = (long*)0xc000d8c4;
	oldcall = (int(*)(void))(syscall[__NR_hellocall]);
	syscall[__NR_hellocall] = (unsigned long )mySyscall;
	printk(KERN_INFO "module load!\n");
	return 0;
}

static void addsyscall_exit(void)
{
	long *syscall=(long*)0xc000d8c4;
	syscall[__NR_hellocall] = (unsigned long )oldcall;
	printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);

/** 
 * list_for_each_entry  -   iterate over list of given type 
  * @pos:    the type * to use as a loop cursor. 
   * @head:   the head for your list. 
    * @member: the name of the list_struct within the struct. 
*/

/** 
 * list_entry - get the struct for this entry 
  * @ptr:    the &struct list_head pointer. 
   * @type:   the type of the struct this is embedded in. 
    * @member: the name of the list_struct within the struct. 
*/  
