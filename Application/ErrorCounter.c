#include "ErrorCounter.h"
#include "system.h"

error_counter_t ERROR_COUNTER;

void CAN_CMS_Error();
void CAN_GIMBLE_Error();
void UART1_REFEREE_Error();
void CAN_MOTOR_Error();


void ErrorCounterInit()
{
	ERROR_COUNTER.LAST_CAN_CMS_Counter = ERROR_COUNTER.CAN_CMS_Counter = 0;
	ERROR_COUNTER.LAST_CAN_GIMBLE_Counter =  ERROR_COUNTER.CAN_GIMBLE_Counter = 0;
	for(uint8_t i=0;i<5;i++)
		ERROR_COUNTER.LAST_CAN_MOTOR_Counter[i] =  ERROR_COUNTER.CAN_MOTOR_Counter[i] = 0;
	ERROR_COUNTER.LAST_UART1_REFEREE_Counter = ERROR_COUNTER.UART1_REFEREE_Counter = 0;
	ERROR_COUNTER.ERRORTYPE = NONEERROR;
}
	
//Í¨ÐÅ´íÎó¼ì²â
uint8_t ErrorFlag = 0;
void CAN_CMS_Error()
{
	if(!(ERROR_COUNTER.CAN_CMS_Counter - ERROR_COUNTER.LAST_CAN_CMS_Counter))
	{
		ERROR_COUNTER.ERRORTYPE = CAN_CMS_ERROR;
		ErrorFlag = 1;
	}
	else
	{
		ERROR_COUNTER.ERRORTYPE = NONEERROR;
		ErrorFlag = 0;
	}
	
	ERROR_COUNTER.LAST_CAN_CMS_Counter = ERROR_COUNTER.CAN_CMS_Counter;
}

void CAN_GIMBLE_Error()
{
	if(!(ERROR_COUNTER.CAN_GIMBLE_Counter - ERROR_COUNTER.LAST_CAN_GIMBLE_Counter) && !ErrorFlag)
	{
		ERROR_COUNTER.ERRORTYPE = CAN_GIMBLE_ERROR;
		ErrorFlag = 1;
	}
	else if(!ErrorFlag) ERROR_COUNTER.ERRORTYPE = NONEERROR;
	
	ERROR_COUNTER.LAST_CAN_GIMBLE_Counter = ERROR_COUNTER.CAN_GIMBLE_Counter;
	
}

void UART1_REFEREE_Error()
{
	for(uint8_t i=0;i<5;i++)
	{
		if(!(ERROR_COUNTER.CAN_MOTOR_Counter[i] - ERROR_COUNTER.LAST_CAN_MOTOR_Counter[i]) && !ErrorFlag)
		{
			ERROR_COUNTER.ERRORTYPE = CAN_MOTOR_1_ERROR + i;
			ErrorFlag = 1;
		}
		else if(!ErrorFlag) ERROR_COUNTER.ERRORTYPE = NONEERROR;
		
		ERROR_COUNTER.LAST_CAN_MOTOR_Counter[i] = ERROR_COUNTER.CAN_MOTOR_Counter[i];
	}
}

void CAN_MOTOR_Error()
{
	if(!(ERROR_COUNTER.UART1_REFEREE_Counter - ERROR_COUNTER.LAST_UART1_REFEREE_Counter) && !ErrorFlag)
	{
		ERROR_COUNTER.ERRORTYPE = UART1_REFEREE_ERROR;
		ErrorFlag = 1;
	}
	else if(!ErrorFlag) ERROR_COUNTER.ERRORTYPE = NONEERROR;
	
	ERROR_COUNTER.LAST_UART1_REFEREE_Counter = ERROR_COUNTER.UART1_REFEREE_Counter;
}

//Ò»ÃëÉÁµÆº¯Êý
void ErrorLED(uint32_t RGB,uint32_t counter)
{
	uint32_t delay;
	uint32_t aRGB;
	
	delay = 500/counter;
	aRGB = 0xff000000 | RGB;
	for(uint8_t i=0;i<counter;i++)
	{
		aRGB_led_show(aRGB);
		osDelay(delay);
		aRGB_led_show(0x0000000);
		osDelay(delay);
	}
	aRGB_led_show(0x0000000);
	osDelay(1000);
}
	
