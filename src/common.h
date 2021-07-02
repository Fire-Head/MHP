#pragma once
#include "config.h"

#define ARRAY_SIZE(array)                (sizeof(array) / sizeof(array[0]))
#define NAK                              __declspec(naked)
#define EAXJMP(a)                        { _asm mov eax, a _asm jmp eax }

#ifdef DEVBUILD
	#define DBG(f,...)                      printf(f, ## __VA_ARGS__)
#else
	#define DBG(f,...)                      
#endif
