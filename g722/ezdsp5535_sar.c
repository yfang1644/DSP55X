//////////////////////////////////////////////////////////////////////////////
// * File name: ezdsp5535_sar.c
//////////////////////////////////////////////////////////////////////////////

#include "ezdsp5535.h"
#include "ezdsp5535_sar.h"

/*
 *   Init_SAR(void)
 *       Initialize SAR ADC
 * 
 *   Returns
 *     0  :  Success
 *     1  :  Fail
 */
void EZDSP5535_SAR_init(void)
{
    /* Initialize the SAR module */
    /* Reset SAR A/D Registers */
    CSL_SAR_REGS->SARCTRL = CSL_ANACTRL_SARCTRL_RESETVAL
    						| CSL_ANACTRL_SARCTRL_CHSEL_MASK;
    
    CSL_SAR_REGS->SARCLKCTRL = CSL_ANACTRL_SARCLKCTRL_RESETVAL
    					    & CSL_ANACTRL_SARCLKCTRL_ADCCLKDIV_MASK;
  
    CSL_SAR_REGS->SARPINCTRL = CSL_ANACTRL_SARPINCTRL_RESETVAL;
    CSL_SAR_REGS->SARGPOCTRL = CSL_ANACTRL_SARGPOCTRL_RESETVAL;

    /* provide delay after reset */
	EZDSP5535_waitusec(10);

    /* Configuring CLOCKOUT pin */
    CSL_FINST (CSL_SYSCTRL_REGS->CCSSR, SYS_CCSSR_SRC, MODE9);
    /* Open SAR channel 3 */
    /* Set multich discharge of array for every conversion*/
    CSL_SAR_REGS->SARCTRL = (3 << 12) | (0<<11);

    /* Reset the SAR clock control Register */
    CSL_SAR_REGS->SARCLKCTRL = CSL_ANACTRL_SARCLKCTRL_RESETVAL;
    /* Configuring ClkDivider value */
    CSL_SAR_REGS->SARCLKCTRL = 0x0b;

    /* Enabling PwrUp SAR A/D module */
    CSL_SAR_REGS->SARPINCTRL =  0x0000	/* status mask bit14 */
							  | 0x2000	/* powerup bias */
							  | 0x1000	/* powerup */
							  | 0x0000	/* disable reference buffer, bit10*/
							  | 0x0000	/* bandgap ref to 0.8v, bit9 */
							  | 0x0100	/* set Reference volatge to VDD_SAR */
							  | 0x0000	/* disable AVddMwas, bit3 */
							  | 0x0000	/* not ground analog ch0, bit1 */
							  | 0x0000;	/* A/D is based on Vin. bit0 */

//    CSL_SAR_REGS->SARCTRL &= ~(1<<11);	/* continuous conversion */

    /* Start the continuous conversion */
    CSL_SAR_REGS->SARCTRL |= 0x8000;
}

/*
 *   Get_Sar_Key(void)
 *     Find key pressed depending on value returned by SAR ADC
 * 
 *   Returns
 *     val  :  Value of key returned
 */
Uint16 EZDSP5535_SAR_getKey(void)
{
    Uint16 val;

    /* Check whether the ADC data is available or not */
    val = CSL_SAR_REGS->SARDATA;
	if(val & 0x8000)
		return NoKey;

	val &= 0x3ff;
 
    /* Account for percentage of error */
    if((val < SW1 + 16) && (val > SW1 - 16))
        val = SW1;
    else if((val < SW2 + 16) && (val > SW2 - 16))
        val = SW2;
    else if((val < SW12 + 16) && (val > SW12 - 16))
        val = SW12;
    else if((val < NoKey + 16) && (val > NoKey - 16))
        val = NoKey;
    else
        val = NoKey;

    return val;
}
