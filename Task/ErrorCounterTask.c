#include "ErrorCounterTask.h"
#include "system.h"

void ErrorCounterTask(void const * argument)
{
	ErrorCounterInit();
	while(1)
	{
		CAN_CMS_Error();
		CAN_GIMBLE_Error();
		UART1_REFEREE_Error();
		CAN_MOTOR_Error();
		switch(ERROR_COUNTER.ERRORTYPE)
		{
			case CAN_CMS_ERROR:
				ErrorLED(0xff0000,1);
				break;
			case CAN_GIMBLE_ERROR:
				ErrorLED(0xff0000,2);
				break;
			case UART1_REFEREE_ERROR:
				ErrorLED(0xff0000,3);
				break;
			case CAN_MOTOR_1_ERROR:
				ErrorLED(0x0000ff,1);
				break;
			case CAN_MOTOR_2_ERROR:
				ErrorLED(0x0000ff,2);
				break;
			case CAN_MOTOR_3_ERROR:
				ErrorLED(0x0000ff,3);
				break;
			case CAN_MOTOR_4_ERROR:
				ErrorLED(0x0000ff,4);
				break;
			case CAN_MOTOR_5_ERROR:
				ErrorLED(0x0000ff,5);
				break;
			case NONEERROR:
				ErrorLED(0x00ff00,1);
				break;
			default:
				osDelay(1);
				break;
		}
	}
}

