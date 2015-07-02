//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_lcd.c
//////////////////////////////////////////////////////////////////////////////
#include"ezdsp5535_i2c.h"

#define OSD9616_I2C_ADDR 0x3C    // OSD9616 I2C address

#pragma DATA_SECTION(CharGen_6x8, ".charrom")
Uint16 CharGen_6x8[256*3];

/*
 *  Int16 EZDSP5535_LCD_send( Uint16 comdat, Uint16 data )
 * 
 *      Sends 2 bytes of data to the OSD9616
 *
 *      comdat :  0x00  => Command
 *                0x40  => Data
 *      data   :  Data to be sent
 * 
 *  Returns
 *      0 => Success
 *      1 => Fail
 */
Int16 EZDSP5535_LCD_send( Uint16 comdat, Uint8 data )
{
    Uint8 cmd[2];
    cmd[0] = comdat & 0x00FF;     // Specifies whether data is Command or Data
    cmd[1] = data;                // Command / Data
//	EZDSP5535_waitusec(1);
    return EZDSP5535_I2C_write( OSD9616_I2C_ADDR, cmd, 2 );
}

/* 
 *  Int16 EZDSP5535_LCD_multiSend( Uint16* data, Uint16 len )
 *
 *      Sends multiple bytes of data to the OSD9616
 * 
 *      *data :  Pointer to start of I2C transfer
 *       len  :  Length of I2C transaction
 * 
 *  Returns
 *      0 => Success
 *      1 => Fail
 */
Int16 EZDSP5535_LCD_multiSend( Uint8* data, Uint16 len )
{
    Uint16 x;
    Uint8 cmd[10];
    for(x=0;x<len;x++) {			// Command / Data
    	cmd[x] = data[x];
    }
//	EZDSP5535_waitusec(1);
    return EZDSP5535_I2C_write( OSD9616_I2C_ADDR, cmd, len );
}

/* 
 *  Int16 EZDSP5535_OSD9616_init( )
 *
 *      Initialize the OSD9616
 */
void EZDSP5535_LCD_init( )
{
	Uint8 cmd[4];                    // For multibyte commands
	int i;

	CSL_GPIO_REGS->IODIR1 |= 0x1000;		// for LCD
	CSL_GPIO_REGS->IOOUTDATA1 |= 0x1000;

	/* Initialize OSD9616 display */
	EZDSP5535_LCD_send(0x00,0x00); // Set low column address
	EZDSP5535_LCD_send(0x00,0x10); // Set high column address
	EZDSP5535_LCD_send(0x00,0x40); // Set start line address

	cmd[0] = 0x00 & 0x00FF;  // Set contrast control register
	cmd[1] = 0x81;
	cmd[2] = 0x7f;
	EZDSP5535_LCD_multiSend( cmd, 3 );

	EZDSP5535_LCD_send(0x00,0xa1); // Set segment re-map 95 to 0
	EZDSP5535_LCD_send(0x00,0xa6); // Set normal display

	cmd[0] = 0x00 & 0x00FF;            // Set multiplex ratio(1 to 16)
	cmd[1] = 0xa8;
	cmd[2] = 0x0f;
	EZDSP5535_LCD_multiSend( cmd, 3 );

	EZDSP5535_LCD_send(0x00,0xd3); // Set display offset
	EZDSP5535_LCD_send(0x00,0x00); // Not offset
	EZDSP5535_LCD_send(0x00,0xd5); // Set display clock divide ratio/oscillator frequency
	EZDSP5535_LCD_send(0x00,0xf0); // Set divide ratio

	cmd[0] = 0x00 & 0x00FF;  // Set pre-charge period
	cmd[1] = 0xd9;
	cmd[2] = 0x22;
	EZDSP5535_LCD_multiSend( cmd, 3 );

	cmd[0] = 0x00 & 0x00FF;  // Set com pins hardware configuration
	cmd[1] = 0xda;
	cmd[2] = 0x02;
	EZDSP5535_LCD_multiSend( cmd, 3 );

    EZDSP5535_LCD_send(0x00,0xdb); // Set vcomh
	EZDSP5535_LCD_send(0x00,0x49); // 0.83*vref

	cmd[0] = 0x00 & 0x00FF;  //--set DC-DC enable
	cmd[1] = 0x8d;
	cmd[2] = 0x14;
	EZDSP5535_LCD_multiSend( cmd, 3 );

	EZDSP5535_LCD_send(0x00,0xaf); // Turn on oled panel    

	EZDSP5535_LCD_send(0x00,0x2e);  // Deactivate Scrolling

	EZDSP5535_LCD_send(0x00,0x00);   	// Set low column address
	EZDSP5535_LCD_send(0x00,0x10);   	// Set high column address
	EZDSP5535_LCD_send(0x00,0xb0 + 0); // Set page for page 0 to page 5
	for(i = 0; i < 128; i++)
		EZDSP5535_LCD_send(0x40, 0x00);	// clear
		
	EZDSP5535_LCD_send(0x00,0x00);		// Set low column address
	EZDSP5535_LCD_send(0x00,0x10);		// Set high column address
	EZDSP5535_LCD_send(0x00,0xb0 + 1);	// Set page for page 0 to page 5
	for(i = 0; i < 128; i++)
		EZDSP5535_LCD_send(0x40, 0x00);	// clear
		
}

void LCD_print(char *s, int page)
{
	int n, i;
	Uint8 c;

	/* Fill page */
	EZDSP5535_LCD_send(0x00,0x00);   	// Set low column address
	EZDSP5535_LCD_send(0x00,0x10);   	// Set high column address
	EZDSP5535_LCD_send(0x00,0xb0+page); // Set page for page 0 to page 5

	for(n = 0; ;n++) {
		i = s[n];
		if(i == 0)	break;
		c = (CharGen_6x8[i*3 + 0] >> 8) & 0x00ff;
		EZDSP5535_LCD_send(0x40, c);
		c = (CharGen_6x8[i*3 + 0]     ) & 0x00ff;
		EZDSP5535_LCD_send(0x40, c);
		c = (CharGen_6x8[i*3 + 1] >> 8) & 0x00ff;
		EZDSP5535_LCD_send(0x40, c);
		c = (CharGen_6x8[i*3 + 1]     ) & 0x00ff;
		EZDSP5535_LCD_send(0x40, c);
		c = (CharGen_6x8[i*3 + 2] >> 8) & 0x00ff;
		EZDSP5535_LCD_send(0x40, c);
		c = (CharGen_6x8[i*3 + 2]     ) & 0x00ff;
		EZDSP5535_LCD_send(0x40, c);
	}
	n = n * 6;
	for(; n < 128; n++)
		EZDSP5535_LCD_send(0x40, 0x00);
}

void LCD_scroll()
{
	Uint8 cmd[10];
	
    /* Set vertical and horizontal scrolling */
    cmd[0] = 0x00;
    cmd[1] = 0x29;  // Vertical and Right Horizontal Scroll
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x03;  // Set time interval between each scroll step
    cmd[5] = 0x01;  // Define end page address
    cmd[6] = 0x01;  // Vertical scrolling offset
    EZDSP5535_LCD_multiSend( cmd, 7 );
    EZDSP5535_LCD_send(0x00,0x2f);

    /* Keep first 8 rows from vertical scrolling  */
    cmd[0] = 0x00;
    cmd[1] = 0xa3;  // Set Vertical Scroll Area
    cmd[2] = 0x08;  // Set No. of rows in top fixed area
    cmd[3] = 0x08;  // Set No. of rows in scroll area
	EZDSP5535_LCD_multiSend( cmd, 4 );
}

extern Int16 rcvbuf[];
Uint16 amplitude2bargraph(Uint16 val)
{
	if(val > 8192)
		return 0xffff;
	if(val > 6000)
		return 0x7fff;
	if(val > 4096)
		return 0x3fff;
	if(val > 3000)
		return 0x1fff;
	if(val > 2048)
		return 0x0fff;
	if(val > 1500)
		return 0x07ff;
	if(val > 1024)
		return 0x03ff;
	if(val > 750)
		return 0x01ff;
	if(val > 512)
		return 0x00ff;
	if(val > 384)
		return 0x007f;
	if(val > 256)
		return 0x003f;
	if(val > 192)
		return 0x001f;
	if(val > 128)
		return 0x000f;
	if(val > 64)
		return 0x0007;
	if(val > 51)
		return 0x0003;
	if(val > 32)
		return 0x0001;
	return 0;
}

void LCD_display_bargraph()
{
	int i;
	Uint16 input[256];

	for (i = 0; i <96; i++)
		input[i] = amplitude2bargraph(rcvbuf[i*5]);

	EZDSP5535_LCD_send(0x00,0x2E);           // Turn off scrolling

	/* Fill page 0 */
	EZDSP5535_LCD_send(0x00,0x00);   // Set low column address
	EZDSP5535_LCD_send(0x00,0x10);   // Set high column address
	EZDSP5535_LCD_send(0x00,0xb0+0); // Set page for page 0 to page 5

	/* Send topline from high to low */
	for (i = 0; i <96; i++)
		EZDSP5535_LCD_send(0x40, input[i] & 0xff);

	/* Write to page 1*/
	EZDSP5535_LCD_send(0x00,0x00);   // Set low column address
	EZDSP5535_LCD_send(0x00,0x10);   // Set high column address
	EZDSP5535_LCD_send(0x00,0xb0+1); // Set page for page 0 to page 5

	/* Send bottomline */
	for (i = 0; i <96; i++)
		EZDSP5535_LCD_send(0x40, input[i] >> 8);
}

