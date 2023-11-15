#include "common.h"
#include "platform.h"

RwSkel *&Skel = *(RwSkel**)0x736DB8;
RwBool &useDefault = *(RwBool*)0x735EC0;
RwSubSystemInfo (&GsubSysInfo)[MAX_SUBSYSTEMS] = *(RwSubSystemInfo (*)[MAX_SUBSYSTEMS])*(int *)(0x7359AC);
RwInt32 &GnumSubSystems = *(RwInt32*)0x735EAC;
RwInt32 &GcurSel = *(RwInt32*)0x735EB0;
RwInt32 &GcurSelVM = *(RwInt32*)0x735EB4;
RwInt32 &GcurSelVI = *(RwInt32*)0x735EB8;
RwBool &ForegroundApp = *(RwBool*)0x7359A8;


UINT GetBestRefreshRate(UINT width, UINT height, UINT depth)
{
	return ((UINT (__cdecl *)(UINT, UINT, UINT))0x4C0830)(width, height, depth);
}

HRESULT _InputInitialiseMouse(bool exclusive)
{
	return ((HRESULT (__cdecl *)(bool))0x4C1C60)(exclusive);
}