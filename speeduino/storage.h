#ifndef STORAGE_H
#define STORAGE_H

/** @file storage.h
 * @brief Functions for reading and writing user settings to/from EEPROM
 *
 * Current layout of EEPROM is as follows (Version 18):
 *
 * |Offset (Dec)|Size (Bytes)| Description                          | Reference                          |
 * | ---------: | :--------: | :----------------------------------: | :--------------------------------- |
 * | 0          |1           | EEPROM version                       | @ref EEPROM_DATA_VERSION           |
 * | 1          |4           | X and Y sizes for fuel table         |                                    |
 * | 5          |576         | Fuel table (16x16)                   | @ref EEPROM_CONFIG1_MAP            |
 * | 581        |24          | Fuel table (X axis) (RPM)            |                                    |
 * | 605        |24          | Fuel table (Y axis) (MAP/TPS)        |                                    |
 * | 629        |128         | Page 2 settings                      | @ref EEPROM_CONFIG2_START          |
 * | 757        |2           | X and Y sizes for ignition table     |                                    |
 * | 759        |256         | Ignition table (16x16)               | @ref EEPROM_CONFIG3_MAP            |
 * | 1015       |16          | Ignition table (X axis) (RPM)        |                                    |
 * | 1031       |16          | Ignition table (Y axis) (MAP/TPS)    |                                    |
 * | 1047       |128         | Page 4 settings                      | @ref EEPROM_CONFIG4_START          |
 * | 1175       |2           | X and Y sizes for AFR target table   |                                    |
 * | 1177       |256         | AFR target table (16x16)             | @ref EEPROM_CONFIG5_MAP            |
 * | 1433       |16          | AFR target table (X axis) (RPM)      |                                    |
 * | 1449       |16          | AFR target table (Y axis) (MAP/TPS)  |                                    |
 * | 1465       |128         | Page 6 settings                      | @ref EEPROM_CONFIG6_START          |
 * | 1593       |2           | X and Y sizes for boost table        |                                    |
 * | 1595       |256         | Boost table (8x8)                    | @ref EEPROM_CONFIG7_MAP1           |
 * | 1851       |16          | Boost table (X axis) (RPM)           |                                    |
 * | 1867       |16          | Boost table (Y axis) (TPS)           |                                    |
 * | 1883       |2           | X and Y sizes for vvt table          |                                    |
 * | 1885       |256         | VVT table (8x8)                      | @ref EEPROM_CONFIG7_MAP2           |
 * | 2141       |16          | VVT table (X axis) (RPM)             |                                    |
 * | 2157       |16          | VVT table (Y axis) (MAP)             |                                    |
 * | 2173       |2           | X and Y sizes for staging table      |                                    |
 * | 2175       |64          | Staging table (8x8)                  | @ref EEPROM_CONFIG7_MAP3           |
 * | 2239       |8           | Staging table (X axis) (RPM)         |                                    |
 * | 2247       |8           | Staging table (Y axis) (MAP)         |                                    |
 * | 2255       |2           | X and Y sizes for trim1 table        |                                    |
 * | 2257       |36          | Trim1 table (6x6)                    | @ref EEPROM_CONFIG8_MAP1           |
 * | 2293       |6           | Trim1 table (X axis) (RPM)           |                                    |
 * | 2299       |6           | Trim1 table (Y axis) (MAP)           |                                    |
 * | 2305       |2           | X and Y sizes for trim2 table        |                                    |
 * | 2307       |36          | Trim2 table (6x6)                    | @ref EEPROM_CONFIG8_MAP2           |
 * | 2343       |6           | Trim2 table (X axis) (RPM)           |                                    |
 * | 2349       |6           | Trim2 table (Y axis) (MAP)           |                                    |
 * | 2355       |2           | X and Y sizes for trim3 table        |                                    |
 * | 2357       |36          | Trim3 table (6x6)                    | @ref EEPROM_CONFIG8_MAP3           |
 * | 2393       |6           | Trim3 table (X axis) (RPM)           |                                    |
 * | 2399       |6           | Trim3 table (Y axis) (MAP)           |                                    |
 * | 2405       |2           | X and Y sizes for trim4 table        |                                    |
 * | 2407       |36          | Trim4 table (6x6)                    | @ref EEPROM_CONFIG8_MAP4           |
 * | 2443       |6           | Trim4 table (X axis) (RPM)           |                                    |
 * | 2449       |6           | Trim4 table (Y axis) (MAP)           |                                    |
 * | 2455       |9           | HOLE ??                              |                                    |
 * | 2464       |192         | Page 9 settings                      | @ref EEPROM_CONFIG9_START          |
 * | 2656       |192         | Page 10 settings                     | @ref EEPROM_CONFIG10_START         |
 * | 2848       |2           | X and Y sizes for fuel2 table        |                                    |
 * | 2850       |256         | Fuel2 table (16x16)                  | @ref EEPROM_CONFIG11_MAP           |
 * | 3106       |16          | Fuel2 table (X axis) (RPM)           |                                    |
 * | 3122       |16          | Fuel2 table (Y axis) (MAP/TPS)       |                                    |
 * | 3138       |1           | HOLE ??                              |                                    |
 * | 3139       |2           | X and Y sizes for WMI table          |                                    |
 * | 3141       |256         | WMI table (8x8)                      | @ref EEPROM_CONFIG12_MAP           |
 * | 3397       |16          | WMI table (X axis) (RPM)             |                                    |
 * | 3413       |16          | WMI table (Y axis) (MAP)             |                                    |
 * | 3429       |2           | X and Y sizes VVT2 table             |                                    |
 * | 3431       |256         | VVT2 table (8x8)                     | @ref EEPROM_CONFIG12_MAP2          |
 * | 3687       |16          | VVT2 table (X axis) (RPM)            |                                    |
 * | 3703       |16          | VVT2 table (Y axis) (MAP)            |                                    |
 * | 3719       |2           | X and Y sizes dwell table            |                                    |
 * | 3721       |16          | Dwell table (4x4)                    | @ref EEPROM_CONFIG12_MAP3          |
 * | 3737       |4           | Dwell table (X axis) (RPM)           |                                    |
 * | 3741       |4           | Dwell table (Y axis) (MAP)           |                                    |
 * | 3745       |5           | HOLE ??                              |                                    |
 * | 3750       |128         | Page 13 settings                     | @ref EEPROM_CONFIG13_START         |
 * | 3878       |2           | X and Y sizes for ignition2 table    |                                    |
 * | 3880       |256         | Ignition2 table (16x16)              | @ref EEPROM_CONFIG14_MAP           |
 * | 4136       |16          | Ignition2 table (X axis) (RPM)       |                                    |
 * | 4152       |16          | Ignition2 table (Y axis) (MAP/TPS)   |                                    |
 * | 4168       |1           | HOLE ??                              |                                    |
 * | 4169       |2           | X and Y sizes for trim5 table        |                                    |
 * | 4171       |36          | Trim5 table (6x6)                    | @ref EEPROM_CONFIG8_MAP5           |
 * | 4207       |6           | Trim5 table (X axis) (RPM)           |                                    |
 * | 4213       |6           | Trim5 table (Y axis) (MAP)           |                                    |
 * | 4219       |2           | X and Y sizes for trim6 table        |                                    |
 * | 4221       |36          | Trim6 table (6x6)                    | @ref EEPROM_CONFIG8_MAP6           |
 * | 4257       |6           | Trim6 table (X axis) (RPM)           |                                    |
 * | 4263       |6           | Trim6 table (Y axis) (MAP)           |                                    |
 * | 4269       |2           | X and Y sizes for trim7 table        |                                    |
 * | 4271       |36          | Trim7 table (6x6)                    | @ref EEPROM_CONFIG8_MAP7           |
 * | 4307       |6           | Trim7 table (X axis) (RPM)           |                                    |
 * | 4313       |6           | Trim7 table (Y axis) (MAP)           |                                    |
 * | 4319       |2           | X and Y sizes for trim8 table        |                                    |
 * | 4321       |36          | Trim8 table (6x6)                    | @ref EEPROM_CONFIG8_MAP8           |
 * | 4357       |6           | Trim8 table (X axis) (RPM)           |                                    |
 * | 4363       |6           | Trim8 table (Y axis) (MAP)           |                                    |
 * | 4369       |2           | X and Y sizes boostLUT table         |                                    |
 * | 4371       |64          | boostLUT table (8x8)                 | @ref EEPROM_CONFIG15_MAP           |
 * | 4435       |8           | boostLUT table (X axis) (RPM)        |                                    |
 * | 4443       |8           | boostLUT table (Y axis) (targetBoost)|                                    |
 * | 4451       |1           | boostLUT enable                      | @ref EEPROM_CONFIG15_START         |
 * | 4452       |1           | boostDCWhenDisabled                  |                                    |
 * | 4453       |1           | boostControlEnableThreshold          |                                    |
 * | 4454       |14          | A/C Control Settings                 |                                    |
 * | 4468       |4           | Knock Detection Settings             |                                    |
 * | 4472       |155         | Page 15 spare                        |                                    |
 * | 4627       |217         | EMPTY                                |                                    |
 * | 4844       |4           | CLT Calibration CRC32                |                                    |
 * | 4848       |4           | IAT Calibration CRC32                |                                    |
 * | 4852       |4           | O2 Calibration CRC32                 |                                    |
 * | 4856       |56          | Page CRC32 sums (4x14)               | Last first, 14 -> 1                |
 * | 4912       |1           | Baro value saved at init             | @ref EEPROM_LAST_BARO              |
 * | 4913       |64          | O2 Calibration Bins                  | @ref EEPROM_CALIBRATION_O2_BINS    |
 * | 4977       |32          | O2 Calibration Values                | @ref EEPROM_CALIBRATION_O2_VALUES  |
 * | 5009       |64          | IAT Calibration Bins                 | @ref EEPROM_CALIBRATION_IAT_BINS   |
 * | 5073       |64          | IAT Calibration Values               | @ref EEPROM_CALIBRATION_IAT_VALUES |
 * | 5137       |64          | CLT Calibration Bins                 | @ref EEPROM_CALIBRATION_CLT_BINS   |
 * | 5201       |64          | CLT Calibration Values               | @ref EEPROM_CALIBRATION_CLT_VALUES |
 * | 5265       |            | END                                  |                                    |
 *
 */

void writeAllConfig(void);
void writeConfig(uint8_t pageNum);
void EEPROMWriteRaw(uint16_t address, uint8_t data);
uint8_t EEPROMReadRaw(uint16_t address);
void loadConfig(void);
void loadCalibration(void);
void writeCalibration(void);
void writeCalibrationPage(uint8_t pageNum);
void resetConfigPages(void);

//These are utility functions that prevent other files from having to use EEPROM.h directly
byte readLastBaro(void);
void storeLastBaro(byte newValue);
uint8_t readEEPROMVersion(void);
void storeEEPROMVersion(byte newVersion);
void storePageCRC32(uint8_t pageNum, uint32_t crcValue);
uint32_t readPageCRC32(uint8_t pageNum);
void storeCalibrationCRC32(uint8_t calibrationPageNum, uint32_t calibrationCRC);
uint32_t readCalibrationCRC32(uint8_t calibrationPageNum);
uint16_t getEEPROMSize(void);
bool isEepromWritePending(void);

extern uint32_t deferEEPROMWritesUntil;

#define EEPROM_CONFIG1_MAP    5         //
#define EEPROM_CONFIG2_START  629       //
#define EEPROM_CONFIG2_END    757       //
#define EEPROM_CONFIG3_MAP    759       //
#define EEPROM_CONFIG4_START  1047       //
#define EEPROM_CONFIG4_END    1175       //
#define EEPROM_CONFIG5_MAP    1177       //
#define EEPROM_CONFIG6_START  1465      //
#define EEPROM_CONFIG6_END    1593      //
#define EEPROM_CONFIG7_MAP1   1595      //
#define EEPROM_CONFIG7_MAP2   1885      //
#define EEPROM_CONFIG7_MAP3   2175      //
#define EEPROM_CONFIG7_END    2255      //
#define EEPROM_CONFIG8_MAP1   2257      //
#define EEPROM_CONFIG8_MAP2   2307      //336
#define EEPROM_CONFIG8_MAP3   2357      //
#define EEPROM_CONFIG8_MAP4   2407      //
#define EEPROM_CONFIG9_START  2464      //
#define EEPROM_CONFIG9_END    2656      //
#define EEPROM_CONFIG10_START 2656      //
#define EEPROM_CONFIG10_END   2848      //
#define EEPROM_CONFIG11_MAP   2850      //
#define EEPROM_CONFIG11_END   3138      //
#define EEPROM_CONFIG12_MAP   3141      //
#define EEPROM_CONFIG12_MAP2  3431      //
#define EEPROM_CONFIG12_MAP3  3721      //
#define EEPROM_CONFIG12_END   3745      //
#define EEPROM_CONFIG13_START 3750      //
#define EEPROM_CONFIG13_END   3878      //336
#define EEPROM_CONFIG14_MAP   3880      //
#define EEPROM_CONFIG14_END   4168      //
//This is OUT OF ORDER as Page 8 was expanded to add fuel trim tables 5-8. The EEPROM for them is simply added here so as not to impact existing tunes
#define EEPROM_CONFIG8_MAP5   4171      //done
#define EEPROM_CONFIG8_MAP6   4221      //done
#define EEPROM_CONFIG8_MAP7   4271      //done
#define EEPROM_CONFIG8_MAP8   4321      //done

//Page 15 added after OUT OF ORDER page 8
#define EEPROM_CONFIG15_MAP   4371      //done
#define EEPROM_CONFIG15_START 4451      //done
#define EEPROM_CONFIG15_END   4627      //done 336


#define EEPROM_CALIBRATION_CLT_CRC  4844//done
#define EEPROM_CALIBRATION_IAT_CRC  4848//done
#define EEPROM_CALIBRATION_O2_CRC   4852//done

//These were the values used previously when all calibration tables were 512 long. They need to be retained so the update process (202005 -> 202008) can work
#define EEPROM_CALIBRATION_O2_OLD   2559
#define EEPROM_CALIBRATION_IAT_OLD  3071
#define EEPROM_CALIBRATION_CLT_OLD  3583

#define EEPROM_DEFER_DELAY          1000000UL //1.0 second pause after large comms before writing to EEPROM

#endif // STORAGE_H