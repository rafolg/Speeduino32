/*
Speeduino - Simple engine management for the Arduino Mega 2560 platform
Copyright (C) Josh Stewart
A full copy of the license may be found in the projects root directory
*/

/*
This is for handling the data broadcasted to various CAN dashes and instrument clusters.
*/
#if defined(NATIVE_CAN_AVAILABLE)
#include "globals.h"

void sendBMWCluster()
{
  DashMessage(CAN_BMW_DME1);
  Can0.write(outMsg);
  DashMessage(CAN_BMW_DME2);
  Can0.write(outMsg);
  DashMessage(CAN_BMW_DME4);
  Can0.write(outMsg);
}

void sendVAGCluster()
{
  DashMessage(CAN_VAG_RPM);
  Can0.write(outMsg);
  DashMessage(CAN_VAG_VSS);
  Can0.write(outMsg);
}

void sendHALTECHv210Hz()
{
  DashMessage(CAN_HTK_SPD); //10Hz
  Can0.write(outMsg);
  //DashMessage(CAN_HTK_GEA); //10Hz
  //Can0.write(outMsg);
  DashMessage(CAN_HTK_BAT); //30Hz
  Can0.write(outMsg);
}

void sendHALTECHv230Hz()
{
  DashMessage(CAN_HTK_RPM); //30Hz
  Can0.write(outMsg);
  //DashMessage(CAN_HTK_DCY); //30Hz
  //Can0.write(outMsg);
  //DashMessage(CAN_HTK_WBO); //30Hz
  //Can0.write(outMsg);
}

void sendHALTECHv24Hz()
{
  //DashMessage(CAN_HTK_FOP); //4Hz
  //Can0.write(outMsg);
  DashMessage(CAN_HTK_CLT); //4Hz
  Can0.write(outMsg);
}

// switch case for gathering all data to message based on CAN Id.
void DashMessage(uint16_t DashMessageID)
{
  switch (DashMessageID)
  {
    case CAN_BMW_DME1:
      uint32_t temp_RPM;
      temp_RPM = currentStatus.RPM * 64;  //RPM conversion is currentStatus.RPM * 6.4, but this does it without floats.
      temp_RPM = temp_RPM / 10;
      outMsg.id = DashMessageID;
      outMsg.len = 8;
      outMsg.buf[0] = 0x05;  //bitfield, Bit0 = 1 = terminal 15 on detected, Bit2 = 1 = the ASC message ASC1 was received within the last 500 ms and contains no plausibility errors
      outMsg.buf[1] = 0x0C;  //Indexed Engine Torque in % of C_TQ_STND TBD do torque calculation.
      outMsg.buf[2] = lowByte(uint16_t(temp_RPM));  //lsb RPM
      outMsg.buf[3] = highByte(uint16_t(temp_RPM)); //msb RPM
      outMsg.buf[4] = 0x0C;  //Indicated Engine Torque in % of C_TQ_STND TBD do torque calculation!! Use same as for byte 1
      outMsg.buf[5] = 0x15;  //Engine Torque Loss (due to engine friction, AC compressor and electrical power consumption)
      outMsg.buf[6] = 0x00;  //not used
      outMsg.buf[7] = 0x35;  //Theorethical Engine Torque in % of C_TQ_STND after charge intervention
    break;

    case CAN_BMW_DME2:
      uint8_t temp_TPS;
      uint8_t temp_BARO;
      uint16_t temp_CLT;
      temp_TPS = map(currentStatus.TPS, 0, 100, 0, 254);//TPS value conversion (from 0x00 to 0xFE)
      temp_CLT = (((currentStatus.coolant - CALIBRATION_TEMPERATURE_OFFSET) + 48)*4/3); //CLT conversion (actual value to add is 48.373, but close enough)
      if (temp_CLT > 255) { temp_CLT = 255; } //CLT conversion can yield to higher values than what fits to byte, so limit the maximum value to 255.
      temp_BARO = currentStatus.baro;

      outMsg.id = DashMessageID;
      outMsg.len = 7;
      outMsg.buf[0] = 0x11;  //Multiplexed Information
      outMsg.buf[1] = temp_CLT;
      outMsg.buf[2] = temp_BARO;
      outMsg.buf[3] = 0x08;  //bitfield, Bit0 = 0 = Clutch released, Bit 3 = 1 = engine running
      outMsg.buf[4] = 0x00;  //TPS_VIRT_CRU_CAN (Not used)
      outMsg.buf[5] = temp_TPS;
      outMsg.buf[6] = 0x00;  //bitfield, Bit0 = 0 = brake not actuated, Bit1 = 0 = brake switch system OK etc...
      outMsg.buf[7] = 0x00;  //not used, but set to zero just in case.
    break;

    case 0x545:       //fuel consumption and CEl light for BMW e46/e39/e38 instrument cluster
                      //fuel consumption calculation not implemented yet. But this still needs to be sent to get rid of the CEL and EML fault lights on the dash.
      outMsg.id = DashMessageID;
      outMsg.len = 5;
      outMsg.buf[0] = 0x00;  //Check engine light (binary 10), Cruise light (binary 1000), EML (binary 10000).
      outMsg.buf[1] = 0x00;  //LSB Fuel consumption
      outMsg.buf[2] = 0x00;  //MSB Fuel Consumption
      if (currentStatus.coolant > 159) { outMsg.buf[3] = 0x08; } //Turn on overheat light if coolant temp hits 120 degrees celsius.
      else { outMsg.buf[3] = 0x00; } //Overheat light off at normal engine temps.
      outMsg.buf[4] = 0x7E; //this is oil temp
    break;

    case 0x280:       //RPM for VW instrument cluster
      temp_RPM =  currentStatus.RPM * 4; //RPM conversion
      outMsg.id = DashMessageID;
      outMsg.len = 8;
      outMsg.buf[0] = 0x49;
      outMsg.buf[1] = 0x0E;
      outMsg.buf[2] = lowByte(uint16_t(temp_RPM));  //lsb RPM
      outMsg.buf[3] = highByte(uint16_t(temp_RPM)); //msb RPM
      outMsg.buf[4] = 0x0E;
      outMsg.buf[5] = 0x00;
      outMsg.buf[6] = 0x1B;
      outMsg.buf[7] = 0x0E;
    break;

    case CAN_HTK_RPM:       //Haltech v2.35 RPM/MAP/TPS
      uint16_t temp_MAP;
      uint16_t temp2_TPS;
      temp_MAP = currentStatus.MAP * 10;
      temp2_TPS = currentStatus.TPS * 10;
      outMsg.id = DashMessageID;
      outMsg.len = 6;
      outMsg.buf[0] = highByte(currentStatus.RPM);
      outMsg.buf[1] = lowByte(currentStatus.RPM);
      outMsg.buf[2] = highByte(temp_MAP);
      outMsg.buf[3] = lowByte(temp_MAP);
      outMsg.buf[4] = highByte(temp2_TPS);
      outMsg.buf[5] = lowByte(temp2_TPS);
      //outMsg.buf[6] = 0x00;
      //outMsg.buf[7] = 0x00;
    break;

    //case CAN_HTK_FOP:      //Haltech v2.35 fuel/oil pressures/APS/wastegate pressure
    //  uint16_t temp_fuelPressure;
    //  uint16_t temp_oilPressure;
    //  temp_fuelPressure = (currentStatus.fuelPressure + (1013/10)) * 10; //Haltech broadcasts this as kPa but this is psi
    //  temp_oilPressure = (currentStatus.oilPressure + (1013/10)) * 10;   //Haltech broadcasts this as kPa but this is psi
    //  outMsg.id = DashMessageID;
    //  outMsg.len = 4;
    //  outMsg.buf[0] = highByte(temp_fuelPressure);
    //  outMsg.buf[1] = lowByte(temp_fuelPressure);
    //  outMsg.buf[2] = highByte(temp_oilPressure);
    //  outMsg.buf[3] = lowByte(temp_oilPressure);
      //outMsg.buf[4] = 0x00;  //Engine demand % y = x/10
      //outMsg.buf[5] = 0x00;  //Engine demand
      //outMsg.buf[6] = 0x00;  //Wastegate pressure kPa y = x/10 - 101.3
      //outMsg.buf[7] = 0x00;  //Wastegate pressure
    //break;
    
    /*
    case CAN_HTK_DCY:
      uint16_t temp_DC;
      uint16_t temp_ADV;
      temp_DC = (currentStatus.PW1/((revolutionTime * 2) / currentStatus.nSquirts)) * 1000;
      temp_ADV = currentStatus.advance * 10;
      outMsg.id = DashMessageID;
      outMsg.len = 6;
      outMsg.buf[0] = highByte(temp_DC);
      outMsg.buf[1] = lowByte(temp_DC);
      outMsg.buf[2] = 0x00;                //Injector duty cycle2 % y = x/10  ===== 100.0*pulseWidth/pulseLimit
      outMsg.buf[3] = 0x00;                //Injector duty cycle2                                    pulseLimit = cycleTime / nSquirts
      outMsg.buf[4] = highByte(temp_ADV);          //Ignition angle deg y = x/10
      outMsg.buf[5] = lowByte(temp_ADV);           //Ignition angle deg y = x/10
    break;
    */

    //case CAN_HTK_WBO:
    //  configPage9.lambda1 = (uint16_t)(((double_t)currentStatus.O2 / (double_t)configPage2.stoich) * 1000);
    //  outMsg.id = DashMessageID;
    //  outMsg.len = 2;
    //  outMsg.buf[0] = highByte(configPage9.lambda1);
    //  outMsg.buf[1] = lowByte(configPage9.lambda1);
      //outMsg.buf[2] = 0x00;
      //outMsg.buf[3] = 0x00;
      //outMsg.buf[4] = 0x00;
      //outMsg.buf[5] = 0x00;
      //outMsg.buf[6] = 0x00;
      //outMsg.buf[7] = 0x00;
    //break;

    case CAN_HTK_SPD:
      uint16_t temp_vss;
      uint16_t temp_gear;
      temp_vss = currentStatus.vss * 10UL;
      temp_gear = (uint16_t)currentStatus.gear;
      outMsg.id = DashMessageID;
      outMsg.len = 4;
      outMsg.buf[0] = highByte(temp_vss);
      outMsg.buf[1] = lowByte(temp_vss);
      outMsg.buf[2] = highByte(temp_gear);
      outMsg.buf[3] = lowByte(temp_gear);
      //outMsg.buf[4] = 0x00;  //Intake cam angle 1 y = x/10
      //outMsg.buf[5] = 0x00;
      //outMsg.buf[5] = 0x00;  //Intake cam angle 2
      //outMsg.buf[5] = 0x00;
    break;

    case CAN_HTK_BAT:
      //uint16_t temp_baro;
      //temp_baro = currentStatus.baro * 10;
      outMsg.id = DashMessageID;
      outMsg.len = 2;
      outMsg.buf[0] = highByte(currentStatus.battery10);
      outMsg.buf[1] = lowByte(currentStatus.battery10);
      //outMsg.buf[2] = 0x00;  //Target boost level kPa y = x/10
      //outMsg.buf[3] = 0x00;
      //outMsg.buf[4] = highByte(temp_baro);
      //outMsg.buf[5] = lowByte(temp_baro);
    break;

    case CAN_HTK_CLT:
      uint16_t temp_coolant;
      uint16_t temp_iat;
      temp_coolant = (currentStatus.coolant + 273.3) * 10;
      temp_iat = (currentStatus.IAT + 273.3) * 10;
      outMsg.id = DashMessageID;
      outMsg.len = 4;
      outMsg.buf[0] = highByte(temp_coolant);
      outMsg.buf[1] = lowByte(temp_coolant);
      outMsg.buf[2] = highByte(temp_iat);
      outMsg.buf[3] = lowByte(temp_iat);
      //outMsg.buf[4] = 0x00;  //Fuel temperature K y = x/10
      //outMsg.buf[5] = 0x00;  //Fuel temperautre
      //outMsg.buf[6] = 0x00;  //Oil temperature K y = x/10
      //outMsg.buf[7] = 0x00;  //Oil temperature
    break;
    
    /*
    case CAN_HTK_GEA:
      int8_t temp_gear;
      int8_t temp_gearsel;
      temp_gear = 2;
      temp_gearsel = -4;
      //temp_gear = (int8_t)currentStatus.gear;
      outMsg.id = DashMessageID;
      outMsg.len = 8;
      outMsg.buf[0] = 0x00;  //Wideband overall lambda y = x/1000
      outMsg.buf[1] = 0x00;  //Wideband overall
      outMsg.buf[2] = 0x00;  //Wideband bank1 lambda y = x/1000
      outMsg.buf[3] = 0x00;  //Wideband bank1
      outMsg.buf[4] = 0x00;  //Wideband bank2 lambda y = x/1000
      outMsg.buf[5] = 0x00;  //Wideband bank2
      outMsg.buf[6] = temp_gearsel;  //Gear selector position y=0=Neutral, y=-1=Reverse, y=-2=Park, y=-3=Unknown, y=-4=Drive, y=-5=Sport, y=-6=Manual, y=-7=Low, y=-8=Overdrive
      outMsg.buf[7] = temp_gear; // y=-1=Reverse, y=0=Neutral, y=1=1st, y=2=2nd, y=3=3rd, y=4=4th, y=5=5th, y=6=6th
    break;
    */

    case 0x5A0:       //VSS for VW instrument cluster
      uint16_t temp_VSS;
      temp_VSS =  currentStatus.vss * 133; //VSS conversion
      outMsg.id = DashMessageID;
      outMsg.len = 8;
      outMsg.buf[0] = 0xFF;
      outMsg.buf[1] = lowByte(temp_VSS);
      outMsg.buf[2] = highByte(temp_VSS);
      outMsg.buf[3] = 0x00;
      outMsg.buf[4] = 0x00;
      outMsg.buf[5] = 0x00;
      outMsg.buf[6] = 0x00;
      outMsg.buf[7] = 0xAD;
    break;

    default:
    break;
  }
}
#endif
