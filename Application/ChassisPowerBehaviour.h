#ifndef CHASSIS_POWER_CONTROL_H
#define CHASSIS_POWER_CONTROL_H
#include "ChassisTask.h"
#include "main.h"

extern float SupKp;
extern fp32 total_current_limit;
extern fp32 total_current;
extern uint8_t CalculCount;
extern float power_limit;

void ChassisPowerControl();

#endif
