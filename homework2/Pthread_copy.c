#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
 

#define T_NUM 5
#define ITEMS 50
 
typedef struct{
    
    int off,size,t_no;
}arg_t;
 
void err_sys(void *str)
{
    perror(str);
    exit(1);
}
 
void err_usr(char *str)
{
    fputs(str,stderr);
    exit(1);
}
 
char *pch_sour_file,*pch_des_file;
int *pi_has_done;
int pnum=T_NUM;
 

void *pt_cp(void *arg)
{
    if(NULL==arg)
        return NULL;
    arg_t *arg_p;int i;
    char *pch,*qch;
    arg_p=(arg_t *)arg;
    pch=pch_sour_file+arg_p->off,qch=pch_des_file+arg_p->off;
    for(i=0;i<arg_p->size;i++)
    {
        *qch++=*pch++;
        pi_has_done[arg_p->t_no]++;
       
        usleep(5000);
    }
    return NULL;
}
void *display_pro(void * arg)
{
    int size,draw=0,sum,i,j,interval;
	char str[50];
    size=(int)arg;
    interval=size/ITEMS;
    while(draw<ITEMS)
	{
        for(i=0,sum=0;i<pnum;i++)
            sum+=pi_has_done[i];
   
        j=sum/interval+1;
        for(;j>draw;draw++)
		{
			  str[draw]='=';
		      printf("%-50s %.2lf%%\r",str,((double)sum/(double)size)*100);
              fflush(stdout);
		}
            
    }
    printf("\n");
    return NULL;
}
int main(int argc,char *argv[])
{
    int src,dst,i,len,off;
    struct stat statbuff;
    pthread_t *tid;
    arg_t *arg_arr;
    if(argc!=3 && argc!=4)    
        err_usr("usage：cp src dst [pthread_no]\n");
    if(argc==4)
        pnum=atoi(argv[3]);
    src=open(argv[1],O_RDONLY);
    if(src<0)
    {printf("fail to open %s\n",argv[1]);return -1;}
 
    dst=open(argv[2],O_RDWR | O_CREAT | O_TRUNC,0644);
    if(dst<-1)
    {printf("fail to open %s\n",argv[2]);return -1;}
 
    if(fstat(src,&statbuff)==-1)
    {
        err_sys("fail to stat");
        return -1;
    }
    lseek(dst,statbuff.st_size-1,SEEK_SET);
    write(dst,"a",1);
 
   //共享映射 
    pch_sour_file=(char *)mmap(NULL,statbuff.st_size,PROT_READ,MAP_SHARED,src,0);
    if(pch_sour_file==MAP_FAILED)
    {
        err_sys("fail to mmap");
        return -1;
    }
 
    pch_des_file=(char *)mmap(NULL,statbuff.st_size,PROT_WRITE,MAP_SHARED,dst,0);
    if(pch_des_file==MAP_FAILED)
    {
        err_sys("fail to mmap");
        return -1;
    }
   
    close(src);
	close(dst);
    tid=(pthread_t *)malloc(sizeof(pthread_t)*(pnum+1));
    if(NULL==tid)
    {
        err_sys("fail to malloc");
        return -1;
    }
 
    pi_has_done=(int *)calloc(sizeof(int),pnum);
    if(NULL==pi_has_done)
    {
        err_sys("fail to calloc");
        return -1;
    }    
    arg_arr=(arg_t *)malloc(sizeof(arg_t)*pnum);
    if(NULL==arg_arr)
    {
        err_sys("fail to malloc");    
        return -1;
    }
    len=statbuff.st_size/pnum,off=0;
    for(i=0;i<pnum;i++,off+=len)
        arg_arr[i].off=off,arg_arr[i].size=len,arg_arr[i].t_no=i;
    //最后一个线程 
    arg_arr[pnum-1].size+=statbuff.st_size%pnum;
    //创建复制线程 
    for(i=0;i<pnum;i++)
        pthread_create((tid+i),NULL,pt_cp,(void *)(arg_arr+i));
    //显示进度和百分比的线程
    pthread_create((tid+pnum),NULL,display_pro,(void *)statbuff.st_size);
    for(i=0;i<pnum;i++)
        pthread_join(tid[i],NULL);
    //关闭文件共享
    munmap(pch_sour_file,statbuff.st_size);
    munmap(pch_des_file,statbuff.st_size);
    //回收内存
    free(tid);
    free(arg_arr);
    free(pi_has_done);
    return 0;
}

