#include "all.h"							
#include "json_conf.h"

struct message_info info;
struct json_object *msg_pub_json;
struct json_object *msg_sub_json;

int get_mac(char *mac_str)
{
    struct ifreq ifreq;
    int i,sock = 0;
    unsigned char mac[20] = {0};

    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("error sock");
        return -1;
    }

    strcpy(ifreq.ifr_name, "eth0");
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) < 0)
    {
        perror("error ioctl");
        return -1;
    }

    for(i = 0; i < 6; i++){
		mac[i] = (unsigned char)ifreq.ifr_hwaddr.sa_data[i];
        sprintf(mac_str+2*i, "%02X", mac[i]);
    }
    mac_str[strlen(mac_str)] = 0;

	return 0;
}

int init_json_data()
{
	msg_pub_json = json_object_new_object();

	if(NULL == msg_pub_json)
	{
		printf("new_object pub err\n");
		return -1;
	}
	memset(info.time, 0 ,sizeof(info.time));
	memset(info.light_id, 0 ,sizeof(info.light_id));
	memset(info.net_mode, 0 ,sizeof(info.net_mode));
	memset(info.ip_camera, 0 ,sizeof(info.ip_camera));

	get_mac(info.light_id);

	info.temp = 0;
	info.hum = 0;
	info.light_s = 0;
	info.flash_v = 0;
	info.voice = 0;
	info.infrared = 0;
	info.pm25 = 0;
	info.pm10 = 0;
	info.bdown = 0;		
	return 0;	
}

/* 配置json */
void put_json()
{
	json_object_object_add(msg_pub_json, TIME, json_object_new_string(info.time));
	json_object_object_add(msg_pub_json, LIGHT_ID, json_object_new_string(info.light_id));
	json_object_object_add(msg_pub_json, NET_MODE, json_object_new_string(info.net_mode));
	json_object_object_add(msg_pub_json, TEMP, json_object_new_int(info.temp));
	json_object_object_add(msg_pub_json, HUM, json_object_new_int(info.hum));
	json_object_object_add(msg_pub_json, SWITCH, json_object_new_int(info.light_s));
	json_object_object_add(msg_pub_json, FLASH_VAL, json_object_new_int(info.flash_v));
	json_object_object_add(msg_pub_json, VOICE_CON, json_object_new_int(info.voice));
	json_object_object_add(msg_pub_json, INFRARED, json_object_new_int(info.infrared));
	json_object_object_add(msg_pub_json, IPCAMERA, json_object_new_string(info.ip_camera));
	json_object_object_add(msg_pub_json, PM25, json_object_new_int(info.pm25));
	json_object_object_add(msg_pub_json, PM10, json_object_new_int(info.pm10));
	json_object_object_add(msg_pub_json, BREAKDOWN, json_object_new_int(info.bdown));

	printf_json((char *)json_object_to_json_string(msg_pub_json));
	save_json_to_file((char *)json_object_to_json_string(msg_pub_json), "/tmp/msg_pub.dat");

}



void get_config_for_json()
{
	char id_buf[16];
	int pwm;
	memset(id_buf, 0 ,sizeof(id_buf));
		
	__json_key_to_string(msg_sub_json, LIGHT_ID, id_buf, 16);
//	info.temp = __json_key_to_int(msg_sub_json, TEMP);
//	info.hum = __json_key_to_int(msg_sub_json, HUM);
	info.light_s = __json_key_to_int(msg_sub_json, SWITCH);
//	info.flash_v = __json_key_to_int(msg_sub_json, FLASH_VAL);
//	info.voice = __json_key_to_int(msg_sub_json, VOICE_CON);
//	__json_key_to_string(msg_sub_json, IPCAMERA, info.ip_camera, 8);
//	info.pm25 = __json_key_to_int(msg_sub_json, PM25);
//	info.pm10 = __json_key_to_int(msg_sub_json, PM10);
//	info.bdown = __json_key_to_int(msg_sub_json, BREAKDOWN);
	pwm = __json_key_to_int(msg_sub_json, PWM);
	
	if(strcasecmp(id_buf, info.light_id) == 0)
	{
		printf_json((char *)json_object_to_json_string(msg_sub_json));
			
		if(info.light_s == 1) {
			gpio_write(14,GPIO_HIGH);
			v_flags = -1;
		 }
		else if(info.light_s == 0) {
			gpio_write(14,GPIO_LOW);
			v_flags = 1;
		 }
		 set_pwm_level(pwm);
	}
}

/*
从msg_sub.dat 获取json 结构体中获取系统信息
*/
int get_json()
{
		msg_sub_json = json_object_from_file("/tmp/msg_sub.dat");
		if(NULL == msg_sub_json) {
			printf("not json_object\n");
			return -1;
		}
		printf("form /tmp/msg_sub.dat : \r\n");
		system("cp /tmp/msg_sub.dat /root/");
		get_config_for_json();
		
		return 0;
	
}



