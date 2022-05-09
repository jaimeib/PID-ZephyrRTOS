/*----------------------------------------------------------------------------
 *-------------------------      M a R T E   O S      ------------------------
 *----------------------------------------------------------------------------
 *                                                             {MARTE_VERSION}
 *
 *                           'l c d _ d r i v e r _ c'
 *
 *                                      C
 *
 *  File 'lcd_driver_c.c'                                  by MAR.
 *
 *  Driver for the LCD of the stm32f769disco board.
 *
 *
 *  {MARTE_COPYRIGHT}
 *
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_lcd.h"

// Defined in GNAT_IO_Driver_Functions
int gnat_io_driver_functions__create();
ssize_t gnat_io_driver_functions__write(int file_descriptor, uint8_t *buffer, size_t bytes);

static int initialized = 0;

// cursor position
static uint16_t cursorLine = 0;
static uint16_t cursorCol = 0;

// screen parameters
static uint16_t lineHeight = 10;
static uint16_t colWidth = 10;
static uint16_t LINES_MX = 10;
static uint16_t COLS_MX = 10;

//static uint8_t lineBuffer[100];
#define TOP_OFFSET 0

#define CURSOR_Y (cursorLine * lineHeight + TOP_OFFSET)
#define CURSOR_X (cursorCol * colWidth)

ssize_t lcd_write(int file_descriptor, uint8_t *buffer, size_t bytes);

/************/
/*  Create  */
/************/

int lcd_create()
{
	// LCD can not be used during MaRTE initialization since, during its
	// initialization, it performs a delay which requires interrupts to be
	// enabled in order to count ticks (otherwise initialization gets blocked in
	// the delay).
	// We use a led to show lcd_write has been called.

	BSP_LED_Init(LED1);
	BSP_LED_Init(LED2);
	BSP_LED_Toggle(LED1);
	return 0;
}

/**************************************/
/*  lcd_end_of_kernel_initialization  */
/**************************************/

int lcd_end_of_kernel_initialization()
{
	uint32_t lcd_status = LCD_OK;

	// Initialize the LCD
	lcd_status = BSP_LCD_Init();
	while (lcd_status != LCD_OK)
		;

	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);

	// Clear the LCD
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	// set font
	BSP_LCD_SetFont(&Font20);

	// console parameters
	lineHeight = ((sFONT *)BSP_LCD_GetFont())->Height;
	colWidth = ((sFONT *)BSP_LCD_GetFont())->Width;
	LINES_MX = (BSP_LCD_GetYSize() - TOP_OFFSET) / lineHeight;
	COLS_MX = BSP_LCD_GetXSize() / colWidth;

	initialized = 1;

	// write marte header
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	lcd_write(1, (uint8_t *)"MaRTE OS executing user's code...\n", 34);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	return 0;
}

static void nextCol();

static void nextLine()
{
	cursorLine = (cursorLine + 1) % LINES_MX;
	cursorCol = 0;
	// clear next line
	for (int i = 0; i < COLS_MX; i++) {
		BSP_LCD_DisplayChar(CURSOR_X, CURSOR_Y, ' ');
		cursorCol++;
	}
	cursorCol = 0;
}

static void nextCol()
{
	cursorCol = (cursorCol + 1) % COLS_MX;
	if (cursorCol == 0) {
		nextLine();
	}
}

/***********/
/*  Write  */
/***********/

ssize_t lcd_write(int file_descriptor, uint8_t *buffer, size_t bytes)
{
	if (!initialized) {
		BSP_LED_Toggle(LED2);
		return bytes;
	}

	for (int i = 0; i < bytes; i++) {
		if (buffer[i] == '\n') {
			nextLine();
		} else {
			BSP_LCD_DisplayChar(CURSOR_X, CURSOR_Y, buffer[i]);
			nextCol();
		}
	}

	/*   if (buffer[bytes-1] != '\n') { */
	/*     BSP_LCD_DisplayChar(CURSOR_X, CURSOR_Y, '\n'); */
	/*   } */

	/*   memcpy(lineBuffer, buffer, bytes); */
	/*   lineBuffer[bytes] = 0; */
	/*    */
	/*   BSP_LCD_DisplayStringAt(CURSOR_X, CURSOR_Y, lineBuffer, LEFT_MODE); */
	/*    */
	/*   cursorLine++; */
	/*   cursorCol = 0; */

	return bytes;
}
