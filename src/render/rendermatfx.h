#pragma once
#include "rw.h"

class MatFXRender
{
	RwUInt32 dwBlendShaders[6][6];
	RwBool   VertexAlphaBlend;
	void    *LastVertexBuffer;

private:
	void ApplyAnimTextureMatrix(const MatFXUVAnimData *anim, RwUInt32 n);

public:
	void Initialise();
	void Shutdown();

	void Begin();
	void End();
	void SetVertexAlpha(RwBool enabled);
	void MeshRenderDefault(RxD3D8InstanceData *instancedMesh, RwUInt32 flags);
	void MeshRenderDualPass_PS2(RxD3D8InstanceData *instancedMesh, RwUInt32 flags);
	void MeshRenderDualPass_PCTwoPass(RxD3D8InstanceData *instancedMesh, RwUInt32 flags);
	void MeshRenderDualPass(RxD3D8InstanceData *instancedMesh, RwUInt32 flags);

	static MatFXRender *GetInstance();
};

void MatFXDualPassRender(RxD3D8InstanceData *instancedData, RwUInt32 flags, RwTexture *texture, RwTexture *dualTexture);
