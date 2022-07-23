#include "ChassisTask.h"
#include "system.h"

ChassisCtrl_t ChassisCtrl;
float XYPid[4][3]={{20000,0,0},
					{20000,0,0},
					{20000,0,0},
					{20000,0,0}};
float WZPid[3] = {0.005,0.0000,0};

int16_t power_limit;
uint32_t CMS_COUNTER = 0;
uint32_t LAST_CMS_COUNTER = 0;
BufferFunction_t BufferFunctionX;
BufferFunction_t BufferFunctionY;
BufferFunction_t BufferFunctionWZ;

void BufferFunctionInit(BufferFunction_t *BufferFunction,fp32 frame_period);
void BufferFunctionCalc(BufferFunction_t *BufferFunction,fp32 input);

void ChassisInit();
void ChassisSetmode();
void ChassisDataLoop();
void Choose_CMS();
void Chassis_Close_ContolSet();
void Chassis_Open_ContolSet();
void ChassisControlLoop();

void ChassisTask(void const * argument)
{
	ChassisInit();
	
	while(1)
	{
		CMS_COUNTER++;
		ChassisSetmode();
		ChassisDataLoop();
		Choose_CMS();
		if(CMS.RxOpen == 0)
			Chassis_Close_ContolSet();
		else Chassis_Open_ContolSet();
		
		ChassisControlLoop();
		ChassisPowerControl();
		Motor_Send(ChassisCtrl.Current[0], ChassisCtrl.Current[1], ChassisCtrl.Current[2], ChassisCtrl.Current[3]);
	
		CMS_Referee_Send((uint16_t)CMS.charge_limit , CMS.TxOpen);
		
		osDelay(1);
	}
}

void ChassisInit()
{
	for(int i=0;i<4;i++) 
	{
		ChassisCtrl.Current[i] = 0;
		ChassisCtrl.WheelSpeed[i] = 0;
		ChassisCtrl.Motor[i] = GetChassisMeasure(i);
		PID_init(&ChassisCtrl.XYPid[i], PID_POSITION, XYPid[i], 16300, 5000);
	}

	ChassisCtrl.Yaw = GetYawMeasure();
	PID_init(&ChassisCtrl.WZPid,PID_POSITION,WZPid,3,1);
	ChassisCtrl.Mode = NOFORCE;
	BufferFunctionInit(&BufferFunctionX,150);
	BufferFunctionInit(&BufferFunctionY,150);
	BufferFunctionInit(&BufferFunctionWZ,500);
	CMS.Enable = 0;
	CMS.RxOpen = 0;
	CMS.TxOpen = 0;
	CMS.Electricity = 0;
	CMS.charge_limit = 0;
	CMS.Power = 0;
	
	power_limit = robot_state.chassis_power_limit;
}

void ChassisDataLoop()
{
	power_limit = robot_state.chassis_power_limit;
	if(power_limit == 0) power_limit = 50;
	
	//判断当前CMS状态
	CMS_STATUS();
}

void ChassisSetmode()
{
	switch(PTZ.ChassisStatueRequest)
	{
		case 0x01:
			ChassisCtrl.Mode = NOFORCE; break;
		case 0x12:
		case 0x92:
			ChassisCtrl.Mode = ROTING; break;
		case 0x0A:
		case 0x8A:
			ChassisCtrl.Mode = FALLOW; break;
		case 0x06:
		case 0x86:
			ChassisCtrl.Mode = STOP; break;
		case 0x42:
		case 0xB2:
			ChassisCtrl.Mode = SLOW; break;
		default:
			break;
	}
}
fp32 rote_powkp = 0;
fp32 Erro_angle = 0;
float del = 0;

float test = 0;

void SET_ERROR_ANGLE(float *Erro_angle,float power_45,float power_50,float power_55,float power_60,float power_80,float power_100,float power_else)
{
	switch(power_limit)
	{
		case 45:
			*Erro_angle = power_45; break;
		case 50:
			*Erro_angle = power_50; break;
		case 55:
			*Erro_angle = power_55; break;
		case 60:
			*Erro_angle = power_60; break;
		case 80:
			*Erro_angle = power_80; break;
		case 100:
			*Erro_angle = power_100; break;
		default:
			*Erro_angle = 0; break; 
		
	}
}

void SET_KP_ROTING(float kp,float roting_stop,float roting_run)
{
	rote_powkp = kp;
	if(PTZ.FBSpeed !=0 || PTZ.LRSpeed !=0)
	{
		ChassisCtrl.wz = roting_run;
	}
	else
	{
		ChassisCtrl.wz = roting_stop;
	}
}

float test2 = 0.85;
void Chassis_Close_ContolSet()
{
	
	if(ChassisCtrl.Mode == ROTING)
	{
		//偏差角度
		SET_ERROR_ANGLE(&Erro_angle,0,0,0,0,0,0,0);//经过起伏路段测试，通过操作手检验偏差可接受
		//设置速度等级
		switch(power_limit)
		{
			case 45:
				SET_KP_ROTING(0.5,0.9,0.8); break;
			case 50:
				SET_KP_ROTING(0.6,0.96,0.74); break;
			case 55:
				SET_KP_ROTING(0.6,1.03,0.9); break;
			case 60:
				SET_KP_ROTING(0.6,1.15,0.97); break;
			case 80:
				SET_KP_ROTING(0.8,1.3,1); break;//此处静止状态下小陀螺最高参数1.4，操作手要求放低为1.3  
			case 100:
				SET_KP_ROTING(0.8,1.5,1.1); break;//此处静止状态下小陀螺最高参数1.62，操作手要求放低为1.5  
			default:
				SET_KP_ROTING(0.5,0.9,0.8); break;
		}
	}
		
	else if(ChassisCtrl.Mode == FALLOW || ChassisCtrl.Mode == STOP)
	{
		if(PTZ.FBSpeed > 0)
			SET_ERROR_ANGLE(&Erro_angle,0,0,0,0,0,0,0);
		else
			SET_ERROR_ANGLE(&Erro_angle,0,0,0,0,0,0,0);
		
		switch(power_limit)
		{
			case 45:
				SET_KP_ROTING(1.4,0,0); break;
			case 50:
				SET_KP_ROTING(1.5,0,0); break;
			case 55:
				SET_KP_ROTING(1.55,0,0); break;
			case 60:
				SET_KP_ROTING(2.0,0,0); break;
			case 80:
				SET_KP_ROTING(2.5,0,0); break;
			case 100:
				SET_KP_ROTING(3,0,0); break;
			default:
				SET_KP_ROTING(1.4,0,0); break;
		}
		ChassisCtrl.wz = PID_calc(&ChassisCtrl.WZPid,10*theta_format(ChassisCtrl.Yaw->angle-FallowAngle),0);
	}
	else if(ChassisCtrl.Mode == SLOW)
	{
		SET_ERROR_ANGLE(&Erro_angle,test,0,0,0,0,0,0);
		SET_KP_ROTING(0.5,0,0);//慢速模式未调试
	}
		
	del = FallowAngle - ChassisCtrl.Yaw->angle + Erro_angle;
	
	//缓启动是否有必要有待测试
	BufferFunctionCalc(&BufferFunctionX,PTZ.FBSpeed/32767.f);
	BufferFunctionCalc(&BufferFunctionY,-PTZ.LRSpeed/32767.f);
	
	BufferFunctionX.out = PTZ.FBSpeed/32767.f;//注释测试
	BufferFunctionY.out = -PTZ.LRSpeed/32767.f;//注释测试
	
	ChassisCtrl.vx = -BufferFunctionX.out * cos(del/180*PI) + BufferFunctionY.out * sin(del/180*PI);
	ChassisCtrl.vy = BufferFunctionX.out * sin(del/180*PI) + BufferFunctionY.out * cos(del/180*PI);

	ChassisCtrl.vx *= rote_powkp;
	ChassisCtrl.vy *= rote_powkp;
}

void Chassis_Open_ContolSet()
{
	if(ChassisCtrl.Mode == ROTING)
	{
		//偏差角度
		SET_ERROR_ANGLE(&Erro_angle,0,0,0,0,0,0,0);//经过起伏路段测试，通过操作手检验偏差可接受
		switch(power_limit)
		{
			case 45:
				SET_KP_ROTING(3,0,0); break;
			case 50:
				SET_KP_ROTING(1,2,1.2); break;
			case 55:
				SET_KP_ROTING(3,0,0); break;
			case 60:
				SET_KP_ROTING(1,2,1.3); break;
			case 80:
				SET_KP_ROTING(1,2,1.6); break;
			case 100:
				SET_KP_ROTING(1.3,2.1,1.7); break;
			default:
				SET_KP_ROTING(3,0,0); break;
		}
	}
	else if(ChassisCtrl.Mode == FALLOW || ChassisCtrl.Mode == STOP)
	{
		if(PTZ.FBSpeed > 0)
			SET_ERROR_ANGLE(&Erro_angle,0,0,0,0,0,0,0);
		else
			SET_ERROR_ANGLE(&Erro_angle,0,0,0,0,0,0,0);
		switch(power_limit)
		{
			case 45:
				SET_KP_ROTING(3,0,0); break;
			case 50:
				SET_KP_ROTING(3,0,0); break;
			case 55:
				SET_KP_ROTING(3,0,0); break;
			case 60:
				SET_KP_ROTING(3,0,0); break;
			case 80:
				SET_KP_ROTING(3,0,0); break;
			case 100:
				SET_KP_ROTING(3,0,0); break;
			default:
				SET_KP_ROTING(3,0,0); break;
			SET_ERROR_ANGLE(&Erro_angle,0,0,0,0,0,0,0);
		}
		ChassisCtrl.wz = PID_calc(&ChassisCtrl.WZPid,10*theta_format(ChassisCtrl.Yaw->angle-FallowAngle),0);
	}
	else if(ChassisCtrl.Mode == SLOW)
	{
		SET_ERROR_ANGLE(&Erro_angle,test,0,0,0,0,0,0);
		SET_KP_ROTING(0.5,0,0);//慢速模式未调试
	}
		
	del = FallowAngle - ChassisCtrl.Yaw->angle + Erro_angle;
	
	//电容模式有必要缓启动
	BufferFunctionCalc(&BufferFunctionX,PTZ.FBSpeed/32767.f);
	BufferFunctionCalc(&BufferFunctionY,-PTZ.LRSpeed/32767.f);

	ChassisCtrl.vx = -BufferFunctionX.out * cos(del/180*PI) + BufferFunctionY.out * sin(del/180*PI);
	ChassisCtrl.vy = BufferFunctionX.out * sin(del/180*PI) + BufferFunctionY.out * cos(del/180*PI);

	ChassisCtrl.vx *= rote_powkp;
	ChassisCtrl.vy *= rote_powkp;
}

void Choose_CMS()
{
	//选择电路
	if((PTZ.ChassisStatueRequest & 0x80) && CMS.Electricity > 1200)
	{
		CMS.TxOpen = 1;
		LAST_CMS_COUNTER = CMS_COUNTER;
	}
	else if(CMS_COUNTER - LAST_CMS_COUNTER > 250)
	{
		CMS.TxOpen = 0;
		CMS.RxOpen = 0;
	}
	else 
	{
		CMS.TxOpen = 1;
		CMS.RxOpen = 0;
	}
}
void ChassisControlLoop()
{
	if(ChassisCtrl.Mode == NOFORCE)
	{
		for(int i = 0;i<4;i++)
			ChassisCtrl.Current[i] = 0;
		return;
	}
	
	else if(ChassisCtrl.Mode == STOP)
	{
		memset(ChassisCtrl.WheelSpeed,0,sizeof(ChassisCtrl.WheelSpeed));
		for(int i =0;i<4;i++)
		{
			ChassisCtrl.Current[i] =  PID_calc(&ChassisCtrl.XYPid[i], ChassisCtrl.Motor[i]->speed_rpm * 0.000415809748903494517209f, ChassisCtrl.WheelSpeed[i]);
		}
	}
	else if(ChassisCtrl.Mode == FALLOW || ChassisCtrl.Mode == ROTING)
	{
		ChassisCtrl.WheelSpeed[0] = -ChassisCtrl.vx - ChassisCtrl.vy + ChassisCtrl.wz;
		ChassisCtrl.WheelSpeed[1] = ChassisCtrl.vx - ChassisCtrl.vy +  ChassisCtrl.wz;
		ChassisCtrl.WheelSpeed[2] = ChassisCtrl.vx + ChassisCtrl.vy + 	ChassisCtrl.wz;
		ChassisCtrl.WheelSpeed[3] = -ChassisCtrl.vx + ChassisCtrl.vy + ChassisCtrl.wz;
		 
		for(int i =0;i<4;i++)
		{
			ChassisCtrl.Current[i] =  PID_calc(&ChassisCtrl.XYPid[i], ChassisCtrl.Motor[i]->speed_rpm * 0.000415809748903494517209f, ChassisCtrl.WheelSpeed[i]);
		}
	}
}


void BufferFunctionInit(BufferFunction_t *BufferFunction,fp32 frame_period)
{
	BufferFunction->frame_period = frame_period;
	BufferFunction->input = 0;
	BufferFunction->out = 0;
	BufferFunction->error = 0;
	BufferFunction->buffer = 0;
}

void BufferFunctionCalc(BufferFunction_t *BufferFunction,fp32 input)
{
	BufferFunction->input = input;
	BufferFunction->error = BufferFunction->input - BufferFunction->out;
	if(BufferFunction->error >= 0)
	{
		BufferFunction->buffer = BufferFunction->error / (exp (0.7*BufferFunction->error));//0.7越小越往上飞
		BufferFunction->out += BufferFunction->buffer / BufferFunction->frame_period;
	}
	else
	{
		BufferFunction->buffer = fabs(BufferFunction->error) / (exp (0.7*fabs(BufferFunction->error)));//0.7越小越往上飞
		BufferFunction->out -= BufferFunction->buffer / BufferFunction->frame_period;
	}
}
