#include <stdint.h>
#include "pwm.h"

void init_pwm(){
    gpio_set_function(PWM_0, GPIO_FUNC_PWM);
    gpio_set_function(PWM_1, GPIO_FUNC_PWM);
    gpio_set_function(PWM_2, GPIO_FUNC_PWM);
    gpio_set_function(PWM_3, GPIO_FUNC_PWM);
    gpio_set_function(PWM_4, GPIO_FUNC_PWM);
    gpio_set_function(PWM_5, GPIO_FUNC_PWM);
}

void setup_pwm_settings(){
    uint slice_pwm0 = pwm_gpio_to_slice_num(PWM_0);
    uint channel_pwm0 = pwm_gpio_to_channel(PWM_0);
    uint slice_pwm1 = pwm_gpio_to_slice_num(PWM_1);
    uint channel_pwm1 = pwm_gpio_to_channel(PWM_1);
    uint slice_pwm2 = pwm_gpio_to_slice_num(PWM_2);
    uint channel_pwm2 = pwm_gpio_to_channel(PWM_2);
    uint slice_pwm3 = pwm_gpio_to_slice_num(PWM_3);
    uint channel_pwm3 = pwm_gpio_to_channel(PWM_3);
    uint slice_pwm4 = pwm_gpio_to_slice_num(PWM_4);
    uint channel_pwm4 = pwm_gpio_to_channel(PWM_4);
    uint slice_pwm5 = pwm_gpio_to_slice_num(PWM_5);
    uint channel_pwm5 = pwm_gpio_to_channel(PWM_5);

    pwm_set_chan_level(slice_pwm0, channel_pwm0, PWM_LEVEL_0);
    pwm_set_chan_level(slice_pwm1, channel_pwm1, PWM_LEVEL_0);
    pwm_set_chan_level(slice_pwm2, channel_pwm2, PWM_LEVEL_0);
    pwm_set_chan_level(slice_pwm3, channel_pwm3, PWM_LEVEL_1);
    pwm_set_chan_level(slice_pwm4, channel_pwm4, PWM_LEVEL_1);
    pwm_set_chan_level(slice_pwm5, channel_pwm5, PWM_LEVEL_1);

    pwm_set_enabled(slice_pwm0, true);
    pwm_set_enabled(slice_pwm1, true);
    pwm_set_enabled(slice_pwm2, true);
    pwm_set_enabled(slice_pwm3, true);
    pwm_set_enabled(slice_pwm4, true);
    pwm_set_enabled(slice_pwm5, true);
}
