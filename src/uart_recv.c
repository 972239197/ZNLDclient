


/***********************************************************
文件名	:	uart.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
	串口，用于跟副处理器(STM32等单片机)通信

***********************************************************/

#include "all.h"
#include "uart_recv.h"

int tty1_fd;
int tty2_fd;

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0) 
    { 
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD; 
    newtio.c_cflag &= ~CSIZE; 

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
    case 'O':                     //奇校验
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     //偶校验
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':                    //无校验
        newtio.c_cflag &= ~PARENB;
        break;
    }

	switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n");
    return 0;
}

int open_port(int fd,int comport)
{

    if (comport == 0)
    {   

    	fd = open( "/dev/ttyS0", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyS0 success !\n");
        }
    }
    else if(comport == 1)
    {    fd = open( "/dev/ttyS1", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyS1...\n");
        }    
    }
    else if (comport == 2)
    {
        fd = open( "/dev/ttyS2", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyS2...\n");
        }
    }
    if(fcntl(fd, F_SETFL, 0)<0)
    {
        printf("fcntl failed!\n");
    }
    else
    {
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
    }
    if(isatty(STDIN_FILENO)==0)
    {
        printf("standard input is not a terminal device\n");
    }
    else
    {
        printf("isatty success!\n");
    }

    return fd;
}


void *uart1_thread(void *pdata)
{
    int maxfd;
	fd_set rd; 
    int nread,i;
    char tmp_buff[8];
	int offset;
	char recvbuf[1024];
	
	struct timeval timeout={0,50000}; 
	int timeout_flg = 0;
	int recv_flg = 0;

    if((tty1_fd=open_port(tty1_fd,1))<0)
    {
        perror("open_port error");
        return NULL;
    }
    if((i=set_opt(tty1_fd,115200,8,'N',1))<0)
    {
        perror("set_opt error");
        return NULL;
    }
    printf("fd1=%d\n",tty1_fd);
	
	FD_ZERO(&rd);

	offset = 0;


	for(;;){
		FD_SET(tty1_fd, &rd);

		maxfd = tty1_fd + 1;

		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;
		select(maxfd, &rd, NULL, NULL, &timeout);
		timeout_flg = 1;
		
		//offset = 0;
		if(FD_ISSET(tty1_fd, &rd)){
			while((nread = read(tty1_fd, tmp_buff, sizeof(tmp_buff))) > 0)  
			{
//				printf("%s", tmp_buff);
				if((offset + nread) > 1024)
					offset = 0;

				memcpy(&recvbuf[offset], tmp_buff, nread);
				offset = offset + nread;
				memset(tmp_buff, 0 , sizeof(tmp_buff));
				timeout_flg = 0;
				recv_flg = 1;
			}
			//printf("%s\n", recvbuf);
		}
		else if(timeout_flg == 1){
			if(recv_flg == 1){				

				/* 处理事件 */
				uart_recv_process(1, recvbuf, offset);

				recv_flg = 0;
				offset = 0;
				memset(recvbuf, 0 , sizeof(recvbuf));
			}
		}
	}

	close (tty1_fd);  
	return NULL;  

}

void *uart2_thread(void *pdata)
{
    int maxfd;
	fd_set rd; 
    int nread,i;
    char tmp_buff[8];
	int offset;
	char recvbuf[1024];
	
	gpio_set_direction(39, GPIO_OUTPUT);  //设置pm2.5传感器的reset引脚
	gpio_write(39, 0);  //reset 
	usleep(1000);
	gpio_write(39, 1);  //拉高
	gpio_set_direction(40, GPIO_OUTPUT);  //设置pm2.5传感器设置数据引脚
	gpio_write(40, 1);  //默认拉高
	
	struct timeval timeout={0,50000}; 
	int timeout_flg = 0;
	int recv_flg = 0;

    if((tty2_fd=open_port(tty2_fd,2))<0)
    {
        perror("open_port error");
        return NULL;
    }
    if((i=set_opt(tty2_fd,9600,8,'N',1))<0)
    {
        perror("set_opt error");
        return NULL;
    }
    printf("fd2=%d\n",tty2_fd);
	
	FD_ZERO(&rd);

	offset = 0;


	for(;;){
		FD_SET(tty2_fd, &rd);

		maxfd = tty2_fd + 1;

		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;
		select(maxfd, &rd, NULL, NULL, &timeout);
		timeout_flg = 1;
		
		//offset = 0;
		if(FD_ISSET(tty2_fd, &rd)){
			while((nread = read(tty2_fd, tmp_buff, sizeof(tmp_buff))) > 0)  
			{
//				printf("%s", tmp_buff);
				if((offset + nread) > 1024)
					offset = 0;

				memcpy(&recvbuf[offset], tmp_buff, nread);
				offset = offset + nread;
				memset(tmp_buff, 0 , sizeof(tmp_buff));
				timeout_flg = 0;
				recv_flg = 1;
			}
			//printf("%s\n", recvbuf);
		}
		else if(timeout_flg == 1){
			if(recv_flg == 1){

				/* 处理事件 */
				uart_recv_process(2,recvbuf, offset);

				recv_flg = 0;
				offset = 0;
				memset(recvbuf, 0 , sizeof(recvbuf));
			}
		}
	}

	close (tty2_fd);  
	return NULL;  

}


/* 往串口中写入数据 */
int write_to_uart(int fd, char *buf, int len)
{
	int ret;
	ret = write(fd, buf, len);
	if(ret < 0)
		printf("write_to_uart failed !\r\n");
	
	return ret;
}

int uart1_init()
{
	pthread_t uart_thread_t;
    pthread_attr_t uart_thread_attr;

    pthread_attr_init(&uart_thread_attr);		//初始化进程属性
    pthread_attr_setdetachstate(&uart_thread_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&uart_thread_t, &uart_thread_attr, uart1_thread, NULL) < 0)
    {
        perror("pthread_create uart1_thread error");
		return -1;
	}
	
	return 0;
}

int uart2_init()
{
    pthread_t uart_thread_t;
    pthread_attr_t uart_thread_attr;

    pthread_attr_init(&uart_thread_attr);       //初始化进程属性
    pthread_attr_setdetachstate(&uart_thread_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&uart_thread_t, &uart_thread_attr, uart2_thread, NULL) < 0)
    {
        perror("pthread_create uart2_thread error");
        return -1;
    }

    return 0;
}



