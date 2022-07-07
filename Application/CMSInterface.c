/*
* 由于容值限制，无法自动调控。
*	只能坚持5秒左右，西理工20多秒还是高速运动，无解
*/


#include "CMSInterface.h"
#include "gpio.h"
#include "gpio.h"
#include "usart.h"
#include "ChassisTask.h"
#include "CanSendTask.h"

//以下几个参数起到通信频率控制的作用
//参数单位是chassis_task的任务节拍
#define WAITINGGap		9					
#define WAITINGGap1P	(int)WAITINGGap/3
#define WAITINGGap2P	(int)WAITINGGap*2/3
#define WAITINGGap3P	WAITINGGap-1

//Modbus协议下的寄存器
bool CoilStatus[64];							
bool InputStatus[64];
int16_t InputRegister[64];
uint16_t HoldingRegister[64];
//Modbus主机初始化部署必须定义的对象类型
RTULocalMasterType rtuMaster;
ObjAccessInfo hgraInfo;
RTUAccessedSlaveType rtuSlave;
CMS_Handler CMS_Hub;



//函数指针结构体，内存有需要读取的数据的数据读取函数
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
												
												
//函数指针数组，内存有要发送的控制指令的指令发送函数	
//主要有用于路径的切换								
void *CommandList[] = {							USER_OpenPower,				//0  //开启电容供电
												USER_ClosePower,			//1	 //关闭电容供电
												USER_SwitchBattDirec,		//2	 //选择电池直供电
												USER_SwitchBuckBoost,		//3  //选择超级电容直供电
												USER_SwitchSutdown			//4  //选择关闭供电											
					  };											
#define ReadCommandLength			5		//总共有的读取指令的数量
int CommandCounter = 0;
// int SwitchCount = 0;
////////////////////////////////////////////////////////////////////////////////////////////
//initialization

void USER_CMS_init(UART_HandleTypeDef *huart,CMS_Handler *CMS,DMA_HandleTypeDef *Tx,DMA_HandleTypeDef *Rx)
{
	//发送串口与基本结构初始化
	CMS->huart = huart;
	CMS->LocalMaster = &rtuMaster;
	CMS->LocalSlave = &rtuSlave;
	CMS->LocaInfo = &hgraInfo;
	//本地需要发送的结构初始化
	CMS->LocaInfo->unitID = 0x1;					//CMS ID is 0x1
	CMS->LocaInfo->functionCode = NULL;		//later will assgin
	CMS->LocaInfo->quantity = 1;
	CMS->LocaInfo->startingAddress = NULL;//later will assgin
	//Slave对象基本初始化
	CMS->LocalSlave->stationAddress = 0x1;
	InitializeRTUMasterObject(CMS->LocalMaster,1,CMS->LocalSlave,
														&UpdateCoilStatus,
														&UpdateInputStatus,
														&UpdateHoldingRegister,
														&UpdateInputResgister);
	//其余
	CMS->MassageLength = 0;
	CMS->LocalSlave->pReadCommand = &CMS->TxMas;
	CMS->CMS_DMA_Rx = Rx;
	CMS->CMS_DMA_Tx = Tx;
	CMS->Send_Flag = 1;							// 1 表示当前可以发送数据
	CMS->Pred_Command = NULL;
	CMS->Next_Command = NULL;
	CMS->SwitchCount = 0;						//发送指令组切换
	CMS->LimitCurrent = 2000;					//超级电容的最大充电电流
	//开始启动
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



uint8_t Error_Code;				//错误代码，用于错误检测 
uint32_t stop_time;		

/*************电容数据参数定义 begin *************/
float CapChageCurrent;
float CapChageVoltage=0.0f;
float CapChageVoltage_Past=0.0f;
float CMSInputVoltage;
float CMSInputCurrent;
bool PowerState ;
uint8_t CMS_Route_ref;
/******************** The end ********************/

/****************主控板和超级电容主控通信底层驱动*******************/
void APP_BatteryCombineBuckBoost2(void)
{
	
	PowerState = CoilStatus[Power_EN];
	//获取当前的电容电流
	CapChageCurrent = ((float)InputRegister[INPUT_CBKL_CAL])/100.0f;
	//获取当前的电容输入电流
	CMSInputCurrent = ((float)InputRegister[INPUT_CDIR_CAL])/100.0f;
	//获取当前的电容输入电压
	CMSInputVoltage = (float)InputRegister[INPUT_VIN_CAL]/100.0f;
	//获取当前的电容电压
	CapChageVoltage = (float)InputRegister[INPUT_VCAP_CAL]/100.0f;
	//获取当前的供电路径
	CMS_Hub.Route_ref = HoldingRegister[HR_CMS_PowerRouting];
	CMS_Route_ref = CMS_Hub.Route_ref;
	
	//计算当前的电容充电功率
	CMS_Hub.CMS_ChagingPower = CapChageCurrent*CapChageVoltage;
	//计算当前的电容输入功率
	CMS_Hub.CMS_InputPower = CMSInputCurrent*CMSInputVoltage;

	/**错误处理，当出现断帧之后，经过1500个任务节拍之后重新开启通信**/
	if(CMS_Hub.Send_Flag == 0)
		stop_time++;
	if(stop_time >= 1000)
	{
		stop_time = 0;
		CMS_Hub.Send_Flag = 1;
	}
	/****************************************************************/
		
	
	//当modbus没有完成完整的收发的通信时  CMS_Hub.Send_Flag 会被复位，此时根据错误码定位错误位置
/****************发送控制指令和读取电容数据 begin *****************/
	if(CMS_Hub.Send_Flag)
	{
		if(CMS_Hub.SwitchCount%WAITINGGap==WAITINGGap1P)		//控制与modbus从机的通信频率
		{
		  if(ChassisCtrl.BukPowerEn == 1)	
		  {
			if(CoilStatus[Power_EN] == false)					//比赛期间默认供电开启，仅当掉电之后重新发送该命令
			{
				CMS_Hub.Next_Command = CommandList[0];			//默认上电开启超级充电，也即功率级使能	
				CMS_Hub.Next_Command();				
				CMS_Hub.Send_Flag = 0;
			}
			
			if( CapChageVoltage >= 14.0f && CMSCounter >100) 		//当且仅当超过100ms才可切换为超级电容端
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
			 else if(CapChageVoltage<14.0f)	//电容电压低于14V时，强制选择电池直接供电	
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
				CMS_Hub.Next_Command = CommandList[1];			//关闭功率级使能	
				CMS_Hub.Next_Command();			
				CMS_Hub.Send_Flag = 0;
				}
		  }
		}
		
		//控制电容充电电流，功率限制和超级电容的核心！
		else if(CMS_Hub.SwitchCount%WAITINGGap==WAITINGGap2P)
		{
			Error_Code = 4;
			USER_SetLimitCurrent(CMS_Hub.LimitCurrent);
			CMS_Hub.Send_Flag = 0;
		}
		
		//读取超级电容的数据参数，不需要仔细看
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
/**********************发送控制指令和读取电容数据 end ***********************************/	

}

/*******************************************************************************/




////////////////////////////////////////////////////////////////////////////////////////////
//Basic communication function

//Reading Message*******************************************************************
//Tra:01 04 00 14 00 01 71 CE
//Rec:01 04 02 00 01 78 F0
//读取错误码
void USER_ReadErrorCode(void)
{
	ReadCommand(ReadInputRegister,Malfunction_Code,7);
//	return InputRegister[Malfunction_Code];
}

//Tra:01 01 00 02 00 01 5C 0A
//Rec:01 01 01 00 51 88
//读取当前CMS是否功率级使能
void USER_ReadPowerState(void)
{
	ReadCommand(ReadCoilStatus,Power_EN,6);
//	return CoilStatus[Power_EN];
}

//Tra:01 04 00 0A 00 01 11 C8
//Rec:01 04 02 F4 7F BE 10
//读取CMS电池直供的输入电流
void USER_ReadInputCurrent(void)
{
	ReadCommand(ReadInputRegister,INPUT_CDIR_CAL,7);
//	return InputRegister[INPUT_CDIR_CAL];
}


//Tra:01 04 00 08 00 01 B0 08
//Rec:01 04 02 00 C6 39 62
//读取输入电压
void USER_ReadInputVoltage(void)
{
	ReadCommand(ReadInputRegister,INPUT_VIN_CAL,7);
//	return InputRegister[INPUT_VIN_CAL];
}

//Tra:01 04 00 0E 00 01 50 09
//Rec:01 04 02 F4 6D 3E 1D
//读取输出电压
void USER_ReadOuputVoltage(void)
{
	ReadCommand(ReadInputRegister,INPUT_VOUT_CAL,7);
//	return InputRegister[INPUT_VOUT_CAL];
}

//Tra:01 04 00 0F 00 01 01 C9
//Rec:01 04 02 FF FC F8 81
//读取输出电流
void USER_ReadOuputCurrent(void)
{
	ReadCommand(ReadInputRegister,INPUT_CCHS_CAL,7);
//	return InputRegister[INPUT_CCHS_CAL];
}


//Tra:01 03 00 38 00 01 05 C7
//Rec:01 03 02 00 03 F8 45
//读取当前CMS的供电模式
void USER_ReadPowerRoute(void)
{
	ReadCommand(ReadHoldingRegister,HR_CMS_PowerRouting,7);
//	return HoldingRegister[HR_CMS_PowerRouting];
}

// 读取电容充电电流
void USER_ReadBuckCurrent(void)
{
	ReadCommand(ReadInputRegister,INPUT_CBKL_CAL,7);
//	return InputRegister[INPUT_CBKL_CAL];
}

//读取电容电压
void USER_ReadCapVoltage(void)
{
	ReadCommand(ReadInputRegister,INPUT_VCAP_CAL,7);
//	return InputRegister[INPUT_VCAP_CAL];
}

//读取电容充电电流限制
void USER_ReadCurrentLimt(void)
{
	ReadCommand(ReadHoldingRegister,HR_CMS_Cap_Current_Limit,7);
}

// Command Message/**********************************************************************
//Tas:01 05 00 02 FF 00 2D FA
//Rec:01 05 00 02 FF 00 2D FA
//开启功率级
void USER_OpenPower(void)
{
	bool datalist = true;
	WriteCommand(WriteSingleCoil,Power_EN,(void *)&datalist);
	return;
}

//Tra:01 06 00 38 00 01 C9 C7
//Rec:01 06 00 38 00 01 C9 C7
//切换CMS供电模式
void USER_SwitchPowerRoute(int *Mode)
{
	if((*Mode!=CMS_PR_BattDirect)&&(*Mode!=CMS_PR_BuckBoost)&&(*Mode==CMS_PR_Shutdown))
	{
		return;
	}
	WriteCommand(WriteSingleRegister,HR_CMS_PowerRouting,(void *)Mode);
	return;
}
//将CMS供电模式切换到电容供电
void USER_SwitchBuckBoost(void)
{
	int Mode = CMS_PR_BuckBoost;
	WriteCommand(WriteSingleRegister,HR_CMS_PowerRouting,(void *)&Mode);
}
//将CMS供电模式切换到电池供电
void USER_SwitchBattDirec(void)
{
	int Mode = CMS_PR_BattDirect;
	WriteCommand(WriteSingleRegister,HR_CMS_PowerRouting,(void *)&Mode);
}
//关闭CMS供电
void USER_SwitchSutdown(void)
{
	int Mode = CMS_PR_Shutdown;
	WriteCommand(WriteSingleRegister,HR_CMS_PowerRouting,(void *)&Mode);
}
//Tra:01 05 00 02 00 00 6C 0A
//Rec:01 05 00 02 00 00 6C 0A
//关闭功率级（基本不用）
void USER_ClosePower(void)
{
	bool datalist = false;
	WriteCommand(WriteSingleCoil,Power_EN,(void *)&datalist);
	return;
}
//设置当前电容的充电电流
void USER_SetLimitCurrent(uint16_t Aim_Current)
{
	WriteCommand(WriteSingleRegister,HR_CMS_Cap_Current_Limit,(void *)&Aim_Current);
}


//下面的函数为底层通信函数与Modbus的中间调用函数，不用太在意

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
 //在客户端（主站）应用中实现
	for (int i=0;i<quantity;i++){
				CoilStatus[startAddress+i]=*stateValue;
				stateValue++;
	}
}
 
/*更新读回来的输入状态值*/
void UpdateInputStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue)
{
 //在客户端（主站）应用中实现
	for (int i=0;i<quantity;i++){
				InputStatus[startAddress+i]=*stateValue;
				stateValue++;
	}
}
 
/*更新读回来的保持寄存器*/
void UpdateHoldingRegister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
 //在客户端（主站）应用中实现
	for (int i=0;i<quantity;i++){
				HoldingRegister[startAddress+i]=*registerValue;
				registerValue++;
	}
}
 
/*更新读回来的输入寄存器*/
void UpdateInputResgister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
 //在客户端（主站）应用中实现
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
		CMS_Hub.Send_Flag = 1;					//发送控制指令并且接收到回传指令之后 CMS_Hub.Send_Flag 被置位
		HAL_UART_Receive_DMA(&huart3, CMS_Hub.RecMas, REVE_BufLENGTH);
	}
  /* USER CODE END USART3_IRQn 1 */
}
