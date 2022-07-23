//û��д250�����Ļ�����ƣ�����
#include "ChassisPowerBehaviour.h"
#include "system.h"

float BufferEnergy = 60; //���̻�������

float SupKp = 1;

//����ϵͳ�õ���ֵ
fp32 chassis_power_buffer = 0.0f;
fp32 chassis_power = 0.0f;

//���ʼ���
fp32 real_buffer_limit = 20;//�ɵ���
fp32 real_power_limit = 0;
fp32 total_power = 0.0f;
fp32 total_powerKP = 2.80099994e-06;//�ɵ���

void ChassisPowerloop();
void ChassisReduceRate();

//ˢ�¹�������
void ChassisPowerloop()
{
	chassis_power_buffer = power_heat_data_t.chassis_power_buffer;
	chassis_power = power_heat_data_t.chassis_power;
}

void ChassisReduceRate()
{
	//�����������ƹ���
	if(chassis_power_buffer < real_buffer_limit)
		real_power_limit = (chassis_power_buffer/real_buffer_limit)*real_buffer_limit;
	else
	{
		real_power_limit = robot_state.chassis_power_limit;
	}
	
	//������Ϲ���
	total_power = 0;
	for(uint32_t i=0;i<4;i++)
	{
		total_power += fabs(ChassisCtrl.Current[i] * ChassisCtrl.Motor[i]->speed_rpm)*total_powerKP;
	}
	
	//���ݳ�繦��
	CMS.charge_limit = (real_power_limit - chassis_power)*100;
	if(chassis_power/real_power_limit > 0.8||chassis_power_buffer < 40) CMS.charge_limit = 0;
	if(CMS.RxOpen == 1) CMS.charge_limit = (robot_state.chassis_power_limit - 10)*100;
	if(CMS.charge_limit < 0) CMS.charge_limit = 0;
	
	//�������ϵ��
	if(CMS.Enable == 0 || CMS.RxOpen == 0)
	{
		//��ʹ�õ���
		if(total_power>real_power_limit)
		{
			SupKp = real_power_limit/total_power;
			
			if(ChassisCtrl.Mode == ROTING && (PTZ.FBSpeed == 0 && PTZ.LRSpeed == 0)) 
			{
				if(power_limit < 90 && power_limit > 58) SupKp*=1.2;
				else if(power_limit > 90) SupKp*=1.4;//�ɵ���
			}
			else if(ChassisCtrl.Mode == ROTING)
			{
				if(power_limit < 90 && power_limit > 58) SupKp*=1.2;
				else if(power_limit > 90) SupKp*=1.6;//�ɵ���
			}
			
			for(uint32_t i=0;i<4;i++)
			{
				ChassisCtrl.Current[i]*= SupKp;//�ɵ���
			}
		}
	}
	else
	{
		//ʹ�õ��ݿ�����
	}
	
}

void ChassisPowerControl()
{
	ChassisPowerloop();
	ChassisReduceRate();
}
