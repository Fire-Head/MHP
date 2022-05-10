#pragma once
#include "config.h"
#include "utils.h"
#include <stdio.h>
#include <cmath>


#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Min(a,b) ((a) < (b) ? (a) : (b))
#define Clamp(v, low, high) ((v)<(low) ? (low) : (v)>(high) ? (high) : (v))

#define ARRAY_SIZE(array)                (sizeof(array) / sizeof(array[0]))

#ifdef DEVBUILD
	#define DBG(f,...)                      printf(f, ## __VA_ARGS__)
#else
	#define DBG(f,...)                      
#endif

