#define _USE_MATH_DEFINES
#include <Windows.h>
#include <stdint.h>

#include "CPatch.h"
#include "debugmenu_public.h"

#include "common.h"
#include "settings.h"
#include "rw.h"
#include "render\im2d.h"
#include "render\render.h"
#include "render\rendermatfx.h"
#include "ColorFilter.h"
#include "MSAASupport.h"
#ifdef LOADSHADERS
#include "shaderloader.h"
#endif

#pragma comment(linker, "/nodefaultlib:libcmt")

enum
{
	mhpID_ENGINEPLUGIN           = MAKECHUNKID(0x4846, 0x00),
};

DebugMenuAPI gDebugMenuAPI;


void InitialiseDebugMenu()
{
	DBG("init menu\n");
	if(DebugMenuLoad())
		Settings::GetInstance()->FillMenu();
}

static void *MHPOpen(void *instance, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{
#ifdef LOADSHADERS
	InitShaderList();
#endif
	Im2D::GetInstance()->Initialise();
	VSRender::GetInstance()->Initialise();
	MatFXRender::GetInstance()->Initialise();
	ColorFilter::GetInstance()->Initialise();

	return instance;
}

static void *MHPClose(void *instance, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{
	ColorFilter::GetInstance()->Shutdown();
	MatFXRender::GetInstance()->Shutdown();
	VSRender::GetInstance()->Shutdown();
	Im2D::GetInstance()->Shutdown();

#ifdef LOADSHADERS
	TermShaderList();
#endif

	return instance;
}

static RwBool (__cdecl *PluginAttachCB)();
RwBool __cdecl PluginAttach(void)
{
	if ( !PluginAttachCB() )
		return FALSE;

	if ( RwEngineRegisterPlugin(0, mhpID_ENGINEPLUGIN, MHPOpen, MHPClose) < 0 )
		return FALSE;

	if ( !MSAARasterPluginAttach() )
		return FALSE;

	return TRUE;
}

static int (__thiscall *RwInitialiseCB)(void*, int, void*);
int __fastcall RwInitialise(void *This, int edx0, int event, void *param)
{
	int result = RwInitialiseCB(This, event, param);
	InitialiseDebugMenu();
	return result;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if(reason == DLL_PROCESS_ATTACH)
	{
#ifdef DEVBUILD
		AllocConsole();

		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);

		printf("DllMain\n");
#endif
		Settings::GetInstance()->Load();

		CInitializer::Apply();

		CALL(0x4C1481, RwInitialise, RwInitialiseCB);
		CALL(0x4BDE00, PluginAttach, PluginAttachCB);
	}
	else if(reason == DLL_PROCESS_DETACH)
	{
#if 0
		Settings::GetInstance()->Save();
#endif
	}
	return TRUE;
}