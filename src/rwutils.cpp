#include "common.h"
#include "settings.h"
#include "rw.h"


void
_rwD3D8EnableClippingIfNeeded(void *object, RwUInt8 type)
{
	int clip;
	if(type == rpATOMIC)
		clip = !RwD3D8CameraIsSphereFullyInsideFrustum(RwCameraGetCurrentCamera(),
		                                               RpAtomicGetWorldBoundingSphere((RpAtomic*)object));
	else
		clip = !RwD3D8CameraIsBBoxFullyInsideFrustum(RwCameraGetCurrentCamera(),
		                                             &((RpWorldSector*)object)->tightBoundingBox);
	RwD3D8SetRenderState(D3DRS_CLIPPING, clip);
}

void RwD3D8DrawIndexedPrimitive_DualPass(RwUInt32 primitiveType, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 numIndices) 
{
	if (!Settings::GetInstance()->bDualPass)
	{
		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);
		return;
	}
	int hasAlpha, alphafunc, alpharef, zwrite;
	RwD3D8GetRenderState(D3DRS_ALPHABLENDENABLE, &hasAlpha);
	RwD3D8GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);
	
	if (hasAlpha && zwrite)
	{
		RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
		RwD3D8GetRenderState(D3DRS_ALPHAREF, &alpharef);
	
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		RwD3D8SetRenderState(D3DRS_ALPHAREF, 128);
	
		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);
		
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	
		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
		RwD3D8SetRenderState(D3DRS_ALPHAREF, alpharef);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, zwrite);	
	}
	else if ( !zwrite )
	{
		RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
				
		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);
				
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
	} 
	else
		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);
}

void RwD3D8DrawPrimitive_DualPass(RwUInt32 primitiveType, RwUInt32 startVertex, RwUInt32 numVertices)	
{
	if (!Settings::GetInstance()->bDualPass)
	{
		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);
		return;
	}

	int hasAlpha, alphafunc, alpharef, zwrite;
	RwD3D8GetRenderState(D3DRS_ALPHABLENDENABLE, &hasAlpha);
	RwD3D8GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);
	
	if (hasAlpha && zwrite)
	{
		RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
		RwD3D8GetRenderState(D3DRS_ALPHAREF, &alpharef);
	
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		RwD3D8SetRenderState(D3DRS_ALPHAREF, 128);

		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);
	
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	
		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);
	
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
		RwD3D8SetRenderState(D3DRS_ALPHAREF, alpharef);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, zwrite);
	}
	else if ( !zwrite )
	{
		RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
				
		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);
				
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
	}
	else
		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);
}
