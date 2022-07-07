#ifndef __CANRECEIVEDECOME_H
#define __CANRECEIVEDECOME_H

#include "struct_typedef.h"
#include "BspMotor.h"

typedef enum
{
	DefaultAimStatusAndTargetId = 0x106,
	SentryAimStatusAndTargetId = 0x107,
	DefaulPTZRequestAndStatusId = 0x110,
	SentryPTZRequestAndStatusId = 0x111,
}CanReceive_e;

typedef __PACKED_STRUCT
{
	uint8_t AimStatus;
	uint8_t AimTarget;
}Aim_t;

typedef __PACKED_STRUCT
{
	uint8_t AimTargetRequest;
	int16_t FBSpeed;
	int16_t LRSpeed;
	uint8_t ChassisStatueRequest;
	uint8_t PTZStatusInformation;
}PTZ_t;

//这两个结构体的使用，参见《CAN总线数据内容及数据格式规定》
extern Aim_t Aim;
extern PTZ_t PTZ;

void can_filter_init(void);
const motor_measure_t *GetChassisMeasure(uint8_t i);
const motor_measure_t *GetYawMeasure(void);

#endif
