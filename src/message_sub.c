#include "all.h"

#define NET_PORT 80  
#define NET_IP "14.215.177.37" //百度80端口  

#define set_4G_out()  gpio_set_direction(17, GPIO_OUTPUT)
#define set_4G_high()   gpio_write( 17, 1 )
#define set_4G_low() 	 gpio_write( 17, 0 )

void delay(int m)
{
	int i;
	for(i=0;i<m;i++)
	{
		usleep(1000);
	}

}

int get_ip(char *ipaddr)
{
    int sock_fd;
    struct sockaddr_in sin;
    struct ifreq ifr;

     sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_fd == -1)
        {
                perror("socket error..");
				strcpy(ipaddr,"noip");
                return -1;
        }

        strcpy(ifr.ifr_name, "eth0");

        if (ioctl(sock_fd, SIOCGIFADDR, &ifr) < 0)
        {
                perror("ioctl error..");
                strcpy(ipaddr,"noip"); 
				close(sock_fd);
                return -1;
        }

        memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
        strcpy(ipaddr, inet_ntoa(sin.sin_addr));
        close(sock_fd);
    return 0;
}

//获取联网状态  
int check_net()  
{  
    int fd;  
    int in_len=0;  
    struct sockaddr_in seraddr;  
    unsigned int inaddr=0l;  
   // struct in_addr *ipaddr;
 
	struct addrinfo hints;
	struct addrinfo *result;
	int ret;
	/* obtaining address matching host */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;
	hints.ai_protocol = 0;  /* any protocol */
 	char hostname[64] = { 0 };

    in_len = sizeof(struct sockaddr_in);  
    fd = socket(AF_INET,SOCK_STREAM,0);  
    if(fd < 0)  
    {  
        perror("socket");  
        return -1;  
    }  
  
    /*设置默认服务器的信息*/  
    seraddr.sin_family = AF_INET;  
    seraddr.sin_port = htons(NET_PORT);   
    memset(seraddr.sin_zero,0,sizeof(seraddr.sin_zero));  
  
    /*判断是主机名还是ip地址*/  
    if( (inaddr=inet_addr(NET_IP)) == INADDR_NONE)  
    {   
		ret = getaddrinfo(NET_IP, NULL, &hints, &result);
		if (ret != 0)
		{
	    	printf("getaddrinfo: %s\n", gai_strerror(ret));
			close(fd);
	    	return 0;
		}
		ret = getnameinfo(result->ai_addr, result->ai_addrlen, hostname, sizeof(hostname), NULL, 0, NI_NUMERICHOST);
	    if (ret != 0)
	    {
	        printf("error in getnameinfo: %s \n", gai_strerror(ret));
			close(fd);
			return 0;
	    }
	    else
	    {
	        printf("IP: %s \n", hostname);
	    }         
        inaddr=inet_addr(hostname); 
        seraddr.sin_addr.s_addr = inaddr;
		memset(hostname, 0, sizeof(hostname));  
    }  
    else    /*是ip地址*/  
    {  
        seraddr.sin_addr.s_addr = inaddr;  
    }  
    /*connect 函数*/  
    if(connect(fd,(struct sockaddr* )&seraddr,in_len) < 0 )  
    {  
        printf("not connect\n ");  
        close(fd);  
        return 0; //没有联网成功  
    }  
    else  
    {  
        printf("connect ok...\n");  
        close(fd);  
        return 1;  
    }  
}

int main()
{
	int i=0,net=0;
	int ret,res;
	int data[5]={0};
	char mbuf[128];
	int flag = 0;
	char ipbuf[20];
	
	set_4G_out();
	set_4G_low();

	memset(ipbuf, 0, sizeof(ipbuf));
	system("cp /root/msg_sub.dat /tmp/");
	system("killall -15 mosquitto_sub");
	sleep(1);
	printf("Start internet listenning...\n");
	
	init_json_data();  // 初始化json数据

	flag = get_netmode();
	if(flag == 1) {	
		res = 0;
		strcpy(info.net_mode,"ethernet");
		get_ip(ipbuf);
		if(strcmp(ipbuf,"noip") == 0) {
			printf("set ethernet mode...\n");
			system("widora_mode router &");
			sleep(20);
		}
		else printf("ethernet mode...\n");
		
		memset(ipbuf, 0, sizeof(ipbuf));
	}
	else {	
		res = 1;
		strcpy(info.net_mode,"LTE4G");
		ret = check_net();
		if(ret < 1) {		  
		  printf("set LTE4G mode...\n");
		  system("widora_mode pppd &");	
		  sleep(20);
		}		
	}
	
	while(1)
	{
		ret = check_net();
		switch(ret) {
			case 0:
				if(res == 0 ) {
					strcpy(info.net_mode,"LTE4G");
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
				break;
			case 1:
				res = -1;
				break;
			default:
				break;
		}
		if(res < 0) break;
	}

	//创建mqtt_sub订阅消息监听线程
	create_notify_pthread();
	
	//创建声控和光阻（故障上报）线程
	creat_get_gpio_thread();
	
	//创建两个串口初始化，消息监听线程
	uart1_init();
	uart2_init();
	
	//初始化联网方式切换线程
	init_thread_netmode();

	init_flash();  //init  flash  light

	memset(mbuf, 0, sizeof(mbuf));
	sprintf(mbuf,"mosquitto_pub -i %d -h 172.16.36.19 -t broker/sciov/lamppost/console/%s -u %s -f /tmp/msg_pub.dat",get_IDtime(),info.light_id,info.light_id);
	while(1)
	{
		for(i=0;i<5;i++)
		{
          data[i] = get_flash();
          delay(1000);
		}

		Sht1xMeasure();
		delay(200);
		info.temp = GetSht1xTemp();
		info.hum = GetSht1xHumi();
		info.flash_v = (data[0]+data[1]+data[2]+data[3]+data[4])/5;
		ipc_online(info.ip_camera);
		get_ctime(info.time);
		//创建mqtt_pub推送消息json格式,并获取所有状态数据
		put_json();
		system(mbuf);
		sleep(52);	
	}	
	return 0;
	
}
