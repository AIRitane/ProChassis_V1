#include "vofa.h"
#include "stdio.h"
//#include "system.h"

//Ĭ��ʹ�ô���һ����
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
//�����#include "stdio.h"
#pragma __use_no_semihosting            
//��׼����Ҫ��֧�ֺ���                 
struct FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (uint8_t) ch;      
	return ch;
}
