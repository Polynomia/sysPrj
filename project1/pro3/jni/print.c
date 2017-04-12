#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>

int main()
{
	pid_t Parent=getpid();
	pid_t Child=fork();
//	printf("515021910213 Parent, %d\n",Parent);
	if(Child==0){
		printf("************ Child, %d\n",getpid());
		execl("/data/testARM","testARM",NULL);
		}
	else if(Child >0)
		{	
			waitpid(Child,NULL,0);
			printf("************ Parent %d\n",getpid());   
		}
	else{
			printf("Child failed\n");
		}
	return 0;
}
