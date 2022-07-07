#ifndef __CMS_H
#define __CMS_H

#include "struct_typedef.h"

typedef enum
{
	CSMCurrentSendID = 0x210,
	CSMRefereeSendID = 0x211,
	CMSRecceiveID = 0x212,
	
}CMS_e;

typedef struct
{
	uint8_t enable;
	int16_t Electricity;
}CMS_t;

extern CMS_t CMS;

void CMS_Current_Send(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void CMS_Referee_Send(int16_t power_limit, int16_t chassis_power);

#endif
