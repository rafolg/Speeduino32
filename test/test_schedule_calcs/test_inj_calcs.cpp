#include <Arduino.h>
 #include <unity.h>
 #include "test_calcs_common.h"
 #include "schedule_calcs.h"
 #include "crankMaths.h"

 #define _countof(x) (sizeof(x) / sizeof (x[0]))

 // void printFreeRam()
 // {
 //     char msg[128];
 //     sprintf(msg, "freeRam: %u", freeRam());
 //     TEST_MESSAGE(msg);
 // }

 struct inj_test_parameters
 {
     uint16_t channelAngle;  // deg
     uint16_t pw;            // uS
     uint16_t crankAngle;    // deg
     uint32_t pending;       // Expected delay when channel status is PENDING
     uint32_t running;       // Expected delay when channel status is RUNNING
 };

 static void test_calc_inj1_timeout(uint16_t pw, uint16_t crankAngle, uint32_t pending, uint32_t running)
 {
     uint16_t PWdivTimerPerDegree = div(pw, timePerDegree).quot;

     memset(&fuelSchedule1, 0, sizeof(fuelSchedule1));

     fuelSchedule1.Status = PENDING;
     uint16_t startAngle = calculateInjectorStartAngle(PWdivTimerPerDegree, 0);
     TEST_ASSERT_EQUAL(pending, calculateInjector1Timeout(startAngle, crankAngle));

     fuelSchedule1.Status = RUNNING;
     startAngle = calculateInjectorStartAngle( PWdivTimerPerDegree, 0);
     TEST_ASSERT_EQUAL(running, calculateInjector1Timeout(startAngle, crankAngle));
 }


 static void test_calc_injN_timeout(const inj_test_parameters &parameters)
 {
     char msg[150];
     uint16_t PWdivTimerPerDegree = div(parameters.pw, timePerDegree).quot;

     memset(&fuelSchedule2, 0, sizeof(fuelSchedule2));

     fuelSchedule2.Status = PENDING;
     uint16_t startAngle = calculateInjectorStartAngle(PWdivTimerPerDegree, parameters.channelAngle);
     sprintf_P(msg, PSTR("PENDING channelAngle: % " PRIu16 ", pw: % " PRIu16 ", crankAngle: % " PRIu16 ", startAngle: % " PRIu16 ""), parameters.channelAngle, parameters.pw, parameters.crankAngle, startAngle);
     TEST_ASSERT_EQUAL_MESSAGE(parameters.pending, calculateInjectorNTimeout(fuelSchedule2, parameters.channelAngle, startAngle, parameters.crankAngle), msg);

     fuelSchedule2.Status = RUNNING;
     startAngle = calculateInjectorStartAngle( PWdivTimerPerDegree, parameters.channelAngle);
     sprintf_P(msg, PSTR("RUNNING channelAngle: % " PRIu16 ", pw: % " PRIu16 ", crankAngle: % " PRIu16 ", startAngle: % " PRIu16 ""), parameters.channelAngle, parameters.pw, parameters.crankAngle, startAngle);
     TEST_ASSERT_EQUAL_MESSAGE(parameters.running, calculateInjectorNTimeout(fuelSchedule2, parameters.channelAngle, startAngle, parameters.crankAngle), msg);
 }


 static void test_calc_inj_timeout(const inj_test_parameters *pStart, const inj_test_parameters *pEnd)
 {
     inj_test_parameters local;
     while (pStart!=pEnd)
     {
         memcpy_P(&local, pStart, sizeof(local));
         test_calc_injN_timeout(local);
         ++pStart;
     }
 }

 // Separate test for fuel 1 - different code path, same results!
 static void test_calc_inj1_timeout()
 {
     setEngineSpeed(4000, 360);
     currentStatus.injAngle = 355;

     static const int16_t test_data[][4] PROGMEM = {
         // ChannelAngle (deg), PW (uS), Crank (deg), Expected Pending, Expected Running
         { 3000, 0, 11562, 11562 },
         { 3000, 45, 9717, 9717 },
         { 3000, 90, 7872, 7872 },
         { 3000, 135, 6027, 6027 },
         { 3000, 180, 4182, 4182 },
         { 3000, 215, 2747, 2747 },
         { 3000, 270, 492, 492 },
         { 3000, 315, 0, 13407 },
         { 3000, 360, 0, 11562 },
         { 3000, 0, 11562, 11562 },
         { 3000, 45, 9717, 9717 },
         { 3000, 90, 7872, 7872 },
         { 3000, 135, 6027, 6027 },
         { 3000, 180, 4182, 4182 },
         { 3000, 215, 2747, 2747 },
         { 3000, 270, 492, 492 },
         { 3000, 315, 0, 13407 },
         { 3000, 360, 0, 11562 },
     };
     const int16_t (*pStart)[4] = &test_data[0];
     const int16_t (*pEnd)[4] = &test_data[0]+_countof(test_data);

     int16_t local[4];
     while (pStart!=pEnd)
     {
         memcpy_P(local, pStart, sizeof(local));
         test_calc_inj1_timeout(local[0], local[1], local[2], local[3]);
         ++pStart;
     }
 }

 static void test_calc_injN_timeout_360()
 {
     setEngineSpeed(4000, 360);
     currentStatus.injAngle = 355;

     static const inj_test_parameters test_data[] PROGMEM = {
         // ChannelAngle (deg), PW (uS), Crank (deg), Expected Pending (uS), Expected Running (uS)
         { 0, 3000, 0, 11562, 11562 },
         { 0, 3000, 45, 9717, 9717 },
         { 0, 3000, 90, 7872, 7872 },
         { 0, 3000, 135, 6027, 6027 },
         { 0, 3000, 180, 4182, 4182 },
         { 0, 3000, 215, 2747, 2747 },
         { 0, 3000, 270, 492, 492 },
         { 0, 3000, 315, 0, 13407 },
         { 0, 3000, 360, 0, 11562 },
         { 72, 3000, 0, 0, 14514 },
         { 72, 3000, 45, 0, 12669 },
         { 72, 3000, 90, 10824, 10824 },
         { 72, 3000, 135, 8979, 8979 },
         { 72, 3000, 180, 7134, 7134 },
         { 72, 3000, 215, 5699, 5699 },
         { 72, 3000, 270, 3444, 3444 },
         { 72, 3000, 315, 1599, 1599 },
         { 72, 3000, 360, 0, 14514 },
         { 80, 3000, 0, 82, 82 },
         { 80, 3000, 45, 0, 12997 },
         { 80, 3000, 90, 11152, 11152 },
         { 80, 3000, 135, 9307, 9307 },
         { 80, 3000, 180, 7462, 7462 },
         { 80, 3000, 215, 6027, 6027 },
         { 80, 3000, 270, 3772, 3772 },
         { 80, 3000, 315, 1927, 1927 },
         { 80, 3000, 360, 82, 82 },
         { 90, 3000, 0, 492, 492 },
         { 90, 3000, 45, 0, 13407 },
         { 90, 3000, 90, 11562, 11562 },
         { 90, 3000, 135, 9717, 9717 },
         { 90, 3000, 180, 7872, 7872 },
         { 90, 3000, 215, 6437, 6437 },
         { 90, 3000, 270, 4182, 4182 },
         { 90, 3000, 315, 2337, 2337 },
         { 90, 3000, 360, 492, 492 },
         { 144, 3000, 0, 2706, 2706 },
         { 144, 3000, 45, 861, 861 },
         { 144, 3000, 90, 0, 13776 },
         { 144, 3000, 135, 0, 11931 },
         { 144, 3000, 180, 10086, 10086 },
         { 144, 3000, 215, 8651, 8651 },
         { 144, 3000, 270, 6396, 6396 },
         { 144, 3000, 315, 4551, 4551 },
         { 144, 3000, 360, 2706, 2706 },
         { 180, 3000, 0, 4182, 4182 },
         { 180, 3000, 45, 2337, 2337 },
         { 180, 3000, 90, 492, 492 },
         { 180, 3000, 135, 0, 13407 },
         { 180, 3000, 180, 11562, 11562 },
         { 180, 3000, 215, 10127, 10127 },
         { 180, 3000, 270, 7872, 7872 },
         { 180, 3000, 315, 6027, 6027 },
         { 180, 3000, 360, 4182, 4182 },
         { 240, 3000, 0, 6642, 6642 },
         { 240, 3000, 45, 4797, 4797 },
         { 240, 3000, 90, 2952, 2952 },
         { 240, 3000, 135, 1107, 1107 },
         { 240, 3000, 180, 0, 14022 },
         { 240, 3000, 215, 0, 12587 },
         { 240, 3000, 270, 10332, 10332 },
         { 240, 3000, 315, 8487, 8487 },
         { 240, 3000, 360, 6642, 6642 },
         { 270, 3000, 0, 7872, 7872 },
         { 270, 3000, 45, 6027, 6027 },
         { 270, 3000, 90, 4182, 4182 },
         { 270, 3000, 135, 2337, 2337 },
         { 270, 3000, 180, 492, 492 },
         { 270, 3000, 215, 0, 13817 },
         { 270, 3000, 270, 11562, 11562 },
         { 270, 3000, 315, 9717, 9717 },
         { 270, 3000, 360, 7872, 7872 },
         { 360, 3000, 0, 11562, 11562 },
         { 360, 3000, 45, 9717, 9717 },
         { 360, 3000, 90, 7872, 7872 },
         { 360, 3000, 135, 6027, 6027 },
         { 360, 3000, 180, 4182, 4182 },
         { 360, 3000, 215, 2747, 2747 },
         { 360, 3000, 270, 492, 492 },
         { 360, 3000, 315, 0, 13407 },
         { 360, 3000, 360, 11562, 11562 },
     };

     test_calc_inj_timeout(&test_data[0], &test_data[0]+_countof(test_data));
 }

 static void test_calc_injN_timeout_720()
 {
     setEngineSpeed(4000, 720);
     currentStatus.injAngle = 355;

     static const inj_test_parameters test_data[] PROGMEM = {
         // ChannelAngle (deg), PW (uS), Crank (deg), Expected Pending (uS), Expected Running (uS)
         { 0, 3000, 90, 7872, 7872 },
         { 0, 3000, 135, 6027, 6027 },
         { 0, 3000, 180, 4182, 4182 },
         { 0, 3000, 215, 2747, 2747 },
         { 0, 3000, 270, 492, 492 },
         { 0, 3000, 315, 0, 28167 },
         { 0, 3000, 360, 0, 26322 },
         { 72, 3000, 0, 0, 14514 },
         { 72, 3000, 45, 0, 12669 },
         { 72, 3000, 90, 10824, 10824 },
         { 72, 3000, 135, 8979, 8979 },
         { 72, 3000, 180, 7134, 7134 },
         { 72, 3000, 215, 5699, 5699 },
         { 72, 3000, 270, 3444, 3444 },
         { 72, 3000, 315, 1599, 1599 },
         { 72, 3000, 360, 0, 29274 },
         { 80, 3000, 0, 0, 14842 },
         { 80, 3000, 45, 0, 12997 },
         { 80, 3000, 90, 11152, 11152 },
         { 80, 3000, 135, 9307, 9307 },
         { 80, 3000, 180, 7462, 7462 },
         { 80, 3000, 215, 6027, 6027 },
         { 80, 3000, 270, 3772, 3772 },
         { 80, 3000, 315, 1927, 1927 },
         { 80, 3000, 360, 82, 82 },
         { 90, 3000, 0, 0, 15252 },
         { 90, 3000, 45, 0, 13407 },
         { 90, 3000, 90, 11562, 11562 },
         { 90, 3000, 135, 9717, 9717 },
         { 90, 3000, 180, 7872, 7872 },
         { 90, 3000, 215, 6437, 6437 },
         { 90, 3000, 270, 4182, 4182 },
         { 90, 3000, 315, 2337, 2337 },
         { 90, 3000, 360, 492, 492 },
         { 144, 3000, 0, 0, 17466 },
         { 144, 3000, 45, 0, 15621 },
         { 144, 3000, 90, 0, 13776 },
         { 144, 3000, 135, 0, 11931 },
         { 144, 3000, 180, 10086, 10086 },
         { 144, 3000, 215, 8651, 8651 },
         { 144, 3000, 270, 6396, 6396 },
         { 144, 3000, 315, 4551, 4551 },
         { 144, 3000, 360, 2706, 2706 },
         { 180, 3000, 0, 0, 18942 },
         { 180, 3000, 45, 0, 17097 },
         { 180, 3000, 90, 0, 15252 },
         { 180, 3000, 135, 0, 13407 },
         { 180, 3000, 180, 11562, 11562 },
         { 180, 3000, 215, 10127, 10127 },
         { 180, 3000, 270, 7872, 7872 },
         { 180, 3000, 315, 6027, 6027 },
         { 180, 3000, 360, 4182, 4182 },
         { 240, 3000, 0, 0, 21402 },
         { 240, 3000, 45, 0, 19557 },
         { 240, 3000, 90, 0, 17712 },
         { 240, 3000, 135, 0, 15867 },
         { 240, 3000, 180, 0, 14022 },
         { 240, 3000, 215, 0, 12587 },
         { 240, 3000, 270, 10332, 10332 },
         { 240, 3000, 315, 8487, 8487 },
         { 240, 3000, 360, 6642, 6642 },
         { 270, 3000, 0, 0, 22632 },
         { 270, 3000, 45, 0, 20787 },
         { 270, 3000, 90, 0, 18942 },
         { 270, 3000, 135, 0, 17097 },
         { 270, 3000, 180, 0, 15252 },
         { 270, 3000, 215, 0, 13817 },
         { 270, 3000, 270, 11562, 11562 },
         { 270, 3000, 315, 9717, 9717 },
         { 270, 3000, 360, 7872, 7872 },
         { 360, 3000, 0, 0, 26322 },
         { 360, 3000, 45, 0, 24477 },
         { 360, 3000, 90, 0, 22632 },
         { 360, 3000, 135, 0, 20787 },
         { 360, 3000, 180, 0, 18942 },
         { 360, 3000, 215, 0, 17507 },
         { 360, 3000, 270, 0, 15252 },
         { 360, 3000, 315, 0, 13407 },
         { 360, 3000, 360, 11562, 11562 },
         { 480, 3000, 0, 1722, 1722 },
         { 480, 3000, 45, 0, 29397 },
         { 480, 3000, 90, 0, 27552 },
         { 480, 3000, 135, 0, 25707 },
         { 480, 3000, 180, 0, 23862 },
         { 480, 3000, 215, 0, 22427 },
         { 480, 3000, 270, 0, 20172 },
         { 480, 3000, 315, 0, 18327 },
         { 480, 3000, 360, 0, 16482 },
         { 540, 3000, 0, 4182, 4182 },
         { 540, 3000, 45, 2337, 2337 },
         { 540, 3000, 90, 492, 492 },
         { 540, 3000, 135, 0, 28167 },
         { 540, 3000, 180, 0, 26322 },
         { 540, 3000, 215, 0, 24887 },
         { 540, 3000, 270, 0, 22632 },
         { 540, 3000, 315, 0, 20787 },
         { 540, 3000, 360, 0, 18942 },
         { 600, 3000, 0, 6642, 6642 },
         { 600, 3000, 45, 4797, 4797 },
         { 600, 3000, 90, 2952, 2952 },
         { 600, 3000, 135, 1107, 1107 },
         { 600, 3000, 180, 0, 28782 },
         { 600, 3000, 215, 0, 27347 },
         { 600, 3000, 270, 0, 25092 },
         { 600, 3000, 315, 0, 23247 },
         { 600, 3000, 360, 0, 21402 },
         { 630, 3000, 0, 7872, 7872 },
         { 630, 3000, 45, 6027, 6027 },
         { 630, 3000, 90, 4182, 4182 },
         { 630, 3000, 135, 2337, 2337 },
         { 630, 3000, 180, 492, 492 },
         { 630, 3000, 215, 0, 28577 },
         { 630, 3000, 270, 0, 26322 },
         { 630, 3000, 315, 0, 24477 },
         { 630, 3000, 360, 0, 22632 },
     };

     test_calc_inj_timeout(&test_data[0], &test_data[0]+_countof(test_data));
 }

 // 
 void test_calc_inj_timeout(void)
 {
     RUN_TEST(test_calc_inj1_timeout);
     RUN_TEST(test_calc_injN_timeout_360);
     RUN_TEST(test_calc_injN_timeout_720);
 }