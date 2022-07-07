#include "BspMotor.h"
#include "can.h"

static CAN_TxHeaderTypeDef  can_tx_message_l;
static uint8_t              can_send_data_l[8];

void ChassisCMD(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
    uint32_t send_mail_box_l;
    can_tx_message_l.StdId = ChassisAllId;
    can_tx_message_l.IDE = CAN_ID_STD;
    can_tx_message_l.RTR = CAN_RTR_DATA;
    can_tx_message_l.DLC = 0x08;
    can_send_data_l[0] = motor1 >> 8;
    can_send_data_l[1] = motor1;
    can_send_data_l[2] = motor2 >> 8;
    can_send_data_l[3] = motor2;
    can_send_data_l[4] = motor3 >> 8;
    can_send_data_l[5] = motor3;
    can_send_data_l[6] = motor4 >> 8;
    can_send_data_l[7] = motor4;

    HAL_CAN_AddTxMessage(&hcan2, &can_tx_message_l, can_send_data_l, &send_mail_box_l);
}
