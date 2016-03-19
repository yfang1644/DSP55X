//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_lcd.c
//////////////////////////////////////////////////////////////////////////////
#include"ezdsp5535_i2c.h"

#define OSD9616_I2C_ADDR 0x3C    // OSD9616 I2C address

long font[] = {
	0x5402402a, 0x081c3e0c, 0x55aa55aa, 0xf824ff04,
	0xf8a09f14, 0x609f0a05, 0xf0101f0a, 0x20502000,
	0x123a1200, 0xf0402ff1, 0xe0e80f08, 0x1010f000,
	0x10101f00, 0x1f101000, 0x1e020200, 0x1010ff10,
	0x80808080, 0x40404040, 0x10101010, 0x02020202,
	0x01010101, 0xff101000, 0x1010ff00, 0x10f01010,
	0x101f1010, 0xff000000, 0x122a4600, 0x462a1200,
	0x1e101e10, 0x141e3c14, 0x123c5224, 0x08000000,
	0x00000000, 0x7a000000, 0x70007000, 0xfe28fe28,
	0x54fe5408, 0x68102c00, 0x6c926c02, 0x70000000,
	0x3c420000, 0x423c0000, 0x2a1c1c2a, 0x08083e08,
	0x01060400, 0x08080808, 0x02070200, 0x06081060,
	0x3c423c00, 0x227e0200, 0x264a4a32, 0x4452724c,
	0x18287e08, 0x7452524c, 0x3c52520c, 0x40465860,
	0x2c52522c, 0x304a4a3c, 0x36360000, 0x01363400,
	0x18244200, 0x14141414, 0x42241800, 0x204a3000,
	0x3e99a578, 0x3e48483e, 0x7e52522c, 0x3c424224,
	0x7e42423c, 0x7e525242, 0x7e505040, 0x3c424a2c,
	0x7e10107e, 0x427e4200, 0x04427c40, 0x7e102c42,
	0x7e020202, 0x7e30307e, 0x7e301c7e, 0x3c42423c,
	0x7e484830, 0x3c4a463d, 0x7e484836, 0x24524a24,
	0x407e4000, 0x7c02027c, 0x78060678, 0x7e0c0c7e,
	0x66181866, 0x60101e60, 0x464a5262, 0x7e424200,
	0x60100806, 0x42427e00, 0x20402000, 0x01010101,
	0x40200000, 0x0c12121e, 0x7e12120c, 0x0c121200,
	0x0c12127e, 0x0c161a08, 0x083e4820, 0x0815150e,
	0x7e10100e, 0x125e0200, 0x02015e00, 0x7e080816,
	0x427e0200, 0x1e0c1c0e, 0x1e10100e, 0x0c12120c,
	0x1f141408, 0x0814141f, 0x1e081008, 0x0a1a1400,
	0x107c1204, 0x1c02021e, 0x1c021c00, 0x1e0c061c,
	0x120c0c12, 0x1a05051e, 0x12161a12, 0x1054aa82,
	0x7e000000, 0x82aa5410, 0x20402040, 0x10101010,
	};
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
Int16 EZDSP5535_LCD_send( Uint16 comdat, Uint16 data )
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
	int n, i, c;

	/* Fill page */
	EZDSP5535_LCD_send(0x00,0x00);   	// Set low column address
	EZDSP5535_LCD_send(0x00,0x10);   	// Set high column address
	EZDSP5535_LCD_send(0x00,0xb0+page); // Set page for page 0 to page 5

	for(n = 0; ;n++) {
		i = s[n];
		if(i == 0)	break;
		c = (Int16)((font[i] >> 24) & 0x000000ff);
		EZDSP5535_LCD_send(0x40, c);
		c = (Int16)((font[i] >> 16) & 0x000000ff);
		EZDSP5535_LCD_send(0x40, c);
		c = (Int16)((font[i] >> 8) & 0x000000ff);
		EZDSP5535_LCD_send(0x40, c);
		c = (Int16)(font[i] & 0x000000ff);
		EZDSP5535_LCD_send(0x40, c);
		EZDSP5535_LCD_send(0x40, 0x00);	// line blank for space
	}
	n = n * 5;
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
