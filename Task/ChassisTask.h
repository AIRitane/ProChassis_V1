#ifndef __CHASSISTASK_H
#define __CHASSISTASK_H

#include "CanReceiveDecom.h"
#include "pid.h"
#include "gpio.h"

#define PI 3.14159265
#define FallowAngle -144
#define RotingBaseSpeed 0.64

typedef enum
{
	NOFORCE,
	STOP,
	FALLOW,
	ROTING
}ChassisMode_e;


typedef struct
{
	float frame_period;
	float input;
	float out;
	float error;
	float buffer;
}BufferFunction_t;

typedef struct
{
	const motor_measure_t *Motor[4];
	const motor_measure_t *Yaw;
	ChassisMode_e Mode;
	
	float vx;
	float vy;
	float wz;
	pid_type_def XYPid[4];
	pid_type_def WZPid;
	float Current[4];
	float WheelSpeed[4];
}ChassisCtrl_t;

extern ChassisCtrl_t ChassisCtrl;
extern int16_t power_limit;
extern int16_t chassis_power;
extern int16_t robot_level;

void ChassisControlLoop();
#endif