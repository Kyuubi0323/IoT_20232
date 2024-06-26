#ifndef _INPUT_HADILAO_H_
#define _INPUT_HADILAO_H_
/**
 * @file input_hadilao.h
 * @Kyuubi {Kyuubi} ({khoi.nv0323.work@gmail.com})
 * @brief 
 * @version 0.1
 * @date 2024-04-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "esp_err.h"
#include "hal/gpio_types.h"

typedef void (*input_callback_t)(int);

typedef enum
{
    LO_TO_HI = 1,
    HI_TO_LO = 2,
    ANY_EDLE = 3
} interrupt_type_edle_t;

void input_io_create(gpio_num_t gpio_num, interrupt_type_edle_t type);
uint8_t input_io_get_level(gpio_num_t gpio_num);
void input_set_callback(void *cb);


#endif
