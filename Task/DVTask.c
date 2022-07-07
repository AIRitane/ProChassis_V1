#include "cmsis_os.h"
#include "DVTask.h"
#include "vofa.h"
#include "stdio.h"
#include "ChassisTask.h"

extern ChassisCtrl_t ChassisCtrl;
extern fp32 total_power;
extern fp32 chassis_power;
extern fp32 chassis_power_buffer;
extern fp32 watch_pre_current;
extern fp32 watch_new_current;
void DVTask(void const * argument)
{
	while(1)
	{
		printf("%f,%f\n",chassis_power,chassis_power_buffer);
		osDelay(1);
	}
}