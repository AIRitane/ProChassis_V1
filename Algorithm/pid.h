/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       pid.c/h
  * @brief      pidʵ�ֺ�����������ʼ����PID���㺯����
  * @note       
  * @history    2021-7-23   �����˴���pid�ĺ����ͽӿ�
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. ���
  *  V2.0.0     Jly-23-2021     Qylann          1. ���Ӳ��ֺ���
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
#ifndef PID_H
#define PID_H
#include "struct_typedef.h"

typedef struct
{
    uint8_t mode;
    //PID ������
    fp32 Kp;
    fp32 Ki;
    fp32 Kd;

    fp32 max_out;  //������
    fp32 max_iout; //���������

    fp32 set;
    fp32 fdb;

    fp32 out;
    fp32 Pout;
    fp32 Iout;
    fp32 Dout;
    fp32 Dbuf[3];  //΢���� 0���� 1��һ�� 2���ϴ�
    fp32 error[3]; //����� 0���� 1��һ�� 2���ϴ�

} pid_type_def;


typedef struct
{
    pid_type_def    pid_inside;
    pid_type_def    pid_outside;
    
    fp32            s_set;
    fp32            s_fdb;
    fp32            v_set;
    fp32            v_fdb;
    fp32            out;
} cascade_pid_t;

enum PID_MODE
{
    PID_POSITION = 0,
    PID_DELTA,
	PID_ANGLE
};

/**
  * @brief          pid struct data init
  * @param[out]     pid: PID struct data point
  * @param[in]      mode: PID_POSITION: normal pid
  *                 PID_DELTA: delta pid
  * @param[in]      PID: 0: kp, 1: ki, 2:kd
  * @param[in]      max_out: pid max out
  * @param[in]      max_iout: pid max iout
  * @retval         none
  */
/**
  * @brief          pid struct data init
  * @param[out]     pid: PID�ṹ����ָ��
  * @param[in]      mode: PID_POSITION:��ͨPID
  *                 PID_DELTA: ���PID
  * @param[in]      PID: 0: kp, 1: ki, 2:kd
  * @param[in]      max_out: pid������
  * @param[in]      max_iout: pid���������
  * @retval         none
  */
extern void PID_init(pid_type_def *pid, uint8_t mode, const fp32 PID[3], fp32 max_out, fp32 max_iout);

/**
  * @brief          cascade pid struct data init
  * @param[out]     c_pid: cascade PID struct data point
  * @param[in]      oPID: 0: kp, 1: ki, 2:kd
  * @param[in]      iPID: 0: kp, 1: ki, 2:kd
  * @param[in]      oPID_max_out: outside pid max out
  * @param[in]      oPID_max_iout: outside pid max iout
  * @param[in]      iPID_max_out: inside pid max out
  * @param[in]      iPID_max_iout: inside pid max iout
  * @retval         none
  */
/**
  * @brief          cascade pid struct data init
  * @param[out]     c_pid: ���PID�ṹ����ָ��
  * @param[in]      oPID: 0: kp, 1: ki, 2:kd
  * @param[in]      iPID: 0: kp, 1: ki, 2:kd
  * @param[in]      oPID_max_out: �⻷pid������
  * @param[in]      oPID_max_iout: �⻷pid���������
  * @param[in]      iPID_max_out: �ڻ�pid������
  * @param[in]      iPID_max_iout: �ڻ�pid���������
  * @retval         none
  */
extern void cascade_PID_init(cascade_pid_t *c_pid, const fp32 oPID[3], const fp32 iPID[3], fp32 oPID_max_out, fp32 oPID_max_iout, fp32 iPID_max_out, fp32 iPID_max_iout);

/**
  * @brief          pid calculate 
  * @param[out]     pid: PID struct data point
  * @param[in]      ref: feedback data 
  * @param[in]      set: set point
  * @retval         pid out
  */
/**
  * @brief          pid����
  * @param[out]     pid: PID�ṹ����ָ��
  * @param[in]      ref: ��������
  * @param[in]      set: �趨ֵ
  * @retval         pid���
  */
extern fp32 PID_calc(pid_type_def *pid, fp32 ref, fp32 set);

/**
  * @brief          cascade pid calculate 
  * @param[out]     c_pid: cascade PID struct data point
  * @param[in]      o_ref: outside feedback data 
  * @param[in]      i_ref: inside feedback data 
  * @param[in]      s_set: outside set point
  * @retval         cascade pid out
  */
/**
  * @brief          ���pid����
  * @param[out]     c_pid: ���PID�ṹ����ָ��
  * @param[in]      o_ref: �⻷��������
  * @param[in]      i_ref: �ڻ���������
  * @param[in]      s_set: �⻷�趨ֵ
  * @retval         ���pid���
  */
extern fp32 cascade_PID_calc(cascade_pid_t *c_pid, fp32 o_ref, fp32 i_ref, fp32 o_set);

/**
  * @brief          pid out clear
  * @param[out]     pid: PID struct data point
  * @retval         none
  */
/**
  * @brief          pid ������
  * @param[out]     pid: PID�ṹ����ָ��
  * @retval         none
  */
extern void PID_clear(pid_type_def *pid);

/**
  * @brief          cascade pid out clear
  * @param[out]     c_pid: cascade PID struct data point
  * @retval         none
  */
/**
  * @brief          ���pid������
  * @param[out]     c_pid: ���PID�ṹ����ָ��
  * @retval         none
  */
extern void cascade_PID_clear(cascade_pid_t *c_pid);

#endif
