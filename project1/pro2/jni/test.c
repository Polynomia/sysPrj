/*
  This is the test program for ptree syscall.
  We need to print the entire process tree (in DFS order) using tabs to indent children with respect to their parents.So I use an array called indent to store the number of tabs of every process.
*/
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct prinfo {
	pid_t parent_pid;		        /* process id of parent */
	pid_t pid;			            /* process id */
	pid_t first_child_pid;		  /* pid of youngest child */
	pid_t next_sibling_pid;		  /* pid of older sibling */
	long state;			            /* current state of process */
	long uid;			              /* user id of process owner */
	char comm[64];			        /* name of program executed */
};

//void print_tab(int n)  { while (n--) printf("    "); }
void print_tree(struct prinfo *buf,int *nr)
{
	int *indent,i;
	indent=malloc((*nr)*sizeof(int));
	for(i=0;i<(*nr);i++)
		indent[i]=0;
	printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[0].comm,
		buf[0].pid, buf[0].state, buf[0].parent_pid,
		buf[0].first_child_pid, buf[0].next_sibling_pid, buf[0].uid);
	int j;
	i=1;
	while (i < *nr) {
		if (buf[i].parent_pid == buf[i - 1].pid) {
			indent[i]=indent[i-1]+1;}
		else { j=i-1;
			for (; j >= 0; --j){
				if (buf[j].parent_pid == buf[i].parent_pid) {
					indent[i] = indent[j];	
					break;
				 } 
			}			
		}
		int n=indent[i];
		while (n--) printf("\t");
		printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[i].comm,
			buf[i].pid, buf[i].state, buf[i].parent_pid,
			buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);
		i++;
	}
	free(indent);
}

int main()
{
	struct prinfo *buf;
	buf= malloc(1000 * sizeof(struct prinfo));
	int *nr = malloc(sizeof(int));
	if (buf == NULL || nr == NULL) {
			printf("Allocation error!\n");return 0;}
	syscall(356, buf, nr);
	printf("There are %d processes!\n", *nr);
	print_tree(buf,nr);
	free(buf);  free(nr);
	return 0;
}





