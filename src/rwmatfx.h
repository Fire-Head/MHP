#pragma once
#include "rw.h"

void _rpMatFXD3D8AtomicMatFXDefaultRender_fixed(RxD3D8InstanceData *inst, int flags, RwTexture *texture);
void _rpMatFXD3D8AtomicMatFXEnvRender_ps2(RxD3D8InstanceData *inst, int flags, int sel, RwTexture *texture, RwTexture *envMap);
void _rpMatFXD3D8AtomicMatFXRenderBlack_fixed(RxD3D8InstanceData *inst);