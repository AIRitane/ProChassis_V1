#ifndef __CMS_INTERFACE_H
#define __CMS_INTERFACE_H

#include "main.h"
#include "mbrtumaster.h"
#include "stm32f4xx_hal.h"

//Holding Register Mapping*******************************************************

		
		//CMS Auto Protection Data
		#define HR_CMS_INPUT_MAX_Voltage  		46		//���������ѹ
		#define HR_CMS_INPUT_MIN_Voltage  		47		//�������С��ѹ
		
		#define HR_CMS_Buck_MAX_Current  			48		//buck��������
		#define HR_CMS_Boost_MAX_Current  		49		//boost��������
		#define HR_CMS_Direct_MAX_Current  		50		//��Դ�����������
		#define HR_CMS_Chassis_MAX_Current  	51		//���̵�������
		
		#define HR_CMS_CAPACITOR_MAX_Voltage	52	//��������ѹ
		#define	HR_CMS_OUTPUT_MAX_Voltage			53	//�������ѹ
		
		//CMS Power Routing Mode Data
		#define HR_CMS_PowerRouting							56	//��Դ·��ѡ��
			#define CMS_PR_BattDirect						0	//1�����ֱ�ӹ���
			#define CMS_PR_BuckBoost						1	//2��buckboost mode
			#define CMS_PR_IdealDiode 						2	//3�����������
			#define CMS_PR_Shutdown							3	//4���ر�
			#define CMS_PR_Automatic						4	//5���Զ�ģʽ ��δ���������ر���
		
		//CMS User Data
		#define HR_CMS_Cap_Voltage_Limit			57	//���ݵ�ѹ���ƣ������ֵ��ʲô���𣿣�
		#define HR_CMS_Charging_Power					58	//��繦��
		#define HR_CMS_Cap_Current_Limit			61	//���ݵ������ƣ������ֵ��ʲô���𣿣�
		#define HR_CMS_Boost_Output_Voltage		62	//boost����Ŀ�������ѹ
		

			

//Input Status Mapping************************************************************************
		
//���󾯱�
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
		
		#define Diagnostics_OK				26	//�Լ�ɹ�
		#define Calibration_OK				27	//У���ɹ�
		


//Input Registers Mapping***************************************************************************************

		//Original Measurement Data
		#define INPUT_VIN			0	//�����ѹ
		#define INPUT_CBKL		1	//buck����
		#define INPUT_CDIR		2	//���ֱ���������
		#define INPUT_VCAP		3	//���ݵ�ѹ
		#define INPUT_VBST		4	//boost��·�����ѹ
		#define INPUT_CBSTL		5	//boost �˵���
		#define INPUT_CCHS		6	//���̵���
		#define INPUT_VOUT		7	//���̵�ѹ

		//Calibrated Measurement Data
		#define INPUT_VIN_CAL			8		//�����ѹ
		#define INPUT_CBKL_CAL		9		//����buck����
		#define INPUT_CDIR_CAL		10	//����ֱ����ص���
		#define INPUT_VCAP_CAL		11	//���ݵ�ѹ
		#define INPUT_VBST_CAL		12	//boost�����ѹ
		#define INPUT_CBSTL_CAL		13	//boost��������
		#define INPUT_CCHS_CAL		14	//�����������
		#define INPUT_VOUT_CAL		15	//���������ѹ
		
		//Functional Data
//		#define INPUT_ChargingPower 16
//		#define INPUT_ChassisPower 	17	//Ҫ��Ļ�����loadUtility������
		
//		#define INPUT_Buck_LoadUtility 		18	//������
//		#define	INPUT_Boost_LoadUtility 	19
		
		#define Malfunction_Code					20		//������

		#define USART1_TXBUF_LENGTH 			300
		#define USART1_RXBUF_LENGTH				300
		#define REVE_BufLENGTH						100


//Coil Status Mapping**********************************************************************************************

		#define Coil_SaveCFG 	0   
		#define Coil_LoadCFG	1
		
		#define	Power_EN		2
		

typedef struct{
  UART_HandleTypeDef *huart;						//ר������CMSͨ�ŵĴ���
	RTULocalMasterType *LocalMaster;				//CMS����Modbus����
	ObjAccessInfo *LocaInfo;						//����Modbus������ͨ�����ݶ���
	RTUAccessedSlaveType *LocalSlave;				//��������������������
	DMA_HandleTypeDef *CMS_DMA_Rx;					//ר������CMS��DMAͨ�Žӿ�
	DMA_HandleTypeDef *CMS_DMA_Tx;

	uint16_t LimitCurrent;							//��̬������
	uint8_t MassageLength;							//Modbusͨ�ŷ�����Ϣ�ĳ���
	uint8_t TxMas[8];								//Modbus������Ϣ������
	uint8_t RecMas[REVE_BufLENGTH];				    //Modbus������Ϣ������
	int Send_Flag;									//ָ��ͱ�־λ
	int SwitchCount;								//ָ���鷢��ת����־λ
	float CMS_ChagingPower;							//CMS��繦��
	float CMS_InputPower;							//CMS���빦��
	uint16_t Route_ref;									//��ǰ���·��
	uint8_t  power_routin;
	
	
	
	
	void (*Next_Command)();							//��һ������ָ��
	void (*Pred_Command)();							//��һ������ָ��
	
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

void USER_OpenPower(void);  //���ݳ��
void USER_SwitchPowerRoute(int *Mode);  //���|����

void USER_SwitchSutdown(void);
void USER_SwitchBattDirec(void);
void USER_SwitchBuckBoost(void);
void USER_ClosePower(void);     //ֹͣ���ݳ��
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
