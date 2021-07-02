#include "common.h"
#include "settings.h"
#include "rwutils.h"
#include "rwlightfinder.h"
#include "rwrender.h"
#include "d3d8vertexshaderutils.h"
#ifdef LOADSHADERS
#include "shaderloader.h"
#endif

static DWORD
VertexShaderDeclarationArray[][32] =
{
	// 0
	#include "vsdecl.def"
	,
	
	// 1 rxGEOMETRY_PRELIT
	#define  WITHDIFFUSE
	#include "vsdecl.def"
	,
	#undef   WITHDIFFUSE
	
	// 2 rxGEOMETRY_NORMALS
	#define  WITHNORMAL
	#include "vsdecl.def"
	,
	#undef   WITHNORMAL
	
	// 3 rxGEOMETRY_PRELIT | rxGEOMETRY_NORMALS
	#define  WITHNORMAL
	#define  WITHDIFFUSE
	#include "vsdecl.def"
	#undef   WITHDIFFUSE
	#undef   WITHNORMAL
};

#ifndef LOADSHADERS

#define  dwVertexShader dwVertexShaderVS0
#include "shaders\VS0.h"
#undef   dwVertexShader

#define  dwVertexShader dwVertexShaderVS1
#include "shaders\VS1.h"
#undef   dwVertexShader

#define  dwVertexShader dwVertexShaderVS2
#include "shaders\VS2.h"
#undef   dwVertexShader

#define  dwVertexShader dwVertexShaderVS3
#include "shaders\VS3.h"
#undef   dwVertexShader

#ifdef DEVBUILD
#define  dwVertexShader dwVertexShaderDebug
#include "shaders\debug.h"
#undef   dwVertexShader
#endif

static DWORD *VertexShaderArray[] =
{
	dwVertexShaderVS0,
	dwVertexShaderVS1,
	dwVertexShaderVS2,
	dwVertexShaderVS3
};

#endif


VSRender *VSRender::GetInstance()
{
	static VSRender render;
	return &render;
}

void VSRender::Initialise()
{
	char buf[MAX_PATH];

	for ( int i = 0; i < ARRAY_SIZE(VertexShaders); i++ )
	{
		sprintf_s(buf, "VS%d.vso", i);
#ifdef LOADSHADERS
		bool isok = CreateVertexShaderFromFile(buf, (RwUInt32 *)VertexShaderDeclarationArray[i], &VertexShaders[i]);
#else
		bool isok = !!RwD3D8CreateVertexShader((RwUInt32 *)VertexShaderDeclarationArray[i], (RwUInt32*)VertexShaderArray[i], &VertexShaders[i], 0);
#endif
		DBG("%s:\t%s\n", buf, isok ? "OK" : "FAILED");
	}
#ifdef DEVBUILD
	for ( int i = 0; i < ARRAY_SIZE(VertexShadersDebug); i++ )
	{
		sprintf_s(buf, "debug.vso");
#ifdef LOADSHADERS
		RwBool isok = CreateVertexShaderFromFile(buf, (RwUInt32 *)VertexShaderDeclarationArray[i], &VertexShadersDebug[i]);
#else
		RwBool isok = RwD3D8CreateVertexShader((RwUInt32 *)VertexShaderDeclarationArray[i], (RwUInt32*)dwVertexShaderDebug, &VertexShadersDebug[i], 0);
#endif
		DBG("%s:\t%s\n", buf, isok ? "OK" : "FAILED");
	}
#endif
}

void VSRender::Shutdown()
{
	for ( int i = 0; i < ARRAY_SIZE(VertexShaders); i++ )
	{
		if (VertexShaders[i])
			RwD3D8DeleteVertexShader(VertexShaders[i]);
	}
#ifdef DEVBUILD
	for ( int i = 0; i < ARRAY_SIZE(VertexShadersDebug); i++ )
	{
		if (VertexShadersDebug[i])
			RwD3D8DeleteVertexShader(VertexShadersDebug[i]);
	}
#endif
}

void VSRender::UpdateLighting(void *object, RwUInt32 type)
{
	RpGeometryFlag	flags;

	if ( type == rpATOMIC )
		flags = (RpGeometryFlag)RpGeometryGetFlags(RpAtomicGetGeometry((RpAtomic *)object));
	else
		flags = (RpGeometryFlag)RpWorldGetFlags((const RpWorld *)RWSRCGLOBAL(curWorld));

	/* ...start ambient at black */
	AmbientColor.x = AmbientColor.y = AmbientColor.z = 0.0f;
	
	if ( flags & rxGEOMETRY_PRELIT )
		AmbientColor.w = 0.0f;
	else
		AmbientColor.w = 1.0f;

	/* if the object's light-able */
	if ( flags & rxGEOMETRY_LIGHT )
	{
		/* ...add to it if we find any ambients */
		if ( NULL != RWSRCGLOBAL(curWorld) )
		{
			_rpD3D8VertexShaderLights  lightsArray;
			RwBool haveLights;

			lightsArray.numDirectional = 0;
			lightsArray.numPoint = 0;
			lightsArray.numSpot = 0;

			if ( type == rpATOMIC )
				haveLights = FindAtomicLights((RpAtomic *)object, &AmbientColor, &lightsArray, (flags & rxGEOMETRY_NORMALS) != 0);
			else
				haveLights = FindSectorLights((const RpWorldSector *)object, &AmbientColor, &lightsArray);
			
			if ( haveLights )
			{
				const RwRGBAReal	*color;
				RwUInt32 n;
				RpLight *light;
				RwV3d	*at;
				RwV3d	*pos;
				RwReal	radiiConstant;
				RwReal	radius;
				RwReal	angle;
				RwReal	cosangle;
				RwUInt32 numLights;
				
				numLights = 0;
				
				/* Directionals */
				if ( numLights < NUMLIGHTS
#ifdef DEVBUILD
					&& !Settings::GetInstance()->bDebugDisableDirlight
#endif
					)
				{
					for ( n = 0; n < lightsArray.numDirectional; n++ )
					{
						if ( !(numLights < NUMLIGHTS) )
							break;
				
						light = lightsArray.directional[n];

						/* Set the lights direction, in object space */
						at = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(light)));

						_rwD3D8VSGetNormalInLocalSpace(at, (RwV3d *)&Lights[numLights].Direction);
						_rwD3D8VSGetNormalInLocalSpace(at, (RwV3d *)&Lights[numLights].Position);

						Lights[numLights].Position.x = -Lights[numLights].Position.x;
						Lights[numLights].Position.y = -Lights[numLights].Position.y;
						Lights[numLights].Position.z = -Lights[numLights].Position.z;

						/* Set the light color */
						color = RpLightGetColor(light);
						Lights[numLights].Color.x = color->red;
						Lights[numLights].Color.y = color->green;
						Lights[numLights].Color.z = color->blue;
						
						Lights[numLights].Angle.x = 0.0f;
						Lights[numLights].Angle.y = 1.0f;
						
						Lights[numLights].Angle.z = 0.0f;
						
						Lights[numLights].Attenuation.x = 1.0f;
						Lights[numLights].Attenuation.y = 0.0f;
						Lights[numLights].Attenuation.z = 0.0f;
						Lights[numLights].Attenuation.w = FLT_MAX;
						Lights[numLights].Angle.w		= -FLT_MAX;

						numLights++;
					}
				}

				/* Point */
				if ( numLights < NUMLIGHTS
#ifdef DEVBUILD
					&& !Settings::GetInstance()->bDebugDisablePointlight
#endif
					)
				{
					for ( n = 0; n < lightsArray.numPoint; n++ )
					{
						if ( !(numLights < NUMLIGHTS) )
							break;
						
						light = lightsArray.point[n];

						/* Set the light's position, in object space */
						pos = RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(light)));

						_rwD3D8VSGetPointInLocalSpace(pos, (RwV3d *)&Lights[numLights].Position);

						/* Set the light color */
						color = RpLightGetColor(light);
						Lights[numLights].Color.x = color->red;
						Lights[numLights].Color.y = color->green;
						Lights[numLights].Color.z = color->blue;

						Lights[numLights].Angle.x = 0.0f;
						Lights[numLights].Angle.y = 1.0f;
							
						Lights[numLights].Angle.z = 1.0f;
						
						
						/* and the radius */
						radius = RpLightGetRadius(light);
						_rwD3D8VSGetRadiusInLocalSpace(radius, &radiiConstant);
						
						radius = radiiConstant;
						RpD3D8AttenuationParams param;
						RpD3D8LightGetAttenuationParams(light, &param);
						
						Lights[numLights].Attenuation.x = param.constant;
						Lights[numLights].Attenuation.y = param.linear / radius;
						Lights[numLights].Attenuation.z = param.quadratic / (radius*radius);
						Lights[numLights].Attenuation.w = radius;
						Lights[numLights].Angle.w		= 1.0f / radius;

						numLights++;
					}
				}

				/* Spot */
				if ( numLights < NUMLIGHTS
#ifdef DEVBUILD
					&& !Settings::GetInstance()->bDebugDisableSpotlight
#endif
					 )
				{
					for ( n = 0; n < lightsArray.numSpot; n++ )
					{
						if ( !(numLights < NUMLIGHTS) )
							break;
						
						light = lightsArray.spot[n];


						/* Set the light's position, in object space */
						pos = RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(light)));

						_rwD3D8VSGetPointInLocalSpace(pos, (RwV3d *)&Lights[numLights].Position);

						/* Set the lights direction, in object space */
						at = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(light)));

						_rwD3D8VSGetNormalInLocalSpace(at, (RwV3d *)&Lights[numLights].Direction);

						/* Angles range */
						angle = RpLightGetConeAngle(light);

						cosangle = (RwReal)RwCos(angle);

						if ( RpLightGetType(light) == rpLIGHTSPOT || cosangle >= 0.999f )
						{
							Lights[numLights].Angle.x = (1.f / 0.001f);
							Lights[numLights].Angle.y = (-cosangle) * (Lights[numLights].Angle.x);
						}
						else
						{
							Lights[numLights].Angle.x = (1.f / (1.f - cosangle));
							Lights[numLights].Angle.y = (-cosangle) * (Lights[numLights].Angle.x);
						}

						/* Set the light color */
						color = RpLightGetColor(light);
						Lights[numLights].Color.x = color->red;
						Lights[numLights].Color.y = color->green;
						Lights[numLights].Color.z = color->blue;
						
						Lights[numLights].Angle.z = 1.0f;

						/* and the radius */
						radius = RpLightGetRadius(light);
						_rwD3D8VSGetRadiusInLocalSpace(radius, &radiiConstant);					
						
						radius = radiiConstant;
						RpD3D8AttenuationParams param;
						RpD3D8LightGetAttenuationParams(light, &param);
						
						Lights[numLights].Attenuation.x = param.constant;
						Lights[numLights].Attenuation.y = param.linear / radius;
						Lights[numLights].Attenuation.z = param.quadratic / (radius*radius);
						Lights[numLights].Attenuation.w = radius;
						Lights[numLights].Angle.w		= 1.0f / radius;

						numLights++;
					}
				}
			}
		}
	}	
}

void VSRender::UpdateLightsColors(RwReal ambientCoef, RwReal diffuseCoef)
{
	AmbientColor.x *= ambientCoef;
	AmbientColor.y *= ambientCoef;
	AmbientColor.z *= ambientCoef;
	
	for ( int i = 0; i < NUMLIGHTS; i++ )
	{
		Lights[i].Color.x *= diffuseCoef;
		Lights[i].Color.y *= diffuseCoef;
		Lights[i].Color.z *= diffuseCoef;
	}
	
	bSetLightConstants = TRUE;
}

void VSRender::UpdateMaterialColor(const RwRGBA *color)
{
	MaterialColor.x = (RwReal)(color->red)   / 255.0f;
	MaterialColor.y = (RwReal)(color->green) / 255.0f;
	MaterialColor.z = (RwReal)(color->blue)  / 255.0f;
	MaterialColor.w = (RwReal)(color->alpha) / 255.0f;
	
	bSetMaterialConstants = TRUE;
}

void VSRender::UpdateShaderConstantsIfNeeded()
{
	if ( bSetConstants )
	{
		RwD3D8SetVertexShaderConstant(VS_TRANSFORM_POS,		(void*)&matWorldViewProj,					 VS_TRANSFORM_SIZE);
		RwD3D8SetVertexShaderConstant(VS_CONSTS_POS,		(void*)&D3DXVECTOR4(0.0f, 0.3f, 0.5f, 1.0f), VS_CONSTS_SIZE);
		bSetConstants = FALSE;
	}
	
	if ( bSetMaterialConstants )
	{
		RwD3D8SetVertexShaderConstant(VS_MATERIALCOLOR_POS, (void*)&MaterialColor,						 VS_MATERIALCOLOR_SIZE);
		bSetMaterialConstants = FALSE;
	}
	
	if ( bSetLightConstants )
	{
		RwD3D8SetVertexShaderConstant(VS_AMBIENTCOLOR_POS,	(void*)&AmbientColor,					     VS_AMBIENTCOLOR_SIZE);
		RwD3D8SetVertexShaderConstant(VS_LIGHTS_POS,		(void*)Lights,						         VS_LIGHTS_SIZE);
		bSetLightConstants = FALSE;
	}
}

void VSRender::Begin(void *object, RwUInt32 type)
{
	AmbientColor.x = 0.0f;
	AmbientColor.y = 0.0f;
	AmbientColor.z = 0.0f;
	AmbientColor.w = 0.0f;
	
	for ( int i = 0; i < NUMLIGHTS; i++ )
	{
		Lights[i].Position.x = 0.0f;
		Lights[i].Position.y = 0.0f;
		Lights[i].Position.z = 0.0f;
		Lights[i].Position.w = 0.0f;

		Lights[i].Direction.x = 0.0f;
		Lights[i].Direction.y = 0.0f;
		Lights[i].Direction.z = 0.0f;
		Lights[i].Direction.w = 0.0f;
		
		Lights[i].Angle.x = 0.0f;
		Lights[i].Angle.y = 0.0f;
		Lights[i].Angle.z = 0.0f;
		Lights[i].Angle.w = 0.0f;
		
		Lights[i].Color.x = 0.0f;
		Lights[i].Color.y = 0.0f;
		Lights[i].Color.z = 0.0f;
		Lights[i].Color.w = 0.0f;
		
		Lights[i].Attenuation.x = 0.0f;
		Lights[i].Attenuation.y = 0.0f;
		Lights[i].Attenuation.z = 0.0f;
		Lights[i].Attenuation.w = 0.0f;
	}
	
	MaterialColor.x = 1.0f;
	MaterialColor.y = 1.0f;
	MaterialColor.z = 1.0f;
	MaterialColor.w = 1.0f;
	
	VertexAlphaBlend  = _rwD3D8RenderStateIsVertexAlphaEnable();
	LastVertexBuffer  = (void *)0xffffffff;
	LastMaterialColor = 0xFFFFFFFF;
	LastAmbientCoef = 1.0f;
	LastDiffuseCoef = 1.0f;
	
	_rwD3D8EnableClippingIfNeeded(object, type);
	
	RwMatrix *matrix = (type == rpATOMIC) ? RwFrameGetLTM(RpAtomicGetFrame(object)) : NULL;	
	_rwD3D8VSSetActiveWorldMatrix(matrix);
	_rwD3D8VSGetComposedTransformMatrix(&matWorldViewProj);
	
	UpdateLighting(object, type);
	
	bSetConstants		  = TRUE;
	bSetMaterialConstants = TRUE;
	bSetLightConstants	  = TRUE;
}

void VSRender::End()
{
	// not needed but looks cool
}

void VSRender::SetVertexAlpha(RwBool enabled)
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

void VSRender::SetSurfaceProperties(const RwRGBA *color, const RwSurfaceProperties *surfaceProps, RwBool modulate)
{
	static RwRGBA pink = {255,0,255,255};

	if ( 
	       (*((const RwInt32 *)&(LastAmbientCoef))) != (*((const RwInt32 *)&(surfaceProps->ambient))) 
		|| (*((const RwInt32 *)&(LastDiffuseCoef))) != (*((const RwInt32 *)&(surfaceProps->diffuse))) )
	{
		UpdateLightsColors((surfaceProps->ambient) / LastAmbientCoef, (surfaceProps->diffuse) / LastDiffuseCoef);
		LastAmbientCoef = surfaceProps->ambient;
		LastDiffuseCoef = surfaceProps->diffuse;			
	}
	
	if ( modulate )
	{
		if ( LastMaterialColor != *((const RwUInt32 *)color))
		{
			UpdateMaterialColor(color);
			
#ifdef DEVBUILD
			if ( Settings::GetInstance()->bDebugShowMaterialMod )
				UpdateMaterialColor(&pink);
#endif
			LastMaterialColor = *((const RwUInt32 *)color);
		}
	}
}

void VSRender::MeshRender(RxD3D8InstanceData *instancedMesh, RwUInt32 flags)
{
	const RpMaterial	*material;
	/*
	 * Set the Default Pixel shader
	 */
	RwD3D8SetPixelShader(NULL);
	
	material = instancedMesh->material;

	if ( material->texture != NULL )
		RwD3D8SetTexture(material->texture, 0);
	else
		RwD3D8SetTexture(NULL, 0);

	/*
	* Vertex shader
	*/	
	RwInt32 shaderid = ((flags & rxGEOMETRY_PRELIT)!= 0) * 1 + ((flags & rxGEOMETRY_NORMALS)!= 0) * 2;
#ifdef DEVBUILD
	RwUInt32 shader = Settings::GetInstance()->bDebugShader ? VertexShadersDebug[shaderid] : VertexShaders[shaderid];
#else
	RwUInt32 shader = VertexShaders[shaderid];
#endif
	RwD3D8SetVertexShader(shader);
	
	UpdateShaderConstantsIfNeeded();
	
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
}

void
_rxD3D8VSRenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
	RxD3D8ResEntryHeader *resEntryHeader;
	RxD3D8InstanceData *instancedData;
	RwInt32 numMeshes;
	
	VSRender *render = VSRender::GetInstance();
	
	render->Begin(object, type);

	/* Get the instanced data */
	resEntryHeader = (RxD3D8ResEntryHeader *)(repEntry + 1);
	instancedData = (RxD3D8InstanceData *)(resEntryHeader + 1);

	/* Get the number of meshes */
	numMeshes = resEntryHeader->numMeshes;
	
	while ( numMeshes-- )
	{
		const RpMaterial	*material;
		const RwRGBA		*matcolor;
		
		material = instancedData->material;
		matcolor = &(material->color);
		
		render->SetVertexAlpha(instancedData->vertexAlpha || (0xFF != matcolor->alpha));
		render->SetSurfaceProperties(matcolor, &material->surfaceProps, (flags & rxGEOMETRY_MODULATE) != 0);
		render->MeshRender(instancedData, flags);

		/* Move onto the next instancedData */
		instancedData++;
	}
	
	render->End();
}