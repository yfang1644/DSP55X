//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_lcd.h
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535.h"

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
void EZDSP5535_LCD_init(void);
void LCD_print(Int8 *s, Uint16 cnt, int page);
void LCD_scroll();

