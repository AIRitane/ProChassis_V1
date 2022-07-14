#ifndef __CMS_H
#define __CMS_H

#include "struct_typedef.h"

typedef enum
{
	CSMCurrentSendID = 0x210,
	CMSRecceiveID = 0x211,
}CMS_e;

typedef struct
{
	uint8_t Enable;
	uint8_t RxOpen;
	uint16_t Power;
	int16_t Electricity;
	int16_t charge_limit;
	uint8_t TxOpen;
}CMS_t;

extern CMS_t CMS;

void Motor_Send(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void CMS_Referee_Send(uint16_t charge_limit, uint8_t enable);
void CMS_STATUS();

#endif
