/*
* ������ֵ���ƣ��޷��Զ����ء�
*	ֻ�ܼ��5�����ң�����20���뻹�Ǹ����˶����޽�
*/


#include "CMSInterface.h"
#include "gpio.h"
#include "gpio.h"
#include "usart.h"
#include "ChassisTask.h"
#include "CanSendTask.h"

//���¼���������ͨ��Ƶ�ʿ��Ƶ�����
//������λ��chassis_task���������
#define WAITINGGap		9					
#define WAITINGGap1P	(int)WAITINGGap/3
#define WAITINGGap2P	(int)WAITINGGap*2/3
#define WAITINGGap3P	WAITINGGap-1

//ModbusЭ���µļĴ���
bool CoilStatus[64];							
bool InputStatus[64];
int16_t InputRegister[64];
uint16_t HoldingRegister[64];
//Modbus������ʼ��������붨��Ķ�������
RTULocalMasterType rtuMaster;
ObjAccessInfo hgraInfo;
RTUAccessedSlaveType rtuSlave;
CMS_Handler CMS_Hub;



//����ָ��ṹ�壬�ڴ�����Ҫ��ȡ�����ݵ����ݶ�ȡ����
void *ReadCommandList[] = {	//USER_ReadErrorCode,	//0
												USER_ReadPowerState,		//1
												//USER_ReadInputCurrent,	//2
												//USER_ReadInputVoltage,	//3
												//USER_ReadOuputVoltage,	//4
												//USER_ReadOuputCurrent,	//5
												USER_ReadPowerRoute,		//6
												USER_ReadBuckCurrent,		//7
												USER_ReadCapVoltage,		//8
												USER_ReadCurrentLimt		//9
												};
												
												
//����ָ�����飬�ڴ���Ҫ���͵Ŀ���ָ���ָ��ͺ���	
//��Ҫ������·�����л�								
void *CommandList[] = {							USER_OpenPower,				//0  //�������ݹ���
												USER_ClosePower,			//1	 //�رյ��ݹ���
												USER_SwitchBattDirec,		//2	 //ѡ����ֱ����
												USER_SwitchBuckBoost,		//3  //ѡ�񳬼�����ֱ����
												USER_SwitchSutdown			//4  //ѡ��رչ���											
					  };											
#define ReadCommandLength			5		//�ܹ��еĶ�ȡָ�������
int CommandCounter = 0;
// int SwitchCount = 0;
////////////////////////////////////////////////////////////////////////////////////////////
//initialization

void USER_CMS_init(UART_HandleTypeDef *huart,CMS_Handler *CMS,DMA_HandleTypeDef *Tx,DMA_HandleTypeDef *Rx)
{
	//���ʹ���������ṹ��ʼ��
	CMS->huart = huart;
	CMS->LocalMaster = &rtuMaster;
	CMS->LocalSlave = &rtuSlave;
	CMS->LocaInfo = &hgraInfo;
	//������Ҫ���͵Ľṹ��ʼ��
	CMS->LocaInfo->unitID = 0x1;					//CMS ID is 0x1
	CMS->LocaInfo->functionCode = NULL;		//later will assgin
	CMS->LocaInfo->quantity = 1;
	CMS->LocaInfo->startingAddress = NULL;//later will assgin
	//Slave���������ʼ��
	CMS->LocalSlave->stationAddress = 0x1;
	InitializeRTUMasterObject(CMS->LocalMaster,1,CMS->LocalSlave,
														&UpdateCoilStatus,
														&UpdateInputStatus,
														&UpdateHoldingRegister,
														&UpdateInputResgister);
	//����
	CMS->MassageLength = 0;
	CMS->LocalSlave->pReadCommand = &CMS->TxMas;
	CMS->CMS_DMA_Rx = Rx;
	CMS->CMS_DMA_Tx = Tx;
	CMS->Send_Flag = 1;							// 1 ��ʾ��ǰ���Է�������
	CMS->Pred_Command = NULL;
	CMS->Next_Command = NULL;
	CMS->SwitchCount = 0;						//����ָ�����л�
	CMS->LimitCurrent = 2000;					//�������ݵ���������
	//��ʼ����
//	USER_OpenPower();
//	CMS->Send_Flag = 0;
}

void LowPassFilter_RC_1order(float *Vi, float *Vo, float *Vo_p)
{
  float  RC, Cof1, Cof2;
  Cof1 = 0.80f;
  *Vo = Cof1 * (*Vi) + (1.0f-Cof1) * (*Vo_p);    
  *Vo_p = *Vo;    
}
//////////////////////////////////////////////////////////////////////////////////////////////
//function API




void APP_SwitchCommand(void (*Pred_Command),void (*Next_Command))
{
}



uint8_t Error_Code;				//������룬���ڴ����� 
uint32_t stop_time;		

/*************�������ݲ������� begin *************/
float CapChageCurrent;
float CapChageVoltage=0.0f;
float CapChageVoltage_Past=0.0f;
float CMSInputVoltage;
float CMSInputCurrent;
bool PowerState ;
uint8_t CMS_Route_ref;
/******************** The end ********************/

/****************���ذ�ͳ�����������ͨ�ŵײ�����*******************/
void APP_BatteryCombineBuckBoost2(void)
{
	
	PowerState = CoilStatus[Power_EN];
	//��ȡ��ǰ�ĵ��ݵ���
	CapChageCurrent = ((float)InputRegister[INPUT_CBKL_CAL])/100.0f;
	//��ȡ��ǰ�ĵ����������
	CMSInputCurrent = ((float)InputRegister[INPUT_CDIR_CAL])/100.0f;
	//��ȡ��ǰ�ĵ��������ѹ
	CMSInputVoltage = (float)InputRegister[INPUT_VIN_CAL]/100.0f;
	//��ȡ��ǰ�ĵ��ݵ�ѹ
	CapChageVoltage = (float)InputRegister[INPUT_VCAP_CAL]/100.0f;
	//��ȡ��ǰ�Ĺ���·��
	CMS_Hub.Route_ref = HoldingRegister[HR_CMS_PowerRouting];
	CMS_Route_ref = CMS_Hub.Route_ref;
	
	//���㵱ǰ�ĵ��ݳ�繦��
	CMS_Hub.CMS_ChagingPower = CapChageCurrent*CapChageVoltage;
	//���㵱ǰ�ĵ������빦��
	CMS_Hub.CMS_InputPower = CMSInputCurrent*CMSInputVoltage;

	/**�����������ֶ�֮֡�󣬾���1500���������֮�����¿���ͨ��**/
	if(CMS_Hub.Send_Flag == 0)
		stop_time++;
	if(stop_time >= 1000)
	{
		stop_time = 0;
		CMS_Hub.Send_Flag = 1;
	}
	/****************************************************************/
		
	
	//��modbusû������������շ���ͨ��ʱ  CMS_Hub.Send_Flag �ᱻ��λ����ʱ���ݴ����붨λ����λ��
/****************���Ϳ���ָ��Ͷ�ȡ�������� begin *****************/
	if(CMS_Hub.Send_Flag)
	{
		if(CMS_Hub.SwitchCount%WAITINGGap==WAITINGGap1P)		//������modbus�ӻ���ͨ��Ƶ��
		{
		  if(ChassisCtrl.BukPowerEn == 1)	
		  {
			if(CoilStatus[Power_EN] == false)					//�����ڼ�Ĭ�Ϲ��翪������������֮�����·��͸�����
			{
				CMS_Hub.Next_Command = CommandList[0];			//Ĭ���ϵ翪��������磬Ҳ�����ʼ�ʹ��	
				CMS_Hub.Next_Command();				
				CMS_Hub.Send_Flag = 0;
			}
			
			if( CapChageVoltage >= 14.0f && CMSCounter >100) 		//���ҽ�������100ms�ſ��л�Ϊ�������ݶ�
			{
				if(CMS_Hub.power_routin == CMS_PR_BattDirect)		
				{
					if(CMS_Hub.Route_ref == CMS_PR_BuckBoost){
						CMSCounter = 0;
						CMS_Hub.Next_Command = CommandList[2];	
						CMS_Hub.Next_Command();
						CMS_Hub.Send_Flag = 0;
					}
				}
				
				else if(CMS_Hub.power_routin == CMS_PR_BuckBoost)	 
				{
					if(CMS_Hub.Route_ref == CMS_PR_BattDirect){
						CMSCounter = 0;
						CMS_Hub.Next_Command = CommandList[3];			
						CMS_Hub.Next_Command();
						CMS_Hub.Send_Flag = 0;
					}
				}
			 }
			 else if(CapChageVoltage<14.0f)	//���ݵ�ѹ����14Vʱ��ǿ��ѡ����ֱ�ӹ���	
			 {	
				if(CMS_Hub.Route_ref == CMS_PR_BuckBoost){
				CMS_Hub.Next_Command = CommandList[2];	
				CMS_Hub.Next_Command();
				Error_Code = 3;
				CMS_Hub.Send_Flag = 0;
				}
			 }
		  }
		  else if(ChassisCtrl.BukPowerEn == -1)
		  {
			 if(CoilStatus[Power_EN] == true){	
				CMS_Hub.Next_Command = CommandList[1];			//�رչ��ʼ�ʹ��	
				CMS_Hub.Next_Command();			
				CMS_Hub.Send_Flag = 0;
				}
		  }
		}
		
		//���Ƶ��ݳ��������������ƺͳ������ݵĺ��ģ�
		else if(CMS_Hub.SwitchCount%WAITINGGap==WAITINGGap2P)
		{
			Error_Code = 4;
			USER_SetLimitCurrent(CMS_Hub.LimitCurrent);
			CMS_Hub.Send_Flag = 0;
		}
		
		//��ȡ�������ݵ����ݲ���������Ҫ��ϸ��
		else if(CMS_Hub.SwitchCount%WAITINGGap==WAITINGGap3P)
		{
			Error_Code = 5;
			CMS_Hub.Next_Command = ReadCommandList[CommandCounter];
			CommandCounter ++;
			CMS_Hub.Send_Flag = 0;
			if(CommandCounter>=ReadCommandLength)CommandCounter =0;
			CMS_Hub.Next_Command();
		}
		CMS_Hub.SwitchCount++;
	}
/**********************���Ϳ���ָ��Ͷ�ȡ�������� end ***********************************/	

}

/*******************************************************************************/




////////////////////////////////////////////////////////////////////////////////////////////
//Basic communication function

//Reading Message*******************************************************************
//Tra:01 04 00 14 00 01 71 CE
//Rec:01 04 02 00 01 78 F0
//��ȡ������
void USER_ReadErrorCode(void)
{
	ReadCommand(ReadInputRegister,Malfunction_Code,7);
//	return InputRegister[Malfunction_Code];
}

//Tra:01 01 00 02 00 01 5C 0A
//Rec:01 01 01 00 51 88
//��ȡ��ǰCMS�Ƿ��ʼ�ʹ��
void USER_ReadPowerState(void)
{
	ReadCommand(ReadCoilStatus,Power_EN,6);
//	return CoilStatus[Power_EN];
}

//Tra:01 04 00 0A 00 01 11 C8
//Rec:01 04 02 F4 7F BE 10
//��ȡCMS���ֱ�����������
void USER_ReadInputCurrent(void)
{
	ReadCommand(ReadInputRegister,INPUT_CDIR_CAL,7);
//	return InputRegister[INPUT_CDIR_CAL];
}


//Tra:01 04 00 08 00 01 B0 08
//Rec:01 04 02 00 C6 39 62
//��ȡ�����ѹ
void USER_ReadInputVoltage(void)
{
	ReadCommand(ReadInputRegister,INPUT_VIN_CAL,7);
//	return InputRegister[INPUT_VIN_CAL];
}

//Tra:01 04 00 0E 00 01 50 09
//Rec:01 04 02 F4 6D 3E 1D
//��ȡ�����ѹ
void USER_ReadOuputVoltage(void)
{
	ReadCommand(ReadInputRegister,INPUT_VOUT_CAL,7);
//	return InputRegister[INPUT_VOUT_CAL];
}

//Tra:01 04 00 0F 00 01 01 C9
//Rec:01 04 02 FF FC F8 81
//��ȡ�������
void USER_ReadOuputCurrent(void)
{
	ReadCommand(ReadInputRegister,INPUT_CCHS_CAL,7);
//	return InputRegister[INPUT_CCHS_CAL];
}


//Tra:01 03 00 38 00 01 05 C7
//Rec:01 03 02 00 03 F8 45
//��ȡ��ǰCMS�Ĺ���ģʽ
void USER_ReadPowerRoute(void)
{
	ReadCommand(ReadHoldingRegister,HR_CMS_PowerRouting,7);
//	return HoldingRegister[HR_CMS_PowerRouting];
}

// ��ȡ���ݳ�����
void USER_ReadBuckCurrent(void)
{
	ReadCommand(ReadInputRegister,INPUT_CBKL_CAL,7);
//	return InputRegister[INPUT_CBKL_CAL];
}

//��ȡ���ݵ�ѹ
void USER_ReadCapVoltage(void)
{
	ReadCommand(ReadInputRegister,INPUT_VCAP_CAL,7);
//	return InputRegister[INPUT_VCAP_CAL];
}

//��ȡ���ݳ���������
void USER_ReadCurrentLimt(void)
{
	ReadCommand(ReadHoldingRegister,HR_CMS_Cap_Current_Limit,7);
}

// Command Message/**********************************************************************
//Tas:01 05 00 02 FF 00 2D FA
//Rec:01 05 00 02 FF 00 2D FA
//�������ʼ�
void USER_OpenPower(void)
{
	bool datalist = true;
	WriteCommand(WriteSingleCoil,Power_EN,(void *)&datalist);
	return;
}

//Tra:01 06 00 38 00 01 C9 C7
//Rec:01 06 00 38 00 01 C9 C7
//�л�CMS����ģʽ
void USER_SwitchPowerRoute(int *Mode)
{
	if((*Mode!=CMS_PR_BattDirect)&&(*Mode!=CMS_PR_BuckBoost)&&(*Mode==CMS_PR_Shutdown))
	{
		return;
	}
	WriteCommand(WriteSingleRegister,HR_CMS_PowerRouting,(void *)Mode);
	return;
}
//��CMS����ģʽ�л������ݹ���
void USER_SwitchBuckBoost(void)
{
	int Mode = CMS_PR_BuckBoost;
	WriteCommand(WriteSingleRegister,HR_CMS_PowerRouting,(void *)&Mode);
}
//��CMS����ģʽ�л�����ع���
void USER_SwitchBattDirec(void)
{
	int Mode = CMS_PR_BattDirect;
	WriteCommand(WriteSingleRegister,HR_CMS_PowerRouting,(void *)&Mode);
}
//�ر�CMS����
void USER_SwitchSutdown(void)
{
	int Mode = CMS_PR_Shutdown;
	WriteCommand(WriteSingleRegister,HR_CMS_PowerRouting,(void *)&Mode);
}
//Tra:01 05 00 02 00 00 6C 0A
//Rec:01 05 00 02 00 00 6C 0A
//�رչ��ʼ����������ã�
void USER_ClosePower(void)
{
	bool datalist = false;
	WriteCommand(WriteSingleCoil,Power_EN,(void *)&datalist);
	return;
}
//���õ�ǰ���ݵĳ�����
void USER_SetLimitCurrent(uint16_t Aim_Current)
{
	WriteCommand(WriteSingleRegister,HR_CMS_Cap_Current_Limit,(void *)&Aim_Current);
}


//����ĺ���Ϊ�ײ�ͨ�ź�����Modbus���м���ú���������̫����

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Basis Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WriteCommand(FunctionCode FunCode,uint16_t Address,void *datalist)
{
	CMS_Hub.LocaInfo->functionCode = FunCode;
	CMS_Hub.LocaInfo->startingAddress = Address;
	CMS_Hub.MassageLength = CreateAccessSlaveCommand(*CMS_Hub.LocaInfo,datalist,CMS_Hub.TxMas);// generating command code
	
	CMS_Hub.LocalSlave->commandNumber = 1;
	HAL_UART_Transmit_DMA(CMS_Hub.huart,CMS_Hub.TxMas,CMS_Hub.MassageLength);
	HAL_UART_Receive_DMA(CMS_Hub.huart,CMS_Hub.RecMas,REVE_BufLENGTH);												
}


void ReadCommand(FunctionCode FunCode,uint16_t Address,int ReceLength)
{
	CMS_Hub.LocaInfo->functionCode = FunCode;
	CMS_Hub.LocaInfo->startingAddress = Address;
	CMS_Hub.MassageLength = CreateAccessSlaveCommand(*CMS_Hub.LocaInfo,NULL,CMS_Hub.TxMas);// generating command code
	
	CMS_Hub.LocalSlave->commandNumber = 1;
	HAL_UART_Transmit_DMA(CMS_Hub.huart,CMS_Hub.TxMas,CMS_Hub.MassageLength);
	HAL_UART_Receive_DMA(CMS_Hub.huart,CMS_Hub.RecMas,REVE_BufLENGTH);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Official Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UpdateCoilStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue)
{
 //�ڿͻ��ˣ���վ��Ӧ����ʵ��
	for (int i=0;i<quantity;i++){
				CoilStatus[startAddress+i]=*stateValue;
				stateValue++;
	}
}
 
/*���¶�����������״ֵ̬*/
void UpdateInputStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue)
{
 //�ڿͻ��ˣ���վ��Ӧ����ʵ��
	for (int i=0;i<quantity;i++){
				InputStatus[startAddress+i]=*stateValue;
				stateValue++;
	}
}
 
/*���¶������ı��ּĴ���*/
void UpdateHoldingRegister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
 //�ڿͻ��ˣ���վ��Ӧ����ʵ��
	for (int i=0;i<quantity;i++){
				HoldingRegister[startAddress+i]=*registerValue;
				registerValue++;
	}
}
 
/*���¶�����������Ĵ���*/
void UpdateInputResgister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
 //�ڿͻ��ˣ���վ��Ӧ����ʵ��
	for (int i=0;i<quantity;i++){
				InputRegister[startAddress+i]=*registerValue;
				registerValue++;
	}
}

void CMS_usart_init(void)
{
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart3, CMS_Hub.RecMas, REVE_BufLENGTH);
}
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
	HAL_UART_IRQHandler(&huart3);
  /* USER CODE END USART3_IRQn 0 */
  
  /* USER CODE BEGIN USART3_IRQn 1 */
	if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE) != RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);											
		HAL_UART_DMAStop(&huart3);
		uint16_t RX2_Length = REVE_BufLENGTH - hdma_usart3_rx.Instance->NDTR;
		ParsingSlaveRespondMessage(CMS_Hub.LocalMaster,CMS_Hub.RecMas,CMS_Hub.TxMas);
		CMS_Hub.Send_Flag = 1;					//���Ϳ���ָ��ҽ��յ��ش�ָ��֮�� CMS_Hub.Send_Flag ����λ
		HAL_UART_Receive_DMA(&huart3, CMS_Hub.RecMas, REVE_BufLENGTH);
	}
  /* USER CODE END USART3_IRQn 1 */
}
