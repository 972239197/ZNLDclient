#include <time.h> //用到了time函数 
#include "id_time.h"
#include "all.h"

#define set_4G_high()   gpio_write( 17, 1 )
#define set_4G_low() 	 gpio_write( 17, 0 )

int get_IDtime()
{  
	int i,number;
	unsigned char data[2] = {0};
   srand((int) time(NULL)); //用时间做种，每次产生随机数不一样
   for (i=0; i<2; i++)
   {
     data[i] = rand() % 0xFF;  //产生0-255的随机数
   }   
	number = (data[0] << 8) | data[1] ;   //最大值65535
   return number;
}

void ipc_online(char *str)
{
	FILE *fp = NULL;
    char buff[8]={0};
    system("/root/./ipcamera.sh &");
    sleep(1);
    fp = fopen("/tmp/online.dat", "r");
    fgets(buff, sizeof(buff), fp);
    if(strstr(buff,"0x0") != NULL) strcpy(str,"off");
    else if(strstr(buff,"0x2") != NULL) strcpy(str,"on");
    fclose(fp);
	//system("rm /tmp/online.dat");
}

void get_ctime(char *cstr)
{
	char buf[30] = { 0 };
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d ", 
	1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	memcpy(cstr, buf, strlen(buf));
	
}

int get_netmode()
{
	FILE *fp = NULL;
    char buff[8]={0};
    system("/root/./netmode.sh &");
    sleep(1);
    fp = fopen("/tmp/netmode.dat", "r");
    fgets(buff, sizeof(buff), fp);
    if(strstr(buff,"0x00") != NULL) {
		
		fclose(fp);
		return 0;
	}
    else if(strstr(buff,"0x01") != NULL) {
		
		fclose(fp);
		return 1;
	}
	else {
		fclose(fp);
		return -1;
	}	
}

void *thread_netmode(void *pdata)
{
	int net=0,tmp = 0;
	int i,ret,res = 0;
	int bak = 0,ms = 0;

	tmp = get_netmode();
	sleep(1);
	while(1)
	{
		i = get_netmode();
		if(i == tmp)
		{
			
		}
		else {
			if(i == 1) {
				res = 0;
				strcpy(info.net_mode,"ethernet");
				printf("set ethernet mode...\n");
				system("widora_mode router &");

			}
			else {
				res = 1;
				strcpy(info.net_mode,"LTE4G");
				printf("set LTE4G mode...\n");
				system("widora_mode pppd &");				
			}
			tmp = i;
			bak = 0;
			sleep(20);
		}
		if(bak == 0)
		{
		ret = check_net();
		switch(ret) {
			case 0:
				if(res == 0 ) {
					strcpy(info.net_mode,"LTE4G");
					printf("set LTE4G mode...\n");
					system("widora_mode pppd &");
					res = 1;
				}
				else if(res == 1) {
					set_4G_high();
					sleep(1);
					set_4G_low();
					res = 2;
				}
				else if(res == 2) {
					strcpy(info.net_mode,"ethernet");
					printf("set ethernet mode...\n");
					system("widora_mode router &");
					res = 3;
				}
				else {
					net+=1;
					if(net%30 == 0) {
						res = 0;
						net = 0;
					}
				}
				sleep(20);
				bak = 0;
				break;
			case 1:
				res = 0;
				bak = 1;
				break;
			default:
				break;
		}

		}
		ms+=1;
		if((ms%15) == 0) {
			ms = 0;
			bak = 0;
		}
		sleep(4);	
	}
	
}

void init_thread_netmode()
{

	pthread_t thread;
    pthread_attr_t thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &thread_attr, thread_netmode, NULL) < 0)
	{
		perror("pthread_create net_mode error");
	}
}

