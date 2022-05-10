#include <Windows.h>
#include <stdint.h>
#include "debugmenu_public.h"
#include "Ini.h"
#include "settings.h"

#define MENUROOT "MHP"

void SaveIni()  { Settings::GetInstance()->Save(); }
void LoadIni()  { Settings::GetInstance()->Load(); }
void ResetIni() { Settings::GetInstance()->Reset(); }

Settings *Settings::GetInstance()
{
	static Settings sets;
	return &sets;
}

void Settings::FillMenu()
{
#if 0
	DebugMenuAddVarBool32(MENUROOT, "__bNewLight", &bNewLight, NULL);

	DebugMenuAddVarBool32(MENUROOT, "__bL1", &bL1, NULL);
	DebugMenuAddVarBool32(MENUROOT, "__bL2", &bL2, NULL);
	DebugMenuAddVarBool32(MENUROOT, "__bL3", &bL3, NULL);
	DebugMenuAddVarBool32(MENUROOT, "__bLFog", &bLFog, NULL);
#endif

	DebugMenuAddVarBool32(MENUROOT, "YCbCr", &bScreenFilter, NULL);
	DebugMenuAddVarBool32(MENUROOT, "PS2 Alpha Test", &bPS2AlphaTest, NULL);
	DebugMenuAddVarBool32(MENUROOT, "New Render", &bNewRender, NULL);
	DebugMenuAddVarBool32(MENUROOT, "PS2 MatFx Env", &bMatFxEnv, NULL);
	DebugMenuAddVarBool32(MENUROOT, "PS2 MatFx DualPass", &bMatFxDualPass, NULL);
	DebugMenuAddVarBool32(MENUROOT, "Fix Halos", &bFixHalos, NULL);
	DebugMenuAddVarBool32(MENUROOT, "Old Halos", &bOldHalosStyle, NULL);
	DebugMenuAddVarBool32(MENUROOT, "UseableLight Fix", &bUseableLightFix, NULL);

	DebugMenuAddVarBool32(MENUROOT"|MSAA", "MSAA", &bEnableMSAA, NULL);
	DebugMenuAddVarBool32(MENUROOT"|MSAA", "RW MSAA Support", &bRwMSAASupport, NULL);

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
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugNoDiffuse", &bDebugNoDiffuse, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "bDebugNoModulate", &bDebugNoModulate, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "bDebugNoNormals", &bDebugNoNormals, NULL);

	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugDamageDemo", &bDebugDamageDemo, NULL);
	DebugMenuAddInt32(MENUROOT"|Debug", "nDebugDamageDemoTime", &nDebugDamageDemoTime, NULL, 100, 0, 10000, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugDamageEffects", &bDebugDamageEffects, NULL);
	DebugMenuAddFloat32(MENUROOT"|Debug", "fDamageEffect1", &fDamageEffect1, NULL, 0.1f, 0.0f, 1.0f);
	DebugMenuAddFloat32(MENUROOT"|Debug", "fDamageEffect2", &fDamageEffect2, NULL, 0.1f, 0.0f, 1.0f);
	DebugMenuAddFloat32(MENUROOT"|Debug", "fDamageEffect3", &fDamageEffect3, NULL, 0.1f, 0.0f, 1.0f);

	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugNewDualTexRender", &bDebugNewDualTexRender, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugNewDualTexRenderPass1", &bDebugNewDualTexRenderPass1, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugNewDualTexRenderPass2", &bDebugNewDualTexRenderPass2, NULL);
	DebugMenuAddVarBool32(MENUROOT"|Debug", "DebugNewDualTexUseFixedStates", &bDebugNewDualTexUseFixedStates, NULL);
#endif
}

void Settings::Reset()
{
	CIni ini;

	nMSAA=1;
	bEnableMSAA=1;
	bRwMSAASupport=1;

	bScreenFilter=1;
	bPS2AlphaTest=1;
	bNewRender=1;
	bFixHalos=1;
	bOldHalosStyle=0;
	bMatFxEnv=1;
	bMatFxDualPass=1;
	bUseableLightFix=1;
	bPS2Brightness=1;
	bPS2Damage=1;
	bPS2CameraFX=1;
	bCrosshairFix=1;
#if 0
	bNewLight=0;
	bL1=1;
	bL2=1;
	bL3=1;
	bLFog=1;
#endif

#ifdef DEVBUILD
	bDebugShader=0;
	bDebugPixelShader=0;
	bDebugDisableDirlight=0;
	bDebugDisablePointlight=0;
	bDebugDisableSpotlight=0;
	bDebugDisableVanillaLight=0;
	bDebugShowMaterialMod=0;
	bDebugNoDiffuse=0;
	bDebugNoModulate=0;
	bDebugNoNormals=0;
	bDebugDamageDemo=0;
	nDebugDamageDemoTime=2000;
	bDebugDamageEffects=0;
	fDamageEffect1=1.0f;
	fDamageEffect2=1.0f;
	fDamageEffect3=1.0f;
	bDebugNewDualTexRender=1;
	bDebugNewDualTexRenderPass1=1;
	bDebugNewDualTexRenderPass2=1;
	bDebugNewDualTexUseFixedStates=0;
#endif
}

void Settings::Load()
{
	CIni ini;

	bScreenFilter            = ini.Get("Main", "bYCbCrColorFilter", bScreenFilter);
	bPS2AlphaTest            = ini.Get("Main", "bPS2AlphaTest", bPS2AlphaTest);
	bNewRender               = ini.Get("Main", "bNewRender", bNewRender);
	bMatFxEnv                = ini.Get("Main", "bPS2MatfxEnv", bMatFxEnv);
	bMatFxDualPass           = ini.Get("Main", "bPS2MatfxDualPass", bMatFxDualPass);

	bFixHalos                = ini.Get("Main", "bFixHalos", bFixHalos);
	bOldHalosStyle           = ini.Get("Main", "bOldHalos", bOldHalosStyle);

	bUseableLightFix         = ini.Get("Main", "bUseableLight", bUseableLightFix);

	bRwMSAASupport           = ini.Get("MSAA", "bRwMSAASupport", bRwMSAASupport);
	nMSAA                    = ini.Get("MSAA", "nMSAA", nMSAA);

	bPS2Brightness           = ini.Get("Other", "bPS2Brightness", bPS2Brightness);
	bPS2Damage               = ini.Get("Other", "bPS2Damage", bPS2Damage);
	bPS2CameraFX             = ini.Get("Other", "bPS2CameraFX", bPS2CameraFX);

	bCrosshairFix            = ini.Get("Other", "bCrosshairFix", bCrosshairFix);
}

void Settings::Save()
{
	CIni ini;

	ini.Set("Main", "bYCbCrColorFilter", bScreenFilter);
	ini.Set("Main", "bPS2AlphaTest", bPS2AlphaTest);
	ini.Set("Main", "bNewRender", bNewRender);
	ini.Set("Main", "bPS2MatfxEnv", bMatFxEnv);
	ini.Set("Main", "bPS2MatfxDualPass", bMatFxDualPass);
	
	ini.Set("Main", "bFixHalos", bFixHalos);
	ini.Set("Main", "bOldHalos", bOldHalosStyle);
	
	ini.Set("Main", "bUseableLight", bUseableLightFix);

	ini.Set("MSAA", "bRwMSAASupport", bRwMSAASupport);
	ini.Set("MSAA", "nMSAA", nMSAA);
	
	ini.Set("Other", "bPS2Brightness", bPS2Brightness);
	ini.Set("Other", "bPS2Damage", bPS2Damage);
	ini.Set("Other", "bPS2CameraFX", bPS2CameraFX);

	ini.Set("Other", "bCrosshairFix", bCrosshairFix);
}