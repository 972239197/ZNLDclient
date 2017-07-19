/*
 * example of GPIO
 *
 * 
 */

#include "all.h"
#include "gpio.h"

#define set_onoff_out()		gpio_set_direction(14, GPIO_OUTPUT)
#define set_voice_in()		gpio_set_direction(15, GPIO_INPUT)
#define set_lwir_in()		gpio_set_direction(44, GPIO_INPUT)
#define set_bd_in()			gpio_set_direction(16, GPIO_INPUT)

#define set_onoff_high()	gpio_write( 14, 1 )
#define set_onoff_low()		gpio_write( 14, 0 )	

int v_flags = 1;
int w_flags = 1;

void send_voice_state(int sta) //声音控制延时
{
	if(sta == 0)
	{
		if(v_flags == 1 || v_flags == 0)
		{
		set_onoff_high();
		set_pwm_level(100);
		v_flags=2;
		info.voice = 1;
		}
	}
	else {
		if(v_flags == 1)
		{
		set_onoff_low();
		set_pwm_level(0);
		v_flags =0;	
		info.voice = 0;	
		}
	}
	if(1 < v_flags && v_flags < 2000) {   //延时大概20秒
		v_flags++;
		if(sta == 0) v_flags=2;
	}
	else if(v_flags > 1999) v_flags=1;
	
}

void send_lwir_state(int sta)   //红外控制延时
{
	if(sta == 1)
	{
		if(w_flags == 1 || w_flags == 0)
		{
		set_onoff_high();
		set_pwm_level(100);
		w_flags=2;
		info.infrared = 1;
		}
	}
	else {
		if(w_flags == 1)
		{
		set_onoff_low();
		set_pwm_level(0);
		w_flags =0;	
		info.infrared = 0;	
		}
	}
	if(1 < w_flags && w_flags < 2000) {   //延时大概20秒
		w_flags++;
		if(sta == 1) w_flags=2;
	}
	else if(w_flags > 1999) w_flags=1;
	
}

void *get_gpio_thread(void *pdata)
{
	int voice,lwir;
	set_onoff_out();
	set_voice_in();
	set_lwir_in();
	set_bd_in();
	set_pwm_dir();
	
	while(1)
	{
		voice = gpio_read(15);//read_gpio(15);
		send_voice_state(voice);  //1 no voice,0 is voice

		lwir = gpio_read(44);//read_gpio(44);
		send_lwir_state(lwir);	//1 is persion，0 no persion
		
		info.bdown = gpio_read(16);//read_gpio(16);
		delay(10);			
	}
	
	
}

void creat_get_gpio_thread()
{

	pthread_t thread;
    pthread_attr_t thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &thread_attr, get_gpio_thread, NULL) < 0)
	{
		perror("pthread_create get gpio error");
	}
}









