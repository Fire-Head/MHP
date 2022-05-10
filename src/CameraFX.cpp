#include "common.h"
#include "rw.h"
#include "game.h"
#include "render\helpers.h"
#include "CameraFX.h"
#include "settings.h"
#include "CPatch.h"


RwRaster *patch__FXBufferCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags)
{
	return RwRasterCreate((int)CFrontend::ms_scrn.fWidth, (int)CFrontend::ms_scrn.fHeight, depth, flags);
}

int patch__CFrontend_IsCameraFXActive()
{
	return !gCameraGlobals.bExecutionStatus && gCameraGlobals.bCutsceneCamera;
}

void patch__CScene_Render(RwCamera *cam)
{
	RwCameraEndUpdate(cam);

	if ( CFrontend::ms_FXModeMenu && !gCameraGlobals.bExecutionStatus && gCameraGlobals.bHandCamSetVideoEffectScrollBar )
	{
		RwCameraBeginUpdate(cam);
		CRenderer::BlackBoxDraw(CFrontend::ms_FXModeFade);
		RwCameraEndUpdate(cam);
	}
}

RwCamera *patch__CScene_RenderStart(RwCamera *camera, RwRGBA *colour, RwInt32 clearMode)
{
	if ( !CFrontend::ms_EnableZClearOnly )
	{
		if ( CFrontend::ms_FXModeMenu && (CFrontend::ms_Nightvision || !CFrontend::ms_FXArtFilter) )
			return RwCameraClear(camera, colour, clearMode);
	}

	return NULL;
}

STARTPATCH
	{
		DBG("CameraFx\n");

		if ( Settings::GetInstance()->bPS2CameraFX )
		{
			// remove pc stuff
			CPatch::Nop(0x5E916D, 0x5E9177-0x5E916D);
			CPatch::Nop(0x5E9180, 0x5E918A-0x5E9180);
			CPatch::Nop(0x5E92EC, 0x5E92F6-0x5E92EC);

			// make the menu option able to disable in-game fx correctly
			CPatch::SetPointer(0x4758D7+2, (void *)0x7D6B30);
			CPatch::SetPointer(0x5E3447+1, (void *)0x7D6B30);
			CPatch::SetPointer(0x5F31EA+2, (void *)0x7D6B30);
			CPatch::SetPointer(0x5F57B4+2, (void *)0x7D6B30);
			CPatch::SetPointer(0x5F58A3+2, (void *)0x7D6B30);

			// ps2 exclusive code
			CPatch::RedirectCall(0x4758E8, patch__CScene_RenderStart);
			CPatch::RedirectCall(0x47612F, patch__CScene_Render);

			// restore ps2 behavior
			CPatch::RedirectJump(0x5EA2C0, patch__CFrontend_IsCameraFXActive);

			// set pc resolution
			CPatch::RedirectCall(0x5E511A, patch__FXBufferCreate);
			CPatch::RedirectCall(0x5E5135, patch__FXBufferCreate);
		}
	}
ENDPATCH