
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/mman.h>
#include <linux/mm.h>
//#include <asm/pgtable.h>
#include <linux/list.h>
#include <asm/memory.h>
#include <linux/slab.h> //alloction function
#include <linux/uaccess.h>	//KtoU
#include <linux/syscalls.h>
#include <asm/errno.h>
#include <linux/pid.h>
#include <asm/page.h>

MODULE_LICENSE("Dual BSD/GPL");
static int(*oldcall)(void);
//static int(*oldcall2)(void);
#define __NR_layoutcall 356
//#define __NR_exposecall 378

struct pagetable_layout_info {
    uint32_t pgdir_shift;
    uint32_t page_shift;
};

int layoutcall(struct pagetable_layout_info * pgtbl_info)
{
    struct pagetable_layout_info layout_info;
    layout_info.pgdir_shift = PGDIR_SHIFT;
    layout_info.page_shift = PAGE_SHIFT;
    if (copy_to_user(pgtbl_info, &layout_info,sizeof(struct pagetable_layout_info)))
        return -EFAULT;
    return 0;
}

int exposecall(pid_t pid, unsigned long fake_pgd,unsigned long addr)
{
	struct task_struct *ts;
	struct pid *pid_struct;
	struct mm_struct *target_mm;
	int ret = 0;

	
	return 0;
}







//No need to change them

static int addsyscall_init(void)
{
    long *syscall = (long*)0xc000d8c4;
    oldcall = (int(*)(void))(syscall[__NR_layoutcall]);
 //   oldcall2=(int(*)(void))(syscall[__NR_exposecall]);
    syscall[__NR_layoutcall] = (unsigned long )layoutcall;//only need to change the name.
//    syscall[__NR_exposecall] = (unsigned long )exposecall;
    printk(KERN_INFO "module load!\n");
    return 0;
}



static void addsyscall_exit(void)
{
    long *syscall=(long*)0xc000d8c4;
    syscall[__NR_layoutcall] = (unsigned long )oldcall;
//    syscall[__NR_exposecall]=(unsigned long)oldcall2;
    printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);