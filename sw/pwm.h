#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWM_0 28
#define PWM_1 27
#define PWM_2 22
#define PWM_3 21
#define PWM_4 20
#define PWM_5 19

// //standard PWM levels
// #define PWM_LEVEL_0 50000
// #define PWM_LEVEL_1 62500

//reduced PWM levels for debugging
#define PWM_LEVEL_0 800
#define PWM_LEVEL_1 800

#define PWM_SYSCLK_DIV 2

#define PWM_DUTY ((uint16_t) 62500)


void init_pwm();
void setup_pwm_settings();
