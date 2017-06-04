#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <asm/page.h>
//#include <asm/pgtable.h>

struct pagetable_layout_info {
    uint32_t pgdir_shift;
    uint32_t user_ptrs_per_pgd;
    uint32_t ptrs_per_pgd;
    uint32_t ptrs_per_pte;
    uint32_t page_shift;
};


int main(int argc, char ** argv)
{
	struct pagetable_layout_info pti;
	syscall(356, &pti);
	printf("Pgd_shift = %d\n", pti.pgdir_shift);
	printf("user_ptrs_per_pgd = %d\n", pti.user_ptrs_per_pgd);
	printf("Ptrs_per_pte = %d\n", pti.ptrs_per_pte);
	printf("Ptrs_per_pgd = %d\n", pti.ptrs_per_pgd);
	printf("page_shift = %d\n", pti.page_shift);
	return 0;
}