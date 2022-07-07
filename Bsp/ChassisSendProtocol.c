/*
* ��������Ҫ�����������λ��������������
*/
#include "ChassisSendProtocol.h"

uint8_t *send_power_heat_data(void);
uint8_t *send_bullet_speed(void);
uint8_t *send_bullet_limit(void);
uint8_t *send_power_limit(void);
uint8_t *send_robot_information(void);
uint8_t *send_enemy_information(void);


//���̹��ʺ�ǹ������
//��ʵʱ�ԣ����ռ���
uint8_t PowerHeatData[22];
uint8_t *send_power_heat_data(void)
{
	memset(PowerHeatData,0,sizeof(PowerHeatData));
	for(int i = 0;i<2;i++)
	{
		PowerHeatData[0+i*11] = (uint8_t)((0x120+i)>>8);
		PowerHeatData[1+i*11] = (uint8_t)(0x120+i);
		PowerHeatData[2+i*11] = 8;
	}
	memcpy(PowerHeatData+3,&power_heat_data_t.chassis_volt,8);
	memcpy(PowerHeatData+14,&power_heat_data_t.chassis_power_buffer,8);
	return PowerHeatData;
}
//ǹ������
uint8_t BulletSpeed[11];
uint8_t *send_bullet_speed(void)
{
	memset(BulletSpeed,0,sizeof(BulletSpeed));
	switch (shoot_data_t.shooter_id)
	{
		case 1://������Сǹ��0
		{
			BulletSpeed[0] = (uint8_t)(0x122>>8);
			BulletSpeed[1] = (uint8_t)(0x122);
			BulletSpeed[2] = 4;
			memcpy(BulletSpeed+3,&shoot_data_t.bullet_speed,4);
			break;
		}
		case 2://������Сǹ��1
		{
			BulletSpeed[0] = (uint8_t)(0x123>>8);
			BulletSpeed[1] = (uint8_t)(0x123);
			BulletSpeed[2] = 4;
			memcpy(BulletSpeed+3,&shoot_data_t.bullet_speed,4);
			break;
		}
		case 3://�����˴�ǹ��
		{
			BulletSpeed[0] = (uint8_t)(0x124>>8);
			BulletSpeed[1] = (uint8_t)(0x124);
			BulletSpeed[2] = 4;
			memcpy(BulletSpeed+3,&shoot_data_t.bullet_speed,4);
			break;
		}
	}
	return BulletSpeed;
}

//�����������
uint8_t BulletLimit[33];
uint8_t *send_bullet_limit(void)
{
	memset(BulletLimit,0,sizeof(BulletLimit));
	for(int i=0;i<3;i++)
	{
		BulletLimit[0+i*11] = (uint8_t)((0x125+i)>>8);
		BulletLimit[1+i*11] = (uint8_t)(0x125+i);
		BulletLimit[2+i*11] = 6;
	}
	memcpy((BulletLimit+3),&robot_state.shooter_id1_17mm_cooling_rate,6);
	memcpy((BulletLimit+14),&robot_state.shooter_id2_17mm_cooling_rate,6);
	memcpy((BulletLimit+25),&robot_state.shooter_id1_42mm_cooling_rate,6);
	return BulletLimit;
}
//���̹�������
uint8_t PowLimit[11];
uint8_t *send_power_limit(void)
{
	memset(PowLimit,0,sizeof(PowLimit));
	PowLimit[0] = (uint8_t)(0x128>>8);
	PowLimit[1] = (uint8_t)(0x128);
	PowLimit[2] = 2;
	memcpy(PowLimit+3,&robot_state.chassis_power_limit,2);
	return PowLimit;
}
//������������Ϣ
robot_information_t robot_information;
uint8_t RobotInformation[11];
uint8_t *send_robot_information(void)
{
	robot_information.robot_id = robot_state.robot_id;
	robot_information.power_output = (uint8_t)(((0x01 & robot_state.mains_power_shooter_output) << 2) |
											((0x01 & robot_state.mains_power_chassis_output) << 2) |
											((0x01 & robot_state.mains_power_gimbal_output) << 2));
	robot_information.remain_HP = robot_state.remain_HP;
	robot_information.max_HP = robot_state.max_HP;
	memset(RobotInformation,0,sizeof(RobotInformation));
	RobotInformation[0] = (uint8_t)(0x129>>8);
	RobotInformation[1] = (uint8_t)(0x129);
	RobotInformation[2] = 6;
	memcpy(RobotInformation+3,&robot_information,6);
	return RobotInformation;
}

//���͵з�Ѫ��
enemy_information_t enemy_information;
uint8_t EnemyInformation[11];
uint8_t *send_enemy_information(void)
{
	if(robot_state.robot_id & 0x100)//����
	{
		enemy_information.hero_remain_HP = game_robot_HP_t.red_1_robot_HP;
		enemy_information.infantry3_remain_HP = game_robot_HP_t.red_3_robot_HP;
		enemy_information.infantry4_remain_HP = game_robot_HP_t.red_4_robot_HP;
		enemy_information.infantry5_remain_HP = game_robot_HP_t.red_5_robot_HP;
	}
	else
	{
		enemy_information.hero_remain_HP = game_robot_HP_t.red_1_robot_HP;
		enemy_information.infantry3_remain_HP = game_robot_HP_t.blue_3_robot_HP;
		enemy_information.infantry4_remain_HP = game_robot_HP_t.blue_4_robot_HP;
		enemy_information.infantry5_remain_HP = game_robot_HP_t.blue_5_robot_HP;
	}
	memset(EnemyInformation,0,sizeof(EnemyInformation));
	EnemyInformation[0] = (uint8_t)(0x12A >> 8);
	EnemyInformation[1] = (uint8_t)(0x12A);
	EnemyInformation[2] = 8;
	memcpy((EnemyInformation+3),&enemy_information,8);
	return EnemyInformation;
}

//���ͱ���״̬��Ϣ
uint8_t GAMESTATUS[11];
send_game_status_t SendGameStatus;
uint8_t *send_game_status(void)
{
	SendGameStatus.game_status = game_state.game_type<<4;
	SendGameStatus.game_status |= game_state.game_progress;
	SendGameStatus.end_time = game_state.stage_remain_time;
	
	memset(GAMESTATUS,0,sizeof(GAMESTATUS));
	GAMESTATUS[0] = (uint8_t)(0x12B >> 8);
	GAMESTATUS[1] = (uint8_t)(0x12B);
	GAMESTATUS[2] = 3;
	memcpy((GAMESTATUS+3),&SendGameStatus,3);
	return GAMESTATUS;
}
