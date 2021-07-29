#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
void mycopy(const char *srcfile,const char *desfile,char *Psize,char *pos){
	int sfd=0;
	int dfd=0;
	int rsize=0;
	char buffer[10000];
	bzero(buffer,sizeof(buffer));

	if((sfd = open(srcfile,O_RDWR))>0 && (dfd = open (desfile,O_RDWR|O_CREAT,0064))>0){
		int copy_pos=atoi(pos);
		int Every_Copy_size=atoi(Psize);

		lseek(sfd,copy_pos,SEEK_SET);
		lseek(dfd,copy_pos,SEEK_SET);

		while(rsize = read(sfd,buffer,Every_Copy_size)){
			write(dfd,buffer,rsize);
			if(rsize == 0){
				close(sfd);
				close(dfd);
			}
		}
	}
}

int main(int argc,char **argv)
{
	if(argc<3){
		printf("²ÎÊý²»×ã!\n");
	}
	if((access(argv[1],F_OK)!=0))
	{
		printf("file open filed!\n");
	}

	mycopy(argv[1],argv[2],argv[3],argv[4]);

	return 0;
}
