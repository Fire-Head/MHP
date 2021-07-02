#define _USE_MATH_DEFINES
#include <Windows.h>
#include <stdint.h>
#include <math.h>

#include "CPatch.h"
#include "debugmenu_public.h"

#include "common.h"
#include "settings.h"
#include "rw.h"
#include "rwutils.h"
#include "rwim2d.h"
#include "rwrender.h"
#include "rwdefaultrender.h"
#include "rwmatfx.h"
#include "ColorFilter.h"
#ifdef LOADSHADERS
#include "shaderloader.h"
#endif

#pragma comment(linker, "/ENTRY:DllMain")
#pragma comment(linker, "/nodefaultlib:libcmt")
#pragma comment(lib, "msvcrt.lib")

DebugMenuAPI gDebugMenuAPI;

void NAK patch_centermouse()
{
	__asm
	{
		test    al, al
		jnz     loc_4C1890

		mov     eax, dword ptr ds: [7D2E28h] // gameIsRunning
		test    al, al
		jz      loc_4C1890

		mov     eax, dword ptr ds: [715BA0h] // ms_stepMode
		test    al, al
		jnz     loc_4C1890

		mov     ecx, dword ptr ds: [736DB8h]
		
		mov     eax, 4C185Ch
		jmp     eax

loc_4C1890:
		mov     eax, 4C1890h
		jmp     eax
	}
}

void CalcUseableLight(unsigned int This)
{
	float alpha;
	unsigned int& CGameTime__ms_currGameTime = *(unsigned int*)0x756270;
	
	float time = (float)CGameTime__ms_currGameTime / 1000.0f;
	float t = time - (float)(int)time;
	if ( t > 0.5f )
		t = 1.0f - t;
	
	float color = powf(2.0f, 4.0f) * ((t*t)*(t*t)) * 0.75f + 0.25f;
	
	RwRGBAReal light;
	light.red = light.green = light.blue = color;
	light.alpha = alpha;

	RwRGBARealAssign((RwRGBAReal*)(This + 0xCC), &light);
}

void NAK patch_UseableRenderLit()
{
	__asm
	{
		push    ebx
		call    CalcUseableLight
		pop     eax
	
		//
		
		fcompp
		
		mov     eax, 4D4EF1h
		jmp     eax
	}
}

void patch_DrawQuad3d()
{
	if ( !Settings::GetInstance()->bFixHalos ) return;
	// on ps2 it's always null here but not on pc, so let's simulate it
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
}

void patch_HaloStyle()
{
	if ( Settings::GetInstance()->bOldHalosStyle )
	{
		// old halo style	
		CPatch::SetChar(0x5FB567+1, rwBLENDONE);
		CPatch::SetChar(0x5FB576+1, rwBLENDONE);
	}
	else
	{
		// retail
		CPatch::SetChar(0x5FB567+1, rwBLENDSRCALPHA);
		CPatch::SetChar(0x5FB576+1, rwBLENDONE);
	}
}

void SwitchableRenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
	if ( Settings::GetInstance()->bNewRender )
		_rxD3D8VSRenderCallback(repEntry, object, type, flags);
	else
		_rxD3D8DefaultRenderCallback_fixed(repEntry, object, type, flags);
}

void InitialiseDebugMenu()
{
	DBG("init menu\n");
	if(DebugMenuLoad())
		Settings::GetInstance()->FillMenu();
}

static void *ShaderOpen(void *instance, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{
#ifdef LOADSHADERS
	InitShaderList();
#endif
	Im2D::GetInstance()->Initialise();
	VSRender::GetInstance()->Initialise();
	ColorFilter::GetInstance()->Initialise();

	return instance;
}

static void *ShaderClose(void *instance, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{
	ColorFilter::GetInstance()->Shutdown();
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

	if ( RwEngineRegisterPlugin(0, MAKECHUNKID(0x4846, 0), ShaderOpen, ShaderClose) < 0 )
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

static RwCamera *(*CreateCameraCB)(RwUInt32, RwUInt32, RwUInt32);
RwCamera *CreateCamera(RwUInt32 w, RwUInt32 h, RwUInt32 c)
{
	RwCamera *result = CreateCameraCB(w, h, c);
	ColorFilter::GetInstance()->Open(result);
	return result;
}

static void (__thiscall *DestroyCameraCB)(void*);
void __fastcall DestroyCamera(void *This)
{
	ColorFilter::GetInstance()->Close();
	DestroyCameraCB(This);
}

static void (__thiscall *ShowRasterCB)(void*, RwCamera *);
void __fastcall ShowRaster(void *This, int edx0, RwCamera *camera)
{
	ColorFilter::GetInstance()->Render(camera, false);
	ShowRasterCB(This, camera);
	ColorFilter::GetInstance()->Render(camera, true);
}

static RwCamera *(*CameraShowRasterCB)(RwCamera *camera, void *dev, RwUInt32 flags);
RwCamera *CameraShowRaster(RwCamera *camera, void *dev, RwUInt32 flags)
{
	ColorFilter::GetInstance()->Render(camera, false);
	RwCamera *cam = CameraShowRasterCB(camera, dev, flags);
	ColorFilter::GetInstance()->Render(camera, true);
	return cam;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline static void CALL(int address, void *func, T &CB)
{
	if ( *(int*)(address + 1) + (address + 5) != (int)func)
	{
		CB = reinterpret_cast <T>(*(int*)(address + 1) + (address + 5));
		CPatch::RedirectCall(address, func);
	}
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

#ifdef ADDWINDOWEDMODE
		// allow windowed mode
		CPatch::Nop(0x4BEE85, 6);
		
		// don't center mouse when in-menu
		CPatch::RedirectJump(0x4C1852, patch_centermouse);
#endif
		CALL(0x4C1481, RwInitialise, RwInitialiseCB);
		CALL(0x4BDE00, PluginAttach, PluginAttachCB);

		CALL(0x5E2420, CreateCamera, CreateCameraCB);
		CALL(0x489D55, DestroyCamera, DestroyCameraCB);
		CALL(0x5934B7, ShowRaster, ShowRasterCB);

		CALL(0x4C0F5B, CameraShowRaster, CameraShowRasterCB);
		CALL(0x4C1051, CameraShowRaster, CameraShowRasterCB);

		if ( Settings::GetInstance()->bUseableLightFix )
			CPatch::RedirectJump(0x4D4E10, patch_UseableRenderLit);
		
		CPatch::RedirectJump(0x5FB217, patch_DrawQuad3d);

		// _rxD3D8DefaultRenderCallback
		CPatch::RedirectJump(0x66B050, SwitchableRenderCallback);
		
		//_rpMatFXD3D8AtomicMatFXDefaultRender
		CPatch::RedirectJump(0x662C60, _rpMatFXD3D8AtomicMatFXDefaultRender_fixed);

		//_rpMatFXD3D8AtomicMatFXRenderBlack
		CPatch::RedirectJump(0x662DE0, _rpMatFXD3D8AtomicMatFXRenderBlack_fixed);
		
		// _rpMatFXD3D8AtomicMatFXEnvRender
		//CPatch::RedirectJump(0x663A30, _rpMatFXD3D8AtomicMatFXEnvRender_ps2);
		CPatch::RedirectCall(0x664713, _rpMatFXD3D8AtomicMatFXEnvRender_ps2);
		CPatch::RedirectCall(0x665228, _rpMatFXD3D8AtomicMatFXEnvRender_ps2);

		// dual pass patch for default pipelines
		struct{unsigned int address;bool isindexed;} DualPassPatches[] =
		{
			// _rxD3D8DefaultRenderCallback
			{0x66B2AA, true},{0x66B2C0, false},
			// _rpMatFXD3D8AtomicMatFXDefaultRender
			{0x662D90, true},{0x662DA6, false},
			// _rpMatFXD3D8AtomicMatFXRenderBlack
			{0x662E72, true},{0x662E88, false},
			// _rpMatFXD3D8AtomicMatFXEnvRender
			{0x663C1F,true},{0x663E6F,true},{0x663D07,false},{0x663D74,true},{0x663D87,false},{0x663DE6,true},{0x663DF9,false},{0x664048,true},{0x66405B,false},{0x663F64,true},{0x663F77,false}
		};

		for ( int i = 0; i < ARRAY_SIZE(DualPassPatches); i++ )
			CPatch::RedirectCall(DualPassPatches[i].address, ((DualPassPatches[i].isindexed) ? (void*)RwD3D8DrawIndexedPrimitive_DualPass : (void*)RwD3D8DrawPrimitive_DualPass));
	}
	else if(reason == DLL_PROCESS_DETACH)
	{
#if 0
		Settings::GetInstance()->Save();
#endif
	}
	return TRUE;
}