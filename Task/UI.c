#include "RefereeTask.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "CRC8_CRC16.h"
#include "fifo.h"
#include "bsp_referee.h"
#include "RefereeBehaviour.h"
#include "RM_Cilent_UI.h"
#include "CanReceiveDecom.h"
#include "ChassisTask.h"

//需要添加UI慢速
//需要添加底盘停止的UI 
Graph_Data deng1,kuang,deng2,deng3,deng4,fu,imagex,imagex1,imagex2,imagex3,imagey,imagey1,imagey2,imagey3,imagey4;
String_Data bullet,bullet3,DAFU,Abuff,Pbuff,Cbuff,state,ZIMIAO,dafustate,zimiaostate,dafustate1,zimiaostate1,state4;
Float_Data capacityD,Min,Sec;
int Time=0,M=10,S=0;
int BTime=0,BM=10,BS=0;
Float_Data BMin,BSec;

Graph_Data Frame;//自瞄框
//准星横线
Graph_Data Collimation_1;
Graph_Data Collimation_2;
Graph_Data Collimation_3;
Graph_Data Collimation_4;
//准星数线
Graph_Data Collimation_5;
//准心距离线
Graph_Data Collimation_6;
Graph_Data Collimation_7;
Graph_Data Collimation_8;
Graph_Data Collimation_9;
Graph_Data Collimation_10;
Graph_Data Collimation_11;

String_Data Capcity,HitRune,AiMBot;
Float_Data CapData;
String_Data HitRuneStatus,AiMBotStatus;

String_Data BulletCover,ChassisStatue;
Graph_Data BulletCircle,ChassisStatueCircle;

String_Data ChassisMove;
Graph_Data ChassisMoveCircle;

String_Data s_rune,b_rune;

uint32_t flash_time = 0;
uint8_t s_rune_flag = 0;
uint8_t b_rune_flag = 0;
int s_time_rune;
int b_time_rune;

extern float CapChageVoltage;


void UI(void const * argument)
{
	 while(1)
    {
			flash_time++;
			//1000毫秒添加一次，需要慢速
			//图层分配： 0为静止图层 1：为ui刷新图层 2:为字符刷新图层
			if(flash_time%10 == 0)
			{
				//框
				Rectangle_Draw(&Frame,"fra",UI_Graph_ADD,0,UI_Color_White,1,6750,380,7460,700);
				
				//准星
				Line_Draw(&Collimation_1,"co1",UI_Graph_ADD,0,UI_Color_Green,2,7010,500,7200,500);
				Line_Draw(&Collimation_2,"co2",UI_Graph_ADD,0,UI_Color_Green,2,7030,460,7180,460);
				Line_Draw(&Collimation_3,"co3",UI_Graph_ADD,0,UI_Color_Green,2,7050,420,7160,420);
				Line_Draw(&Collimation_4,"co4",UI_Graph_ADD,0,UI_Color_Green,2,7070,380,7140,380);
				
				Line_Draw(&Collimation_5,"co5",UI_Graph_ADD,0,UI_Color_Green,1,7105,560,7105,360);
				
				Line_Draw(&Collimation_6,"co6",UI_Graph_ADD,0,UI_Color_Green,2,7045,496,7045,504);
				Line_Draw(&Collimation_7,"co7",UI_Graph_ADD,0,UI_Color_Green,2,7080,496,7080,504);
				Line_Draw(&Collimation_8,"co8",UI_Graph_ADD,0,UI_Color_Green,2,7130,496,7130,504);
				Line_Draw(&Collimation_9,"co9",UI_Graph_ADD,0,UI_Color_Green,2,7165,496,7165,504);
				Line_Draw(&Collimation_10,"coa",UI_Graph_ADD,0,UI_Color_Green,2,7077,416,7077,424);
				Line_Draw(&Collimation_11,"cob",UI_Graph_ADD,0,UI_Color_Green,2,7132,416,7132,424);
				UI_ReFresh(7,Frame,Collimation_1,Collimation_2,Collimation_3,Collimation_4,Collimation_5,Collimation_6);
				UI_ReFresh(5,Collimation_7,Collimation_8,Collimation_9,Collimation_10,Collimation_11);
				
				//字符
				Char_Draw(&Capcity,"cap",UI_Graph_ADD,0,UI_Color_Pink,27,8,2,6870,300,"CAPCITY");
				Char_ReFresh(Capcity);
				Char_Draw(&HitRune,"hit",UI_Graph_ADD,0,UI_Color_Pink,27,7,2,6870,240,"HITRUNE");
				Char_ReFresh(HitRune);
				Char_Draw(&AiMBot,"aim",UI_Graph_ADD,0,UI_Color_Pink,27,6,2,6870,180,"AIMBOT");
				Char_ReFresh(AiMBot);

				Char_Draw(&HitRuneStatus,"his",UI_Graph_ADD,1,UI_Color_Cyan,27,2,2,7120,240,"NO");
				Char_ReFresh(HitRuneStatus);
				Char_Draw(&AiMBotStatus,"ais",UI_Graph_ADD,1,UI_Color_Cyan,27,2,2,7120,180,"NO");
				Char_ReFresh(AiMBotStatus);
				Float_Draw(&CapData,"cad",UI_Graph_ADD,1,UI_Color_Cyan,27,2,5,7120,300,1000*CapChageVoltage);
				UI_ReFresh(1,CapData);
				
				//弹舱盖
				Circle_Draw(&BulletCircle,"buc",UI_Graph_ADD,0,UI_Color_Cyan,2,6800,150,40);
				Char_Draw(&BulletCover,"bul",UI_Graph_ADD,1,UI_Color_Pink,10,5,2,6780,153,"OPEN ");
				UI_ReFresh(1,BulletCircle);
				Char_ReFresh(BulletCover);
				
				//底盘运动模式
				Circle_Draw(&ChassisMoveCircle,"chm",UI_Graph_ADD,0,UI_Color_Cyan,2,7400,150,40);
				Char_Draw(&ChassisMove,"chc",UI_Graph_ADD,1,UI_Color_Cyan,10,6,2,7376,153,"FALLOW");
				UI_ReFresh(1,ChassisMoveCircle);
				Char_ReFresh(ChassisMove);
				
				//能量机关间隔时间
				if(game_state.stage_remain_time >= 240)
				{
					//初始阶段
					if(game_state.stage_remain_time >= 360) 
						Time = 60 + game_state.stage_remain_time - 420;
					
					//打小符
					else if(buff_musk_t.power_rune_buff & 0x08 && s_rune_flag==0) 
					{
						if(!s_rune_flag)
						{
							Time = 75;
							s_rune_flag = 1;
							s_time_rune = game_state.stage_remain_time;
						}
					}
					else if(s_rune_flag == 1) 
					{
						Time = 75 - s_time_rune + game_state.stage_remain_time;
					}
				}
				else 
				{
					Time = 60;
				}
				if(Time == 0)
				{
					s_rune_flag = 0;
					Time = 0;
				}

				M = Time/60;
				S = Time%60;
				Char_Draw(&s_rune,"sru",UI_Graph_ADD,0,UI_Color_Pink,27,6,2,7460,800,"S_RUNE");
				Char_ReFresh(s_rune);
				Char_Draw(&b_rune,"bru",UI_Graph_ADD,0,UI_Color_Pink,27,6,2,7460,760,"B_RUNE");
				Char_ReFresh(b_rune);
				Float_Draw(&Min,"Min",UI_Graph_ADD,3,UI_Color_Yellow,27,2,5,7640,800,M*1000);
				Float_Draw(&Sec,"Sec",UI_Graph_ADD,3,UI_Color_Yellow,27,2,5,7700,800,S*1000);
				UI_ReFresh(2,Min,Sec);
				
				
				
				
			
				//初始阶段
				if(game_state.stage_remain_time >= 180) BTime = 240 - (420 - game_state.stage_remain_time);//倒计时
				else
				{
					//打大符
					if(buff_musk_t.power_rune_buff & 0x04 && b_rune_flag==0) 
					{
						if(!b_rune_flag)
						{
							BTime = 75;
							b_rune_flag = 1;
							b_time_rune = game_state.stage_remain_time;
						}
					}
					else if(b_rune_flag == 1) 
					{
						BTime = 75 - b_time_rune + game_state.stage_remain_time;
					}
				}
				
				if(BTime == 0)
				{
					b_rune_flag = 0;
					BTime = 0;
				}

				BM = BTime/60;
				BS = BTime%60;
				Float_Draw(&BMin,"BMi",UI_Graph_ADD,3,UI_Color_Yellow,27,2,5,7640,760,BM*1000);
				Float_Draw(&BSec,"BSe",UI_Graph_ADD,3,UI_Color_Yellow,27,2,5,7700,760,BS*1000);
				UI_ReFresh(2,BMin,BSec);
				
				//增益
				if(buff_musk_t.power_rune_buff & 0x02)                //增益
				{
					Char_Draw(&Cbuff,"Cbu",UI_Graph_ADD,5,UI_Color_Yellow,20,13,2,7680,380,"COOLDOWN BUFF");
					Char_ReFresh(Cbuff);
				}
				else
				{
					Char_Draw(&Cbuff,"Cbu",UI_Graph_Del,5,UI_Color_Yellow,20,13,2,7680,380,"COOLDOWN BUFF");
					Char_ReFresh(Cbuff);
				}
				if(buff_musk_t.power_rune_buff & 0x04)
				{
					Char_Draw(&Pbuff,"Pbu",UI_Graph_ADD,5,UI_Color_Yellow,20,12,2,7700,410,"Protect BUFF");
					Char_ReFresh(Pbuff);
				}
				else
				{
					Char_Draw(&Pbuff,"Pbu",UI_Graph_Del,5,UI_Color_Yellow,20,12,2,7700,410,"Protect BUFF");
					Char_ReFresh(Pbuff);
				}
				if(buff_musk_t.power_rune_buff & 0x08)
				{
					Char_Draw(&Abuff,"Abu",UI_Graph_ADD,5,UI_Color_Yellow,20,11,2,7720,440,"Attack BUFF");
					Char_ReFresh(Abuff);
				}
				else
				{
					Char_Draw(&Abuff,"Abu",UI_Graph_Del,5,UI_Color_Yellow,20,11,2,7720,440,"Attack BUFF");
					Char_ReFresh(Abuff);
				}
				
				
				
				
				
				
				if(PTZ.PTZStatusInformation & 0x20)          //打大幅
				{
					Char_Draw(&HitRuneStatus,"his",UI_Graph_Change,1,UI_Color_Pink,27,2,2,7120,240,"OK");
					Char_ReFresh(HitRuneStatus);
				}
				else
				{
					Char_Draw(&HitRuneStatus,"his",UI_Graph_Change,1,UI_Color_Cyan,27,2,2,7120,240,"NO");
					Char_ReFresh(HitRuneStatus);
				}
				
				if(PTZ.PTZStatusInformation & 0x02)          //自瞄
				{
					Char_Draw(&AiMBotStatus,"ais",UI_Graph_Change,1,UI_Color_Pink,27,2,2,7120,180,"OK");
					Char_ReFresh(AiMBotStatus);	
				}
				else
				{
					Char_Draw(&AiMBotStatus,"ais",UI_Graph_Change,1,UI_Color_Cyan,27,2,2,7120,180,"NO");
					Char_ReFresh(AiMBotStatus);
				}
				
				if(CapChageVoltage>14)
				{
					Float_Draw(&CapData,"cad",UI_Graph_Change,1,UI_Color_Cyan,27,2,5,7120,300,1000*CapChageVoltage);
					UI_ReFresh(1,CapData);
				}
				else
				{
					Float_Draw(&CapData,"cad",UI_Graph_Change,1,UI_Color_Pink,27,2,5,7120,300,1000*CapChageVoltage);
					UI_ReFresh(1,CapData);
				}
				
				
				if(PTZ.PTZStatusInformation & 0x10)
				{
					Char_Draw(&BulletCover,"bul",UI_Graph_Change,1,UI_Color_Main,10,5,2,6780,153,"OPEN ");
					Char_ReFresh(BulletCover);
				}
				else
				{
					Char_Draw(&BulletCover,"bul",UI_Graph_Change,1,UI_Color_Pink,10,5,2,6780,153,"CLOSE");
					Char_ReFresh(BulletCover);
				}
				
				//底盘运动模式
				if((PTZ.ChassisStatueRequest & 0x02 )&&(PTZ.ChassisStatueRequest & 0x40 ))
				{
					Char_Draw(&ChassisMove,"chc",UI_Graph_Change,1,UI_Color_Pink,10,7,2,7376,153,"  LOW  ");
					Char_ReFresh(ChassisMove);
				}
				else if(ChassisCtrl.Mode == NOFORCE)
				{
					Char_Draw(&ChassisMove,"chc",UI_Graph_Change,1,UI_Color_Pink,10,7,2,7376,153,"NOFORCE");
					Char_ReFresh(ChassisMove);
				}
				else if(ChassisCtrl.Mode == STOP)
				{
					Char_Draw(&ChassisMove,"chc",UI_Graph_Change,1,UI_Color_Pink,10,7,2,7376,153,"  STOP ");
					Char_ReFresh(ChassisMove);
				}
				else if(ChassisCtrl.Mode == FALLOW)
				{
					Char_Draw(&ChassisMove,"chc",UI_Graph_Change,1,UI_Color_Cyan,10,7,2,7376,153,"FALLOW ");
					Char_ReFresh(ChassisMove);
				}
				else if(ChassisCtrl.Mode == ROTING)
				{
					Char_Draw(&ChassisMove,"chc",UI_Graph_Change,1,UI_Color_Cyan,10,7,2,7376,153,"ROTING ");
					Char_ReFresh(ChassisMove);
				}
				if(game_state.stage_remain_time>Time)
				{
					Float_Draw(&Min,"Min",UI_Graph_Change,3,UI_Color_Yellow,27,2,5,7640,800,M*1000);
					Float_Draw(&Sec,"Sec",UI_Graph_Change,3,UI_Color_Yellow,27,2,5,7700,800,S*1000);
					UI_ReFresh(2,Min,Sec);
				}
				
				if(game_state.stage_remain_time>BTime)
				{
					Float_Draw(&BMin,"BMi",UI_Graph_Change,3,UI_Color_Yellow,27,2,5,7640,760,BM*1000);
					Float_Draw(&BSec,"BSe",UI_Graph_Change,3,UI_Color_Yellow,27,2,5,7700,760,BS*1000);
					UI_ReFresh(2,BMin,BSec);
				}
			}

        osDelay(1);
    }

 }