#ifndef _OUTPUT_HADILAO_H_
#define _OUTPUT_HADILAO_H_
/**
 * @file output_hadilao.h
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

void output_io_create(gpio_num_t);
void output_io_set_level(gpio_num_t, int);
void output_io_toggle(gpio_num_t);

#endif 
