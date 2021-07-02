#include <Windows.h>
#include <stdint.h>
#include "debugmenu_public.h"
#include "Ini.h"
#include "settings.h"

#define MENUROOT "MHP"

void SaveIni()  { Settings::GetInstance()->Save(); }
void LoadIni()  { Settings::GetInstance()->Load(); }
void ResetIni() { Settings::GetInstance()->Reset(); }
extern void patch_HaloStyle();

Settings *Settings::GetInstance()
{
	static Settings sets;
	return &sets;
}

void Settings::FillMenu()
{
	DebugMenuAddVarBool32(MENUROOT, "YCbCr", &bScreenFilter, NULL);
	DebugMenuAddVarBool32(MENUROOT, "Dual Pass", &bDualPass, NULL);
	DebugMenuAddVarBool32(MENUROOT, "New Render", &bNewRender, NULL);
	DebugMenuAddVarBool32(MENUROOT, "PS2 MatFx", &bMatFxEnv, NULL);
	DebugMenuAddVarBool32(MENUROOT, "Fix Halos", &bFixHalos, NULL);
	DebugMenuAddVarBool32(MENUROOT, "Old Halos", &bOldHalosStyle, patch_HaloStyle);

	DebugMenuAddCmd(MENUROOT, "Save Settings", SaveIni);
	DebugMenuAddCmd(MENUROOT, "ReLoad Settings", LoadIni);
	DebugMenuAddCmd(MENUROOT, "Reset Settings", ResetIni);

#ifdef DEVBUILD
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugShader", &bDebugShader, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugPixelShader", &bDebugPixelShader, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugDisableDirlight", &bDebugDisableDirlight, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugDisablePointlight", &bDebugDisablePointlight, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugDisableSpotlight", &bDebugDisableSpotlight, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugDisableVanillaLight", &bDebugDisableVanillaLight, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugShowMaterialMod", &bDebugShowMaterialMod, NULL);
#endif
}

void Settings::Reset()
{
	CIni ini;

	bScreenFilter=1;
	bDualPass=1;
	bNewRender=1;
	bFixHalos=1;
	bOldHalosStyle=0;
	bMatFxEnv=1;
	bUseableLightFix=1;
#ifdef DEVBUILD
	bDebugShader=0;
	bDebugPixelShader=0;
	bDebugDisableDirlight=0;
	bDebugDisablePointlight=0;
	bDebugDisableSpotlight=0;
	bDebugDisableVanillaLight=0;
	bDebugShowMaterialMod=0;
#endif

	patch_HaloStyle();
}

void Settings::Load()
{
	CIni ini;

	bScreenFilter            = ini.Get("Main", "bYCbCrColorFilter", bScreenFilter);
	bDualPass                = ini.Get("Main", "bDualPass", bDualPass);
	bNewRender               = ini.Get("Main", "bNewRender", bNewRender);
	bMatFxEnv                = ini.Get("Main", "bPS2Matfx", bMatFxEnv);
	bFixHalos                = ini.Get("Main", "bFixHalos", bFixHalos);
	bOldHalosStyle           = ini.Get("Main", "bOldHalos", bOldHalosStyle);

	bUseableLightFix         = ini.Get("Main", "bUseableLight", bUseableLightFix);

	patch_HaloStyle();
}

void Settings::Save()
{
	CIni ini;

	ini.Set("Main", "bYCbCrColorFilter", bScreenFilter);
	ini.Set("Main", "bDualPass", bDualPass);
	ini.Set("Main", "bNewRender", bNewRender);
	ini.Set("Main", "bPS2Matfx", bMatFxEnv);
	ini.Set("Main", "bFixHalos", bFixHalos);
	ini.Set("Main", "bOldHalos", bOldHalosStyle);

	ini.Set("Main", "bUseableLight", bUseableLightFix);
}