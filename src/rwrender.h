#pragma once
#include "rw.h"

class VSRender
{
	enum
	{
		NUMLIGHTS              = 6,
		
		//

		VS_TRANSFORM_POS       = 0,
		VS_TRANSFORM_SIZE      = 4,
		
		VS_CONSTS_POS          = 4,
		VS_CONSTS_SIZE         = 1,
		
		VS_MATERIALCOLOR_POS   = 5,
		VS_MATERIALCOLOR_SIZE  = 1,
		
		VS_AMBIENTCOLOR_POS    = 6,
		VS_AMBIENTCOLOR_SIZE   = 1,
		
		VS_LIGHTS_POS          = 7,
		VS_LIGHTS_SIZE         = 5*NUMLIGHTS,
	};
	
	RwUInt32 VertexShaders[4];
#ifdef DEVBUILD
	RwUInt32 VertexShadersDebug[4];
#endif

	RwBool   bSetConstants;
	RwBool   bSetMaterialConstants;
	RwBool   bSetLightConstants;

	RwBool   VertexAlphaBlend;
	void    *LastVertexBuffer;
	RwUInt32 LastMaterialColor;
	RwReal   LastAmbientCoef;
    RwReal   LastDiffuseCoef;

	// shader data
	D3DXMATRIX matWorldViewProj;
	RwV4d MaterialColor;
	RwV4d AmbientColor;
	struct
	{
		RwV4d    Position;
		RwV4d    Direction;
		RwV4d    Angle;
		RwV4d    Color;
		RwV4d    Attenuation;
	}Lights[NUMLIGHTS];
	
private:
	void UpdateLighting(void *object, RwUInt32 type);
	void UpdateLightsColors(RwReal ambientCoef, RwReal diffuseCoef);
	void UpdateMaterialColor(const RwRGBA *color);
	void UpdateShaderConstantsIfNeeded();
	
public:
	void Initialise();
	void Shutdown();

	void Begin(void *object, RwUInt32 type);
	void End();
	void SetVertexAlpha(RwBool enabled);
	void SetSurfaceProperties(const RwRGBA *color, const RwSurfaceProperties *surfaceProps, RwBool modulate);
	void MeshRender(RxD3D8InstanceData *instancedMesh, RwUInt32 flags);
	
	static VSRender *GetInstance();
};

void _rxD3D8VSRenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags);
