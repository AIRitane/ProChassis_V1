#include "vofa.h"
#include "stdio.h"
//#include "system.h"

//默认使用串口一发送
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
//需包含#include "stdio.h"
#pragma __use_no_semihosting            
//标准库需要的支持函数                 
struct FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 
	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
	USART6->DR = (uint8_t) ch;      
	return ch;
}
