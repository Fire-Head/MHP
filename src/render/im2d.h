#pragma once
#include "rw.h"

class Im2D
{
	enum
	{
		FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0)),
		BUFFER_MAX_INDEX = 10000,
	};

	RwUInt32                 CurrentBaseIndex;
	LPDIRECT3DINDEXBUFFER8   IndexBuffer2D;
	LPDIRECT3DVERTEXBUFFER8  CurrentVertexBuffer;
	RwInt32                  NumIndices;
	RwInt32                  NumVerts;
	RwUInt32                 VertexShader;
	RwUInt32                 PixelShader;

private:
	RwBool FillBuffers(RwIm2DVertex *verts, RwInt32 numVerts, RwImVertexIndex *indices, RwInt32 numIndices);
	void Render();

public:
	void Initialise();
	void Shutdown();

	void SetVertexShader(RwUInt32 shader);
	void SetPixelShader(RwUInt32 shader);

	// TRILIST
	void RenderIndexedPrimitive(RwIm2DVertex *verts, RwInt32 numVerts, RwImVertexIndex *indices, RwInt32 numIndices);

	static Im2D *GetInstance();
};

