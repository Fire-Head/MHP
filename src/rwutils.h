#pragma once
#include "rw.h"

#define RwD3D8DrawIndexedPrimitive RwD3D8DrawIndexedPrimitive_DualPass
#define RwD3D8DrawPrimitive RwD3D8DrawPrimitive_DualPass

void _rwD3D8EnableClippingIfNeeded(void *object, RwUInt8 type);
void RwD3D8DrawIndexedPrimitive_DualPass(RwUInt32 primitiveType, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 numIndices);
void RwD3D8DrawPrimitive_DualPass(RwUInt32 primitiveType, RwUInt32 startVertex, RwUInt32 numVertices);