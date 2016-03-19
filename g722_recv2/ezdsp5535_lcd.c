//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_lcd.c
//////////////////////////////////////////////////////////////////////////////

#include"ezdsp5535_i2c.h"

#define OSD9616_I2C_ADDR 0x3C    // OSD9616 I2C address

long font[] = {			// ASCII 0-127
		0x5402402a, 0x307c3810, 0x55aa55aa, 0x20ff241f,
		0x28f9051f, 0xa050f906, 0x50f8080f, 0x00040a04,
		0x00485c48, 0x8ff4020f, 0x10f01707, 0x000f0808,
		0x00f80808, 0x000808f8, 0x00404078, 0x08ff0808,
		0x01010101, 0x02020202, 0x08080808, 0x40404040,
		0x80808080, 0x000808ff, 0x00ff0808, 0x08080f08,
		0x0808f808, 0x000000ff, 0x00625448, 0x00485462,
		0x08780878, 0x283c7828, 0x244a3c48, 0x00000010,
		0x00000000, 0x0000005e, 0x000e000e, 0x147f147f,
		0x102a7f2a, 0x00340816, 0x40364936, 0x0000000e,
		0x0000423c, 0x00003c42, 0x54383854, 0x107c1010,
		0x00206080, 0x10101010, 0x0040e040, 0x06081060,
		0x003c423c, 0x00407e44, 0x4c525264, 0x324e4a22,
		0x107e1418, 0x324a4a2e, 0x304a4a3c, 0x061a6202,
		0x344a4a34, 0x3c52520c, 0x00006c6c, 0x002c6c80,
		0x00422418, 0x28282828, 0x00182442, 0x000c5204,
		0x1ea5997c, 0x7c12127c, 0x344a4a7e, 0x2442423c,
		0x3c42427e, 0x424a4a7e, 0x020a0a7e, 0x3452423c,
		0x7e08087e, 0x00427e42, 0x023e4220, 0x4234087e,
		0x4040407e, 0x7e0c0c7e, 0x7e380c7e, 0x3c42423c,
		0x0c12127e, 0xbc62523c, 0x6c12127e, 0x24524a24,
		0x00027e02, 0x3e40403e, 0x1e60601e, 0x7e30307e,
		0x66181866, 0x06780806, 0x464a5262, 0x0042427e,
		0x60100806, 0x007e4242, 0x00040204, 0x80808080,
		0x00000402, 0x78484830, 0x3048487e, 0x00484830,
		0x7e484830, 0x10586830, 0x04127c10, 0x70a8a810,
		0x7008087e, 0x00407a48, 0x007a8040, 0x6810107e,
		0x00407e42, 0x70383078, 0x70080878, 0x30484830,
		0x102828f8, 0xf8282810, 0x10081078, 0x00285850,
		0x20483e08, 0x78404038, 0x00384038, 0x38603078,
		0x48303048, 0x78a0a058, 0x48586848, 0x41552a08,
		0x0000007e, 0x082a5541, 0x02040204, 0x08080808
};
/*
 *  Int16 EZDSP5535_LCD_send(Int8 comdat, Int8 data)
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
Int16 EZDSP5535_LCD_send(Int8 comdat, Int8 data)
{
    Int8 cmd[2];
    cmd[0] = comdat;    		 // Specifies whether data is Command or Data
    cmd[1] = data;                // Command / Data
    EZDSP5535_waitusec(1000);
    return EZDSP5535_I2C_write(OSD9616_I2C_ADDR, cmd, 2);
}

/* 
 *  Int16 EZDSP5535_LCD_multiSend( Int8* data, Uint16 len )
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
Int16 EZDSP5535_LCD_multiSend(Int8* data, Uint16 len)
{
    Uint16 x;
    Int8 cmd[10];
    for(x=0;x<len;x++)               // Command / Data
    {
    	cmd[x] = data[x];
    }
    EZDSP5535_waitusec( 1000 );
    return EZDSP5535_I2C_write(OSD9616_I2C_ADDR, cmd, len);
}

/* 
 *  Int16 EZDSP5535_OSD9616_init( )
 *
 *      Initialize the OSD9616
 */
void EZDSP5535_LCD_init(void)
{
	Int8 cmd[4];                    // For multibyte commands

	/* Initialize OSD9616 display */
	EZDSP5535_LCD_send(0x00,0x00); // Set low column address
	EZDSP5535_LCD_send(0x00,0x10); // Set high column address
	EZDSP5535_LCD_send(0x00,0x40);	// Set start line address

	cmd[0] = 0x00;					// Set contrast control register
	cmd[1] = 0x81;
	cmd[2] = 0x7f;
	EZDSP5535_LCD_multiSend(cmd, 3);

	EZDSP5535_LCD_send(0x00,0xa1); // Set segment re-map 95 to 0
	EZDSP5535_LCD_send(0x00,0xa6); // Set normal display

	cmd[0] = 0x00;					// Set multiplex ratio(1 to 16)
	cmd[1] = 0xa8;
	cmd[2] = 0x0f;
	EZDSP5535_LCD_multiSend(cmd, 3);

	EZDSP5535_LCD_send(0x00,0xd3); // Set display offset
	EZDSP5535_LCD_send(0x00,0x00); // Not offset
	EZDSP5535_LCD_send(0x00,0xd5); // Set display clock divide ratio/oscillator frequency
	EZDSP5535_LCD_send(0x00,0xf0); // Set divide ratio

	cmd[0] = 0x00;					// Set pre-charge period
	cmd[1] = 0xd9;
	cmd[2] = 0x22;
	EZDSP5535_LCD_multiSend(cmd, 3);

	cmd[0] = 0x00;					// Set com pins hardware configuration
	cmd[1] = 0xda;
	cmd[2] = 0x02;
	EZDSP5535_LCD_multiSend(cmd, 3);

    EZDSP5535_LCD_send(0x00,0xdb); // Set vcomh
	EZDSP5535_LCD_send(0x00,0x49); // 0.83*vref

	cmd[0] = 0x00;					//--set DC-DC enable
	cmd[1] = 0x8d;
	cmd[2] = 0x14;
	EZDSP5535_LCD_multiSend( cmd, 3 );

	EZDSP5535_LCD_send(0x00,0xaf); // Turn on oled panel    

	EZDSP5535_LCD_send(0x00,0x2e);  // Deactivate Scrolling
}

void printString(Int8 *s, Uint16 cnt)
{
	int n, i;
	Int8 c;
	for(n = cnt - 1; n >= 0; n--)
	{
		i = s[n];
		c = (Int8)((font[i] >> 24) & 0x000000ff);
		EZDSP5535_LCD_send(0x40, c);
		c = (Int8)((font[i] >> 16) & 0x000000ff);
		EZDSP5535_LCD_send(0x40, c);
		c = (Int8)((font[i] >> 8) & 0x000000ff);
		EZDSP5535_LCD_send(0x40, c);
		c = (Int8)(font[i] & 0x000000ff);
		EZDSP5535_LCD_send(0x40, c);
		EZDSP5535_LCD_send(0x40, 0x00);	// line blank for space
	}
}

void LCD_print(Int8 *s, Uint16 cnt, int page)
{
	int i;

	/* Fill page */
	EZDSP5535_LCD_send(0x00,0x00);   	// Set low column address
	EZDSP5535_LCD_send(0x00,0x10);   	// Set high column address
	EZDSP5535_LCD_send(0x00,0xb0+page); // Set page for page 0 to page 5
	for(i=0;i<128;i++)
		EZDSP5535_LCD_send(0x40,0x00);

	printString(s, cnt);
}

void LCD_scroll()
{
	Int8 cmd[10];
	
    /* Set vertical and horizontal scrolling */
    cmd[0] = 0x00;
    cmd[1] = 0x29;  // Vertical and Right Horizontal Scroll
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x03;  // Set time interval between each scroll step
    cmd[5] = 0x01;  // Define end page address
    cmd[6] = 0x01;  // Vertical scrolling offset
    EZDSP5535_LCD_multiSend(cmd, 7);
    EZDSP5535_LCD_send(0x00,0x2f);

    /* Keep first 8 rows from vertical scrolling  */
    cmd[0] = 0x00;
    cmd[1] = 0xa3;  // Set Vertical Scroll Area
    cmd[2] = 0x08;  // Set No. of rows in top fixed area
    cmd[3] = 0x08;  // Set No. of rows in scroll area
	EZDSP5535_LCD_multiSend(cmd, 4);
}
