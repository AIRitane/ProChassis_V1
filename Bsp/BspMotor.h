#ifndef BSP_CAN_H
#define BSP_CAN_H
#include "struct_typedef.h"

/* CAN send and receive ID */
typedef enum
{
    ChassisAllId = 0x200,
    CanMotor1Id = 0x201,
    CanMotor2Id = 0x202,
    CanMotor3Id = 0x203,
    CanMotor4Id = 0x204
} ChassisID;

typedef enum
{
    YawAllId = 0x1ff,
	YawMotorId = 0x205
} YawID;
//rm motor data
typedef struct
{
    uint16_t ecd;
    int16_t speed_rpm;
    int16_t given_current;
    uint8_t temperate;
    uint16_t last_ecd;
	
    fp32 angle;
	fp32 speed;
} motor_measure_t;

extern void ChassisCMD(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);

#endif
