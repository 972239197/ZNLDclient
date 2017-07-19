

#ifndef __ALL_H__
#define __ALL_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/inotify.h>  
#include <limits.h>  
#include <linux/sockios.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <pthread.h>
#include <malloc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <semaphore.h> 
#include <sys/sem.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <termios.h>

/* json */

#include <json-c/json.h>
#include <json-c/json_util.h>
#include <json-c/json_object.h>

#include "message_sub.h"
#include "gpio.h"
#include "notify.h"
#include "uart_process.h"
#include "uart_recv.h"
#include "c_json.h"
#include "json_conf.h"
#include "light_iic.h"
#include "SHT1x.h"
#include "id_time.h"
#include "pwm.h"

#endif

