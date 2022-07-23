#include "system.h"
#include "CMS.h"

CMS_t CMS;

static CAN_TxHeaderTypeDef  can_tx_message[2];
static uint8_t              can_send_data[2][8];

uint32_t cms_count = 0;
void CMS_STATUS()
{
	
	if(CMS.Electricity < 1200 || CMS.Enable == 0)
	{
		cms_count++;
	}
	else 
		cms_count = 0;
	if(cms_count > 200)
	{
		CMS.RxOpen = 0; 
	}
}
void Motor_Send(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
    uint32_t send_mail_box;
    can_tx_message[0].StdId = ChassisAllId;
    can_tx_message[0].IDE = CAN_ID_STD;
    can_tx_message[0].RTR = CAN_RTR_DATA;
    can_tx_message[0].DLC = 0x08;
    can_send_data[0][0] = motor1 >> 8;
    can_send_data[0][1] = motor1;
    can_send_data[0][2] = motor2 >> 8;
    can_send_data[0][3] = motor2;
    can_send_data[0][4] = motor3 >> 8;
    can_send_data[0][5] = motor3;
    can_send_data[0][6] = motor4 >> 8;
    can_send_data[0][7] = motor4;

    HAL_CAN_AddTxMessage(&hcan1, &can_tx_message[0], can_send_data[0], &send_mail_box);
}

void CMS_Referee_Send(uint16_t charge_limit, uint8_t enable)
{
		uint32_t send_mail_box;
    can_tx_message[1].StdId = CSMCurrentSendID;
    can_tx_message[1].IDE = CAN_ID_STD;
    can_tx_message[1].RTR = CAN_RTR_DATA;
    can_tx_message[1].DLC = 0x03;
    can_send_data[1][0] = charge_limit >> 8;
    can_send_data[1][1] = charge_limit;
    can_send_data[1][2] = enable;
    HAL_CAN_AddTxMessage(&hcan1, &can_tx_message[1], can_send_data[1], &send_mail_box);
}
