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

/*---------------------------------------CAN�Ľ��ջص�����-------------------------------------*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
	/*-------------------------------------������ݵĽ���-------------------------------------*/
	//���̵��
	if(hcan->Instance == CAN1)
	{
		switch(rx_header.StdId)
		{
			case CanMotor1Id:
			case CanMotor2Id:
			case CanMotor3Id:
			case CanMotor4Id:
			{
				static uint8_t i = 0;
				i = rx_header.StdId - CanMotor1Id;
				get_motor_measure(&ChassisMotor[i], rx_data);
				
				break;
			}
			default:
			{
				//����ɫ����CAN�߽Ӵ�������δ֪ID
				aRGB_led_show(0xFFBF16F0);
				break;
			}
		}
	}
	
	else if(hcan->Instance == CAN2)
	{
		switch(rx_header.StdId)
		{
			//��̨YAW���
			case YawMotorId:
			{
				get_motor_measure(&YawMotor, rx_data);
				
				break;
			}
			/*-------------------------------------��̨�����·�����-------------------------------------*/
			case DefaultAimStatusAndTargetId:
			{
				memcpy(&Aim,rx_data,sizeof(Aim_t));
				break;
			}
			case SentryAimStatusAndTargetId:
			{
				break;//�ڱ���أ���ʱ����
			}
			case DefaulPTZRequestAndStatusId:
			{
				memcpy(&PTZ,rx_data,sizeof(PTZ_t));
				break;
			}
			case SentryPTZRequestAndStatusId:
			{
				break;//�ڱ���أ�����
			}
			default:
			{
				//���ǵ�CAN2�����ϻ��������豸�����������Ƽ��
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