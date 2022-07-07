#include "system.h"
#include "CMS.h"

CMS_t CMS;

static CAN_TxHeaderTypeDef  can_tx_message[2];
static uint8_t              can_send_data[2][8];

void CMS_Current_Send(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
    uint32_t send_mail_box;
    can_tx_message[0].StdId = CSMCurrentSendID;
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

void CMS_Referee_Send(int16_t power_limit, int16_t chassis_power)
{
		uint32_t send_mail_box;
    can_tx_message[1].StdId = CSMCurrentSendID;
    can_tx_message[1].IDE = CAN_ID_STD;
    can_tx_message[1].RTR = CAN_RTR_DATA;
    can_tx_message[1].DLC = 0x04;
    can_send_data[1][0] = power_limit >> 8;
    can_send_data[1][1] = power_limit;
    can_send_data[1][2] = chassis_power >> 8;
    can_send_data[1][3] = chassis_power;
    HAL_CAN_AddTxMessage(&hcan1, &can_tx_message[1], can_send_data[1], &send_mail_box);
}