#include "globals.h"
#include "scheduler.h"
#include "crankMaths.h"

inline uint16_t calculateInjectorStartAngle(uint16_t PWdivTimerPerDegree, int16_t injChannelDegrees)
{
  uint16_t tempInjectorStartAngle = (currentStatus.injAngle + injChannelDegrees);
  if(tempInjectorStartAngle < PWdivTimerPerDegree) { tempInjectorStartAngle += CRANK_ANGLE_MAX_INJ; }
  tempInjectorStartAngle -= PWdivTimerPerDegree;
  while(tempInjectorStartAngle > (uint16_t)CRANK_ANGLE_MAX_INJ) { tempInjectorStartAngle -= CRANK_ANGLE_MAX_INJ; }

  return tempInjectorStartAngle;
}

inline uint32_t calculateInjector1Timeout(const FuelSchedule &schedule, int injectorStartAngle, int crankAngle)
{
    int tempCrankAngle = crankAngle - CRANK_ANGLE_MAX_INJ + currentStatus.injAngle;
    if( tempCrankAngle < 0) { tempCrankAngle += CRANK_ANGLE_MAX_INJ; }
    int tempStartAngle = injectorStartAngle - CRANK_ANGLE_MAX_INJ + currentStatus.injAngle;
    if ( tempStartAngle < 0) { tempStartAngle += CRANK_ANGLE_MAX_INJ; }
    if ( (tempStartAngle <= tempCrankAngle) && (schedule.Status == RUNNING) ) { tempStartAngle += CRANK_ANGLE_MAX_INJ; }
    if ( tempStartAngle > tempCrankAngle )
    {
        return (((uint32_t)tempStartAngle - (uint32_t)tempCrankAngle) * (uint32_t)timePerDegree);
    }
    return 0U;
}

inline uint32_t calculateInjectorNTimeout(const FuelSchedule &schedule, int channelInjDegrees, int injectorStartAngle, int crankAngle)
{
    int tempCrankAngle = crankAngle - channelInjDegrees + currentStatus.injAngle;
    if( tempCrankAngle < 0) { tempCrankAngle += CRANK_ANGLE_MAX_INJ; }
    int tempStartAngle = injectorStartAngle - channelInjDegrees + currentStatus.injAngle;
    if ( tempStartAngle < 0) { tempStartAngle += CRANK_ANGLE_MAX_INJ; }
    if ( (tempStartAngle <= tempCrankAngle) && (schedule.Status == RUNNING) ) { tempStartAngle += CRANK_ANGLE_MAX_INJ; }
    if ( tempStartAngle > tempCrankAngle )
    {
        return (((uint32_t)tempStartAngle - (uint32_t)tempCrankAngle) * (uint32_t)timePerDegree);
    }
    return 0U;
}


inline void calculateIgnitionAngle1(int dwellAngle)
{
  ignition1EndAngle = CRANK_ANGLE_MAX_IGN - currentStatus.advance;
  if(ignition1EndAngle > CRANK_ANGLE_MAX_IGN) {ignition1EndAngle -= CRANK_ANGLE_MAX_IGN;}
  ignition1StartAngle = ignition1EndAngle - dwellAngle; // 360 - desired advance angle - number of degrees the dwell will take
  if(ignition1StartAngle < 0) {ignition1StartAngle += CRANK_ANGLE_MAX_IGN;}
}

inline void calculateIgnitionAngle2(int dwellAngle)
{
  ignition2EndAngle = channel2IgnDegrees - currentStatus.advance;
  if(ignition2EndAngle > CRANK_ANGLE_MAX_IGN) {ignition2EndAngle -= CRANK_ANGLE_MAX_IGN;}
  ignition2StartAngle = ignition2EndAngle - dwellAngle;
  if(ignition2StartAngle < 0) {ignition2StartAngle += CRANK_ANGLE_MAX_IGN;}
}

inline void calculateIgnitionAngle3(int dwellAngle)
{
  ignition3EndAngle = channel3IgnDegrees - currentStatus.advance;
  if(ignition3EndAngle > CRANK_ANGLE_MAX_IGN) {ignition3EndAngle -= CRANK_ANGLE_MAX_IGN;}
  ignition3StartAngle = ignition3EndAngle - dwellAngle;
  if(ignition3StartAngle < 0) {ignition3StartAngle += CRANK_ANGLE_MAX_IGN;}
}

// ignition 3 for rotary
inline void calculateIgnitionAngle3(int dwellAngle, int rotarySplitDegrees)
{
  ignition3EndAngle = ignition1EndAngle + rotarySplitDegrees;
  ignition3StartAngle = ignition3EndAngle - dwellAngle;
  if(ignition3StartAngle > CRANK_ANGLE_MAX_IGN) {ignition3StartAngle -= CRANK_ANGLE_MAX_IGN;}
  if(ignition3StartAngle < 0) {ignition3StartAngle += CRANK_ANGLE_MAX_IGN;}
}

inline void calculateIgnitionAngle4(int dwellAngle)
{
  ignition4EndAngle = channel4IgnDegrees - currentStatus.advance;
  if(ignition4EndAngle > CRANK_ANGLE_MAX_IGN) {ignition4EndAngle -= CRANK_ANGLE_MAX_IGN;}
  ignition4StartAngle = ignition4EndAngle - dwellAngle;
  if(ignition4StartAngle < 0) {ignition4StartAngle += CRANK_ANGLE_MAX_IGN;}
}

// ignition 4 for rotary
inline void calculateIgnitionAngle4(int dwellAngle, int rotarySplitDegrees)
{
  ignition4EndAngle = ignition2EndAngle + rotarySplitDegrees;
  ignition4StartAngle = ignition4EndAngle - dwellAngle;
  if(ignition4StartAngle > CRANK_ANGLE_MAX_IGN) {ignition4StartAngle -= CRANK_ANGLE_MAX_IGN;}
  if(ignition4StartAngle < 0) {ignition4StartAngle += CRANK_ANGLE_MAX_IGN;}
}

inline void calculateIgnitionAngle5(int dwellAngle)
{
  ignition5EndAngle = channel5IgnDegrees - currentStatus.advance;
  if(ignition5EndAngle > CRANK_ANGLE_MAX_IGN) {ignition5EndAngle -= CRANK_ANGLE_MAX_IGN;}
  ignition5StartAngle = ignition5EndAngle - dwellAngle;
  if(ignition5StartAngle < 0) {ignition5StartAngle += CRANK_ANGLE_MAX_IGN;}
}

inline void calculateIgnitionAngle6(int dwellAngle)
{
  ignition6EndAngle = channel6IgnDegrees - currentStatus.advance;
  if(ignition6EndAngle > CRANK_ANGLE_MAX_IGN) {ignition6EndAngle -= CRANK_ANGLE_MAX_IGN;}
  ignition6StartAngle = ignition6EndAngle - dwellAngle;
  if(ignition6StartAngle < 0) {ignition6StartAngle += CRANK_ANGLE_MAX_IGN;}
}

inline void calculateIgnitionAngle7(int dwellAngle)
{
  ignition7EndAngle = channel7IgnDegrees - currentStatus.advance;
  if(ignition7EndAngle > CRANK_ANGLE_MAX_IGN) {ignition7EndAngle -= CRANK_ANGLE_MAX_IGN;}
  ignition7StartAngle = ignition7EndAngle - dwellAngle;
  if(ignition7StartAngle < 0) {ignition7StartAngle += CRANK_ANGLE_MAX_IGN;}
}

inline void calculateIgnitionAngle8(int dwellAngle)
{
  ignition8EndAngle = channel8IgnDegrees - currentStatus.advance;
  if(ignition8EndAngle > CRANK_ANGLE_MAX_IGN) {ignition8EndAngle -= CRANK_ANGLE_MAX_IGN;}
  ignition8StartAngle = ignition8EndAngle - dwellAngle;
  if(ignition8StartAngle < 0) {ignition8StartAngle += CRANK_ANGLE_MAX_IGN;}
}

void calculateKnockAngle1(int knkStart, int knkWindow)
{
  if (configPage15.knockRef == 0)
  {
    knock1StartAngle = CRANK_ANGLE_MAX_IGN + knkStart;
    if(knock1StartAngle > CRANK_ANGLE_MAX_IGN) {knock1StartAngle -= CRANK_ANGLE_MAX_IGN;}
    knock1EndAngle = knock1StartAngle + knkWindow;
    if(knock1EndAngle > CRANK_ANGLE_MAX_IGN) {knock1EndAngle -= CRANK_ANGLE_MAX_IGN;}
  }
  else
  {
    knock1StartAngle = ignition1EndAngle + knkStart;
    if(knock1StartAngle > CRANK_ANGLE_MAX_IGN) {knock1StartAngle -= CRANK_ANGLE_MAX_IGN;}
    knock1EndAngle = knock1StartAngle + knkWindow;
    if(knock1EndAngle > CRANK_ANGLE_MAX_IGN) {knock1EndAngle -= CRANK_ANGLE_MAX_IGN;}
  }
  
}

void calculateKnockAngle2(int knkStart, int knkWindow)
{
  if (configPage15.knockRef == 0)
  {
    knock2StartAngle = channel2KnkDegrees + knkStart;
    if(knock2StartAngle > CRANK_ANGLE_MAX_IGN) {knock2StartAngle -= CRANK_ANGLE_MAX_IGN;}
    knock2EndAngle = knock2StartAngle + knkWindow;
    if(knock2EndAngle > CRANK_ANGLE_MAX_IGN) {knock2EndAngle -= CRANK_ANGLE_MAX_IGN;}
  }
  else
  {
    knock2StartAngle = ignition2EndAngle + knkStart;
    if(knock2StartAngle > CRANK_ANGLE_MAX_IGN) {knock2StartAngle -= CRANK_ANGLE_MAX_IGN;}
    knock2EndAngle = knock2StartAngle + knkWindow;
    if(knock2EndAngle > CRANK_ANGLE_MAX_IGN) {knock2EndAngle -= CRANK_ANGLE_MAX_IGN;}
  }
}

void calculateKnockAngle3(int knkStart, int knkWindow)
{
  if (configPage15.knockRef == 0)
  {
    knock3StartAngle = channel3KnkDegrees + knkStart;
    if(knock3StartAngle > CRANK_ANGLE_MAX_IGN) {knock3StartAngle -= CRANK_ANGLE_MAX_IGN;}
    knock3EndAngle = knock3StartAngle + knkWindow;
    if(knock3EndAngle > CRANK_ANGLE_MAX_IGN) {knock3EndAngle -= CRANK_ANGLE_MAX_IGN;}
  }
  else
  {
    knock3StartAngle = ignition3EndAngle + knkStart;
    if(knock3StartAngle > CRANK_ANGLE_MAX_IGN) {knock3StartAngle -= CRANK_ANGLE_MAX_IGN;}
    knock3EndAngle = knock3StartAngle + knkWindow;
    if(knock3EndAngle > CRANK_ANGLE_MAX_IGN) {knock3EndAngle -= CRANK_ANGLE_MAX_IGN;}
  }
}

void calculateKnockAngle4(int knkStart, int knkWindow)
{
  if (configPage15.knockRef == 0)
  {
    knock4StartAngle = channel4KnkDegrees + knkStart;
    if(knock4StartAngle > CRANK_ANGLE_MAX_IGN) {knock4StartAngle -= CRANK_ANGLE_MAX_IGN;}
    knock4EndAngle = knock4StartAngle + knkWindow;
    if(knock4EndAngle > CRANK_ANGLE_MAX_IGN) {knock4EndAngle -= CRANK_ANGLE_MAX_IGN;}
  }
  else
  {
    knock4StartAngle = ignition4EndAngle + knkStart;
    if(knock4StartAngle > CRANK_ANGLE_MAX_IGN) {knock4StartAngle -= CRANK_ANGLE_MAX_IGN;}
    knock4EndAngle = knock4StartAngle + knkWindow;
    if(knock4EndAngle > CRANK_ANGLE_MAX_IGN) {knock4EndAngle -= CRANK_ANGLE_MAX_IGN;}
  }
}

inline uint32_t calculateIgnition1Timeout(int crankAngle)
{
    if ( (ignition1StartAngle <= crankAngle) && (ignitionSchedule1.Status == RUNNING) ) { ignition1StartAngle += CRANK_ANGLE_MAX_IGN; }
    if ( ignition1StartAngle > crankAngle)
    {
        return angleToTime((ignition1StartAngle - crankAngle), CRANKMATH_METHOD_INTERVAL_REV);
    }
    return 0;
}

inline uint32_t calculateIgnitionNTimeout(const Schedule &schedule, int startAngle, int channelIgnDegrees, int crankAngle)
{
    int tempCrankAngle = crankAngle - channelIgnDegrees;
    if( tempCrankAngle < 0) { tempCrankAngle += CRANK_ANGLE_MAX_IGN; }
    int tempStartAngle = startAngle - channelIgnDegrees;
    if ( tempStartAngle < 0) { tempStartAngle += CRANK_ANGLE_MAX_IGN; }

    if ( (tempStartAngle <= tempCrankAngle) && (schedule.Status == RUNNING) ) { tempStartAngle += CRANK_ANGLE_MAX_IGN; }
    if(tempStartAngle > tempCrankAngle)
    { 
        return angleToTime((tempStartAngle - tempCrankAngle), CRANKMATH_METHOD_INTERVAL_REV); 
    }

    return 0U;

}

inline uint32_t calculateKnockNTimeout(const KnockSchedule &schedule, int startAngle, int channelKnkDegrees, int crankAngle, int knkStart)
{
    int tempCrankAngle = crankAngle - channelKnkDegrees + knkStart;
    if( tempCrankAngle < 0) { tempCrankAngle += CRANK_ANGLE_MAX_IGN; }
    int tempStartAngle = startAngle - channelKnkDegrees + knkStart;
    if ( tempStartAngle < 0) { tempStartAngle += CRANK_ANGLE_MAX_IGN; }

    if ((tempStartAngle < tempCrankAngle) && (schedule.Status != RUNNING) ) { return angleToTime((CRANK_ANGLE_MAX_IGN - tempCrankAngle + tempStartAngle), CRANKMATH_METHOD_INTERVAL_REV); }
    else if ( (tempStartAngle <= tempCrankAngle) && (schedule.Status == RUNNING) ) { tempStartAngle += CRANK_ANGLE_MAX_IGN; }
    if(tempStartAngle > tempCrankAngle)
    { 
        return angleToTime((tempStartAngle - tempCrankAngle), CRANKMATH_METHOD_INTERVAL_REV); 
    }

    return 0U;
}