#include "common.h"
#include "settings.h"
#include "helpers.h"
#include "rendermatfx.h"
#ifdef LOADSHADERS
#include "shaderloader.h"
#endif

#ifndef LOADSHADERS

#define  dwPixelShader dwBlend1PixelShader
#include "shaders\blend_dstzero_srczero.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend2PixelShader
#include "shaders\blend_dstzero_srcone.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend3PixelShader
#include "shaders\blend_dstzero_srcsrcalpha.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend4PixelShader
#include "shaders\blend_dstzero_srcinvsrcalpha.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend5PixelShader
#include "shaders\blend_dstzero_srcdestalpha.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend6PixelShader
#include "shaders\blend_dstzero_srcinvdestalpha.h"
#undef   dwPixelShader

#define  dwPixelShader dwBlend7PixelShader
#include "shaders\blend_dstone_srczero.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend8PixelShader
#include "shaders\blend_dstone_srcone.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend9PixelShader
#include "shaders\blend_dstone_srcsrcalpha.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend11PixelShader
#include "shaders\blend_dstone_srcdestalpha.h"
#undef   dwPixelShader

#define  dwPixelShader dwBlend13PixelShader
#include "shaders\blend_dstsrcalpha_srczero.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend14PixelShader
#include "shaders\blend_dstsrcalpha_srcone.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend16PixelShader
#include "shaders\blend_dstsrcalpha_srcinvsrcalpha.h"
#undef   dwPixelShader

#define  dwPixelShader dwBlend19PixelShader
#include "shaders\blend_dstinvsrcalpha_srczero.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend21PixelShader
#include "shaders\blend_dstinvsrcalpha_srcsrcalpha.h"
#undef   dwPixelShader

#define  dwPixelShader dwBlend25PixelShader
#include "shaders\blend_dstdestalpha_srczero.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend26PixelShader
#include "shaders\blend_dstdestalpha_srcone.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend30PixelShader
#include "shaders\blend_dstdestalpha_srcinvdestalpha.h"
#undef   dwPixelShader

#define  dwPixelShader dwBlend31PixelShader
#include "shaders\blend_dstinvdestalpha_srczero.h"
#undef   dwPixelShader
#define  dwPixelShader dwBlend35PixelShader
#include "shaders\blend_dstinvdestalpha_srcdestalpha.h"
#undef   dwPixelShader

#endif

DWORD *dwBlendShadersCode[6][6] =
{
	// rwBLENDZERO
	{
		/* rwBLENDZERO        */ dwBlend1PixelShader,
		/* rwBLENDONE         */ dwBlend2PixelShader,
		/* rwBLENDSRCALPHA    */ dwBlend3PixelShader,
		/* rwBLENDINVSRCALPHA */ dwBlend4PixelShader,
		/* rwBLENDDESTALPHA   */ dwBlend5PixelShader,
		/* rwBLENDINVDESTALPHA*/ dwBlend6PixelShader,
	},

	//rwBLENDONE
	{
		/* rwBLENDZERO        */ dwBlend7PixelShader,
		/* rwBLENDONE         */ dwBlend8PixelShader,
		/* rwBLENDSRCALPHA    */ dwBlend9PixelShader,
		/* rwBLENDINVSRCALPHA */ NULL,
		/* rwBLENDDESTALPHA   */ dwBlend11PixelShader,
		/* rwBLENDINVDESTALPHA*/ NULL,
	},

	// rwBLENDSRCALPHA
	{
		/* rwBLENDZERO        */ dwBlend13PixelShader,
		/* rwBLENDONE         */ dwBlend14PixelShader,
		/* rwBLENDSRCALPHA    */ NULL,
		/* rwBLENDINVSRCALPHA */ dwBlend16PixelShader,
		/* rwBLENDDESTALPHA   */ NULL,
		/* rwBLENDINVDESTALPHA*/ NULL,
	},

	// rwBLENDINVSRCALPHA
	{
		/* rwBLENDZERO        */ dwBlend19PixelShader,
		/* rwBLENDONE         */ NULL,
		/* rwBLENDSRCALPHA    */ dwBlend21PixelShader,
		/* rwBLENDINVSRCALPHA */ NULL,
		/* rwBLENDDESTALPHA   */ NULL,
		/* rwBLENDINVDESTALPHA*/ NULL,
	},

	// rwBLENDDESTALPHA
	{
		/* rwBLENDZERO        */ dwBlend25PixelShader,
		/* rwBLENDONE         */ dwBlend26PixelShader,
		/* rwBLENDSRCALPHA    */ NULL,
		/* rwBLENDINVSRCALPHA */ NULL,
		/* rwBLENDDESTALPHA   */ NULL,
		/* rwBLENDINVDESTALPHA*/ dwBlend30PixelShader,
	},

	// rwBLENDINVDESTALPHA
	{
		/* rwBLENDZERO        */ dwBlend31PixelShader,
		/* rwBLENDONE         */ NULL,
		/* rwBLENDSRCALPHA    */ NULL,
		/* rwBLENDINVSRCALPHA */ NULL,
		/* rwBLENDDESTALPHA   */ dwBlend35PixelShader,
		/* rwBLENDINVDESTALPHA*/ NULL,
	}
};

RwInt32 Blend2ID(RwBlendFunction blend)
{
	switch (blend)
	{
		case rwBLENDZERO:         return 0;
		case rwBLENDONE:          return 1;
		case rwBLENDSRCALPHA:     return 2;
		case rwBLENDINVSRCALPHA:  return 3;
		case rwBLENDDESTALPHA:    return 4;
		case rwBLENDINVDESTALPHA: return 5;
		default: return -1;
	}
}

MatFXRender *MatFXRender::GetInstance()
{
	static MatFXRender render;
	return &render;
}

void MatFXRender::Initialise()
{
	for ( int d = 0; d < 6; d++ )
	{
		for ( int s = 0; s < 6; s++ )
		{
			if ( dwBlendShadersCode[d][s] )
			{
				RwBool isok = RwD3D8CreatePixelShader((RwUInt32*)dwBlendShadersCode[d][s], &dwBlendShaders[d][s]);

				DBG("%d_%dPS:\t%s\n", d, s, isok ? "OK" : "FAILED");
			}
			else
				dwBlendShaders[d][s] = NULL;
		}
	}
}

void MatFXRender::Shutdown()
{
	for ( int d = 0; d < 6; d++ )
	{
		for ( int s = 0; s < 6; s++ )
		{
			if ( dwBlendShadersCode[d][s] )
				RwD3D8DeletePixelShader(dwBlendShaders[d][s]);
		}
	}
}

void MatFXRender::Begin()
{
	VertexAlphaBlend  = _rwD3D8RenderStateIsVertexAlphaEnable();
	LastVertexBuffer  = (void *)0xffffffff;
}

void MatFXRender::End()
{
	// not needed but looks cool
}

void MatFXRender::ApplyAnimTextureMatrix(const MatFXUVAnimData *anim, RwUInt32 n)
{
	static D3DMATRIX texMat =
	{
		0.0f,  0.0f,  0.0f,  0.0f,
		0.0f,  0.0f,  0.0f,  0.0f,
		0.0f,  0.0f,  1.0f,  0.0f,
		0.0f,  0.0f,  0.0f,  0.0f
	};

	RwMatrix *transform = anim->dualTransform;
	if ( transform )
	{
		if ( !(transform->flags & rwMATRIXINTERNALIDENTITY) )
		{
			RwD3D8SetTextureStageState(n, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			memcpy(&texMat.m[0], &transform->right, sizeof(texMat.m[0]));
			memcpy(&texMat.m[1], &transform->up,    sizeof(texMat.m[1]));
			memcpy(&texMat.m[2], &transform->pos,   sizeof(texMat.m[2]));
			RwD3D8SetTransform(D3DTS_TEXTURE0+n, &texMat);
		}
	}
}

void MatFXRender::SetVertexAlpha(RwBool enabled)
{
	if ( enabled )
	{
		if ( !VertexAlphaBlend )
		{
			VertexAlphaBlend = TRUE;
			_rwD3D8RenderStateVertexAlphaEnable(TRUE);
		}
	}
	else
	{
		if ( VertexAlphaBlend )
		{
			VertexAlphaBlend = FALSE;
			_rwD3D8RenderStateVertexAlphaEnable(FALSE);
		}
	}
}

void MatFXRender::MeshRenderDefault(RxD3D8InstanceData *instancedMesh, RwUInt32 flags)
{
	const RpMaterial	      *material;
	const rpMatFXMaterialData *matfx;
	const MatFXUVAnimData     *animData;

	material = instancedMesh->material;
	matfx = *MATFXMATERIALGETDATA(material);

	if ( matfx->flags == rpMATFXEFFECTDUALUVTRANSFORM )
		animData = &matfx->data[rpSECONDPASS].data.uvAnim;
	else
		animData = NULL;

	/*
	 * Textures
	 */
	if ( flags & (rxGEOMETRY_TEXTURED|rpGEOMETRYTEXTURED2) )
	{
		RwD3D8SetTexture(material->texture, 0);

		if ( animData )
			ApplyAnimTextureMatrix(animData, 0);
	}
	else
		RwD3D8SetTexture(NULL, 0);

	/*
	 * Set the Default Pixel shader
	 */
	RwD3D8SetPixelShader(NULL);

	SetVertexAlpha(instancedMesh->vertexAlpha || (0xFF != material->color.alpha));

	if ( instancedMesh->vertexAlpha )
		RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	else
		RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

	/*
     * Vertex shader
	 */
	RwD3D8SetVertexShader(instancedMesh->vertexShader);

	/*
	 * Render
	 */
	if ( LastVertexBuffer != instancedMesh->vertexBuffer )
	{
		RwD3D8SetStreamSource(0, instancedMesh->vertexBuffer, instancedMesh->stride);
		LastVertexBuffer = instancedMesh->vertexBuffer;
	}

	if ( instancedMesh->indexBuffer != NULL )
	{
		RwD3D8SetIndices(instancedMesh->indexBuffer, instancedMesh->baseIndex);
		/* Draw the indexed primitive */
		RwD3D8DrawIndexedPrimitive((D3DPRIMITIVETYPE)instancedMesh->primType, 0, instancedMesh->numVertices, 0, instancedMesh->numIndices);
	}
	else
		RwD3D8DrawPrimitive((D3DPRIMITIVETYPE)instancedMesh->primType, instancedMesh->baseIndex, instancedMesh->numVertices);

	if ( animData )
		RwD3D8SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void MatFXRender::MeshRenderDualPass_PS2(RxD3D8InstanceData *instancedMesh, RwUInt32 flags)
{
	const RpMaterial	      *material;
	const rpMatFXMaterialData *matfx;
	const MatFXDualData       *dualData;
	const MatFXUVAnimData     *animData;


	material = instancedMesh->material;
	matfx = *MATFXMATERIALGETDATA(material);

	if ( matfx->flags == rpMATFXEFFECTDUALUVTRANSFORM )
	{
		animData = &matfx->data[rpSECONDPASS].data.uvAnim;
		dualData = &matfx->data[rpTHIRDPASS].data.dual;
	}
	else
	{
		animData = NULL;
		dualData = &matfx->data[rpSECONDPASS].data.dual;
	}

	/*
	 * Textures
	 */
	if ( flags & (rxGEOMETRY_TEXTURED|rpGEOMETRYTEXTURED2) )
	{
		RwD3D8SetTexture(material->texture, 0);
		RwD3D8SetTexture(dualData->texture, 1);

		if ( animData )
		{
			ApplyAnimTextureMatrix(animData, 0);
			ApplyAnimTextureMatrix(animData, 1);
		}
	}
	else
	{
		RwD3D8SetTexture(NULL, 0);
		RwD3D8SetTexture(NULL, 1);
	}

	if ( !(flags & rpGEOMETRYTEXTURED2) )
		RwD3D8SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

	/*
	 * Set the Default Pixel shader
	 */
	RwD3D8SetPixelShader(dwBlendShaders[Blend2ID(dualData->dstBlendMode)][Blend2ID(dualData->srcBlendMode)]);

	SetVertexAlpha(instancedMesh->vertexAlpha || (0xFF != material->color.alpha));

	if ( instancedMesh->vertexAlpha )
		RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	else
		RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

	/*
     * Vertex shader
	 */
	RwD3D8SetVertexShader(instancedMesh->vertexShader);

	/*
	 * Render
	 */
	if ( LastVertexBuffer != instancedMesh->vertexBuffer )
	{
		RwD3D8SetStreamSource(0, instancedMesh->vertexBuffer, instancedMesh->stride);
		LastVertexBuffer = instancedMesh->vertexBuffer;
	}

	if ( instancedMesh->indexBuffer != NULL )
	{
		RwD3D8SetIndices(instancedMesh->indexBuffer, instancedMesh->baseIndex);
		/* Draw the indexed primitive */
		RwD3D8DrawIndexedPrimitive((D3DPRIMITIVETYPE)instancedMesh->primType, 0, instancedMesh->numVertices, 0, instancedMesh->numIndices);
	}
	else
		RwD3D8DrawPrimitive((D3DPRIMITIVETYPE)instancedMesh->primType, instancedMesh->baseIndex, instancedMesh->numVertices);


	RwD3D8SetPixelShader(NULL);
	RwD3D8SetTexture(NULL, 1);

	if ( !(flags & rpGEOMETRYTEXTURED2) )
		RwD3D8SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	if ( animData )
	{
		RwD3D8SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		RwD3D8SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}
}

void MatFXRender::MeshRenderDualPass_PCTwoPass(RxD3D8InstanceData *instancedMesh, RwUInt32 flags)
{
	const RpMaterial	      *material;
	const rpMatFXMaterialData *matfx;
	const MatFXDualData       *dualData;
	const MatFXUVAnimData     *animData;
	RwBlendFunction srcBlend;
	RwBlendFunction dstBlend;
	DWORD  zwrite, fog, fogcol;


	material = instancedMesh->material;
	matfx = *MATFXMATERIALGETDATA(material);

	if ( matfx->flags == rpMATFXEFFECTDUALUVTRANSFORM )
	{
		animData = &matfx->data[rpSECONDPASS].data.uvAnim;
		dualData = &matfx->data[rpTHIRDPASS].data.dual;
	}
	else
	{
		animData = NULL;
		dualData = &matfx->data[rpSECONDPASS].data.dual;
	}

	/*
	 * Textures
	 */
	if ( flags & (rxGEOMETRY_TEXTURED|rpGEOMETRYTEXTURED2) )
	{
		RwD3D8SetTexture(material->texture, 0);

		if ( animData )
			ApplyAnimTextureMatrix(animData, 0);
	}
	else
		RwD3D8SetTexture(NULL, 0);

	if ( !(flags & rpGEOMETRYTEXTURED2) )
		RwD3D8SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

	/*
	 * Set the Default Pixel shader
	 */
	RwD3D8SetPixelShader(NULL);

	SetVertexAlpha(instancedMesh->vertexAlpha || (0xFF != material->color.alpha));

	if ( instancedMesh->vertexAlpha )
		RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	else
		RwD3D8SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

	/*
     * Vertex shader
	 */
	RwD3D8SetVertexShader(instancedMesh->vertexShader);

	/*
	 * Render
	 */
	if ( LastVertexBuffer != instancedMesh->vertexBuffer )
	{
		RwD3D8SetStreamSource(0, instancedMesh->vertexBuffer, instancedMesh->stride);
		LastVertexBuffer = instancedMesh->vertexBuffer;
	}

	if ( instancedMesh->indexBuffer != NULL )
	{
		RwD3D8SetIndices(instancedMesh->indexBuffer, instancedMesh->baseIndex);
		/* Draw the indexed primitive */
		RwD3D8DrawIndexedPrimitive((D3DPRIMITIVETYPE)instancedMesh->primType, 0, instancedMesh->numVertices, 0, instancedMesh->numIndices);
	}
	else
		RwD3D8DrawPrimitive((D3DPRIMITIVETYPE)instancedMesh->primType, instancedMesh->baseIndex, instancedMesh->numVertices);

	/*
	 * Draw second pass
	 */
	{
		RwD3D8SetTexture(dualData->texture, 0);

		if ( animData )
			ApplyAnimTextureMatrix(animData, 0);

		SetVertexAlpha(TRUE);
		/*
		if (  (dualData->srcBlendMode == rwBLENDDESTCOLOR && dualData->dstBlendMode == rwBLENDZERO)
			||(dualData->srcBlendMode == rwBLENDZERO      && dualData->dstBlendMode == rwBLENDSRCCOLOR)
			||(dualData->srcBlendMode == rwBLENDDESTCOLOR && dualData->dstBlendMode == rwBLENDSRCCOLOR) )
		{
			RwD3D8SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		}*/

		RwRenderStateGet(rwRENDERSTATESRCBLEND, &srcBlend);
		RwRenderStateGet(rwRENDERSTATEDESTBLEND, &dstBlend);

		/*
		 * Set appropiate blending mode
		 */
		_rwD3D8RenderStateSrcBlend(dualData->srcBlendMode);
		_rwD3D8RenderStateDestBlend(dualData->dstBlendMode);

		RwD3D8GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		RwD3D8GetRenderState(D3DRS_FOGENABLE, &fog);

		if ( fog )
		{
			RwD3D8GetRenderState(D3DRS_FOGCOLOR, &fogcol);

			if ( dualData->dstBlendMode == rwBLENDONE )
				RwD3D8SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_RGBA(0, 0, 0, 0));
			else if ( dualData->srcBlendMode == rwBLENDDESTCOLOR || dualData->dstBlendMode == rwBLENDSRCCOLOR )
				RwD3D8SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_RGBA(255, 255, 255, 255));
		}

		if ( (flags & rxGEOMETRY_TEXTURED2) )
			RwD3D8SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 1);

		if ( instancedMesh->indexBuffer != NULL )
		{
			/* Draw the indexed primitive */
			RwD3D8DrawIndexedPrimitive((D3DPRIMITIVETYPE)instancedMesh->primType,
										0, instancedMesh->numVertices,
										0, instancedMesh->numIndices);
		}
		else
		{
			RwD3D8DrawPrimitive((D3DPRIMITIVETYPE)instancedMesh->primType,
								instancedMesh->baseIndex,
								instancedMesh->numVertices);
		}

		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, zwrite);

		if ( fog )
			RwD3D8SetRenderState(D3DRS_FOGCOLOR, fogcol);

		if ( (flags & rxGEOMETRY_TEXTURED2) )
			RwD3D8SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

		SetVertexAlpha(FALSE);

		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)srcBlend);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)dstBlend);
	}

	RwD3D8SetTexture(NULL, 1);

	if ( animData )
	{
		RwD3D8SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		RwD3D8SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}
}

void MatFXRender::MeshRenderDualPass(RxD3D8InstanceData *instancedMesh, RwUInt32 flags)
{
	const RpMaterial	      *material;
	rpMatFXMaterialData *matfx;
	MatFXDualData       *dualData;

	material = instancedMesh->material;
	matfx = *MATFXMATERIALGETDATA(material);

	if ( matfx->flags == rpMATFXEFFECTDUALUVTRANSFORM )
		dualData = &matfx->data[rpTHIRDPASS].data.dual;
	else
		dualData = &matfx->data[rpSECONDPASS].data.dual;

#ifndef DEVBUILD
	if ( Blend2ID(dualData->dstBlendMode) != -1 && Blend2ID(dualData->srcBlendMode) != -1
		&& dwBlendShaders[Blend2ID(dualData->dstBlendMode)][Blend2ID(dualData->srcBlendMode)]
#ifdef DEVBUILD
		&& !Settings::GetInstance()->bDebugNewDualTexUseFixedStates
#endif
			)
	{
		MeshRenderDualPass_PS2(instancedMesh, flags);
	}
	else
		MeshRenderDualPass_PCTwoPass(instancedMesh, flags);

#else

	static RwBlendFunction stateids[] =
	{
		rwBLENDZERO,
		rwBLENDONE,
		rwBLENDSRCALPHA,
		rwBLENDINVSRCALPHA,
		rwBLENDDESTALPHA,
		rwBLENDINVDESTALPHA
	};

	//dualData->dstBlendMode = stateids[Settings::GetInstance()->bDebugDualTexDst];
	//dualData->srcBlendMode = stateids[Settings::GetInstance()->bDebugDualTexSrc];


	if ( !Settings::GetInstance()->bDebugNewDualTexUseFixedStates )
	{
		if ( Blend2ID(dualData->dstBlendMode) != -1 && Blend2ID(dualData->srcBlendMode) != -1
			&& dwBlendShaders[Blend2ID(dualData->dstBlendMode)][Blend2ID(dualData->srcBlendMode)] )
			MeshRenderDualPass_PS2(instancedMesh, flags);
	}
	else
		MeshRenderDualPass_PCTwoPass(instancedMesh, flags);
#endif
}

void MatFXDualPassRender(RxD3D8InstanceData *instancedData, RwUInt32 flags, RwTexture *texture, RwTexture *dualTexture)
{
	MatFXRender *render = MatFXRender::GetInstance();

	render->Begin();

	if ( dualTexture )
		render->MeshRenderDualPass(instancedData, flags);
	else
		render->MeshRenderDefault(instancedData, flags);

	render->End();
}