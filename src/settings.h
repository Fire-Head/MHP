#pragma once
#include "common.h"

class Settings
{
public:
	int nMSAA;
	int bEnableMSAA;
	int bRwMSAASupport;

	int bScreenFilter;
	int bPS2AlphaTest;
	int bNewRender;
	int bFixHalos;
	int bOldHalosStyle;
	int bMatFxEnv;
	int bMatFxDualPass;
	int bUseableLightFix;
	int bPS2Brightness;
	int bPS2Damage;
	int bPS2CameraFX;
	int bCrosshairFix;
	int bSkinFallbackFix;

	int bLockAtVSync;
	int bHide16bitResolution;
	int nWindowPositionX;
	int nWindowPositionY;
	int bAlwaysOnTop;
	int nWindowedWidth;
	int nWindowedHeight;
	int nBorderlessWindowedWidth;
	int nBorderlessWindowedHeight;
#if 0
	int bNewLight;

	int bL1;
	int bL2;
	int bL3;
	int bLFog;
#endif

#ifdef DEVBUILD
	int bDebugShader;
	int bDebugPixelShader;
	int bDebugDisableDirlight;
	int bDebugDisablePointlight;
	int bDebugDisableSpotlight;
	int bDebugDisableVanillaLight;
	int bDebugShowMaterialMod;
	int bDebugNoDiffuse;
	int bDebugNoModulate;
	int bDebugNoNormals;
	int bDebugDamageDemo;
	int nDebugDamageDemoTime;
	int bDebugDamageEffects;
	float fDamageEffect1;
	float fDamageEffect2;
	float fDamageEffect3;
	int bDebugNewDualTexRender;
	int bDebugNewDualTexRenderPass1;
	int bDebugNewDualTexRenderPass2;
	int bDebugNewDualTexUseFixedStates;
#endif

	Settings() { Reset(); }

	void FillMenu();
	void Reset();
	void Save();
	void Load();

	static Settings *GetInstance();
};

