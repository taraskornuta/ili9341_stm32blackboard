#ifndef ILI_9341_SPI_FB_H_
#define ILI_9341_SPI_FB_H_
//#include <stdint.h>



#define ILI9341_PIXEL_WIDTH    240
#define ILI9341_PIXEL_HEIGHT   320
#define ILI9341_PIXEL_COUNT    ILI9341_PIXEL_WIDTH * ILI9341_PIXEL_HEIGHT

// Any LCD needs to implement these common methods, which allow the low-level
// initialisation and pixel-setting details to be abstracted away from the
// higher level drawing and graphics code.

typedef enum
{
  LCD_ORIENTATION_PORTRAIT           = 0,
  LCD_ORIENTATION_LANDSCAPE         = 1,
  LCD_ORIENTATION_PORTRAIT_MIRROR   = 2,
  LCD_ORIENTATION_LANDSCAPE_MIRROR   = 3
} lcdOrientationTypeDef;



typedef struct
{
  unsigned short  x;
  unsigned short  y;
}lcdCursorPosTypeDef;

// This struct is used to indicate the capabilities of different LCDs
typedef struct
{
  unsigned short        width;         // LCD width in pixels (default orientation)
  unsigned short        height;        // LCD height in pixels (default orientation)
  lcdOrientationTypeDef  orientation;   // Whether the LCD orientation can be modified
  unsigned char          touchscreen;   // Whether the LCD has a touch screen
  unsigned char          hwscrolling;   // Whether the LCD support HW scrolling
} lcdPropertiesTypeDef;


#endif //ILI_9341_SPI_FB_H_

