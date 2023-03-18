#ifndef CANBROADCAST_H
#define CANBROADCAST_H
#if defined(NATIVE_CAN_AVAILABLE)

//For BMW e46/e39/e38, rover and mini other CAN instrument clusters
#define CAN_BMW_ASC1 0x153 //Rx message from ACS unit that includes speed
#define CAN_BMW_DME1 0x316 //Tx message that includes RPM
#define CAN_BMW_DME2 0x329 //Tx message that includes CLT and TPS
#define CAN_BMW_DME4 0x545 //Tx message that includes CLT and TPS
#define CAN_BMW_ICL2 0x613
#define CAN_BMW_ICL3 0x615

//For VAG CAN instrument clusters
#define CAN_VAG_RPM 0x280
#define CAN_VAG_VSS 0x5A0

//For Haltech v2.35 Broadcast
#define CAN_HTK_RPM 0x360
#define CAN_HTK_FOP 0x361
//#define CAN_HTK_DCY 0x362
#define CAN_HTK_WBO 0x368
#define CAN_HTK_SPD 0x370
#define CAN_HTK_BAT 0x372
#define CAN_HTK_CLT 0x3E0
//#define CAN_HTK_GEA 0x470

void sendBMWCluster();
void sendVAGCluster();
void sendHALTECHv210Hz();
void sendHALTECHv230Hz();
void sendHALTECHv24Hz();
void DashMessages(uint16_t DashMessageID);
#endif
#endif // CANBROADCAST_H
