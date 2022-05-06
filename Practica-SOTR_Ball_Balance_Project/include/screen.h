#include <stdint.h>
#undef LED_GREEN
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_lcd.h"
#include "stm32f769i_discovery_ts.h"

int screen_initialize();

int screen_get_x_size();

int screen_get_x_touch();

void screen_fill_rectangle(uint16_t x_pos, uint16_t y_pos,
                           uint16_t ancho_x, uint16_t alto_y,
                           uint32_t color);

void screen_delete_rectangle(uint16_t x_pos, uint16_t y_pos,
                             uint16_t ancho_x, uint16_t alto_y);

void screen_fill_circle(uint16_t x_pos, uint16_t y_pos,
                        uint16_t radio,
                        uint32_t color);

void screen_draw_circle(uint16_t x_pos, uint16_t y_pos,
                        uint16_t radio,
                        uint32_t color);

void screen_delete_circle(uint16_t x_pos, uint16_t y_pos,
                          uint16_t radio);

void screen_print_text(uint16_t x_pos, uint16_t y_pos,
                       char *text,
                       uint32_t color);
