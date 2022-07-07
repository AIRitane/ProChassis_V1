#include "cmsis_os.h"
#include "ComTask.h"

void ComTask(void const * argument)
{
	while(1)
	{
		osDelay(1);
	}
}