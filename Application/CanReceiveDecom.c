#include "system.h"
#include "CanReceiveDecom.h"

#define get_motor_measure(ptr, data)                                    \
    {                                                                   \
        (ptr)->last_ecd = (ptr)->ecd;                                   \
        (ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);            \
        (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);      \
        (ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]);  \
        (ptr)->temperate = (data)[6];                                   \
        (ptr)->angle = (ptr)->ecd / 8191.0f *360 - 180;          		\
		(ptr)->speed = (ptr)->speed_rpm * 360.0f;						\
    }
	
motor_measure_t ChassisMotor[4];
motor_measure_t YawMotor;
Aim_t Aim;
PTZ_t PTZ;

/*---------------------------------------CAN的接收回调函数-------------------------------------*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
	/*-------------------------------------电机数据的接收-------------------------------------*/
	//底盘电机
	if(hcan->Instance == CAN1)
	{
		switch(rx_header.StdId)
		{
			case CMSRecceiveID:
				ERROR_COUNTER.CAN_CMS_Counter++;
				CMS.Enable = rx_data[0];
				CMS.RxOpen = rx_data[1];
				CMS.Power = (int16_t)rx_data[2]<<8|rx_data[3];
				CMS.Electricity = (int16_t)rx_data[4]<<8|rx_data[5];
				break;
			case CanMotor1Id:
			case CanMotor2Id:
			case CanMotor3Id:
			case CanMotor4Id:
			{
				static uint8_t i = 0;
				i = rx_header.StdId - CanMotor1Id;
				get_motor_measure(&ChassisMotor[i], rx_data);
				
				ERROR_COUNTER.CAN_MOTOR_Counter[i]++;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	else if(hcan->Instance == CAN2)
	{
		switch(rx_header.StdId)
		{
			//云台YAW电机
			case YawMotorId:
			{
				get_motor_measure(&YawMotor, rx_data);
				
				ERROR_COUNTER.CAN_MOTOR_Counter[4]++;
				break;
			}
			/*-------------------------------------云台数据下发接收-------------------------------------*/
			case DefaultAimStatusAndTargetId:
			{
				ERROR_COUNTER.CAN_GIMBLE_Counter++;
				memcpy(&Aim,rx_data,sizeof(Aim_t));
				break;
			}
			case SentryAimStatusAndTargetId:
			{
				break;//哨兵相关，暂时不管
			}
			case DefaulPTZRequestAndStatusId:
			{
				ERROR_COUNTER.CAN_GIMBLE_Counter++;
				memcpy(&PTZ,rx_data,sizeof(PTZ_t));
				break;
			}
			case SentryPTZRequestAndStatusId:
			{
				break;//哨兵相关，不管
			}
			default:
			{
				break;
			}
		}
	}
}

const motor_measure_t *GetChassisMeasure(uint8_t i)
{
	return &ChassisMotor[i];
}
const motor_measure_t *GetYawMeasure(void)
{
	return &YawMotor;
}
