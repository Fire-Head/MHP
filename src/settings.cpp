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
	DebugMenuAddVarBool32(MENUROOT, "New Renderer", &bNewRender, NULL);
	DebugMenuAddVarBool32(MENUROOT, "PS2 MatFX Env", &bMatFxEnv, NULL);
	DebugMenuAddVarBool32(MENUROOT, "PS2 MatFX DualPass", &bMatFxDualPass, NULL);
	DebugMenuAddVarBool32(MENUROOT, "Fix Halos", &bFixHalos, NULL);
	DebugMenuAddVarBool32(MENUROOT, "Old Halos", &bOldHalosStyle, NULL);
	DebugMenuAddVarBool32(MENUROOT, "UseableLight Fix", &bUseableLightFix, NULL);
	DebugMenuAddVarBool32(MENUROOT, "Skin Fallback Fix", &bSkinFallbackFix, NULL);

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
	bSkinFallbackFix=1;
	bLockAtVSync=0;
	bHide16bitResolution=1;
	nWindowPositionX=INT_MAX;
	nWindowPositionY=INT_MAX;
	bAlwaysOnTop=0;
	nWindowedWidth=640;
	nWindowedHeight=480;
	nBorderlessWindowedWidth=INT_MAX;
	nBorderlessWindowedHeight=INT_MAX;
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

#define CURSECTION "Main"
	bScreenFilter				= ini.Get(CURSECTION, "bYCbCrColorFilter", bScreenFilter);
	bPS2AlphaTest				= ini.Get(CURSECTION, "bPS2AlphaTest", bPS2AlphaTest);
	bNewRender					= ini.Get(CURSECTION, "bNewRenderer", bNewRender);
	bMatFxEnv					= ini.Get(CURSECTION, "bPS2MatFXEnv", bMatFxEnv);
	bMatFxDualPass				= ini.Get(CURSECTION, "bPS2MatFXDualPass", bMatFxDualPass);
	bFixHalos					= ini.Get(CURSECTION, "bFixHalos", bFixHalos);
	bOldHalosStyle				= ini.Get(CURSECTION, "bOldHalos", bOldHalosStyle);
	bUseableLightFix			= ini.Get(CURSECTION, "bUseableLight", bUseableLightFix);
	bPS2Brightness				= ini.Get(CURSECTION, "bPS2Brightness", bPS2Brightness);
	bPS2Damage					= ini.Get(CURSECTION, "bPS2Damage", bPS2Damage);
	bPS2CameraFX				= ini.Get(CURSECTION, "bPS2CameraFX", bPS2CameraFX);
#undef CURSECTION

#define CURSECTION "Misc"
	bCrosshairFix				= ini.Get(CURSECTION, "bCrosshairFix", bCrosshairFix);
	bSkinFallbackFix			= ini.Get(CURSECTION, "bSkinFallbackFix", bSkinFallbackFix);
#undef CURSECTION

#define CURSECTION "Display"
	bLockAtVSync				= ini.Get(CURSECTION, "bLockAtVSync", bLockAtVSync);
	bHide16bitResolution		= ini.Get(CURSECTION, "bHide16bitResolution", bHide16bitResolution);
	nWindowPositionX			= ini.Get(CURSECTION, "nWindowPositionX", nWindowPositionX);
	nWindowPositionY			= ini.Get(CURSECTION, "nWindowPositionY", nWindowPositionY);
	bAlwaysOnTop				= ini.Get(CURSECTION, "bAlwaysOnTop", bAlwaysOnTop);
	nWindowedWidth				= ini.Get(CURSECTION, "nWindowedWidth", nWindowedWidth);
	nWindowedHeight				= ini.Get(CURSECTION, "nWindowedHeight", nWindowedHeight);
	nBorderlessWindowedWidth	= ini.Get(CURSECTION, "nBorderlessWindowedWidth", nBorderlessWindowedWidth);
	nBorderlessWindowedHeight	= ini.Get(CURSECTION, "nBorderlessWindowedHeight", nBorderlessWindowedHeight);
#undef CURSECTION

#define CURSECTION "MSAA"
	bRwMSAASupport				= ini.Get(CURSECTION, "bRwMSAASupport", bRwMSAASupport);
	nMSAA						= ini.Get(CURSECTION, "nMSAA", nMSAA);
#undef CURSECTION
}

void Settings::Save()
{
	CIni ini;

#define CURSECTION "Main"
	ini.Set(CURSECTION, "bYCbCrColorFilter", bScreenFilter);
	ini.Set(CURSECTION, "bPS2AlphaTest", bPS2AlphaTest);
	ini.Set(CURSECTION, "bNewRenderer", bNewRender);
	ini.Set(CURSECTION, "bPS2MatFXEnv", bMatFxEnv);
	ini.Set(CURSECTION, "bPS2MatFXDualPass", bMatFxDualPass);
	
	ini.Set(CURSECTION, "bFixHalos", bFixHalos);
	ini.Set(CURSECTION, "bOldHalos", bOldHalosStyle);
	
	ini.Set(CURSECTION, "bUseableLight", bUseableLightFix);

	ini.Set(CURSECTION, "bPS2Brightness", bPS2Brightness);
	ini.Set(CURSECTION, "bPS2Damage", bPS2Damage);
	ini.Set(CURSECTION, "bPS2CameraFX", bPS2CameraFX);
#undef CURSECTION

#define CURSECTION "Misc"
	ini.Set(CURSECTION, "bCrosshairFix", bCrosshairFix);
	ini.Set(CURSECTION, "bSkinFallbackFix", bSkinFallbackFix);
#undef CURSECTION
	
#define CURSECTION "Display"
	ini.Set(CURSECTION, "bLockAtVSync", bLockAtVSync);
	ini.Set(CURSECTION, "bHide16bitResolution", bHide16bitResolution);
	ini.Set(CURSECTION, "nWindowPositionX", nWindowPositionX != INT_MAX ? ini.ToA(nWindowPositionX) : "");
	ini.Set(CURSECTION, "nWindowPositionY", nWindowPositionY != INT_MAX ? ini.ToA(nWindowPositionY) : "");
	ini.Set(CURSECTION, "bAlwaysOnTop", bAlwaysOnTop);
	ini.Set(CURSECTION, "nWindowedWidth", nWindowedWidth);
	ini.Set(CURSECTION, "nWindowedHeight", nWindowedHeight);
	ini.Set(CURSECTION, "nBorderlessWindowedWidth", nBorderlessWindowedWidth != INT_MAX ? ini.ToA(nBorderlessWindowedWidth) : "");
	ini.Set(CURSECTION, "nBorderlessWindowedHeight", nBorderlessWindowedHeight != INT_MAX ? ini.ToA(nBorderlessWindowedHeight) : "");
#undef CURSECTION

#define CURSECTION "MSAA"
	ini.Set(CURSECTION, "bRwMSAASupport", bRwMSAASupport);
	ini.Set(CURSECTION, "nMSAA", nMSAA);
#undef CURSECTION
}