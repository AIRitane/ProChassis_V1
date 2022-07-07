#ifndef USER_LIB_H
#define USER_LIB_H
#include "struct_typedef.h"

typedef __PACKED_STRUCT
{
    fp32 input;        //��������
    fp32 out;          //�������
    fp32 min_value;    //�޷���Сֵ
    fp32 max_value;    //�޷����ֵ
    fp32 frame_period; //ʱ����
} ramp_function_source_t;

typedef __PACKED_STRUCT
{
    fp32 input;        //��������
    fp32 out;          //�˲����������
    fp32 num[1];       //�˲�����
    fp32 frame_period; //�˲���ʱ���� ��λ s
} first_order_filter_type_t;

typedef __PACKED_STRUCT
{
    fp32 derivative;        //导数
		fp32 last_derivative;   //上一次导数
    fp32 out;          //输出值
		fp32 last_out;		 //上一次输出值
    fp32 delta;        //系数delta
    fp32 frame_period; //代码运行时长
} TD_differential_tracker_t;


//���ٿ���
extern fp32 invSqrt(fp32 num);

//б��������ʼ��
void ramp_init(ramp_function_source_t *ramp_source_type, fp32 frame_period, fp32 max, fp32 min);

//б����������
void ramp_calc(ramp_function_source_t *ramp_source_type, fp32 input);
//һ���˲���ʼ��
extern void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period, const fp32 num[1]);
//һ���˲�����
extern void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input);
//��������
extern void abs_limit(fp32 *num, fp32 Limit);
//�жϷ���λ
extern fp32 sign(fp32 value);
//��������
extern fp32 fp32_deadline(fp32 Value, fp32 minValue, fp32 maxValue);
//int26����
extern int16_t int16_deadline(int16_t Value, int16_t minValue, int16_t maxValue);
//�޷�����
extern fp32 fp32_constrain(fp32 Value, fp32 minValue, fp32 maxValue);
//�޷�����
extern int16_t int16_constrain(int16_t Value, int16_t minValue, int16_t maxValue);
//ѭ���޷�����
extern fp32 loop_fp32_constrain(fp32 Input, fp32 minValue, fp32 maxValue);
//�Ƕ� ���޷� 180 ~ -180
extern fp32 theta_format(fp32 Ang);

//TD微分跟踪器
//用于过度
extern void TD_differential_tracker(TD_differential_tracker_t *TD_differential_tracker_type, fp32 input);
//TD微分跟踪器初始化
//用于过度
extern void TD_differential_tracker_init(TD_differential_tracker_t *TD_differential_tracker_type, fp32 frame_period, const fp32 delta);
//���ȸ�ʽ��Ϊ-PI~PI
#define rad_format(Ang) theta_format((Ang))

#endif
