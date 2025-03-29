#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWM_0 28
#define PWM_1 27
#define PWM_2 22
#define PWM_3 21
#define PWM_4 20
#define PWM_5 19

#define PWM_LEVEL_0 10000
#define PWM_LEVEL_1 12500


void init_pwm();
void setup_pwm_settings();