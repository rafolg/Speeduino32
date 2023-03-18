#ifndef TEENSY41_H
#define TEENSY41_H
#if defined(CORE_TEENSY)&& defined(__IMXRT1062__)

/*
***********************************************************************************************************
* General
*/
  void initBoard(void);
  uint16_t freeRam(void);
  void doSystemReset(void);
  void jumpToBootloader(void);
  time_t getTeensy3Time(void);
  #define PORT_TYPE uint32_t //Size of the port variables
  #define PINMASK_TYPE uint32_t
  #define COMPARE_TYPE uint32_t
  #define COUNTER_TYPE uint32_t
  #define SERIAL_BUFFER_SIZE 517 //Size of the serial buffer used by new comms protocol. For SD transfers this must be at least 512 + 1 (flag) + 4 (sector)
  #define FPU_MAX_SIZE 32 //Size of the FPU buffer. 0 means no FPU.
  #define BOARD_MAX_DIGITAL_PINS 54
  #define BOARD_MAX_IO_PINS 54 //digital pins + analog channels + 1
  #define EEPROM_LIB_H <EEPROM.h>
  typedef int eeprom_address_t;
  #define RTC_ENABLED
  #define SD_LOGGING //SD logging enabled by default for Teensy 4.1 as it has the slot built in
  #define RTC_LIB_H "TimeLib.h"
  #define SD_CONFIG  SdioConfig(FIFO_SDIO) //Set Teensy to use SDIO in FIFO mode. This is the fastest SD mode on Teensy as it offloads most of the writes

  #define micros_safe() micros() //timer5 method is not used on anything but AVR, the micros_safe() macro is simply an alias for the normal micros()
  #define PWM_FAN_AVAILABLE
  //#define pinIsReserved(pin)  ( ((pin) == 0) ) //Forbidden pins like USB
  #define pinIsReserved(pin)  ( ((pin) == 34) || ((pin) == 35) || ((pin) == 7) || ((pin) == 8) || ((pin) == 22) || ((pin) == 23) ) //Forbidden pins Serial1, Serial2, Can1
/*
***********************************************************************************************************
* Schedules
*/
  /*
  https://github.com/luni64/TeensyTimerTool/wiki/Supported-Timers#pit---periodic-timer
  https://github.com/luni64/TeensyTimerTool/wiki/Configuration#clock-setting-for-the-gpt-and-pit-timers
  The Quad timer (TMR) provides 4 timers each with 4 usable compare channels. The down compare and alternating compares are not usable
  FUEL 1-4: TMR1
  IGN 1-4 : TMR2
  FUEL 5-8: TMR3
  IGN 5-8 : TMR4
  */
  //#define    _COUNTER TMR1_CNTR0 //In use for pwmWMI
  //#define    _COUNTER TMR1_CNTR1 //In use for pwmFAN
  //#define    _COUNTER TMR1_CNTR2
  //#define    _COUNTER TMR1_CNTR3
  #define KNK1_COUNTER TMR3_CNTR0
  #define KNK2_COUNTER TMR3_CNTR1
  #define KNK3_COUNTER TMR3_CNTR2
  #define KNK4_COUNTER TMR3_CNTR3

  #define IGN1_COUNTER  TMR2_CNTR0
  #define IGN2_COUNTER  TMR2_CNTR1
  #define IGN3_COUNTER  TMR2_CNTR2
  #define IGN4_COUNTER  TMR2_CNTR3
  
  #define FUEL1_COUNTER  TMR4_CNTR0
  #define FUEL2_COUNTER  TMR4_CNTR1
  #define FUEL3_COUNTER  TMR4_CNTR2
  #define FUEL4_COUNTER  TMR4_CNTR3

  //#define    _COMPARE TMR1_COMP10 //In use for pwmWMI
  //#define    _COMPARE TMR1_COMP11 //In use for pwmFAN
  //#define    _COMPARE TMR1_COMP12
  //#define    _COMPARE TMR1_COMP13
  
  #define KNK1_COMPARE  TMR3_COMP10
  #define KNK2_COMPARE  TMR3_COMP11
  #define KNK3_COMPARE  TMR3_COMP12
  #define KNK4_COMPARE  TMR3_COMP13

  #define IGN1_COMPARE  TMR2_COMP10
  #define IGN2_COMPARE  TMR2_COMP11
  #define IGN3_COMPARE  TMR2_COMP12
  #define IGN4_COMPARE  TMR2_COMP13
 
  #define FUEL1_COMPARE  TMR4_COMP10
  #define FUEL2_COMPARE  TMR4_COMP11
  #define FUEL3_COMPARE  TMR4_COMP12
  #define FUEL4_COMPARE  TMR4_COMP13

  //#define    _TIMER_ENABLE() TMR1_CSCTRL0 |= TMR_CSCTRL_TCF1EN //Write 1 to the TCFIEN (Channel Interrupt Enable) bit of channel 0 Status/Control
  //#define    _TIMER_ENABLE() TMR1_CSCTRL1 |= TMR_CSCTRL_TCF1EN
  //#define    _TIMER_ENABLE() TMR1_CSCTRL2 |= TMR_CSCTRL_TCF1EN
  //#define    _TIMER_ENABLE() TMR1_CSCTRL3 |= TMR_CSCTRL_TCF1EN
  #define KNK1_TIMER_ENABLE() TMR3_CSCTRL0 |= TMR_CSCTRL_TCF1EN
  #define KNK2_TIMER_ENABLE() TMR3_CSCTRL1 |= TMR_CSCTRL_TCF1EN
  #define KNK3_TIMER_ENABLE() TMR3_CSCTRL2 |= TMR_CSCTRL_TCF1EN
  #define KNK4_TIMER_ENABLE() TMR3_CSCTRL3 |= TMR_CSCTRL_TCF1EN

  //#define    _TIMER_DISABLE() TMR1_CSCTRL0 &= ~TMR_CSCTRL_TCF1EN //Write 0 to the TCFIEN (Channel Interrupt Enable) bit of channel 0 Status/Control
  //#define    _TIMER_DISABLE() TMR1_CSCTRL1 &= ~TMR_CSCTRL_TCF1EN
  //#define    _TIMER_DISABLE() TMR1_CSCTRL2 &= ~TMR_CSCTRL_TCF1EN
  //#define    _TIMER_DISABLE() TMR1_CSCTRL3 &= ~TMR_CSCTRL_TCF1EN
  #define KNK1_TIMER_DISABLE() TMR3_CSCTRL0 &= ~TMR_CSCTRL_TCF1EN
  #define KNK2_TIMER_DISABLE() TMR3_CSCTRL1 &= ~TMR_CSCTRL_TCF1EN
  #define KNK3_TIMER_DISABLE() TMR3_CSCTRL2 &= ~TMR_CSCTRL_TCF1EN
  #define KNK4_TIMER_DISABLE() TMR3_CSCTRL3 &= ~TMR_CSCTRL_TCF1EN

  #define IGN1_TIMER_ENABLE() TMR2_CSCTRL0 |= TMR_CSCTRL_TCF1EN
  #define IGN2_TIMER_ENABLE() TMR2_CSCTRL1 |= TMR_CSCTRL_TCF1EN
  #define IGN3_TIMER_ENABLE() TMR2_CSCTRL2 |= TMR_CSCTRL_TCF1EN
  #define IGN4_TIMER_ENABLE() TMR2_CSCTRL3 |= TMR_CSCTRL_TCF1EN
  
  #define FUEL1_TIMER_ENABLE() TMR4_CSCTRL0 |= TMR_CSCTRL_TCF1EN
  #define FUEL2_TIMER_ENABLE() TMR4_CSCTRL1 |= TMR_CSCTRL_TCF1EN
  #define FUEL3_TIMER_ENABLE() TMR4_CSCTRL2 |= TMR_CSCTRL_TCF1EN
  #define FUEL4_TIMER_ENABLE() TMR4_CSCTRL3 |= TMR_CSCTRL_TCF1EN

  #define IGN1_TIMER_DISABLE() TMR2_CSCTRL0 &= ~TMR_CSCTRL_TCF1EN
  #define IGN2_TIMER_DISABLE() TMR2_CSCTRL1 &= ~TMR_CSCTRL_TCF1EN
  #define IGN3_TIMER_DISABLE() TMR2_CSCTRL2 &= ~TMR_CSCTRL_TCF1EN
  #define IGN4_TIMER_DISABLE() TMR2_CSCTRL3 &= ~TMR_CSCTRL_TCF1EN
  
  #define FUEL1_TIMER_DISABLE() TMR4_CSCTRL0 &= ~TMR_CSCTRL_TCF1EN
  #define FUEL2_TIMER_DISABLE() TMR4_CSCTRL1 &= ~TMR_CSCTRL_TCF1EN
  #define FUEL3_TIMER_DISABLE() TMR4_CSCTRL2 &= ~TMR_CSCTRL_TCF1EN
  #define FUEL4_TIMER_DISABLE() TMR4_CSCTRL3 &= ~TMR_CSCTRL_TCF1EN

  //Clock is 150Mhz
  #define MAX_TIMER_PERIOD 55923 // 0.85333333uS * 65535
  #define uS_TO_TIMER_COMPARE(uS) ((uS * 75) >> 6) //Converts a given number of uS into the required number of timer ticks until that time has passed. 
  
  /*
  To calculate the above uS_TO_TIMER_COMPARE
  Choose number of bit of precision. Eg: 6
  Divide 2^6 by the time per tick (0.853333) = 75
  Multiply and bitshift back by the precision: (uS * 75) >> 6
  */

/*
***********************************************************************************************************
* Auxiliaries
*/
  //Put it back after playing with FlexTimers
  /*
  #define ENABLE_BOOST_TIMER()  TMR1_CSCTRL2 |= TMR_CSCTRL_TCF1EN
  #define DISABLE_BOOST_TIMER() TMR1_CSCTRL2 &= ~TMR_CSCTRL_TCF1EN
  */
  
  //Put it back after playing with FlexTimers
  /*
  #define ENABLE_VVT_TIMER()    TMR1_CSCTRL3 |= TMR_CSCTRL_TCF1EN
  #define DISABLE_VVT_TIMER()   TMR1_CSCTRL3 &= ~TMR_CSCTRL_TCF1EN
  */

  // This implementation works!
  #define ENABLE_VVT_TIMER() FLEXPWM1_SM0INTEN |= FLEXPWM_SMINTEN_CMPIE(1<<0)
  #define DISABLE_VVT_TIMER() FLEXPWM1_SM0INTEN &= ~FLEXPWM_SMINTEN_CMPIE(1<<0)
  #define COMPARE_VVT_UPDATE_ENA() FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(1<<0)
  #define COMPARE_VVT_DISABLE_DIS() FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(1<<0)

  #define ENABLE_BOOST_TIMER()  FLEXPWM1_SM1INTEN |= FLEXPWM_SMINTEN_CMPIE(1<<1)
  #define DISABLE_BOOST_TIMER() FLEXPWM1_SM1INTEN &= ~FLEXPWM_SMINTEN_CMPIE(1<<1)
  #define COMPARE_BOOST_UPDATE_ENA() FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(1<<1)
  #define COMPARE_BOOST_DISABLE_DIS() FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(1<<1)

  #define ENABLE_WMI_TIMER()    FLEXPWM1_SM2INTEN |= FLEXPWM_SMINTEN_CMPIE(1<<2)
  #define DISABLE_WMI_TIMER()   FLEXPWM1_SM2INTEN &= ~FLEXPWM_SMINTEN_CMPIE(1<<2)
  #define COMPARE_WMI_UPDATE_ENA() FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(1<<2)
  #define COMPARE_WMI_DISABLE_DIS() FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(1<<2)

  #define ENABLE_FAN_TIMER()    FLEXPWM1_SM3INTEN |= FLEXPWM_SMINTEN_CMPIE(1<<3)
  #define DISABLE_FAN_TIMER()   FLEXPWM1_SM3INTEN &= ~FLEXPWM_SMINTEN_CMPIE(1<<3)
  #define COMPARE_FAN_UPDATE_ENA() FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_CLDOK(1<<3)
  #define COMPARE_FAN_DISABLE_DIS() FLEXPWM1_MCTRL |= FLEXPWM_MCTRL_LDOK(1<<3)

  /*
  * CAN'T HAVE THIS == The PITs dont play nicely among oneMStimer, idle, vvt and boost
  #define ENABLE_BOOST_TIMER()  PIT_TCTRL1 |= PIT_TCTRL_TEN
  #define DISABLE_BOOST_TIMER() PIT_TCTRL1 &= ~PIT_TCTRL_TEN
  #define ENABLE_VVT_TIMER()    PIT_TCTRL2 |= PIT_TCTRL_TEN
  #define DISABLE_VVT_TIMER()   PIT_TCTRL2 &= ~PIT_TCTRL_TEN

  #define BOOST_TIMER_COUNTER   0
  #define BOOST_TIMER_COMPARE   PIT_LDVAL1
  #define VVT_TIMER_COUNTER   0
  #define VVT_TIMER_COMPARE   PIT_LDVAL2
  */

  /* TMR1 implementation
  #define ENABLE_WMI_TIMER()    TMR1_CSCTRL0 |= TMR_CSCTRL_TCF1EN
  #define DISABLE_WMI_TIMER()   TMR1_CSCTRL0 &= ~TMR_CSCTRL_TCF1EN

  #define ENABLE_FAN_TIMER()    TMR1_CSCTRL1 |= TMR_CSCTRL_TCF1EN
  #define DISABLE_FAN_TIMER()   TMR1_CSCTRL1 &= ~TMR_CSCTRL_TCF1EN
  */
  
  

  #define VVT_TIMER_COUNTER   FLEXPWM1_SM0CNT
  #define VVT_TIMER_COMPARE   FLEXPWM1_SM0VAL1
  
  #define BOOST_TIMER_COUNTER   FLEXPWM1_SM1CNT
  #define BOOST_TIMER_COMPARE   FLEXPWM1_SM1VAL1

  #define WMI_TIMER_COUNTER     FLEXPWM1_SM2CNT
  #define WMI_TIMER_COMPARE     FLEXPWM1_SM2VAL1

  #define FAN_TIMER_COUNTER     FLEXPWM1_SM3CNT
  #define FAN_TIMER_COMPARE     FLEXPWM1_SM3VAL1

  //Put it back after playing with FlexTimers
  /*
  #define BOOST_TIMER_COMPARE   TMR1_COMP12
  #define BOOST_TIMER_COUNTER   TMR1_CNTR2
  */
  
  //Put it back after playing with FlexTimers
  /*
  #define VVT_TIMER_COMPARE     TMR1_COMP13
  #define VVT_TIMER_COUNTER     TMR1_CNTR3
  */

  /*
  #define WMI_TIMER_COMPARE     TMR1_COMP10
  #define WMI_TIMER_COUNTER     TMR1_CNTR0
  #define FAN_TIMER_COMPARE     TMR1_COMP11
  #define FAN_TIMER_COUNTER     TMR1_CNTR1
  */
/*
***********************************************************************************************************
* Idle
*/
  /*
  #define IDLE_COUNTER 0
  #define IDLE_COMPARE PIT_LDVAL0
  */

  #define IDLE_COUNTER FLEXPWM2_SM0CNT
  #define IDLE_COMPARE FLEXPWM2_SM0VAL1
  
  /*
  #define IDLE_TIMER_ENABLE() PIT_TCTRL0 |= PIT_TCTRL_TEN
  #define IDLE_TIMER_DISABLE() PIT_TCTRL0 &= ~PIT_TCTRL_TEN
  */

  #define IDLE_TIMER_ENABLE() FLEXPWM2_SM0INTEN |= FLEXPWM_SMINTEN_CMPIE(1<<0)
  #define IDLE_TIMER_DISABLE() FLEXPWM2_SM0INTEN &= ~FLEXPWM_SMINTEN_CMPIE(1<<0)
  #define COMPARE_IDLE_UPDATE_ENA() FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_CLDOK(1<<0)
  #define COMPARE_IDLE_DISABLE_DIS() FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_LDOK(1<<0)
/*
***********************************************************************************************************
* CAN / Second serial
*/
  #define USE_SERIAL3
  #include <FlexCAN_T4.h>
  extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;
  //extern FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can1;
  //extern FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can2;
  static CAN_message_t outMsg;
  static CAN_message_t inMsg;
  #define NATIVE_CAN_AVAILABLE //Disable for now as it causes lockup
  extern HardwareSerial &BTserial;
#endif //CORE_TEENSY
#endif //TEENSY41_H
