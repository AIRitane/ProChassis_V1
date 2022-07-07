#ifndef STRUCT_TYPEDEF_H
#define STRUCT_TYPEDEF_H

#include "main.h"

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* exact-width unsigned integer types */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char bool_t;
typedef float fp32;
typedef double fp64;




typedef enum 
{
  INF_OK       = 0x00U,
  INF_ERROR    = 0x01U,
  INF_BUSY     = 0x02U,
  INF_TIMEOUT  = 0x03U
} INF_StatusTypeDef;






#endif



