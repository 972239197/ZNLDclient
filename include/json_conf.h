#ifndef __JSON_CONF_H__
#define __JSON_CONF_H__

struct message_info{
	char time[30];
	char light_id[16];
	char net_mode[8];
	int temp;
	int hum;
	int light_s;
	int flash_v;
	int voice;
	int infrared;
	char ip_camera[8];
	int pm25;
	int pm10;
	int bdown;
};


extern struct message_info info;

extern struct json_object *msg_pub_json;  //mqtt推送json对象

struct json_object *msg_sub_json;   //mqtt订阅消息json对象

/* 当前时间 */
#define TIME	"time"

/* 灯的ID号 */
#define LIGHT_ID	"light_id"

/* 联网方式 */
#define NET_MODE	"net_Mode"

/* 温度 */
#define TEMP	"temperature"

/*湿度 */
#define HUM 	"humidity"

/*灯开关 */
#define SWITCH	"light_switch"

/* 光感阈值 */
#define FLASH_VAL	"flash_value"

/* 声控开关*/
#define VOICE_CON	"voice_console"

/*红外感应 */
#define INFRARED	"infrared"

/* 摄像头连接状态 */
#define IPCAMERA	"ip_camera"

/* PM2.5数据 */
#define PM25	"pm2.5"

/* PM10数据 */
#define PM10	"pm10"

/* 灯故障 */
#define BREAKDOWN	"breakdown"

/* pwm  */
#define PWM		"pwm"

extern int init_json_data();
extern void put_json();
extern int get_json();

extern void get_config_for_json();


#endif

