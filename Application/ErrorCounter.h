#ifndef __ERROR_COUNTER
#define __ERROR_COUNTER

#include "struct_typedef.h"

typedef enum
{
	CAN_CMS_ERROR,
	CAN_GIMBLE_ERROR,
	UART1_REFEREE_ERROR,
	CAN_MOTOR_1_ERROR = 16,
	CAN_MOTOR_2_ERROR = 17,
	CAN_MOTOR_3_ERROR = 18,
	CAN_MOTOR_4_ERROR = 19,
	CAN_MOTOR_5_ERROR = 20,
	NONEERROR
}error_type_e;

typedef struct
{
	uint32_t CAN_CMS_Counter;
	uint32_t CAN_GIMBLE_Counter;
	uint32_t UART1_REFEREE_Counter;
	uint32_t CAN_MOTOR_Counter[5];
	uint32_t LAST_CAN_CMS_Counter;
	uint32_t LAST_CAN_GIMBLE_Counter;
	uint32_t LAST_UART1_REFEREE_Counter;
	uint32_t LAST_CAN_MOTOR_Counter[5];
	error_type_e ERRORTYPE;
}error_counter_t;

extern error_counter_t ERROR_COUNTER;

void ErrorCounterInit();
void ErrorLED(uint32_t RGB,uint32_t counter);


void CAN_CMS_Error();
void CAN_GIMBLE_Error();
void UART1_REFEREE_Error();
void CAN_MOTOR_Error();

#endif
