//没有写250焦耳的缓存控制！！！
#include "ChassisPowerBehaviour.h"
#include "system.h"

float BufferEnergy = 60; //底盘缓冲能量

float SupKp = 1;

//裁判系统得到数值
fp32 chassis_power_buffer = 0.0f;
fp32 chassis_power = 0.0f;

//功率计算
fp32 real_buffer_limit = 20;//可调整
fp32 real_power_limit = 0;
fp32 total_power = 0.0f;
fp32 total_powerKP = 2.80099994e-06;//可调整

void ChassisPowerloop();
void ChassisReduceRate();

//刷新功率数据
void ChassisPowerloop()
{
	chassis_power_buffer = power_heat_data_t.chassis_power_buffer;
	chassis_power = power_heat_data_t.chassis_power;
}

void ChassisReduceRate()
{
	//计算最终限制功率
	if(chassis_power_buffer < real_buffer_limit)
		real_power_limit = (chassis_power_buffer/real_buffer_limit)*real_buffer_limit;
	else
	{
		real_power_limit = robot_state.chassis_power_limit;
	}
	
	//计算拟合功率
	total_power = 0;
	for(uint32_t i=0;i<4;i++)
	{
		total_power += fabs(ChassisCtrl.Current[i] * ChassisCtrl.Motor[i]->speed_rpm)*total_powerKP;
	}
	
	//电容充电功率
	CMS.charge_limit = (real_power_limit - chassis_power)*100;
	if(chassis_power/real_power_limit > 0.8||chassis_power_buffer < 40) CMS.charge_limit = 0;
	if(CMS.RxOpen == 1) CMS.charge_limit = (robot_state.chassis_power_limit - 10)*100;
	if(CMS.charge_limit < 0) CMS.charge_limit = 0;
	
	//计算减幅系数
	if(CMS.Enable == 0 || CMS.RxOpen == 0)
	{
		//不使用电容
		if(total_power>real_power_limit)
		{
			SupKp = real_power_limit/total_power;
			
			if(ChassisCtrl.Mode == ROTING && (PTZ.FBSpeed == 0 && PTZ.LRSpeed == 0)) 
			{
				if(power_limit < 90 && power_limit > 58) SupKp*=1.2;
				else if(power_limit > 90) SupKp*=1.4;//可调整
			}
			else if(ChassisCtrl.Mode == ROTING)
			{
				if(power_limit < 90 && power_limit > 58) SupKp*=1.2;
				else if(power_limit > 90) SupKp*=1.6;//可调整
			}
			
			for(uint32_t i=0;i<4;i++)
			{
				ChassisCtrl.Current[i]*= SupKp;//可调整
			}
		}
	}
	else
	{
		//使用电容开环？
	}
	
}

void ChassisPowerControl()
{
	ChassisPowerloop();
	ChassisReduceRate();
}
