#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>

void CreateProcess(const char *srcfile,const char *desfile,int Pnumber,int Psize){
	pid_t pid;
	int i=0;
	for(i=0;i<Pnumber;i++){
		pid = fork();
		if(pid == 0)
			break;
	}
	if(pid>0){
		printf("parent process\n");
	}
	else if(pid == 0){
		int pos= i*Psize;
		char strpos[20];
		char strsize[20];

		bzero(strpos,sizeof(strpos));
		bzero(strsize,sizeof(strsize));

		sprintf(strpos,"%d",pos);
		sprintf(strsize,"%d",Psize);

		printf("Process %d,startpos %d,Every_Process_Size %d\n",getpid(),pos,Psize);

		if(execl("/home/colin/2021-06-05/Homework/app","app",srcfile,desfile,strsize,strpos)==-1){
			perror("failed");
			exit(0);
		}
	}

	else{
		perror("create failed");
		exit(0);
	}
}

void WaitPid(){
	pid_t sonpid;

	while((sonpid=waitpid(-1,NULL,WNOHANG)) !=-1 ){
		if(sonpid>0)
			printf("childprocess %d is recycled!\n",sonpid);
		else
			continue;
	}
}

int main(int argc,char **argv)
{
	int Process_Number=10;
	int Every_Process_Size=0;
	int sfd=0;
	int FileSize=0;
	if(argc<3){
		printf("Please press srcfile and desfiles!\n");
		exit(0);
	}
	//ÎÄ¼þ²âÊÔº¯Êýaccess
	if((access(argv[1],F_OK))!=0){
		printf("%s srcfile exists!\n",argv[1]);
		exit(0);
	}

	if(argv[3]!=0){
		Process_Number = atoi(argv[3]);
		if(Process_Number >100 || Process_Number<0){
			printf("process parameter error!\n");
			exit(0);
		}
	}
	if((sfd = open(argv[1],O_RDWR)) == -1){
		perror("Open Failed!!!\n");
		exit(0);
	}
	if((FileSize = lseek(sfd,0,SEEK_END)) == -1){
		perror("Lseek Failed!!!\n");
		exit(0);
	}
	if(FileSize/Process_Number == 0)
		Every_Process_Size=FileSize/Process_Number;
	else
		Every_Process_Size=FileSize/Process_Number+1;
	

	CreateProcess(argv[1],argv[2],Process_Number,Every_Process_Size);
	WaitPid();

	return 0;
}
