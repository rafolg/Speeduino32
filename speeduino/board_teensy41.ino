#if defined(CORE_TEENSY) && defined(__IMXRT1062__)
#include "board_teensy41.h"
#include "globals.h"
#include "auxiliaries.h"
#include "idle.h"
#include "scheduler.h"
#include "timers.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;
//FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can1;
//FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can2;

//Bluetooth Serial for controlling TS wirelesslly
HardwareSerial &BTserial = Serial8;

void initBoard(void)
{
    /*
    ***********************************************************************************************************
    * General
    */

    /*
    Idle + Boost + VVT use the PIT timer. THIS IS ALSO USED BY THE INTERVAL TIMER THAT CALLS THE 1MS LOW RES TIMER!
    This has 4 channels that don't have compare registers, but will run for a period of time and then fire an interrupt
    The clock for these is set to 24Mhz and a prescale of 48 is used to give a 2uS tick time
    Set Prescaler
      * This is ideally too fast, but appears to be the slowest that the PIT can be set
      * 24Mhz = PER_clock
      * 48 * 1000000uS / PER_clock = 2uS
    */
    CCM_CSCMR1 |= CCM_CSCMR1_PERCLK_CLK_SEL; // 24MHz
    CCM_CSCMR1 |= CCM_CSCMR1_PERCLK_PODF(0b101111); //Prescale to 48

    /*
    LPUART1_BAUD |= LPUART_BAUD_OSR(0b01111);
    //LPUART1_CTRL = 0;
    LPUART1_CTRL &= ~LPUART_CTRL_TE;
    LPUART1_CTRL &= ~LPUART_CTRL_RE;
    LPUART1_BAUD |= LPUART_BAUD_SBR(0b1101);
    LPUART1_CTRL |= LPUART_CTRL_TE;
    LPUART1_CTRL |= LPUART_CTRL_RE;
    */
    
    /*
    ***********************************************************************************************************
    * Idle
    */
    
    if( (configPage6.iacAlgorithm == IAC_ALGORITHM_PWM_OL) || (configPage6.iacAlgorithm == IAC_ALGORITHM_PWM_CL) || (configPage6.iacAlgorithm == IAC_ALGORITHM_PWM_OLCL))
    {
      /*
      PIT_TCTRL0 = 0;
      PIT_TCTRL0 |= PIT_TCTRL_TIE; // enable Timer 1 interrupts
      PIT_TCTRL0 |= PIT_TCTRL_TEN; // start Timer 1
      PIT_LDVAL0 = 1; //1 * 2uS = 2uS
      */
      FLEXPWM2_SM0CTRL = 0;
      FLEXPWM2_SM0CTRL |= FLEXPWM_SMCTRL_FULL;
      FLEXPWM2_SM0CTRL |= FLEXPWM_SMCTRL_PRSC(0b111); //128 prescaler
    }
    

    /*
    ***********************************************************************************************************
    * Timers
    */
    //Uses the PIT timer channel 4 on Teensy 4.1.
    //lowResTimer.begin(oneMSInterval, 1000);
    
    PIT_TCTRL3 = 0;
    PIT_TCTRL3 |= PIT_TCTRL_TIE; // enable Timer 2 interrupts
    PIT_TCTRL3 |= PIT_TCTRL_TEN; // start Timer 2
    PIT_LDVAL3 = 500; //500 * 2uS = 1ms
    

    //TODO: Configure timers here
    /*
    GPT1_CR = 0;
    GPT1_IR = 0;
    GPT1_IR |= GPT_IR_OF1IE;         //Fire interrupt on compare value
    GPT1_OCR1 = 24000;               //When counter reaches compare value of 24000, it is 1 mS
    GPT1_CNT = 0;
    GPT1_CR |= GPT_CR_CLKSRC(0b101); //This should be 24Mhz
    GPT1_PR = 0;                     //Prescaler of 1
    GPT1_CR |= GPT_CR_DOZEEN;
    GPT1_CR |= GPT_CR_WAITEN;
    GPT1_CR |= GPT_CR_EN;            //Turn Timer On
    
    attachInterruptVector(IRQ_GPT1, GPT1_isr);
    NVIC_ENABLE_IRQ(IRQ_GPT1);
    */
    /*
    ***********************************************************************************************************
    * Auxiliaries
    */
    
    /*
    if (configPage6.boostEnabled == 1)
    {
      PIT_TCTRL1 = 0;
      PIT_TCTRL1 |= PIT_TCTRL_TIE; // enable Timer 2 interrupts
      PIT_TCTRL1 |= PIT_TCTRL_TEN; // start Timer 2
      PIT_LDVAL1 = 1; //1 * 2uS = 2uS
    }
    if (configPage6.vvtEnabled == 1)
    {
      PIT_TCTRL2 = 0;
      PIT_TCTRL2 |= PIT_TCTRL_TIE; // enable Timer 3 interrupts
      PIT_TCTRL2 |= PIT_TCTRL_TEN; // start Timer 3
      PIT_LDVAL2 = 1; //1 * 2uS = 2uS
    }
    */
    
    if (configPage6.boostEnabled == 1)
    {
      FLEXPWM1_SM0CTRL = 0;
      FLEXPWM1_SM0CTRL |= FLEXPWM_SMCTRL_FULL;
      FLEXPWM1_SM0CTRL |= FLEXPWM_SMCTRL_PRSC(0b111); //128 prescaler
    }
    if (configPage6.vvtEnabled == 1)
    {
      FLEXPWM1_SM1CTRL = 0;
      FLEXPWM1_SM1CTRL |= FLEXPWM_SMCTRL_FULL;
      FLEXPWM1_SM1CTRL |= FLEXPWM_SMCTRL_PRSC(0b111); //128 prescaler
    }

    FLEXPWM1_SM2CTRL = 0;
    FLEXPWM1_SM2CTRL |= FLEXPWM_SMCTRL_FULL;
    FLEXPWM1_SM2CTRL |= FLEXPWM_SMCTRL_PRSC(0b111); //128 prescaler

    FLEXPWM1_SM3CTRL = 0;
    FLEXPWM1_SM3CTRL |= FLEXPWM_SMCTRL_FULL;
    FLEXPWM1_SM3CTRL |= FLEXPWM_SMCTRL_PRSC(0b111); //128 prescaler

    attachInterruptVector(IRQ_FLEXPWM1_0, FLEXPWM1_0_isr);
    NVIC_ENABLE_IRQ(IRQ_FLEXPWM1_0);
    attachInterruptVector(IRQ_FLEXPWM1_1, FLEXPWM1_1_isr);
    NVIC_ENABLE_IRQ(IRQ_FLEXPWM1_1);
    attachInterruptVector(IRQ_FLEXPWM1_2, FLEXPWM1_2_isr);
    NVIC_ENABLE_IRQ(IRQ_FLEXPWM1_2);
    attachInterruptVector(IRQ_FLEXPWM1_3, FLEXPWM1_3_isr);
    NVIC_ENABLE_IRQ(IRQ_FLEXPWM1_3);

    attachInterruptVector(IRQ_FLEXPWM2_0, FLEXPWM2_0_isr);
    NVIC_ENABLE_IRQ(IRQ_FLEXPWM2_0);
    

    //2uS resolution Min 8Hz, Max 5KHz  if using Flex or TMR timers multiply by 8533 and divide by 10000
    boost_pwm_max_count = 1000000L / ((8533 * configPage6.boostFreq * 2) / 10000); //Converts the frequency in Hz to the number of ticks (at 2uS) it takes to complete 1 cycle. The x2 is there because the frequency is stored at half value (in a byte) to allow frequencies up to 511Hz
    vvt_pwm_max_count = 1000000L / ((8533 * configPage6.vvtFreq * 2) / 10000); //Converts the frequency in Hz to the number of ticks (at 2uS) it takes to complete 1 cycle
    fan_pwm_max_count = 1000000L / ((8533 * configPage6.fanFreq * 2) / 10000); //Converts the frequency in Hz to the number of ticks (at 2.1333uS) it takes to complete 1 cycle
    wmi_pwm_max_count = 1000000L / ((8533 * configPage10.wmiFreq * 2) / 10000);
    
    //TODO: Configure timers here
    attachInterruptVector(IRQ_PIT, PIT_isr);
    NVIC_ENABLE_IRQ(IRQ_PIT);
    /*
    ***********************************************************************************************************
    * Schedules
    */
    //Use the Quad timer, 60Mhz IP_bus_clk divided by 128 for a 2.133333uS period
    //TMR1 - Fuel 1-4
    //0
    //TMR1_CTRL0 = 0;
    //TMR1_SCTRL0 = 0;
    //TMR1_CSCTRL0 = 0;
    //TMR1_LOAD0 = 0; /* Reset load register */
    //TMR1_CTRL0 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    //TMR1_CTRL0 |= TMR_CTRL_CM(1); //Start the timer
    //1
    //TMR1_CTRL1 = 0;
    //TMR1_SCTRL1 = 0;
    //TMR1_CSCTRL1 = 0;
    //TMR1_LOAD1 = 0; /* Reset load register */
    //TMR1_CTRL1 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    //TMR1_CTRL1 |= TMR_CTRL_CM(1); //Start the timer
    //2
    //TMR1_CTRL2 = 0;
    //TMR1_SCTRL2 = 0;
    //TMR1_CSCTRL2 = 0;
    //TMR1_LOAD2 = 0; /* Reset load register */
    //TMR1_CTRL2 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    //TMR1_CTRL2 |= TMR_CTRL_CM(1); //Start the timer
    //3
    //TMR1_CTRL3 = 0;
    //TMR1_SCTRL3 = 0;
    //TMR1_CSCTRL3 = 0;
    //TMR1_LOAD3 = 0; /* Reset load register */
    //TMR1_CTRL3 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    //TMR1_CTRL3 |= TMR_CTRL_CM(1); //Start the timer
    //TMR2 - Ign 1-4
    //0
    TMR2_CTRL0 = 0;
    TMR2_SCTRL0 = 0;
    TMR2_CSCTRL0 = 0;
    TMR2_LOAD0 = 0; /* Reset load register */
    TMR2_CTRL0 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR2_CTRL0 |= TMR_CTRL_CM(1); //Start the timer
    //1
    TMR2_CTRL1 = 0;
    TMR2_SCTRL1 = 0;
    TMR2_CSCTRL1 = 0;
    TMR2_LOAD1 = 0; /* Reset load register */
    TMR2_CTRL1 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR2_CTRL1 |= TMR_CTRL_CM(1); //Start the timer
    //2
    TMR2_CTRL2 = 0;
    TMR2_SCTRL2 = 0;
    TMR2_CSCTRL2 = 0;
    TMR2_LOAD2 = 0; /* Reset load register */
    TMR2_CTRL2 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR2_CTRL2 |= TMR_CTRL_CM(1); //Start the timer
    //3
    TMR2_CTRL3 = 0;
    TMR2_SCTRL3 = 0;
    TMR2_CSCTRL3 = 0;
    TMR2_LOAD3 = 0; /* Reset load register */
    TMR2_CTRL3 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR2_CTRL3 |= TMR_CTRL_CM(1); //Start the timer
    //TMR3 - Fuel 5-8
    //0
    TMR3_CTRL0 = 0;
    TMR3_SCTRL0 = 0;
    TMR3_CSCTRL0 = 0;
    TMR3_LOAD0 = 0; /* Reset load register */
    TMR3_CTRL0 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR3_CTRL0 |= TMR_CTRL_CM(1); //Start the timer
    //1
    TMR3_CTRL1 = 0;
    TMR3_SCTRL1 = 0;
    TMR3_CSCTRL1 = 0;
    TMR3_LOAD1 = 0; /* Reset load register */
    TMR3_CTRL1 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR3_CTRL1 |= TMR_CTRL_CM(1); //Start the timer
    //2
    TMR3_CTRL2 = 0;
    TMR3_SCTRL2 = 0;
    TMR3_CSCTRL2 = 0;
    TMR3_LOAD2 = 0; /* Reset load register */
    TMR3_CTRL2 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR3_CTRL2 |= TMR_CTRL_CM(1); //Start the timer
    //3
    TMR3_CTRL3 = 0;
    TMR3_SCTRL3 = 0;
    TMR3_CSCTRL3 = 0;
    TMR3_LOAD3 = 0; /* Reset load register */
    TMR3_CTRL3 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR3_CTRL3 |= TMR_CTRL_CM(1); //Start the timer
    //TMR4 - IGN 5-8
    //0
    TMR4_CTRL0 = 0;
    TMR4_SCTRL0 = 0;
    TMR4_CSCTRL0 = 0;
    TMR4_LOAD0 = 0; /* Reset load register */
    TMR4_CTRL0 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR4_CTRL0 |= TMR_CTRL_CM(1); //Start the timer
    //1
    TMR4_CTRL1 = 0;
    TMR4_SCTRL1 = 0;
    TMR4_CSCTRL1 = 0;
    TMR4_LOAD1 = 0; /* Reset load register */
    TMR4_CTRL1 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR4_CTRL1 |= TMR_CTRL_CM(1); //Start the timer
    //2
    TMR4_CTRL2 = 0;
    TMR4_SCTRL2 = 0;
    TMR4_CSCTRL2 = 0;
    TMR4_LOAD2 = 0; /* Reset load register */
    TMR4_CTRL2 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR4_CTRL2 |= TMR_CTRL_CM(1); //Start the timer
    //3
    TMR4_CTRL3 = 0;
    TMR4_SCTRL3 = 0;
    TMR4_CSCTRL3 = 0;
    TMR4_LOAD3 = 0; /* Reset load register */
    TMR4_CTRL3 |= TMR_CTRL_PCS(0b1111); //Set the prescaler to 128
    TMR4_CTRL3 |= TMR_CTRL_CM(1); //Start the timer

    //attachInterruptVector(IRQ_QTIMER1, TMR1_isr);
    //NVIC_ENABLE_IRQ(IRQ_QTIMER1);
    attachInterruptVector(IRQ_QTIMER2, TMR2_isr);
    NVIC_ENABLE_IRQ(IRQ_QTIMER2);
    attachInterruptVector(IRQ_QTIMER3, TMR3_isr);
    NVIC_ENABLE_IRQ(IRQ_QTIMER3);
    attachInterruptVector(IRQ_QTIMER4, TMR4_isr);
    NVIC_ENABLE_IRQ(IRQ_QTIMER4);
}

/*
void GPT1_isr(void)
{
  //oneMSInterval();
  bool interrupt = (GPT1_SR & GPT_SR_OF1);

  if (interrupt) { GPT1_SR |= GPT_SR_OF1; oneMSInterval(); }
}
*/

void PIT_isr(void)
{
  //bool interrupt2 = (PIT_TFLG1 & PIT_TFLG_TIF);
  //bool interrupt3 = (PIT_TFLG2 & PIT_TFLG_TIF);
  bool interrupt1 = (PIT_TFLG3 & PIT_TFLG_TIF);
  //bool interrupt2 = (PIT_TFLG0 & PIT_TFLG_TIF);

  if(interrupt1) { PIT_TFLG3 = 1; oneMSInterval(); }
  //if(interrupt2) { PIT_TFLG0 = 1; idleInterrupt(); }
  //if(interrupt2) { PIT_TFLG1 = 1; boostInterrupt();}
  //if(interrupt3) { PIT_TFLG2 = 1; vvtInterrupt();  }
}

void FLEXPWM1_0_isr(void)
{
  bool interrupt1 = ( FLEXPWM1_SM0STS & FLEXPWM_SMSTS_CMPF(1<<0) );
  if(interrupt1) { FLEXPWM1_SM0STS |= FLEXPWM_SMSTS_CMPF(1<<0); vvtInterrupt(); }
}

void FLEXPWM1_1_isr(void)
{
  bool interrupt1 = ( FLEXPWM1_SM1STS & FLEXPWM_SMSTS_CMPF(1<<1) );
  if(interrupt1) { FLEXPWM1_SM1STS |= FLEXPWM_SMSTS_CMPF(1<<1); boostInterrupt(); }
}

void FLEXPWM1_2_isr(void)
{
  bool interrupt1 = ( FLEXPWM1_SM2STS & FLEXPWM_SMSTS_CMPF(1<<2) );
  if(interrupt1) { FLEXPWM1_SM2STS |= FLEXPWM_SMSTS_CMPF(1<<2); wmiInterrupt(); }
}

void FLEXPWM1_3_isr(void)
{
  bool interrupt1 = ( FLEXPWM1_SM3STS & FLEXPWM_SMSTS_CMPF(1<<3) );
  if(interrupt1) { FLEXPWM1_SM3STS |= FLEXPWM_SMSTS_CMPF(1<<3); fanInterrupt(); }
}

void FLEXPWM2_0_isr(void)
{
  bool interrupt1 = ( FLEXPWM2_SM0STS & FLEXPWM_SMSTS_CMPF(1<<0) );
  if(interrupt1) { FLEXPWM2_SM0STS |= FLEXPWM_SMSTS_CMPF(1<<0); idleInterrupt(); }
}

//void TMR1_isr(void)
//{
  //TMR1 is fuel channels 1-4
  //bool interrupt1 = (TMR1_CSCTRL0 & TMR_CSCTRL_TCF1);
  //bool interrupt2 = (TMR1_CSCTRL1 & TMR_CSCTRL_TCF1);
  //bool interrupt3 = (TMR1_CSCTRL2 & TMR_CSCTRL_TCF1);
  //bool interrupt4 = (TMR1_CSCTRL3 & TMR_CSCTRL_TCF1); //Put it back after playig with FlexTimers

  //if(interrupt1)      { TMR1_CSCTRL0 &= ~TMR_CSCTRL_TCF1; wmiInterrupt();  }
  //if(interrupt2) { TMR1_CSCTRL1 &= ~TMR_CSCTRL_TCF1; fanInterrupt();  }
  //else if(interrupt3) { TMR1_CSCTRL2 &= ~TMR_CSCTRL_TCF1; boostInterrupt();}
  //else if(interrupt4) { TMR1_CSCTRL3 &= ~TMR_CSCTRL_TCF1; vvtInterrupt();  } //Put it back after playing with FlexTimers
//}
void TMR2_isr(void)
{
  //TMR2 is IGN channels 1-4
  bool interrupt1 = (TMR2_CSCTRL0 & TMR_CSCTRL_TCF1);
  bool interrupt2 = (TMR2_CSCTRL1 & TMR_CSCTRL_TCF1);
  bool interrupt3 = (TMR2_CSCTRL2 & TMR_CSCTRL_TCF1);
  bool interrupt4 = (TMR2_CSCTRL3 & TMR_CSCTRL_TCF1);

  if(interrupt1)      { TMR2_CSCTRL0 &= ~TMR_CSCTRL_TCF1; ignitionSchedule1Interrupt(); }
  else if(interrupt2) { TMR2_CSCTRL1 &= ~TMR_CSCTRL_TCF1; ignitionSchedule2Interrupt(); }
  else if(interrupt3) { TMR2_CSCTRL2 &= ~TMR_CSCTRL_TCF1; ignitionSchedule3Interrupt(); }
  else if(interrupt4) { TMR2_CSCTRL3 &= ~TMR_CSCTRL_TCF1; ignitionSchedule4Interrupt(); }
}
void TMR3_isr(void)
{
  //TMR3 is Knock  channels 1-4
  bool interrupt1 = (TMR3_CSCTRL0 & TMR_CSCTRL_TCF1);
  bool interrupt2 = (TMR3_CSCTRL1 & TMR_CSCTRL_TCF1);
  bool interrupt3 = (TMR3_CSCTRL2 & TMR_CSCTRL_TCF1);
  bool interrupt4 = (TMR3_CSCTRL3 & TMR_CSCTRL_TCF1);

  if(interrupt1)      { TMR3_CSCTRL0 &= ~TMR_CSCTRL_TCF1; knockSchedule1Interrupt(); }
  else if(interrupt2) { TMR3_CSCTRL1 &= ~TMR_CSCTRL_TCF1; knockSchedule2Interrupt(); }
  else if(interrupt3) { TMR3_CSCTRL2 &= ~TMR_CSCTRL_TCF1; knockSchedule3Interrupt(); }
  else if(interrupt4) { TMR3_CSCTRL3 &= ~TMR_CSCTRL_TCF1; knockSchedule4Interrupt(); }
}

void TMR4_isr(void)
{
  //TMR4 is IGN channels 5-8
  bool interrupt1 = (TMR4_CSCTRL0 & TMR_CSCTRL_TCF1);
  bool interrupt2 = (TMR4_CSCTRL1 & TMR_CSCTRL_TCF1);
  bool interrupt3 = (TMR4_CSCTRL2 & TMR_CSCTRL_TCF1);
  bool interrupt4 = (TMR4_CSCTRL3 & TMR_CSCTRL_TCF1);

  if(interrupt1)      { TMR4_CSCTRL0 &= ~TMR_CSCTRL_TCF1; fuelSchedule1Interrupt(); }
  else if(interrupt2) { TMR4_CSCTRL1 &= ~TMR_CSCTRL_TCF1; fuelSchedule2Interrupt(); }
  else if(interrupt3) { TMR4_CSCTRL2 &= ~TMR_CSCTRL_TCF1; fuelSchedule3Interrupt(); }
  else if(interrupt4) { TMR4_CSCTRL3 &= ~TMR_CSCTRL_TCF1; fuelSchedule4Interrupt(); }
}

uint16_t freeRam(void)
{
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is the free, available ram.
    return (uint16_t)stackTop - heapTop;
}

//This function is used for attempting to set the RTC time during compile
time_t getTeensy3Time(void)
{
  return Teensy3Clock.get();
}

void doSystemReset(void) { return; }
void jumpToBootloader(void) { return; }

#endif
