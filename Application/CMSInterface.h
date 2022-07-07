#ifndef __CMS_INTERFACE_H
#define __CMS_INTERFACE_H

#include "main.h"
#include "mbrtumaster.h"
#include "stm32f4xx_hal.h"

//Holding Register Mapping*******************************************************

		
		//CMS Auto Protection Data
		#define HR_CMS_INPUT_MAX_Voltage  		46		//输入的最大电压
		#define HR_CMS_INPUT_MIN_Voltage  		47		//输入的最小电压
		
		#define HR_CMS_Buck_MAX_Current  			48		//buck的最大电流
		#define HR_CMS_Boost_MAX_Current  		49		//boost的最大电流
		#define HR_CMS_Direct_MAX_Current  		50		//电源输入的最大电流
		#define HR_CMS_Chassis_MAX_Current  	51		//底盘的最大电流
		
		#define HR_CMS_CAPACITOR_MAX_Voltage	52	//电容最大电压
		#define	HR_CMS_OUTPUT_MAX_Voltage			53	//输出最大电压
		
		//CMS Power Routing Mode Data
		#define HR_CMS_PowerRouting							56	//电源路径选择
			#define CMS_PR_BattDirect						0	//1、电池直接供电
			#define CMS_PR_BuckBoost						1	//2、buckboost mode
			#define CMS_PR_IdealDiode 						2	//3、理想二极管
			#define CMS_PR_Shutdown							3	//4、关闭
			#define CMS_PR_Automatic						4	//5、自动模式 尚未开发，被关闭了
		
		//CMS User Data
		#define HR_CMS_Cap_Voltage_Limit			57	//电容电压限制（与最大值有什么区别？）
		#define HR_CMS_Charging_Power					58	//充电功率
		#define HR_CMS_Cap_Current_Limit			61	//电容电流限制（与最大值有什么区别？）
		#define HR_CMS_Boost_Output_Voltage		62	//boost级的目标输出电压
		

			

//Input Status Mapping************************************************************************
		
//错误警报
		#define VIN_OVLO_Warning			14	//over voltage lock
		#define VIN_UVLO_Warning			15	//under voltgage lock
		
		#define	VCAP_OVLO_Warning			16	//over voltage lock
		#define VOUT_OVLO_Warning			17	//over voltage lock
		
		#define	CBKL_OCLO_Warning			18	//over current lock
		#define	CBSTL_OCLO_Warning		19
		#define	CDIR_OCLO_Warning			20
		#define	CCHS_OCLO_Warning			21
		
		#define IN_OPLO_Warning				22	//over power lock
		#define OUT_OPLO_Warning			23
		
		#define BUCK_OLLO_Warning			24	//over load lock
		#define BOOST_OLLO_Warning		25
		
		#define Diagnostics_OK				26	//自检成功
		#define Calibration_OK				27	//校正成功
		


//Input Registers Mapping***************************************************************************************

		//Original Measurement Data
		#define INPUT_VIN			0	//输入电压
		#define INPUT_CBKL		1	//buck电流
		#define INPUT_CDIR		2	//电池直接输入电流
		#define INPUT_VCAP		3	//电容电压
		#define INPUT_VBST		4	//boost电路输出电压
		#define INPUT_CBSTL		5	//boost 端电流
		#define INPUT_CCHS		6	//底盘电流
		#define INPUT_VOUT		7	//底盘电压

		//Calibrated Measurement Data
		#define INPUT_VIN_CAL			8		//输入电压
		#define INPUT_CBKL_CAL		9		//输入buck电流
		#define INPUT_CDIR_CAL		10	//输入直供电池电流
		#define INPUT_VCAP_CAL		11	//电容电压
		#define INPUT_VBST_CAL		12	//boost输出电压
		#define INPUT_CBSTL_CAL		13	//boost工作电流
		#define INPUT_CCHS_CAL		14	//底盘输出电流
		#define INPUT_VOUT_CAL		15	//底盘输出电压
		
		//Functional Data
//		#define INPUT_ChargingPower 16
//		#define INPUT_ChassisPower 	17	//要算的话就在loadUtility那里算
		
//		#define INPUT_Buck_LoadUtility 		18	//负载率
//		#define	INPUT_Boost_LoadUtility 	19
		
		#define Malfunction_Code					20		//故障码

		#define USART1_TXBUF_LENGTH 			300
		#define USART1_RXBUF_LENGTH				300
		#define REVE_BufLENGTH						100


//Coil Status Mapping**********************************************************************************************

		#define Coil_SaveCFG 	0   
		#define Coil_LoadCFG	1
		
		#define	Power_EN		2
		

typedef struct{
  UART_HandleTypeDef *huart;						//专门用于CMS通信的串口
	RTULocalMasterType *LocalMaster;				//CMS本地Modbus主机
	ObjAccessInfo *LocaInfo;						//本地Modbus主机的通信数据对象
	RTUAccessedSlaveType *LocalSlave;				//本地主机定义的虚拟对象
	DMA_HandleTypeDef *CMS_DMA_Rx;					//专门用于CMS的DMA通信接口
	DMA_HandleTypeDef *CMS_DMA_Tx;

	uint16_t LimitCurrent;							//动态充电电流
	uint8_t MassageLength;							//Modbus通信发送消息的长度
	uint8_t TxMas[8];								//Modbus发送消息数据组
	uint8_t RecMas[REVE_BufLENGTH];				    //Modbus接收消息数据组
	int Send_Flag;									//指令发送标志位
	int SwitchCount;								//指令组发送转换标志位
	float CMS_ChagingPower;							//CMS充电功率
	float CMS_InputPower;							//CMS输入功率
	uint16_t Route_ref;									//当前充电路径
	uint8_t  power_routin;
	
	
	
	
	void (*Next_Command)();							//下一条发送指令
	void (*Pred_Command)();							//上一条发送指令
	
}CMS_Handler;

void APP_SwitchCommand(void (*Pred_Command),void (*Next_Command));
void APP_BatteryCombineBuckBoost(float limitInputPower,int OperationMode);  
void APP_BatteryCombineBuckBoost2(void);

void USER_CMS_init(UART_HandleTypeDef *huart,CMS_Handler *CMS,DMA_HandleTypeDef *Tx,DMA_HandleTypeDef *Rx);  //
void USER_ReadErrorCode(void);  			//
void USER_ReadPowerState(void);     	//
void USER_ReadInputCurrent(void);
void USER_ReadInputVoltage(void);
void USER_ReadOuputVoltage(void);
void USER_ReadOuputCurrent(void);
void USER_ReadPowerRoute(void);
void USER_ReadBuckCurrent(void);
void USER_ReadCapVoltage(void);
void USER_ReadCurrentLimt(void);

void USER_OpenPower(void);  //电容充电
void USER_SwitchPowerRoute(int *Mode);  //电池|电容

void USER_SwitchSutdown(void);
void USER_SwitchBattDirec(void);
void USER_SwitchBuckBoost(void);
void USER_ClosePower(void);     //停止电容充电
void USER_SetLimitCurrent(uint16_t Aim_Current);

void UpdateCoilStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue);  //
void UpdateInputStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue);
void UpdateHoldingRegister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);
void UpdateInputResgister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);

void WriteCommand(FunctionCode FunCode,uint16_t Address,void *datalist);  
void ReadCommand(FunctionCode FunCode,uint16_t Address,int ReceLength);

extern CMS_Handler CMS_Hub;
extern uint16_t Route;
extern float CapChageVoltage;

extern void CMS_usart_init(void);
#endif
