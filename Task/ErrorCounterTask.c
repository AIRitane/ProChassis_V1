#include "ErrorCounterTask.h"
#include "system.h"

/*
 * 监控通信是否离线，可以根据需要动态拓展
 * CMS离线 红灯一秒一次
 * 云台通信离线 红灯一秒两次
 * 裁判系统离线 红灯一秒闪三次
 * 电机离线 ID为n,蓝灯按照一秒闪烁n次
 * 正常 白灯一秒一次
 */
void ErrorCounterTask(void const * argument)
{
	ErrorCounterInit();
	while(1)
	{
		//printf("%f,%f,%f\n",chassis_power_buffer,chassis_power,CMS.Electricity/100.0f);
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
				osDelay(4);
				break;
		}
	}
}

