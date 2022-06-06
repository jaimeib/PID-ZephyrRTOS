#include <stdio.h>
#include "stm32f7xx_hal.h"
#include <stm32f7xx.h>
#include "stm32f7xx_hal_ltdc.h"
#include "screen.h"

#define BACKGROUND_COLOR LCD_COLOR_WHITE

int screen_initialize()
{
	BSP_LCD_SetFont(&Font24);

	if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize())) {
		printf("Error: BSP_TS_Init\n");
		return -1;
	}
	return 0;
}

int screen_get_x_size()
{
	return BSP_LCD_GetXSize();
}

int screen_get_y_size()
{
	return BSP_LCD_GetYSize();
}

int screen_get_x_touch()
{
	TS_StateTypeDef touch;
	BSP_TS_GetState(&touch);

	if (touch.touchDetected) {
		return touch.touchX[0];
	}
	return -1;
}

void screen_fill_rectangle(uint16_t x_pos, uint16_t y_pos, uint16_t ancho_x, uint16_t alto_y,
			   uint32_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(x_pos, y_pos, ancho_x, alto_y);
}

void screen_delete_rectangle(uint16_t x_pos, uint16_t y_pos, uint16_t ancho_x, uint16_t alto_y)
{
	BSP_LCD_SetTextColor(BACKGROUND_COLOR);
	BSP_LCD_FillRect(x_pos, y_pos, ancho_x, alto_y);
}

void screen_fill_circle(uint16_t x_pos, uint16_t y_pos, uint16_t radio, uint32_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillCircle(x_pos, y_pos, radio);
}

void screen_draw_circle(uint16_t x_pos, uint16_t y_pos, uint16_t radio, uint32_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_DrawCircle(x_pos, y_pos, radio);
}

void screen_delete_circle(uint16_t x_pos, uint16_t y_pos, uint16_t radio)
{
	BSP_LCD_SetTextColor(BACKGROUND_COLOR);
	BSP_LCD_FillCircle(x_pos, y_pos, radio);
}

void screen_print_text(uint16_t x_pos, uint16_t y_pos, char *text, uint32_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_DisplayStringAt(x_pos, y_pos, (unsigned char *)text, LEFT_MODE);
}
