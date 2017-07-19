#include <linux/i2c-dev.h>
#include "light_iic.h"
#include "all.h"

#define I2C_ADDR 0x23

int fd;
int init_flash()
{
        int res;
        
        char val;
		
        fd=open("/dev/i2c-0",O_RDWR);
        if(fd<0)
        {
                printf("err open file:%s\n",strerror(errno));
				 return -1;
        }
		res = ioctl(fd,I2C_TENBIT,0);   //not 10bit
        if((res=ioctl( fd,I2C_SLAVE,I2C_ADDR)) < 0 )
        {
                printf("ioctl error : %s\n",strerror(errno));
				return -1;
        }
		
        val=0x01;
        if(write(fd,&val,1)<0)
        {
                printf("write 0x01 err\n");
        }
	return 0;
}

int get_flash()
{
		int i;
		char buf[3];
        char val;
        int temp;
        float flight;

		val=0x01;
        if(write(fd,&val,1)<0)
        {
			printf("write 0x01 err\n");
			return -1;
        }
		usleep(10000);
        val=0x10;
        if(write(fd,&val,1)<0)
        {
            printf("write 0x10 err\n");
			return -1;
        }
        for(i=0;i<18;i++)
        {
        usleep(10000);
        }

         if(read(fd,&buf,3) > 0)
           {
			temp = buf[0];
			temp = (temp << 8) + buf[1];
             flight=(float)temp/1.2;
			//	flight=(buf[0]*256+buf[1])/1.2;
             // printf("light is %6.2f\r\n",flight);
			 if(flight < 0) return -1;
			 else return (int)flight;
            }
          else
          {
			close(fd);    
		    printf("read light error\n");
			return -1;
          }

}





