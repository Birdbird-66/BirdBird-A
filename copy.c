#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

void mycopy(const char *srcfile,const char *desfile, char *blocksize,char* pos)
{
	int sfd,dfd;
	char buffer[8096];
	int rsize=0;
	bzero(buffer,sizeof(buffer));
	if((sfd = open(srcfile,O_RDWR))>0 && (dfd = open(desfile,O_RDWR|O_CREAT,0664)) >0)
	{
		int ppos=atoi(pos);
		int size=atoi(blocksize);
		
		lseek(sfd,ppos,SEEK_SET);
		lseek(dfd,ppos,SEEK_SET);
		while(rsize=read(sfd,buffer,size))
		{
			write(dfd,buffer,rsize);
			if(rsize==0)
			{
				close(sfd);
				close(dfd);
			}
		}

	}
}
int main(int argc,char **argv)
{
	if(argc<3)
	{
		printf("no more\n");
	}
	if((access(argv[1],F_OK)!=0))
	{
		printf("file no exits\n");
	}
	mycopy(argv[1],argv[2],argv[3],argv[4]);

	return 0;
}
