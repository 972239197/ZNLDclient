#ifndef __PWM_H__
#define __PWM_H__

extern int pwm_export(int pin_number);
extern int pwm_is_exported(int pin_number);
extern int pwm_set_direction(int pin_number, int direction, int period);
extern int pwm_set_level(int pin_number, int value);

#define set_pwm_dir()	pwm_set_direction(0, 1, 100000)
#define set_pwm_level(a)	pwm_set_level(0, a)

#endif
