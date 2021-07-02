#pragma once
#include "common.h"

class Settings
{
public:
	int bScreenFilter;
	int bDualPass;
	int bNewRender;
	int bFixHalos;
	int bOldHalosStyle;
	int bMatFxEnv;
	int bUseableLightFix;
#ifdef DEVBUILD
	int bDebugShader;
	int bDebugPixelShader;
	int bDebugDisableDirlight;
	int bDebugDisablePointlight;
	int bDebugDisableSpotlight;
	int bDebugDisableVanillaLight;
	int bDebugShowMaterialMod;
#endif

	Settings() { Reset(); }

	void FillMenu();
	void Reset();
	void Save();
	void Load();

	static Settings *GetInstance();
};

