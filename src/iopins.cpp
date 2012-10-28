/****************************************************************************************/
#include <avr/sleep.h>
#include <avr/eeprom.h>
/****************************************************************************************/
#include "avrlibtypes.h"
#include "avrlibdefs.h"
#include "common.h"
/****************************************************************************************/
#include "iopins.h"
/****************************************************************************************/
void InitIOPins(void)
{
  //====================SET ADDRESS BUS TO INPUTS=======================
  DDRA = 0x00;
  PORTA = 0x00;

  DDRC = 0x00;
  PORTC = 0x00;

  //====================SET TO INPUTS=======================
  BIT_SET_LO(OPTO_0_DDR, OPTO_0_PIN); /* INPUT*/
  BIT_SET_LO(OPTO_1_DDR, OPTO_1_PIN); /* INPUT*/
  BIT_SET_LO(POWERFAIL_DDR, POWERFAIL_PIN); /* INPUT*/

  BIT_SET_HI(OPTO_0_DDR, OPTO_0_PIN); /* INPUT*/
  BIT_SET_HI(OPTO_1_DDR, OPTO_1_PIN); /* INPUT*/
  BIT_SET_HI(POWERFAIL_DDR, POWERFAIL_PIN); /* INPUT*/

  BIT_SET_HI(MDM_DTR_DDR, MDM_DTR_PIN); /* OUTPUT FROM MICRO TO MDM */
  BIT_SET_HI(MDM_DTR_PORT, MDM_DTR_PIN);

  BIT_SET_LO(MDM_DCD_DDR, MDM_DCD_PIN); /* INPUT TO MICRO*/
  BIT_SET_HI(MDM_DCD_PORT, MDM_DCD_PIN);

  BIT_SET_HI(MDM_RTS_DDR, MDM_RTS_PIN); /* OUTPUT FROM MICRO TO MDM */
  BIT_SET_HI(MDM_RTS_PORT, MDM_RTS_PIN);

  BIT_SET_LO(MDM_CTS_DDR, MDM_CTS_PIN); /* INPUT TO MICRO*/
  BIT_SET_HI(MDM_CTS_PORT, MDM_CTS_PIN);

  BIT_SET_LO(SIMPRES_CTRL_DDR,SIMPRES_CTRL_PIN);
  BIT_SET_LO(SIMPRES_CTRL_PORT,SIMPRES_CTRL_PIN);

  //====================SET TO OUTPUTS =======================


  BIT_SET_HI(SIMSEL_CTRL_DDR, SIMSEL_CTRL_PIN); /* SET OUTPUT*/
  BIT_SET_HI(SIMSEL_CTRL_PORT, SIMSEL_CTRL_PIN); /* DEFAULT OFF */


  BIT_SET_HI(MDM_VCC_CTRL_DDR, MDM_VCC_CTRL_PIN); /* SET OUTPUT*/
  BIT_SET_HI(MDM_VCC_CTRL_PORT, MDM_VCC_CTRL_PIN); /* DEFAULT OFF */

  BIT_SET_HI(MDM_PWR_KEY_DDR, MDM_PWR_KEY_PIN); /* OUTPUT*/
  BIT_SET_HI(MDM_PWR_KEY_PORT, MDM_PWR_KEY_PIN); /* DEFAULT OFF BIT_SET_HI is off for P-CHANNEL FET*/

  BIT_SET_HI(MDM_CTS_DDR, MDM_CTS_PIN);
  BIT_SET_HI(MDM_CTS_PORT, MDM_CTS_PIN);

  BIT_SET_HI(MDM_RTS_DDR, MDM_RTS_PIN);
  BIT_SET_HI(MDM_RTS_PORT, MDM_RTS_PIN);

  BIT_SET_HI(LED_0_DDR, LED_0_PIN); /* OUTPUT*/
  BIT_SET_HI(LED_0_PORT, LED_0_PIN); /* DEFAULT OFF */

  BIT_SET_HI(LED_1_DDR, LED_1_PIN); /* OUTPUT*/
  BIT_SET_HI(LED_1_PORT, LED_1_PIN); /* DEFAULT OFF */

  BIT_SET_HI(LED_2_DDR, LED_2_PIN); /* OUTPUT*/
  BIT_SET_HI(LED_2_PORT, LED_2_PIN); /* DEFAULT OFF */

  BIT_SET_HI(LED_3_DDR, LED_3_PIN); /* OUTPUT*/
  BIT_SET_HI(LED_3_PORT, LED_3_PIN); /* DEFAULT OFF */

  BIT_SET_HI(DATAFLASH_CS_DDR, DATAFLASH_CS_PIN); /* OUTPUT*/
  BIT_SET_HI(DATAFLASH_CS_PORT, DATAFLASH_CS_PIN); /* DEFAULT OFF */
}
