#include "common.h"
#include "platform.h"
#include "game.h"
#include "render\helpers.h"
#include "VideoMode.h"
#include "settings.h"
#include "CPatch.h"
#include "res\resource.h"

struct VideoMode
{
	RwInt32 vidMode;
	RwChar  modeStr[100];
	RwVideoMode vidModemInfo;
};

VideoMode **gVideoModeList = NULL;
RwInt32 gNumVideoModes = 0;
RwInt32 gWindowedMode = -1;
RwInt32 gBorderlessWindowedMode = -1;
RECT gMonitorRect;

bool gbOnlyExclusiveVideoModes = false;
bool gbBorderlessWindowed = false;
bool gbWindowed = false;

float gfCursorX, gfCursorY;
int gInputMode = -1;


int VideoModeCmp(const void *a, const void *b)
{
	VideoMode *pa = *(VideoMode **)a;
	VideoMode *pb = *(VideoMode **)b;

	if (((pa->vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE)!=0) < ((pb->vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE)!=0) )  return -1;
	if (((pa->vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE)!=0) > ((pb->vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE)!=0) )  return 1;
	if (pa->vidModemInfo.width  < pb->vidModemInfo.width)  return -1;
	if (pa->vidModemInfo.width  > pb->vidModemInfo.width)  return 1;
	if (pa->vidModemInfo.height < pb->vidModemInfo.height) return -1;
	if (pa->vidModemInfo.height > pb->vidModemInfo.height) return 1;
	if (pa->vidModemInfo.depth  < pb->vidModemInfo.depth)  return -1;
	if (pa->vidModemInfo.depth  > pb->vidModemInfo.depth)  return 1;

	return 0;
}

void BuildVideoModeList()
{
	RwInt32             vidMode, numVidModes;
	RwVideoMode         vidModemInfo;

	if ( gVideoModeList != NULL )
		return;

	numVidModes = RwEngineGetNumVideoModes();

	gVideoModeList = (VideoMode **)RwCalloc(numVidModes, sizeof(VideoMode *), rwMEMHINTDUR_GLOBAL);

	/* Add the available video modes to the dialog */
	for (vidMode = 0; vidMode < numVidModes; vidMode++)
	{
		gVideoModeList[gNumVideoModes] = NULL;

		RwEngineGetVideoModeInfo(&vidModemInfo, vidMode);

		if ( vidModemInfo.width < 640 || vidModemInfo.height < 480 )
			continue;

		gVideoModeList[gNumVideoModes] = (VideoMode *)RwMalloc(sizeof(VideoMode), rwMEMHINTDUR_GLOBAL);
		memset(gVideoModeList[gNumVideoModes], 0, sizeof(VideoMode));

		if ( Settings::GetInstance()->bHide16bitResolution )
		{
			rwsprintf(gVideoModeList[gNumVideoModes]->modeStr, RWSTRING("%lu x %lu %s"),
					vidModemInfo.width, vidModemInfo.height,
					!gbOnlyExclusiveVideoModes && (vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE) ? RWSTRING(" ( Fullscreen )") : RWSTRING(""));
		}
		else
		{
			rwsprintf(gVideoModeList[gNumVideoModes]->modeStr, RWSTRING("%lu x %lu x %lu %s"),
					vidModemInfo.width, vidModemInfo.height, vidModemInfo.depth,
					!gbOnlyExclusiveVideoModes && (vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE) ? RWSTRING(" ( Fullscreen )") : RWSTRING(""));
		}

		if ( !(vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE) )
		{
			if ( Settings::GetInstance()->bHide16bitResolution )
				rwsprintf(gVideoModeList[gNumVideoModes]->modeStr, RWSTRING("Windowed"));
			else
				rwsprintf(gVideoModeList[gNumVideoModes]->modeStr, RWSTRING("%lu bit color windowed"), vidModemInfo.depth);
		}

		gVideoModeList[gNumVideoModes]->vidMode = vidMode;

		RwEngineGetVideoModeInfo(&gVideoModeList[gNumVideoModes]->vidModemInfo, vidMode);

		gNumVideoModes++;
	}

	qsort (gVideoModeList, gNumVideoModes, sizeof(VideoMode*), VideoModeCmp);

	for ( int i = 0; i < gNumVideoModes; i++ )
	{
		if ( !(gVideoModeList[i]->vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE) )
		{
			gWindowedMode = gVideoModeList[i]->vidMode;
			break;
		}
	}
}

void FreeVideoModeList()
{
	if ( gVideoModeList )
	{
		for ( RwInt32 i = 0; i < gNumVideoModes; i++ )
		{
			if ( gVideoModeList[i] )
				RwFree(gVideoModeList[i]);
		}

		RwFree(gVideoModeList);

		gVideoModeList = NULL;
		gNumVideoModes = 0;
	}
}

void
dialogAddModes(HWND wndListVideMode)
{
	DBG("dialogAddModes\n");

	RwInt32             vidMode;

	BuildVideoModeList();

	/* Add the available video modes to the dialog */
	for (vidMode = 0; vidMode < gNumVideoModes; vidMode++)
	{
		if ( gbOnlyExclusiveVideoModes && !(gVideoModeList[vidMode]->vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE) )
			continue;
		if ( Settings::GetInstance()->bHide16bitResolution && gVideoModeList[vidMode]->vidModemInfo.depth == 16 )
			continue;

		DBG("%s\n", gVideoModeList[vidMode]->modeStr);

		int                 index;
		/* Add name and an index so we can ID it later */
		index =
			SendMessage(wndListVideMode, CB_ADDSTRING, 0, (LPARAM) gVideoModeList[vidMode]->modeStr);
		SendMessage(wndListVideMode, CB_SETITEMDATA, index, (LPARAM) gVideoModeList[vidMode]->vidMode);

		// Add fake mode for borderless windowed
		if ( !(gVideoModeList[vidMode]->vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE) )
		{
			index = SendMessage(wndListVideMode, CB_ADDSTRING, 0, (LPARAM) "Borderless Windowed");
			SendMessage(wndListVideMode, CB_SETITEMDATA, index, (LPARAM)gBorderlessWindowedMode);
		}
	}

	return;
}

void
dialogInit(HWND hDlg,
		   UINT message __RWUNUSED__,
		   WPARAM wParam __RWUNUSED__, LPARAM lParam __RWUNUSED__)
{
	HWND                wndList, wndListVideMode;
	RwInt32             subSysNum;
	RwInt32             videModeNum;
	RwInt32             videModeCount;
	RwBool              firstLaunch;
	RwInt32             vmSel;

	FreeVideoModeList();

	firstLaunch = FALSE;

	REGGETDWORDVALUE("VideoMode", "Device", GcurSel,   0);
	REGGETDWORDVALUE("VideoMode", "Mode",   GcurSelVM, 'NONE');

	if ( GcurSelVM == 'NONE' )
	{
		GcurSelVM = 0;
		firstLaunch = TRUE;
	}

	if ( GcurSel >= GnumSubSystems )
		GcurSel = 0;

	RwEngineSetSubSystem(GcurSel);

	if ( GcurSelVM >= RwEngineGetNumVideoModes() )
		GcurSelVM = 0;

	/* Handle the list box */
	wndList = GetDlgItem(hDlg, IDC_DEVICESEL);
	wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);

	/* Add the names of the sub systems to the dialog */
	for (subSysNum = 0; subSysNum < GnumSubSystems; subSysNum++)
	{
		/* Add name and an index so we can ID it later */
		SendMessage(wndList, CB_ADDSTRING, 0,
					(LPARAM) GsubSysInfo[subSysNum].name);
		SendMessage(wndList, CB_SETITEMDATA, subSysNum, (LPARAM) subSysNum);
	}
	SendMessage(wndList, CB_SETCURSEL, GcurSel, 0);

	/* display avalible modes */
	dialogAddModes(wndListVideMode);

	videModeCount = SendMessage(wndListVideMode, CB_GETCOUNT, 0, 0);
	vmSel         = videModeCount - 1;

	// try to find vm
	if ( !firstLaunch )
	{
		for ( videModeNum = 0; videModeNum < videModeCount; videModeNum++ )
		{
			if ( SendMessage(wndListVideMode, CB_GETITEMDATA, videModeNum, 0) == GcurSelVM )
			{
				vmSel = videModeNum;
				break;
			}
		}
	}

	SendMessage(wndListVideMode, CB_SETCURSEL, vmSel, 0);

	GcurSelVM = SendMessage(wndListVideMode, CB_GETITEMDATA, vmSel, 0);

	SetFocus(wndList);

	return;
}

static void
dialogTerm()
{
	FreeVideoModeList();
}

static void
dialogDevSelect(HWND hDlg,
				UINT message __RWUNUSED__,
				WPARAM wParam __RWUNUSED__, LPARAM lParam __RWUNUSED__)
{
	HWND                wndList, wndListVideMode;
	RwInt32             selection;
	RwInt32             videModeCount;
	RwInt32             vmSel;

	/* Handle the list box */
	wndList = GetDlgItem(hDlg, IDC_DEVICESEL);
	wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);

	/* Update the selected entry */
	selection = SendMessage(wndList, CB_GETCURSEL, 0, 0);
	if (selection != GcurSel)
	{
		FreeVideoModeList();

		GcurSel = SendMessage(wndList, CB_GETITEMDATA, selection, 0);

		RwEngineSetSubSystem(GcurSel);

		wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);
		/* changed device so update video modes listbox */
		SendMessage(wndListVideMode, CB_RESETCONTENT, 0, 0);

		/* display avalible modes */
		dialogAddModes(wndListVideMode);

		videModeCount = SendMessage(wndListVideMode, CB_GETCOUNT, 0, 0);
		vmSel         = videModeCount - 1;

		SendMessage(wndListVideMode, CB_SETCURSEL, vmSel, 0);

		GcurSelVM = SendMessage(wndListVideMode, CB_GETITEMDATA, vmSel, 0);
	}
}

static BOOL CALLBACK
DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			dialogInit(hDlg, message, wParam, lParam);

			return FALSE;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_DEVICESEL:
				{
					dialogDevSelect(hDlg, message, wParam, lParam);

					return TRUE;
				}

				case IDC_VIDMODE:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						HWND                wndListVideMode;
						RwInt32             vmSel;

						wndListVideMode =
							GetDlgItem(hDlg, IDC_VIDMODE);

						/* Update the selected entry */
						vmSel =
							SendMessage(wndListVideMode,
										CB_GETCURSEL, 0, 0);
						GcurSelVM =
							SendMessage(wndListVideMode,
										CB_GETITEMDATA, vmSel, 0);
					}

					return TRUE;
				}

				case IDOK:
				{
					if (HIWORD(wParam) == BN_CLICKED)
					{
						REGSETDWORDVALUE("VideoMode", "Device", GcurSel);
						REGSETDWORDVALUE("VideoMode", "Mode",   GcurSelVM);

						if ( GcurSelVM == gWindowedMode )
							gbWindowed = true;
						else if ( GcurSelVM == gBorderlessWindowedMode )
						{
							gbBorderlessWindowed = true;
							GcurSelVM = gWindowedMode;
						}

						if ( gbWindowed || gbBorderlessWindowed )
						{
							HMONITOR hmon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
							MONITORINFO mi;
							mi.cbSize = sizeof(MONITORINFO);
							if ( GetMonitorInfo(hmon, &mi) )
								gMonitorRect = mi.rcMonitor;
							else
							{
								gMonitorRect.left = 0;
								gMonitorRect.top = 0;
								gMonitorRect.right  = GetSystemMetrics(SM_CXSCREEN);
								gMonitorRect.bottom = GetSystemMetrics(SM_CYSCREEN);
							}
						}
						
						dialogTerm();

						EndDialog(hDlg, TRUE);
					}

					return TRUE;
				}

				case IDEXIT:
				case IDCANCEL:
				{
					if (HIWORD(wParam) == BN_CLICKED)
					{
						dialogTerm();

						EndDialog(hDlg, FALSE);
					}

					return TRUE;
				}

				default:
				{
					return FALSE;
				}
			}
		}

		default:
		{
			return FALSE;
		}
	}

	return FALSE;
}

RwBool
psSelectDevice()
{
	HWND                hWnd;
	HINSTANCE           hInstance;
	RwVideoMode         vm;
	RwInt32             subSysNum;

	hWnd = Skel->ps->window;
	hInstance = Skel->ps->instance;

	if ( !useDefault )
	{
		GnumSubSystems = RwEngineGetNumSubSystems();
		if (!GnumSubSystems)
		{
			return FALSE;
		}

		/* Just to be sure ... */
		GnumSubSystems =
			(GnumSubSystems > MAX_SUBSYSTEMS) ? MAX_SUBSYSTEMS : GnumSubSystems;

		/* Get the names of all the sub systems */
		for (subSysNum = 0; subSysNum < GnumSubSystems; subSysNum++)
		{
			RwEngineGetSubSystemInfo(&GsubSysInfo[subSysNum], subSysNum);
		}

		/* Get the default selection */
		GcurSel = RwEngineGetCurrentSubSystem();
	}

	if ( !useDefault )
	{
		SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);

		/* Allow the user to choose */
		if (!(DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogProc)))
		{
			return FALSE;
		}
	}

	FreeVideoModeList();

	/* Set the driver to use the correct sub system */
	if (!RwEngineSetSubSystem(GcurSel))
	{
		return FALSE;
	}

	RwEngineGetVideoModeInfo(&vm, GcurSelVM);

	/* Set up the video mode and set the apps window
	* dimensions to match */
	if (!RwEngineSetVideoMode(GcurSelVM))
	{
		return FALSE;
	}

	if (vm.flags & rwVIDEOMODEEXCLUSIVE)
	{
		UINT refresh = GetBestRefreshRate(vm.width, vm.height, vm.depth);
		if ( refresh != (UINT)-1 )
			RwD3D8EngineSetRefreshRate((RwUInt32)refresh);
	}

	if (vm.flags & rwVIDEOMODEEXCLUSIVE)
	{
		Skel->maximumWidth = vm.width;
		Skel->maximumHeight = vm.height;
		Skel->width = vm.width;
		Skel->height = vm.height;
		Skel->ps->fullScreen = TRUE;
	}
	else
	{
		int monX = gMonitorRect.left;
		int monY = gMonitorRect.top;
		int monW = gMonitorRect.right  - gMonitorRect.left;
		int monH = gMonitorRect.bottom - gMonitorRect.top;

		int winX, winY, winW, winH;

		winX = Settings::GetInstance()->nWindowPositionX;
		winY = Settings::GetInstance()->nWindowPositionY;

		if ( gbBorderlessWindowed )
		{
			winW = Settings::GetInstance()->nBorderlessWindowedWidth  != INT_MAX
				? Settings::GetInstance()->nBorderlessWindowedWidth  : monW;
			winH = Settings::GetInstance()->nBorderlessWindowedHeight != INT_MAX
				? Settings::GetInstance()->nBorderlessWindowedHeight : monH;
		}
		else
		{
			winW = Settings::GetInstance()->nWindowedWidth;
			winH = Settings::GetInstance()->nWindowedHeight;
		}

		RECT rect;
		rect.left   = winX != INT_MAX ? winX : max(monX, monX + monW/2 - winW/2);
		rect.top    = winY != INT_MAX ? winY : max(monY, monY + monH/2 - winH/2);
		rect.right  = rect.left + winW;
		rect.bottom = rect.top  + winH;

		DWORD style = GetWindowLong(hWnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW;
		if ( !gbBorderlessWindowed )
			style |= WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX|WS_MAXIMIZEBOX);

		AdjustWindowRect(&rect, style, FALSE);

		SetWindowLong(hWnd, GWL_STYLE, style);

		SetWindowPos(hWnd, Settings::GetInstance()->bAlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, rect.left, rect.top,
			(rect.right  - rect.left),
			(rect.bottom - rect.top), SWP_SHOWWINDOW | SWP_FRAMECHANGED);

		GetClientRect(hWnd, &rect);
		Skel->maximumWidth = rect.right;
		Skel->maximumHeight = rect.bottom;
		Skel->width = rect.right;
		Skel->height = rect.bottom;

		Skel->ps->fullScreen = FALSE;
	}

	// fix icons
	HICON hicon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG,   (LPARAM)hicon);
	DestroyIcon(hicon);

	return TRUE;
}

void patch_D3D8SetPresentParameters()
{
	if ( Settings::GetInstance()->bLockAtVSync )
		Present.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	Present.EnableAutoDepthStencil = TRUE;
}

void __cdecl patch_MouseDraw(float posx, float posy, float scalex, float scaley, long R, long G, long B, long A, RwRaster *raster)
{
	if ( !gbWindowed )
		CRenderer::DrawQuad2d(posx, posy, scalex, scaley, R, G, B, A, raster);
}

void __cdecl patch_MouseUpdate()
{
	if ( gbWindowed )
	{
		CFrontendMenu::Mouse.fPointerX = gfCursorX;
		CFrontendMenu::Mouse.fPointerY = gfCursorY;
	}
	else
	{
		CFrontendMenu::Mouse.fPointerX += CFrontendMenu::Mouse.fX;
		CFrontendMenu::Mouse.fPointerY += CFrontendMenu::Mouse.fY;
	}
}

HRESULT patch_InitialiseMouse(bool exclusive)
{
	if ( gInputMode != -1 )
		return _InputInitialiseMouse(gInputMode != 0 ? true : false);
	else
		return _InputInitialiseMouse(exclusive);
}

void _InputSetMouseMode(bool exclusive)
{
	HRESULT hr;

	if ( PSGLOBAL(mouse) )
	{
		PSGLOBAL(mouse)->Unacquire();

		if( FAILED( hr = PSGLOBAL(mouse)->SetCooperativeLevel( PSGLOBAL(window), (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND ) ) )
			return;

		PSGLOBAL(mouse)->Acquire();
	}
}

void SetInputMode()
{
	if ( gbWindowed
		&& ( CFrontendMenu::m_menuCurrent != -1 || CFrontendMenu::ms_dBox.draw != 0 || CGameInfo::m_gameInfo != 0 ) )
	{
		if ( gInputMode != 0 )
		{
			// inclusive
			gInputMode = 0;

			_InputSetMouseMode(false);
		}
	}
	else
	{
		if ( gInputMode != 1 )
		{
			// exclusive
			gInputMode = 1;

			_InputSetMouseMode(true);
		}
	}
}

void NAK patch_WM_KILLFOCUS()
{
	__asm
	{
		cmp     gbWindowed, 0
		jnz     _WINDOWED

		cmp     gbBorderlessWindowed, 0
		jnz     _WINDOWED

		mov     eax, 4C02E0h
		jmp     eax

_WINDOWED:
		mov     eax, 4C0813h
		jmp     eax
	}
}

void NAK patch_WM_ACTIVATE()
{
	__asm
	{
		cmp     gbWindowed, 0
		jnz     _WINDOWED

		mov     eax, 4C0530h
		jmp     eax

_WINDOWED:
		movzx   edi, bp
		test    di, di
		jnz     _ACTIVATE

		mov     eax, 4C0538h
		jmp     eax

_ACTIVATE:
		mov     eax, 4C0581h
		jmp     eax
	}
}

void NAK patch_WM_SETCURSOR()
{
	__asm
	{
		cmp     gbWindowed, 0
		jnz     _WINDOWED

		mov     eax, 4C02F0h
		jmp     eax

_WINDOWED:
		mov     eax, 4C02F8h
		jmp     eax
	}
}

void UpdateCursorPos(HWND hWnd, LPARAM lParam)
{
	POINTS p = *((POINTS*)&lParam);

	RECT r;
	GetClientRect(hWnd, &r);

	gfCursorX = float(p.x+1) / float(r.right);
	gfCursorY = float(p.y+1) / float(r.bottom);
}

void NAK patch_WM_MOUSEMOVE()
{
	__asm
	{
		push    ebx
		push    esi
		call    UpdateCursorPos
		add     esp, 8

		mov     eax, 4C0403h
		jmp     eax
	}
}

void NAK patch_WM_MOUSEBUTTONDOWN()
{
	__asm
	{
		push    ebx
		push    esi
		call    UpdateCursorPos
		add     esp, 8

		mov     eax, 4C03F0h
		jmp     eax
	}
}

void NAK patch_WM_MOUSEBUTTONUP()
{
	__asm
	{
		push    ebx
		push    esi
		call    UpdateCursorPos
		add     esp, 8

		mov     eax, 4C0410h
		jmp     eax
	}
}

STARTPATCH
	{
		DBG("VideoMode\n");

		CPatch::RedirectCall(0x4C3D80, psSelectDevice);

		// D3D8SetPresentParameters
		CPatch::Nop(0x64012A, 0x640134-0x64012A);
		CPatch::RedirectCall(0x64012A, patch_D3D8SetPresentParameters);

		// Frontend mouse
		CPatch::RedirectCall(0x5DB0C4, patch_MouseDraw);
		CPatch::RedirectCall(0x5DAFE4, patch_MouseUpdate);
		CPatch::RedirectJump(0x5DAFE4+5, (void*)0x5DB008);

		// WinMain dinput mouse mode
		CPatch::RedirectCall(0x609214, patch_InitialiseMouse);
		CPatch::RedirectCall(0x4C182A, SetInputMode);
		CPatch::RedirectJump(0x4C182A+5, (void *)0x4C1890);

		CPatch::RedirectJZ(0x4C0227, patch_WM_KILLFOCUS);
		CPatch::RedirectJZ(0x4C0215, patch_WM_ACTIVATE);
		CPatch::RedirectJZ(0x4C0242, patch_WM_SETCURSOR);
		CPatch::RedirectJZ(0x4C027C, patch_WM_MOUSEMOVE);
		CPatch::RedirectJZ(0x4C0285, patch_WM_MOUSEBUTTONDOWN);
		CPatch::RedirectJZ(0x4C0297, patch_WM_MOUSEBUTTONDOWN);
		CPatch::RedirectJZ(0x4C02A9, patch_WM_MOUSEBUTTONDOWN);
		CPatch::RedirectJZ(0x4C028E, patch_WM_MOUSEBUTTONUP);
		CPatch::RedirectJZ(0x4C02A0, patch_WM_MOUSEBUTTONUP);
		CPatch::RedirectJZ(0x4C02B2, patch_WM_MOUSEBUTTONUP);
	}
ENDPATCH