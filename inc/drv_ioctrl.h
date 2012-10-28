#ifndef DRV_IOCTRL_H
#define DRV_IOCTRL_H

#include <avr/io.h>

//====================PORTB=======================

#define DATAFLASH_CS_PORT           PORTB
#define DATAFLASH_CS_DDR            DDRB
#define DATAFLASH_CS_PORTIN         PINB
#define DATAFLASH_CS_PIN            0


#define OPTO_0_PORT           PORTB
#define OPTO_0_DDR            DDRB
#define OPTO_0_PORTIN         PINB
#define OPTO_0_PIN            4

#define OPTO_1_PORT           PORTB
#define OPTO_1_DDR            DDRB
#define OPTO_1_PORTIN         PINB
#define OPTO_1_PIN            5

#define MDM_LED_PORT          PORTB
#define MDM_LED_DDR           DDRB
#define MDM_LED_PORTIN        PINB
#define MDM_LED_PIN           6

//====================PORTD=======================
#define MDM_PWR_RDY_PORT      PORTD
#define MDM_PWR_RDY_DDR       DDRD
#define MDM_PWR_RDY_PORTIN    PIND
#define MDM_PWR_RDY_PIN       4

#define SIMPRES_CTRL_PORT     PORTD
#define SIMPRES_CTRL_DDR      DDRD
#define SIMPRES_CTRL_PORTIN   PIND
#define SIMPRES_CTRL_PIN      5

#define MDM_PWR_KEY_PORT      PORTD
#define MDM_PWR_KEY_DDR       DDRD
#define MDM_PWR_KEY_PORTIN    PIND
#define MDM_PWR_KEY_PIN       6

//====================PORTH=======================
#define MDM_CTS_PORT         PORTH
#define MDM_CTS_DDR          DDRH
#define MDM_CTS_PORTIN       PINH
#define MDM_CTS_PIN          2

#define MDM_RTS_PORT         PORTH
#define MDM_RTS_DDR          DDRH
#define MDM_RTS_PORTIN       PINH
#define MDM_RTS_PIN          3

#define MDM_DTR_PORT          PORTH
#define MDM_DTR_DDR           DDRH
#define MDM_DTR_PORTIN        PINH
#define MDM_DTR_PIN           4

#define MDM_RI_PORT           PORTH
#define MDM_RI_DDR            DDRH
#define MDM_RI_PORTIN         PINH
#define MDM_RI_PIN            5

#define MDM_DCD_PORT          PORTH
#define MDM_DCD_DDR           DDRH
#define MDM_DCD_PORTIN        PINH
#define MDM_DCD_PIN           6

#define MDM_VCC_CTRL_PORT     PORTH
#define MDM_VCC_CTRL_DDR      DDRH
#define MDM_VCC_CTRL_PORTIN   PINH
#define MDM_VCC_CTRL_PIN      7

//====================PORTJ=======================
#define RS485_EN_PORT         PORTJ
#define RS485_EN_DDR          DDRJ
#define RS485_EN_PORTIN       PINJ
#define RS485_EN_PIN          2

#define BUZZER_PORT           PORTJ
#define BUZZER_DDR            DDRJ
#define BUZZER_PORTIN         PINJ
#define BUZZER_PIN            4

#define POWER_CTRL_PORT       PORTJ
#define POWER_CTRL_DDR        DDRJ
#define POWER_CTRL_PORTIN     PINJ
#define POWER_CTRL_PIN        5

#define CTRL_OUT_0_PORT       PORTJ
#define CTRL_OUT_0_DDR        DDRJ
#define CTRL_OUT_0_PORTIN     PINJ
#define CTRL_OUT_0_PIN        6

#define CTRL_OUT_1_PORT       PORTJ
#define CTRL_OUT_1_DDR        DDRJ
#define CTRL_OUT_1_PORTIN     PINJ
#define CTRL_OUT_1_PIN        7

//====================PORTL=======================
#define LED_0_PORT            PORTL
#define LED_0_DDR             DDRL
#define LED_0_PORTIN          PINL
#define LED_0_PIN             0

#define LED_1_PORT            PORTL
#define LED_1_DDR             DDRL
#define LED_1_PORTIN          PINL
#define LED_1_PIN             1

#define LED_2_PORT            PORTL
#define LED_2_DDR             DDRL
#define LED_2_PORTIN          PINL
#define LED_2_PIN             2

#define LED_3_PORT            PORTL
#define LED_3_DDR             DDRL
#define LED_3_PORTIN          PINL
#define LED_3_PIN             3

#define POWERFAIL_PORT        PORFL
#define POWERFAIL_DDR         DDRF
#define POWERFAIL_PORTIN      PINF
#define POWERFAIL_PIN         0




//====================MACROS=======================
#define MDM_PWR_FET_ON()      cbi(MDM_VCC_CTRL_PORT,MDM_VCC_CTRL_PIN)
#define MDM_PWR_FET_OFF()     sbi(MDM_VCC_CTRL_PORT,MDM_VCC_CTRL_PIN)

#define MDM_PWR_KEY_ON()      sbi(MDM_PWR_KEY_PORT,MDM_PWR_KEY_PIN)
#define MDM_PWR_KEY_OFF()     cbi(MDM_PWR_KEY_PORT,MDM_PWR_KEY_PIN)



// =============== INPUTS ======================
#define GET_DSR_STATUS()      bit_is_clear(MDM_DSR_PORTIN,MDM_DSR_PIN)  // voltage logic 0 is asserted status - OK
#define GET_CTS_STATUS()      bit_is_clear(MDM_CTS_PORTIN,MDM_CTS_PIN)

#define GET_OPTO_0()          bit_is_clear(OPTO_0_PORTIN,OPTO_0_PIN)
#define GET_OPTO_1()          bit_is_clear(OPTO_1_PORTIN,OPTO_1_PIN)
#define GET_MDM_LED()         bit_is_clear(MDM_LED_PORTIN,MDM_LED_PIN)
#define GET_MDM_PWR_RDY()     bit_is_clear(MDM_PWR_RDY_PORTIN,MDM_PWR_RDY_PIN)

// =============== OUTPUTS ======================
#define SET_DTR()             cbi(MDM_DTR_PORT,MDM_DTR_PIN)
#define CLR_DTR()             sbi(MDM_DTR_PORT,MDM_DTR_PIN)

#define SET_RTS()             cbi(MDM_RTS_PORT,MDM_RTS_PIN)
#define CLR_RTS()             sbi(MDM_RTS_PORT,MDM_RTS_PIN)

#define LED_0_ON()            cbi(LED_0_PORT,LED_0_PIN)
#define LED_0_OFF()           sbi(LED_0_PORT,LED_0_PIN)
#define LED_0_TOGGLE()        BIT_TOGGLE(LED_0_PORT,LED_0_PIN)

#define LED_1_ON()            cbi(LED_1_PORT,LED_1_PIN)
#define LED_1_OFF()           sbi(LED_1_PORT,LED_1_PIN)
#define LED_1_TOGGLE()        BIT_TOGGLE(LED_1_PORT,LED_1_PIN)

#define LED_2_ON()            cbi(LED_2_PORT,LED_2_PIN)
#define LED_2_OFF()           sbi(LED_2_PORT,LED_2_PIN)
#define LED_2_TOGGLE()        BIT_TOGGLE(LED_2_PORT,LED_2_PIN)

#define LED_3_ON()            cbi(LED_3_PORT,LED_3_PIN)
#define LED_3_OFF()           sbi(LED_3_PORT,LED_3_PIN)
#define LED_3_TOGGLE()        BIT_TOGGLE(LED_3_PORT,LED_3_PIN)

#define OUTPUT_0_ON()         sbi(CTRL_OUT_0_PORT,CTRL_OUT_0_PIN)
#define OUTPUT_0_OFF()        cbi(CTRL_OUT_0_PORT,CTRL_OUT_0_PIN)
#define OUTPUT_0_TOGGLE()     BIT_TOGGLE(CTRL_OUT_0_PORT,CTRL_OUT_0_PIN)

#define OUTPUT_1_ON()         sbi(CTRL_OUT_1_PORT,CTRL_OUT_1_PIN)
#define OUTPUT_1_OFF()        cbi(CTRL_OUT_1_PORT,CTRL_OUT_1_PIN)
#define OUTPUT_1_TOGGLE()     BIT_TOGGLE(CTRL_OUT_1_PORT,CTRL_OUT_1_PIN)

#define BUZZER_ON()           sbi(BUZZER_PORT,BUZZER_PIN)
#define BUZZER_OFF()          cbi(BUZZER_PORT,BUZZER_PIN)
#define BUZZER_TOGGLE()       BIT_TOGGLE(BUZZER_PORT,BUZZER_PIN)



#define SWITCH_MDM_ON()       MDM_PWR_FET_ON();\
                              timerPause(200);\
                              MDM_PWR_KEY_ON();\
                              timerPause(2000);\
                              MDM_PWR_KEY_OFF();\
                              timerPause(2000);

#define SWITCH_MDM_OFF()      MDM_PWR_KEY_ON();\
                              timerPause(2000);\
                              MDM_PWR_KEY_OFF();\
                              timerPause(2000);\
                              MDM_PWR_FET_OFF();


#define PWR_SWITCH_ON()       sbi(POWER_CTRL_PORT,POWER_CTRL_PIN)
#define PWR_SWITCH_OFF()      cbi(POWER_CTRL_PORT,POWER_CTRL_PIN)

void InitIOPins(void);
#endif
