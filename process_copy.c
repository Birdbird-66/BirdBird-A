#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/types.h>

int file_block(const char *srcfile,int proNum)
{
	int sfd;
	int filesize;
	if((sfd = open(srcfile,O_RDWR)) == -1)
	{
		perror("open failed");
		exit(0);
	}
	if((filesize = lseek(sfd,0,SEEK_END)) ==-1)
	{
		perror("lseek failed");
		exit(0);
	}
	return (filesize/proNum==0) ? filesize/proNum : filesize/proNum + 1;
}

void create_process(const char*srcfile,const char*desfile,int proNum,int blocksize)
{
	pid_t pid;
	int i;
	for(i=0;i<proNum;i++)
	{
		pid = fork();
		if(pid==0)
			break;
	}
	if(pid>0)
	{
		printf("parent process\n");
	}else if(pid==0)
	{
		int pos=i*blocksize;
		char strpos[20];
		char strsize[20];
		bzero(strpos,sizeof(strpos));
		bzero(strsize,sizeof(strsize));
		sprintf(strpos,"%d",pos);
		sprintf(strsize,"%d",blocksize);

		printf("process %d, startpos %d, blocksize %d\n",getpid(),pos,blocksize);

		if(execl("/home/colin/0605Linux/processCopy/copy","copy",srcfile,desfile,strsize,strpos,NULL)==-1)
		{
			perror("failed");
			exit(0);
		}

	}else{
		perror("create failed");
		exit(0);
	}
}
int main(int argc,char **argv)
{
	int process_num=5;
	int blocksize;
	if(argc<3)
	{
		printf("参数太少\n");
		exit(0);
	}
	if((access(argv[1],F_OK)!=0))
	{
		printf("file no exits\n");
		exit(0);
	}
	if(argv[3]!=0)
	{
		process_num = atoi(argv[3]);
		if(process_num<0 || process_num>100)
		{
			printf("进程数目错误\n");
			exit(0);
		}
	}
	blocksize=file_block(argv[1],process_num);
	create_process(argv[1],argv[2],process_num,blocksize);

	//回收子进程
	pid_t zpid;
	while((zpid = waitpid(-1,NULL,WNOHANG)) !=-1)
	{
		if(zpid>0)printf("kill process success\n");
		else
			continue;
	}

	return 0;
}
