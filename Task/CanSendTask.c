#include "system.h"
#include "CanSendTask.h"


uint8_t DataBuffer[11];
extern osThreadId CanSendHandle;
extern osThreadId RefereeHandle;

static CAN_TxHeaderTypeDef can_tx_message;

static void Can_Send(CAN_HandleTypeDef *hcan,uint32_t id,uint8_t lenth,uint8_t *buffer);

void CanSendTask(void const * argument)
{
	int flag = 0;
	uint8_t SendLenth;
	uint32_t SendId;
	while(1)
	{
		Pop(SendBuffer,(SendBuffer+1),DataBuffer);
		
		SendId = (uint32_t)(DataBuffer[0]<<8|DataBuffer[1]);
		SendLenth = DataBuffer[2];
		Can_Send(&hcan2,SendId,SendLenth,&DataBuffer[3]);

		osDelay(1);
	}
}

static void Can_Send(CAN_HandleTypeDef *hcan,uint32_t id,uint8_t lenth,uint8_t *buffer)
{
	uint32_t send_mail_box;
    can_tx_message.StdId = id;
    can_tx_message.IDE = CAN_ID_STD;
    can_tx_message.RTR = CAN_RTR_DATA;
    can_tx_message.DLC = lenth;
	
    HAL_CAN_AddTxMessage(hcan, &can_tx_message, buffer, &send_mail_box);
}
