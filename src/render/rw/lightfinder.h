#pragma once
#include "rw.h"

#define MAX_LIGHTS      15

typedef struct _rpD3D8VertexShaderLights
{
    RwUInt32 numDirectional;
    RpLight *directional[MAX_LIGHTS];

    RwUInt32 numPoint;
    RpLight *point[MAX_LIGHTS];

    RwUInt32 numSpot;
    RpLight *spot[MAX_LIGHTS];

} _rpD3D8VertexShaderLights;

RwBool
FindAtomicLights(RpAtomic *atomic, RwV4d *ambientLightConstant, _rpD3D8VertexShaderLights  *lightsArray, RwBool hasNormals);

RwBool
FindSectorLights(const RpWorldSector *worldSector, RwV4d *ambientLightConstant, _rpD3D8VertexShaderLights  *lightsArray);