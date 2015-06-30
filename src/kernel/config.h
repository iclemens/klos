#ifndef __CONFIG_H_
#define __CONFIG_H_

#define NULL 0
//#define PF_DEBUG 1

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

typedef signed char  int8_t;
typedef signed short int16_t;
typedef signed int   int32_t;
typedef signed long  int64_t;

//typedef uint64_t ptr_t;

typedef uint32_t ptr_t;
typedef uint32_t size_t;

//typedef int8_t bool;

//extern const true;
//extern const false;

typedef __builtin_va_list va_list;

#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)

#endif
