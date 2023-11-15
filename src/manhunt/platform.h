#pragma once
#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "rw.h"

#define RsGlobal (*Skel)
#define PSGLOBAL(var) (((RsGlobal.ps))->var)

#define MAX_SUBSYSTEMS      (16)


#define REGGETKEY(_result)                                                 \
{                                                                          \
	HKEY hMHKey = 0;                                                       \
	HKEY hSWKey = 0;                                                       \
	HKEY hRGKey = 0;                                                       \
	DWORD dwDisposition;                                                   \
	if ( !RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE", 0,                   \
						KEY_READ | KEY_WRITE, &hSWKey) )                   \
	{                                                                      \
		if ( !RegCreateKeyEx(hSWKey, "Rockstar Games", 0,                  \
				REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE,   \
				NULL, &hRGKey, &dwDisposition) )                           \
			RegCreateKeyEx(hRGKey, "Manhunt", 0, REG_NONE,                 \
							REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, \
							NULL, &hMHKey, &dwDisposition);                \
	}                                                                      \
	if ( hSWKey )                                                          \
		RegCloseKey(hSWKey);                                               \
	if ( hRGKey )                                                          \
		RegCloseKey(hRGKey);                                               \
	(_result) = hMHKey;                                                    \
}

#define REGGETDWORDVALUE(_section, _name, _val, _def)                      \
{                                                                          \
	DWORD val = _def;                                                      \
	HKEY hKey;                                                             \
	DWORD dwDisposition;                                                   \
	REGGETKEY(hKey)                                                        \
	if ( hKey )                                                            \
		RegCreateKeyEx(hKey, (_section), 0, REG_NONE,                      \
						REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE,     \
						NULL, &hKey, &dwDisposition);                      \
	if ( hKey )                                                            \
	{                                                                      \
		DWORD _size = sizeof(DWORD);                                       \
		LONG lr = RegQueryValueEx(hKey, (_name), NULL, NULL,               \
									(LPBYTE)&val, &_size);                 \
		if ( lr != ERROR_SUCCESS )                                         \
		{                                                                  \
			if ( lr == ERROR_FILE_NOT_FOUND )                              \
				RegSetValueEx(hKey, (_name), 0, REG_DWORD,                 \
								(const BYTE *)&val, sizeof(DWORD));        \
		}                                                                  \
		RegCloseKey(hKey);                                                 \
	}                                                                      \
	(_val) = val;                                                          \
}


#define REGSETDWORDVALUE(_section, _name, _val)                            \
{                                                                          \
	DWORD val = (_val);                                                    \
	HKEY hKey;                                                             \
	DWORD dwDisposition;                                                   \
	REGGETKEY(hKey)                                                        \
	if ( hKey )                                                            \
		RegCreateKeyEx(hKey, (_section), 0, REG_NONE,                      \
						REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE,     \
						NULL, &hKey, &dwDisposition);                      \
	if ( hKey )                                                            \
	{                                                                      \
		RegSetValueEx(hKey, (_name), 0, REG_DWORD,                         \
					(const BYTE *)&val, sizeof(DWORD));                    \
		RegCloseKey(hKey);                                                 \
	}                                                                      \
}

typedef enum RsInputDeviceType RsInputDeviceType;
typedef enum RsEventStatus RsEventStatus;
typedef enum RsEvent RsEvent;
typedef RsEventStatus (*RsInputEventHandler)(RsEvent event, void *param);

struct RsInputDevice
{
	RsInputDeviceType inputDeviceType;
	RwBool used;
	RsInputEventHandler inputEventHandler;
};

class CPCPlatform
{
public:
	DWORD                vtable;
	RwBool               fullScreen;
	RwV2d                lastMousePos;
	RwBool               loadingData;
	DWORD                field_14;
	HWND                 window;
	HINSTANCE            instance;
	LPDIRECTINPUT8       dinterface;
	LPDIRECTINPUTDEVICE8 mouse;
	LPDIRECTINPUTDEVICE8 joy1;
	LPDIRECTINPUTDEVICE8 joy2;
	LPDIRECTINPUTDEVICE8 keyboard;
};

struct RwSkel
{
	const RwChar *name;
	RwInt32       maximumWidth;
	RwInt32       maximumHeight;
	CPCPlatform  *ps;
	RwInt32       width;
	RwInt32       height;
	RwInt32       maxFPS;
	RwBool        quit;
	RsInputDevice keyboard;
	RsInputDevice mouse;
	RsInputDevice pad;
	void         *ptr;
	char          memoryOk;
	char          skipBink;
	char _pad[2];
};

extern RwSkel *&Skel;
extern RwBool &useDefault;
extern RwSubSystemInfo (&GsubSysInfo)[MAX_SUBSYSTEMS];
extern RwInt32 &GnumSubSystems;
extern RwInt32 &GcurSel;
extern RwInt32 &GcurSelVM;
extern RwInt32 &GcurSelVI;
extern RwBool &ForegroundApp;

UINT GetBestRefreshRate(UINT width, UINT height, UINT depth);
HRESULT _InputInitialiseMouse(bool exclusive);