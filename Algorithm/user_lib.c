#include "user_lib.h"
#include "math.h"

//���ٿ���
fp32 invSqrt(fp32 num)
{
    fp32 halfnum = 0.5f * num;
    fp32 y = num;
    long i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(fp32 *)&i;
    y = y * (1.5f - (halfnum * y * y));
    return y;
}

/**
  * @brief          б��������ʼ��
  * @author         RM
  * @param[in]      б�������ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @param[in]      ���ֵ
  * @param[in]      ��Сֵ
  * @retval         ���ؿ�
  */
void ramp_init(ramp_function_source_t *ramp_source_type, fp32 frame_period, fp32 max, fp32 min)
{
    ramp_source_type->frame_period = frame_period;
    ramp_source_type->max_value = max;
    ramp_source_type->min_value = min;
    ramp_source_type->input = 0.0f;
    ramp_source_type->out = 0.0f;
}

/**
  * @brief          б���������㣬���������ֵ���е��ӣ� ���뵥λΪ /s ��һ������������ֵ
  * @author         RM
  * @param[in]      б�������ṹ��
  * @param[in]      ����ֵ
  * @param[in]      �˲�����
  * @retval         ���ؿ�
  */
void ramp_calc(ramp_function_source_t *ramp_source_type, fp32 input)
{
			ramp_source_type->input = input;
			ramp_source_type->out -= ramp_source_type->input * ramp_source_type->frame_period;
		
    if (ramp_source_type->out > ramp_source_type->max_value)
    {
        ramp_source_type->out = ramp_source_type->max_value;
    }
    else if (ramp_source_type->out < ramp_source_type->min_value)
    {
        ramp_source_type->out = ramp_source_type->min_value;
    }
    
}
/**
  * @brief          һ�׵�ͨ�˲���ʼ��,�Ѹ��ģ���ֹ�������LOGOTAO��
  * @author         RM,LOGOTAO
  * @param[in]      һ�׵�ͨ�˲��ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @param[in]      �˲�����
  * @retval         ���ؿ�
  */
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period, const fp32 num[1])
{
    first_order_filter_type->frame_period = frame_period;
    first_order_filter_type->num[0] = num[0];
    first_order_filter_type->input = 0.0f;
    first_order_filter_type->out = 0.0f;
}
//TD΢�ָ�������ʼ��
//���ڹ���
void TD_differential_tracker_init(TD_differential_tracker_t *TD_differential_tracker_type, fp32 frame_period, const fp32 delta)
{
    TD_differential_tracker_type->frame_period = frame_period;
    TD_differential_tracker_type->delta = delta;
		TD_differential_tracker_type->last_derivative=0.0f;
		TD_differential_tracker_type->last_out=0.0f;
    TD_differential_tracker_type->out = 0.0f;
}
/**
  * @brief          һ�׵�ͨ�˲�����
  * @author         RM
  * @param[in]      һ�׵�ͨ�˲��ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @retval         ���ؿ�
  */
void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input)
{
    first_order_filter_type->input = input;
	if(first_order_filter_type->input == 0)
	{
		first_order_filter_type->input = 0.0000000001f;
	}
	if(first_order_filter_type->out == 0)
	{
		first_order_filter_type->out = 0.0000000001f;
	}
    first_order_filter_type->out =
        first_order_filter_type->num[0] / (first_order_filter_type->num[0] +\
				first_order_filter_type->frame_period) * first_order_filter_type->out +\
				first_order_filter_type->frame_period / (first_order_filter_type->num[0] + \
				first_order_filter_type->frame_period) * first_order_filter_type->input;

}

//��������
void abs_limit(fp32 *num, fp32 Limit)
{
    if (*num > Limit)
    {
        *num = Limit;
    }
    else if (*num < -Limit)
    {
        *num = -Limit;
    }
}

//�жϷ���λ
fp32 sign(fp32 value)
{
    if (value >= 0.0f)
    {
        return 1.0f;
    }
    else
    {
        return -1.0f;
    }
}

//��������
fp32 fp32_deadline(fp32 Value, fp32 minValue, fp32 maxValue)
{
    if (Value < maxValue && Value > minValue)
    {
        Value = 0.0f;
    }
    return Value;
}

//int26����
int16_t int16_deadline(int16_t Value, int16_t minValue, int16_t maxValue)
{
    if (Value < maxValue && Value > minValue)
    {
        Value = 0;
    }
    return Value;
}

//�޷�����
fp32 fp32_constrain(fp32 Value, fp32 minValue, fp32 maxValue)
{
    if (Value < minValue)
        return minValue;
    else if (Value > maxValue)
        return maxValue;
    else
        return Value;
}

//�޷�����
int16_t int16_constrain(int16_t Value, int16_t minValue, int16_t maxValue)
{
    if (Value < minValue)
        return minValue;
    else if (Value > maxValue)
        return maxValue;
    else
        return Value;
}

//ѭ���޷�����
fp32 loop_fp32_constrain(fp32 Input, fp32 minValue, fp32 maxValue)
{
    if (maxValue < minValue)
    {
        return Input;
    }

    if (Input >= maxValue)
    {
        fp32 len = maxValue - minValue;
        while (Input >= maxValue)
        {
            Input -= len;
        }
    }
    else if (Input <= minValue)
    {
        fp32 len = maxValue - minValue;
        while (Input <= minValue)
        {
            Input += len;
        }
    }
    return Input;
}


//�Ƕȸ�ʽ��Ϊ-180~180
fp32 theta_format(fp32 Ang)
{
    return loop_fp32_constrain(Ang, -180.0f, 180.0f);
}

//TD΢�ָ�����
//���ڹ���
void TD_differential_tracker(TD_differential_tracker_t *TD_differential_tracker_type, fp32 input)
{
	fp32 difference_1,difference_2,d,d0,a0,y,a,fst;
	difference_1 = TD_differential_tracker_type->last_out - input;
	difference_2 = TD_differential_tracker_type->last_derivative;
	d = TD_differential_tracker_type->delta * TD_differential_tracker_type->frame_period;
	d0= TD_differential_tracker_type->frame_period * d;
	y = difference_1 + difference_2 * TD_differential_tracker_type->frame_period;
	a0= invSqrt(d*d+8*TD_differential_tracker_type->delta*fabs(y));
	if(fabs(y)>d0)
	{
		a = difference_2 + sign(y)*(a0-d)/2;
	}
	else
	{
		a = difference_2+ y/TD_differential_tracker_type->frame_period;
	}
	
	if(fabs(a)>d)
	{
		fst = -TD_differential_tracker_type->delta * sign(a);
	}
	else
	{
		fst = -TD_differential_tracker_type->delta * a/d;
	}
		
	TD_differential_tracker_type->derivative = TD_differential_tracker_type->last_derivative + TD_differential_tracker_type->frame_period * fst;
	TD_differential_tracker_type->out = TD_differential_tracker_type->derivative;
	
	TD_differential_tracker_type->last_derivative = TD_differential_tracker_type->derivative;
	TD_differential_tracker_type->last_out = TD_differential_tracker_type->out;
}