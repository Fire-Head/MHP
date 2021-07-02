#include "common.h"
#include "settings.h"
#include "rwim2d.h"
#include "ColorFilter.h"

#define  dwPixelShader dwYCbCrPixelShader
#include "shaders\YCbCr.h"
#undef   dwPixelShader

#define  dwPixelShader dwDebugPixelShader
#include "shaders\debugPS.h"
#undef   dwPixelShader

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
	RwReal w = rect->w;
	RwReal h = rect->h;
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
		((void (__cdecl *)(long))0x5F5A80)(0);
		OverlayRender(camera, reset);
		((void (__cdecl *)(long))0x5F5A80)(1);

		RwCameraEndUpdate(camera);
	}
}