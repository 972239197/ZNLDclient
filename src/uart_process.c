#include "all.h"

#include "uart_process.h"


/* 串口数据处理 */
void uart_recv_process(int val, char *buf, int len)
{
	
	switch(val)
	{
		case 1:
			printf("uart1 recv:%d\n",len);
			
			break;
		case 2:
//			printf("uart2 recv:%d\n",len);
			if(len == 24)
			{
			  info.pm25 = (buf[12]*256) + buf[13];
			  info.pm10 = (buf[14]*256) + buf[15];
//			  printf("pm2.5:%d\n",info.pm25);
//			  printf("pm10:%d\n",info.pm10);
			}
			
			break;
		default:
			break;
		
	}
	
}


