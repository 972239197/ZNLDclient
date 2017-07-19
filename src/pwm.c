#include "all.h"
#include "pwm.h"

#define FILENAME_SIZE  64
#define high	1
#define low		0
#define PWM_PATH "/sys/class/pwm/pwmchip0"

int pwm_export(int pin_number) {
	char buf[FILENAME_SIZE];
	int  fp;
	int  length;

	if ( pwm_is_exported(pin_number) )
		return 1;

	//write to export file
	fp = open(PWM_PATH "/export", O_WRONLY);
	if ( fp < 0) {
		return -1;
	}

	length = snprintf(buf, sizeof(buf), "%d", pin_number);
	if (write(fp, buf, length * sizeof(char)) == -1) {
		close(fp);
		return -1;
	}

	close(fp);
	return pwm_is_exported(pin_number);
}

int pwm_is_exported(int pin_number) {
	char  buf[FILENAME_SIZE];
	struct stat dir;

	//check if the pwmXX directory exists
	snprintf(buf, sizeof(buf), PWM_PATH "/pwm%d/", pin_number);
	if (stat(buf, &dir) == 0 && S_ISDIR(dir.st_mode)) {
		return 1;
	} else {
		return 0;
	}
}

int  pwm_set_direction(int pin_number, int direction, int period) {
	char buf[FILENAME_SIZE];
	int  fp,fp_t;
	int  length;
	
	if ( !pwm_is_exported(pin_number) ) {
		if ( 1 != pwm_export(pin_number) )
			return -1;
	}

	//write enable file
	snprintf(buf, sizeof(buf),  "%s/pwm%d/enable", PWM_PATH, pin_number);
	fp = open(buf, O_RDWR);
	if ( fp == -1) {
		return -1;
	}

	if ( lseek(fp, 0, SEEK_SET) < 0 ) {
		close(fp);
		return -1;
	}

	switch (direction) {
		case high:
			length = snprintf(buf, sizeof(buf), "1");
			break;
		case low:
			length = snprintf(buf, sizeof(buf), "0");
			break;	
		default:
			close(fp);
			return -1;
	}

	if (write(fp, buf, length * sizeof(char)) == -1) {
		close(fp);
		return -1;
	}
	close(fp);
	
	//write period file
	snprintf(buf, sizeof(buf),  "%s/pwm%d/period", PWM_PATH, pin_number);
	
	fp_t = open(buf, O_RDWR);
	
	if ( fp_t == -1) {
		return -1;
	}
	
	if ( lseek(fp_t, 0, SEEK_SET) < 0 ) {
		close(fp_t);
		return -1;
	}
	
	length = snprintf(buf, sizeof(buf), "%d", period);
	
	if (write(fp_t, buf, length * sizeof(char)) == -1) {
		close(fp_t);
		return -1;
	}
	close(fp_t);
	return 0;
}

int  pwm_set_level(int pin_number, int value) {
	char buf[FILENAME_SIZE];
	int  fp;
	int  length;


	if ( !pwm_is_exported(pin_number) ) {
		if ( 1 != pwm_export(pin_number) )
			return -1;
	}

	//write to value file
	snprintf(buf, sizeof(buf),  "%s/pwm%d/duty_cycle", PWM_PATH, pin_number);
	fp = open(buf, O_RDWR);
	if ( fp == -1) {
		return -1;
	}

	if ( lseek(fp, 0, SEEK_SET) < 0 ) {
		close(fp);
	}

	length = snprintf(buf, sizeof(buf), "%d", value*1000);
	if ( write(fp, buf, length * sizeof(char)) == -1) {
		close(fp);
		return -1;
	}

	close(fp);

	return 0;
}


