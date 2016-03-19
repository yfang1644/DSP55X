//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_led.h
//////////////////////////////////////////////////////////////////////////////
#include "ezdsp5535.h"

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16 EZDSP5535_LED_init( );
Int16 EZDSP5535_LED_getall( Uint16 *pattern );
Int16 EZDSP5535_LED_setall( Uint16 pattern );
Int16 EZDSP5535_LED_on( Uint16 number );
Int16 EZDSP5535_LED_off( Uint16 number );
Int16 EZDSP5535_LED_toggle( Uint16 number );
Int16 EZDSP5535_XF_on();
Int16 EZDSP5535_XF_off();
Int16 EZDSP5535_XF_get();
Int16 EZDSP5535_XF_toggle();
