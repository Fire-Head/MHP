#include "rwim2d.h"

Im2D *Im2D::GetInstance()
{
	static Im2D im2d;
	return &im2d;
}

void Im2D::Initialise()
{
	CurrentBaseIndex = 0;
	IndexBuffer2D = NULL;
	CurrentVertexBuffer = NULL;
	VertexShader = FVF;
	PixelShader = 0;

	RwD3D8IndexBufferCreate(BUFFER_MAX_INDEX, (void**)&IndexBuffer2D);
}

void Im2D::Shutdown()
{
	if ( IndexBuffer2D )
    {
        IndexBuffer2D->Release();
        IndexBuffer2D = NULL;
    }
}

RwBool Im2D::FillBuffers(RwIm2DVertex *verts, RwInt32 numVerts, RwImVertexIndex *indices, RwInt32 numIndices)
{
	const RwRaster	*raster;
	RwIm2DVertex	*bufferMem = NULL;

	NumIndices = numIndices;
	NumVerts = numVerts;

	raster = RwCameraGetRaster(RwCameraGetCurrentCamera());
	
	RwD3D8DynamicVertexBufferLock(sizeof(RwIm2DVertex),
									numVerts,
									(void**)&CurrentVertexBuffer,
									(void**)&bufferMem,
									&CurrentBaseIndex);

	if ( bufferMem != NULL )
	{
		if ( raster->nOffsetX || raster->nOffsetY )
		{
			const RwReal offsetX = (RwReal)(raster->nOffsetX);
			const RwReal offsetY = (RwReal)(raster->nOffsetY);

			for ( RwInt32 i = 0; i < numVerts; i++ )
			{
				bufferMem[i].x = verts[i].x + offsetX;
				bufferMem[i].y = verts[i].y + offsetY;
				bufferMem[i].z= verts[i].z;
				bufferMem[i].rhw= verts[i].rhw;
				bufferMem[i].emissiveColor= verts[i].emissiveColor;
				bufferMem[i].u= verts[i].u;
				bufferMem[i].v= verts[i].v;
			}
		}
		else
		{
			memcpy(bufferMem, verts, numVerts * sizeof(RwIm2DVertex));
		}

		RwD3D8DynamicVertexBufferUnlock(CurrentVertexBuffer);

		if  (IndexBuffer2D && (numIndices <= BUFFER_MAX_INDEX) )
		{
			HRESULT hr;

			hr = IDirect3DIndexBuffer8_Lock(IndexBuffer2D,
											0,
											numIndices * sizeof(RwUInt16),
											(RwUInt8 **)&bufferMem,
											D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);

			if( SUCCEEDED(hr) )
			{
				memcpy(bufferMem, indices, numIndices * sizeof(RwUInt16));

				IDirect3DIndexBuffer8_Unlock(IndexBuffer2D);

				return TRUE;
			}
		}
	}
	
	return FALSE;
}

void Im2D::Render()
{
	RwD3D8SetVertexShader(VertexShader);
	RwD3D8SetPixelShader(PixelShader);
	RwD3D8SetRenderState(D3DRS_CLIPPING, FALSE);
	RwD3D8SetRenderState(D3DRS_LIGHTING, FALSE);
	RwD3D8SetStreamSource(0, CurrentVertexBuffer, sizeof(RwIm2DVertex));
	RwD3D8SetIndices(IndexBuffer2D, CurrentBaseIndex);
	RwD3D8DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NumVerts, 0, NumIndices);
}

void Im2D::SetVertexShader(RwUInt32 shader)
{
	VertexShader = shader ? shader : FVF;
}

void Im2D::SetPixelShader(RwUInt32 shader)
{
	PixelShader = shader ? shader : 0;
}

void Im2D::RenderIndexedPrimitive(RwIm2DVertex *verts, RwInt32 numVerts, RwImVertexIndex *indices, RwInt32 numIndices)
{
	if ( FillBuffers(verts, numVerts, indices, numIndices) )
		Render();

	VertexShader = FVF;
	PixelShader = 0;
}