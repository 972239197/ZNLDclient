#include "all.h"
#include "notify.h"   

#define BUF_LEN 1024 
  
int displayInotifyEvent(struct inotify_event *event)  
{  
  //  printf("  wd = %2d; ",event->wd);  
  
    if(event->cookie > 0)  
    {  
    //    printf("cokkie = %4d; ",event->cookie);  
    }  

    if(event->mask & IN_ACCESS) {
	  //printf("IN_ACCESS\n"); 
		return 0; 
	}
    else if(event->mask & IN_DELETE_SELF) {
	 // printf("IN_DELETE_SELF\n");  
		return 1;
	}
	else if(event->mask & IN_DELETE) {
	//  printf("IN_DELETE\n");
		return 2;
	}
    else if(event->mask & IN_MODIFY) {
	  printf("IN_MODIFY\n");
		return 3;
	}  
    else if(event->mask & IN_OPEN) {
	 // printf("IN_OPEN\n");
		return 4;
	}  
    else return -1;
}  
  
void *notify_pthread(void *pdata) 
{  
    int inotifyFd,wd;  
    char buf[BUF_LEN];  
    ssize_t numRead;  
    char *p;
	char mbuf[128];
    struct inotify_event *event;     
    int flag = 0;

	memset(mbuf, 0, sizeof(mbuf));
	sprintf(mbuf,"mosquitto_sub -i %d -h 172.16.36.19 -t broker/sciov/lamppost/device/%s -u %s & ",get_IDtime(),info.light_id,info.light_id);

	system(mbuf);
	sleep(1);
    inotifyFd = inotify_init();  

    if(inotifyFd == -1)  
    {  
        printf("notify init failure\n");  
    }  
  
    wd = inotify_add_watch(inotifyFd,"/tmp/msg_sub.dat",IN_ALL_EVENTS);  //add notify
    if(wd == -1)  
    {  
        printf("add watch error\n");  
    }  
  
    while(1)  
    {  
        numRead = read(inotifyFd,buf,BUF_LEN);  
        if(numRead == -1)  
        {  
            printf("read error\n");  
        }  
    
        for(p=buf;p < buf+numRead;)  
        {  
            event = (struct inotify_event *)p;  
            flag= displayInotifyEvent(event);  
		    if(flag == 3) {
			//	do something;
				get_json();			
			}
            p+=sizeof(struct inotify_event) + event->len;  
        }  
		delay(20);	
    }  
	inotify_rm_watch(inotifyFd, wd); // 移除一个watcher
   
  
}


/* 
	创建mqtt_sub接收线程
*/
void create_notify_pthread(void)
{
	pthread_t thread;
    pthread_attr_t thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &thread_attr, notify_pthread, NULL) < 0)
	{
		perror("pthread_create notify error");
	}
}



