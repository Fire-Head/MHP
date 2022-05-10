#include "common.h"
#include "settings.h"
#include "game.h"
#include "render\helpers.h"
#include "render\im2d.h"
#include "ColorFilter.h"
#include "CPatch.h"

#define  dwPixelShader dwYCbCrPixelShader
#include "render\shaders\YCbCr.h"
#undef   dwPixelShader

#define  dwPixelShader dwDebugPixelShader
#include "render\shaders\debugPS.h"
#undef   dwPixelShader

RwUInt32  ColorFilter::PS;
RwUInt32  ColorFilter::debugPS;

ColorFilter *ColorFilter::GetInstance()
{
	static ColorFilter filter;
	return &filter;
}

void
ColorFilter::Initialise()
{
	Index[0] = 0;
	Index[1] = 1;
	Index[2] = 2;
	Index[3] = 0;
	Index[4] = 2;
	Index[5] = 3;

	{
		RwBool isok = RwD3D8CreatePixelShader((RwUInt32*)dwYCbCrPixelShader, &PS);

		DBG("%s:\t%s\n", "YCbCrPS", isok ? "OK" : "FAILED");
	}

#ifdef DEVBUILD
	{
		RwBool isok = RwD3D8CreatePixelShader((RwUInt32*)dwDebugPixelShader, &debugPS);

		DBG("%s:\t%s\n", "DebugPS", isok ? "OK" : "FAILED");
	}
#endif
}

void
ColorFilter::Shutdown()
{
	RwD3D8DeletePixelShader(PS);
	PS = 0;

#ifdef DEVBUILD
	RwD3D8DeletePixelShader(debugPS);
	debugPS = 0;
#endif
}

void
ColorFilter::CreateImmediateModeData(RwCamera *camera, RwRect *rect)
{
	RwReal w = RwReal(rect->w);
	RwReal h = RwReal(rect->h);
	RwReal U, V, u, v;
	RwUInt32 xSize, ySize;

	xSize = RwRasterGetWidth(pFrontBuffer);
	ySize = RwRasterGetHeight(pFrontBuffer);

	U = (w + 0.5f) / xSize;
	V = (h + 0.5f) / ySize;
	u =     (0.5f) / xSize;
	v =     (0.5f) / ySize;

	RwIm2DVertexSetScreenX(&Vertex[0], 0.0f);
	RwIm2DVertexSetScreenY(&Vertex[0], 0.0f);
	RwIm2DVertexSetScreenZ(&Vertex[0], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex[0], RwCameraGetNearClipPlane(camera));
	RwIm2DVertexSetRecipCameraZ(&Vertex[0], 1.0f/RwCameraGetNearClipPlane(camera));
	RwIm2DVertexSetU(&Vertex[0], u, 1.0f);
	RwIm2DVertexSetV(&Vertex[0], v, 1.0f);
	RwIm2DVertexSetIntRGBA(&Vertex[0], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex[1], 0.0f);
	RwIm2DVertexSetScreenY(&Vertex[1], (RwReal)RwCameraGetRaster(camera)->height);
	RwIm2DVertexSetScreenZ(&Vertex[1], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex[1], RwCameraGetNearClipPlane(camera));
	RwIm2DVertexSetRecipCameraZ(&Vertex[1], 1.0f/RwCameraGetNearClipPlane(camera));
	RwIm2DVertexSetU(&Vertex[1], u, 1.0f);
	RwIm2DVertexSetV(&Vertex[1], V, 1.0f);
	RwIm2DVertexSetIntRGBA(&Vertex[1], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex[2], (RwReal)RwCameraGetRaster(camera)->width);
	RwIm2DVertexSetScreenY(&Vertex[2], (RwReal)RwCameraGetRaster(camera)->height);
	RwIm2DVertexSetScreenZ(&Vertex[2], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex[2], RwCameraGetNearClipPlane(camera));
	RwIm2DVertexSetRecipCameraZ(&Vertex[2], 1.0f/RwCameraGetNearClipPlane(camera));
	RwIm2DVertexSetU(&Vertex[2], U, 1.0f);
	RwIm2DVertexSetV(&Vertex[2], V, 1.0f);
	RwIm2DVertexSetIntRGBA(&Vertex[2], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex[3], (RwReal)RwCameraGetRaster(camera)->width);
	RwIm2DVertexSetScreenY(&Vertex[3], 0.0f);
	RwIm2DVertexSetScreenZ(&Vertex[3], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex[3], RwCameraGetNearClipPlane(camera));
	RwIm2DVertexSetRecipCameraZ(&Vertex[3], 1.0f/RwCameraGetNearClipPlane(camera));
	RwIm2DVertexSetU(&Vertex[3], U, 1.0f);
	RwIm2DVertexSetV(&Vertex[3], v, 1.0f);
	RwIm2DVertexSetIntRGBA(&Vertex[3], 255, 255, 255, 255);
}

void
ColorFilter::UpdateFrontBuffer(RwCamera *camera)
{
	RwRasterPushContext(pFrontBuffer);
	RwRasterRenderFast(RwCameraGetRaster(camera), 0, 0);
	RwRasterPopContext();
}

void
ColorFilter::OverlayRender(RwCamera *camera, bool reset)
{
	RwRect rect = { 0, 0, 0, 0};
	rect.w = RwRasterGetWidth(RwCameraGetRaster(camera));
    rect.h = RwRasterGetHeight(RwCameraGetRaster(camera));

	if(!Settings::GetInstance()->bScreenFilter)
		return;

	if ( !reset )
		UpdateFrontBuffer(camera);

	CreateImmediateModeData(camera, &rect);

	Im2D *im2d = Im2D::GetInstance();

	if ( !reset )
	{
#ifdef DEVBUILD
		RwUInt32 shader = Settings::GetInstance()->bDebugPixelShader ? debugPS : PS;
#else
		RwUInt32 shader = PS;
#endif

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pFrontBuffer);
		im2d->SetPixelShader(shader);
		im2d->RenderIndexedPrimitive(Vertex, 4, Index, 6);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	}
	else
	{
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pFrontBuffer);
		im2d->RenderIndexedPrimitive(Vertex, 4, Index, 6);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	}
}

void
ColorFilter::Open(RwCamera *camera)
{
	RwUInt32 width, height, depth;

	width = RwRasterGetWidth(RwCameraGetRaster(camera));
	height = RwRasterGetHeight(RwCameraGetRaster(camera));
	depth = RwRasterGetDepth(RwCameraGetRaster(camera));

	pFrontBuffer = RwRasterCreate(width, height, depth, rwRASTERTYPECAMERATEXTURE);
}

void
ColorFilter::Close()
{
	if(pFrontBuffer)
	{
		RwRasterDestroy(pFrontBuffer);
		pFrontBuffer = NULL;
	}
}

void
ColorFilter::Render(RwCamera *camera, bool reset)
{
	if ( !camera )
		return;

	if ( !pFrontBuffer )
		return;

	if ( RwCameraBeginUpdate(camera) )
	{
		CRenderer::SetIngameInfoRenderStates(0);
		OverlayRender(camera, reset);
		CRenderer::SetIngameInfoRenderStates(1);

		RwCameraEndUpdate(camera);
	}
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

STARTPATCH
	{
		DBG("colorfilter\n");

		CALL(0x5E2420, CreateCamera, CreateCameraCB);
		CALL(0x489D55, DestroyCamera, DestroyCameraCB);
		CALL(0x5934B7, ShowRaster, ShowRasterCB);

		CALL(0x4C0F5B, CameraShowRaster, CameraShowRasterCB);
		CALL(0x4C1051, CameraShowRaster, CameraShowRasterCB);
	}
ENDPATCH