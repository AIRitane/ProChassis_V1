#include "ChassisPowerBehaviour.h"
#include "system.h"

float BufferEnergy = 60; //底盘缓冲能量

float SupKp = 1;

//裁判系统得到数值
fp32 chassis_volt = 0;   //单位是V
fp32 chassis_power_buffer = 0.0f;


//功率计算
fp32 real_buffer_limit = 20;//可调整
fp32 real_power_limit = 0;	//不可调整
fp32 total_power = 0.0f;
fp32 total_powerKP = 2.80099994e-06;

extern ChassisCtrl_t ChassisCtrl;


void ChassisPowerloop();
void ChassisReduceRate();

//刷新功率数据
void ChassisPowerloop()
{
	chassis_power_buffer = power_heat_data_t.chassis_power_buffer;
}

void ChassisReduceRate()
{
	//计算最终限制功率
	if(chassis_power_buffer < real_buffer_limit)
		real_power_limit = (chassis_power_buffer/real_buffer_limit)*real_buffer_limit;
	else
	{
		if(robot_level  == 1)
		{
			real_power_limit = 60;
		}
		else if(robot_level  == 2)
		{
			real_power_limit = 80;
		}
		else if(robot_level  == 3)
		{
			real_power_limit = 100;
		}
		else
		{
			real_power_limit = 60;
		}
	}
	
	//计算拟合功率
	total_power = 0;
	for(uint32_t i=0;i<4;i++)
	{
		total_power += fabs(ChassisCtrl.Current[i] * ChassisCtrl.Motor[i]->speed_rpm)*total_powerKP;
	}
	

	
	//计算减幅系数
	if(total_power>real_power_limit)
	{
		SupKp = real_power_limit/total_power;
		
		if(ChassisCtrl.Mode == ROTING && (PTZ.FBSpeed == 0 && PTZ.LRSpeed == 0)) 
		{
			if(power_limit < 90 && power_limit > 58) SupKp*=1.2;
			else if(power_limit > 90) SupKp*=1.4;
		}
		else if(ChassisCtrl.Mode == ROTING)
		{
			if(power_limit < 90 && power_limit > 58) SupKp*=1.2;
			else if(power_limit > 90) SupKp*=1.6;
		}			
		
		for(uint32_t i=0;i<4;i++)
		{
			ChassisCtrl.Current[i]*=SupKp;
		}
	}
}

void ChassisPowerControl()
{
	ChassisPowerloop();
	ChassisReduceRate();
}
