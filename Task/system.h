#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "string.h"
#include "stdio.h"
#include "math.h"

#include "can.h"
#include "tim.h"
#include "main.h"
#include "usart.h"
#include "cmsis_os.h"

#include "bsp_led.h"
#include "ChassisSendProtocol.h"
#include "RefereeBehaviour.h"
#include "CMS.h"
#include "user_lib.h"
#include "ChassisTask.h"
#include "ChassisPowerBehaviour.h"
#include "RM_Cilent_UI.h"
#include "ErrorCounter.h"



void sys_int();
void error_counter_Callback();
void usart1_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num);
void usart1_tx_dma_enable(uint8_t *data, uint16_t len);

#endif
