#include "ChassisPowerBehaviour.h"
#include "system.h"

float BufferEnergy = 60; //���̻�������

float SupKp = 1;

//����ϵͳ�õ���ֵ
fp32 chassis_volt = 0;   //��λ��V
fp32 chassis_power_buffer = 0.0f;


//���ʼ���
fp32 real_buffer_limit = 20;//�ɵ���
fp32 real_power_limit = 0;	//���ɵ���
fp32 total_power = 0.0f;
fp32 total_powerKP = 2.80099994e-06;

extern ChassisCtrl_t ChassisCtrl;


void ChassisPowerloop();
void ChassisReduceRate();

//ˢ�¹�������
void ChassisPowerloop()
{
	chassis_power_buffer = power_heat_data_t.chassis_power_buffer;
}

void ChassisReduceRate()
{
	//�����������ƹ���
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
	
	//������Ϲ���
	total_power = 0;
	for(uint32_t i=0;i<4;i++)
	{
		total_power += fabs(ChassisCtrl.Current[i] * ChassisCtrl.Motor[i]->speed_rpm)*total_powerKP;
	}
	

	
	//�������ϵ��
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
