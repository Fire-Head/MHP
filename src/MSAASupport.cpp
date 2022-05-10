#include "common.h"
#include "rw.h"
#include "render\helpers.h"
#include "MSAASupport.h"
#include "settings.h"
#include "CPatch.h"

enum
{
	mhpID_RASTERPLUGIN           = MAKECHUNKID(0x4846, 0x01),
};

#define RASTERPLUGINFROMRASTER(raster) \
    ((MSAARasterPlugin *)(((RwUInt8 *)(raster)) + MSAARasterPluginOffset))

struct MSAARasterPlugin
{
	LPSURFACE renderTarget;
};

RwInt32 MSAARasterPluginOffset;

static void *MSAARasterCtor(void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
	HRESULT     hr;
	RwUInt32    rasFormat;
	D3DFORMAT   colorFormat = (D3DFORMAT) 0;
	RwRaster    *raster;
	MSAARasterPlugin *plg;

	raster = (RwRaster *)object;
	plg = RASTERPLUGINFROMRASTER(object);

	plg->renderTarget = NULL;

	if ( !( raster->cType == rwRASTERTYPECAMERATEXTURE
		&& (raster->width) && (raster->height)
		&& !(raster->cFlags & rwRASTERDONTALLOCATE) ) )
	{
		RWRETURN(object);
	}

	rasFormat = RwRasterGetFormat(raster);

	if ((rasFormat & rwRASTERFORMATPAL4) || (rasFormat & rwRASTERFORMATPAL8))
		RWRETURN(object);;

	colorFormat = _rwRasterConvTable[FMT2TBL(rasFormat)].format;

	hr = IDirect3DDevice8_CreateRenderTarget(_RwD3DDevice, raster->width, raster->height, colorFormat, Present.MultiSampleType, FALSE, &plg->renderTarget);

	if ( hr == D3DERR_OUTOFVIDEOMEMORY )
	{
		IDirect3DDevice8_ResourceManagerDiscardBytes(_RwD3DDevice, 0);
		hr = IDirect3DDevice8_CreateRenderTarget(_RwD3DDevice, raster->width, raster->height, colorFormat, Present.MultiSampleType, FALSE, &plg->renderTarget);
	}

	if (hr != D3DERR_OUTOFVIDEOMEMORY && FAILED(hr))
		hr = DXCHECK(IDirect3DDevice8_CreateRenderTarget(_RwD3DDevice, raster->width, raster->height, _RwD3D8AdapterInformation.mode.Format, Present.MultiSampleType, FALSE, &plg->renderTarget));

	DBG("CreateCam r %X renderTarget %X(0x%X)\n", raster, plg->renderTarget, &plg->renderTarget);

	if (FAILED(hr))
		RWRETURN(object);;

	RWRETURN(object);
}

static void *MSAARasterDtor(void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
	RwRaster    *raster;
	MSAARasterPlugin *plg;

	raster = (RwRaster *)object;
	plg = RASTERPLUGINFROMRASTER(raster);

	if ( !( raster->cType == rwRASTERTYPECAMERATEXTURE
		&& !(raster->cFlags & rwRASTERDONTALLOCATE) ) )
	{
		RWRETURN(object);
	}

	if (plg->renderTarget)
	{
		IDirect3DSurface8_Release(plg->renderTarget);
		plg->renderTarget = NULL;
	}
	RWRETURN(object);
}

RwBool MSAARasterPluginAttach(void)
{
	MSAARasterPluginOffset = RwRasterRegisterPlugin(sizeof(MSAARasterPlugin), mhpID_RASTERPLUGIN, MSAARasterCtor, MSAARasterDtor, NULL);

	if (0 > MSAARasterPluginOffset)
		RWRETURN(FALSE);

	RWRETURN(TRUE);
}

void VideoMemoryRasterListRelease(void)
{
    RxD3D8VideoMemoryRaster *currentVideoRaster;

    currentVideoRaster = VideoMemoryRasters;
    while (currentVideoRaster)
    {
        RwRaster                *raster = currentVideoRaster->raster;
		MSAARasterPlugin        *plg  = RASTERPLUGINFROMRASTER(raster);
        RxD3D8VideoMemoryRaster *next = currentVideoRaster->next;

        if (raster->parent == raster)
        {
            switch(raster->cType)
            {
                case rwRASTERTYPECAMERATEXTURE:
                {
					if (plg->renderTarget)
					{
						IDirect3DSurface8_Release(plg->renderTarget);
						plg->renderTarget = NULL;
					}
                }
                break;

                default:
                    break;
            }
        }

        currentVideoRaster = next;
    }

    RWRETURNVOID();
}

RwBool VideoMemoryRasterListRestore()
{
    RwBool                  result;
    HRESULT                 hr;
    RxD3D8VideoMemoryRaster *currentVideoRaster;

    result = TRUE;
    hr = D3D_OK;
    currentVideoRaster = VideoMemoryRasters;
    while (currentVideoRaster)
    {
        RwRaster                *raster = currentVideoRaster->raster;
		MSAARasterPlugin        *plg  = RASTERPLUGINFROMRASTER(raster);
        RxD3D8VideoMemoryRaster *next = currentVideoRaster->next;

        if (raster->parent == raster)
        {
            switch(raster->cType)
            {
                case rwRASTERTYPECAMERATEXTURE:
                    {
						if (plg->renderTarget == NULL)
						{
							RwUInt32    rasFormat = RwRasterGetFormat(raster);
							RwUInt32    colorFormat = _rwRasterConvTable[FMT2TBL(rasFormat)].format;
							RwUInt32    levels = ((rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1);

							hr = DXCHECK(IDirect3DDevice8_CreateRenderTarget(_RwD3DDevice, raster->width, raster->height, (D3DFORMAT)colorFormat, Present.MultiSampleType, FALSE, &plg->renderTarget));


							if (FAILED(hr))
							{
								if ( hr != D3DERR_OUTOFVIDEOMEMORY )
								{
									hr = DXCHECK(IDirect3DDevice8_CreateRenderTarget(_RwD3DDevice, raster->width, raster->height, _RwD3D8AdapterInformation.mode.Format, Present.MultiSampleType, FALSE, &plg->renderTarget));
								}
							}
						}
					}
                    break;

                default:
                    break;
            }
        }

        if (FAILED(hr))
        {
            result = FALSE;
        }

        currentVideoRaster = next;
    }

    RWRETURN(result);
}

void NAK patch__VideoMemoryRasterListRestore()
{
	__asm
	{
		add     esp, 68h

		push    ecx
		call    VideoMemoryRasterListRestore
		pop     ecx

		test    ecx, ecx
		jz      RESTORE_FALSE

		test    eax, eax
		jz      RESTORE_FALSE

		mov     eax, 1
		retn

RESTORE_FALSE:
		xor     eax, eax
		retn
	}
}

RwBool CameraBeginUpdate(RwCamera *camera)
{
    RwRaster            *raster;
    RwRaster            *zRaster;
    RwRaster            *zTopRaster;
    _rwD3D8RasterExt    *zRasterExt;
    HRESULT             hr;

	raster = RwCameraGetRaster(camera);

	MSAARasterPlugin *plg = RASTERPLUGINFROMRASTER(raster);

	if ( plg->renderTarget && Settings::GetInstance()->bRwMSAASupport )
	{
		zRaster = RwCameraGetZRaster((RwCamera *) camera);
		if (zRaster)
		{
			/*
			* Got to get the top level raster as this is the only one with a
			* texture surface
			*/
			zTopRaster = RwRasterGetParent(zRaster);

			zRasterExt = RASTEREXTFROMRASTER(zTopRaster);

			hr = DXCHECK(IDirect3DDevice8_SetRenderTarget
						(_RwD3DDevice, plg->renderTarget,
						(LPDIRECT3DSURFACE8) zRasterExt->texture));
		}
		else
		{
			hr = DXCHECK(IDirect3DDevice8_SetRenderTarget
						(_RwD3DDevice, plg->renderTarget, NULL));
		}

		return TRUE;
	}

	return FALSE;
}

void NAK patch__CameraBeginUpdate()
{
	__asm
	{
		cmp     byte ptr [edx+20h], 5
		jnz     loc_6428BD

		mov     edx, dword ptr [esp+0E4h+8h]

		push    edx
		call    CameraBeginUpdate
		pop     edx

		test    eax, eax
		jz      CAMERABEGINUPDATE_DEFAULT

		jmp     loc_6428BD

CAMERABEGINUPDATE_DEFAULT:

		mov     eax, 642823h
		jmp     eax

loc_6428BD:
		mov     eax, 6428BDh
		jmp     eax
	}
}

RwBool CameraEndUpdate()
{
    RwRaster            *raster;
	RwRaster            *topRaster;
	 const _rwD3D8RasterExt  *topRasterExt;

    raster = RwCameraGetRaster(dgGGlobals.curCamera);

    topRaster = RwRasterGetParent(raster);
    topRasterExt = RASTEREXTFROMCONSTRASTER(topRaster);

	MSAARasterPlugin *plg = RASTERPLUGINFROMRASTER(raster);

	if ( plg->renderTarget && Settings::GetInstance()->bRwMSAASupport )
	{
		LPSURFACE           surfaceLevel;
		RECT                rectSrc;

		if (topRasterExt->cube)
		{
			IDirect3DCubeTexture8_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE8)topRasterExt->texture,
													(D3DCUBEMAP_FACES)topRasterExt->face,
													0,
													&surfaceLevel);
		}
		else
		{
			IDirect3DTexture8_GetSurfaceLevel(topRasterExt->texture, 0,
											&surfaceLevel);
		}

		/* Copy from the backbuffer to the camera texture */
		rectSrc.left = raster->nOffsetX;
		rectSrc.top = raster->nOffsetY;
		rectSrc.right = rectSrc.left + raster->width;
		rectSrc.bottom = rectSrc.top + raster->height;

		DXCHECK(IDirect3DDevice8_CopyRects
					(_RwD3DDevice, plg->renderTarget, &rectSrc,
					1, surfaceLevel, NULL));

		IDirect3DSurface8_Release(surfaceLevel);

		DXCHECK(IDirect3DDevice8_SetRenderTarget
						(_RwD3DDevice, _RwD3D8RenderSurface,
						_RwD3D8DepthStencilSurface));

		return TRUE;
	}

	return FALSE;
}

void NAK patch__CameraEndUpdate()
{
	__asm
	{
		cmp     byte ptr [eax+20h], 5
		jnz     loc_642A20

		push    eax
		push    ecx
		call    CameraEndUpdate

		test    eax, eax
		pop     ecx
		pop     eax

		jz      CAMERAENDUPDATE_DEFAULT

		jmp     loc_642A20

CAMERAENDUPDATE_DEFAULT:
		mov     edx, 642971h
		jmp     edx

loc_642A20:
		mov     eax, 642A20h
		jmp     eax
	}
}

RwBool CameraClear(RwCamera *camera, void *color, RwInt32 clearFlags)
{
	RwRaster           *raster;
    RwRaster           *topRaster;
    _rwD3D8RasterExt   *topRasterExt;
    RwRaster           *zRaster;
    RwRaster            *zTopRaster;
    _rwD3D8RasterExt    *zrasExt;
    D3DCOLOR            packedColor = 0;
    HRESULT             hr;

	static const DWORD  (&clearFlagsConvTable)[8] = *(DWORD (*)[8])*(int *)0x679A08;

	/* The clear color */
    if (rwCAMERACLEARIMAGE & clearFlags)
    {
        RwRGBA             *col = (RwRGBA *) color;

        /* Pack the clear color */
        packedColor = (D3DCOLOR) (((col->alpha) << 24) |
                                  ((col->red) << 16) |
                                  ((col->green) << 8) | (col->blue));
    }

	raster = RwCameraGetRaster(camera);

	topRaster = RwRasterGetParent(raster);

	topRasterExt = RASTEREXTFROMRASTER(topRaster);

	MSAARasterPlugin *plg = RASTERPLUGINFROMRASTER(raster);

	if ( plg->renderTarget && Settings::GetInstance()->bRwMSAASupport )
	{
		LPSURFACE           surfaceLevel;
		D3DVIEWPORT8        viewport;
		RwUInt32            d3d8ClearFlags;


		zRaster = RwCameraGetZRaster((RwCamera *) camera);
		if (zRaster)
		{
			/*
			* Got to get the top level raster as this is the only one with a
			* texture surface
			*/
			zTopRaster = RwRasterGetParent(zRaster);

			zrasExt = RASTEREXTFROMRASTER(zTopRaster);

			hr = DXCHECK(IDirect3DDevice8_SetRenderTarget
						(_RwD3DDevice, plg->renderTarget,
						(LPSURFACE) zrasExt->texture));
		}
		else
		{
			hr = DXCHECK(IDirect3DDevice8_SetRenderTarget
						(_RwD3DDevice, plg->renderTarget, NULL));
		}

		/* Initialize the viewport to clear */
		viewport.X = raster->nOffsetX;
		viewport.Y = raster->nOffsetY;
		viewport.Width = raster->width;
		viewport.Height = raster->height;
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;

		/* Set the viewport to clear */
		DXCHECK(IDirect3DDevice8_SetViewport
				(_RwD3DDevice, &viewport));

		/* check clear flags */
		d3d8ClearFlags = clearFlagsConvTable[clearFlags];

		if (d3d8ClearFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL))
		{
			if (RwCameraGetZRaster((RwCamera *) camera) == NULL)
			{
				d3d8ClearFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
			}
		}

		/* Clear */
		DXCHECK(IDirect3DDevice8_Clear
				(_RwD3DDevice, 0, NULL,
				d3d8ClearFlags, packedColor,
				1.0f, StencilClearValue));

		/* If the set render target fails, we need to copy from the back buffer */
		if (FAILED(hr))
		{
			RECT                rectSrc;

			rectSrc.left = raster->nOffsetX;
			rectSrc.top = raster->nOffsetY;
			rectSrc.right = rectSrc.left + raster->width;
			rectSrc.bottom = rectSrc.top + raster->height;

			hr = DXCHECK(IDirect3DDevice8_CopyRects
						(_RwD3DDevice, _RwD3D8RenderSurface,
						&rectSrc, 1, plg->renderTarget, NULL));
		}
		else //
		{
			if (topRasterExt->cube)
			{
				IDirect3DCubeTexture8_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE8)topRasterExt->texture,
														(D3DCUBEMAP_FACES)topRasterExt->face,
														0,
														&surfaceLevel);
			}
			else
			{
				IDirect3DTexture8_GetSurfaceLevel(topRasterExt->texture, 0,
												&surfaceLevel);
			}

			RECT                rectSrc;

			rectSrc.left = raster->nOffsetX;
			rectSrc.top = raster->nOffsetY;
			rectSrc.right = rectSrc.left + raster->width;
			rectSrc.bottom = rectSrc.top + raster->height;

			DXCHECK(IDirect3DDevice8_CopyRects
						(_RwD3DDevice, plg->renderTarget, &rectSrc,
						1, surfaceLevel, NULL));

			IDirect3DSurface8_Release(surfaceLevel);
		}

		/* Set the render & depth/stencil surface */
		DXCHECK(IDirect3DDevice8_SetRenderTarget
				(_RwD3DDevice, _RwD3D8RenderSurface,
				_RwD3D8DepthStencilSurface));

		return TRUE;
	}

	return FALSE;
}

void NAK patch__CameraClear()
{
	__asm
	{
		cmp     byte ptr [edx+20h], 5
		jnz     loc_64144F

		mov     eax, dword ptr [esp+84h+0h+0Ch] // clearFlags
		push    eax
		mov     eax, dword ptr [esp+84h+4h+8h] // color
		push    eax
		mov     eax, dword ptr [esp+84h+8h+4h] // camera
		push    eax
		call    CameraClear
		pop     ecx
		pop     ecx
		pop     ecx

		test    eax, eax
		jz      CAMERACLEAR_DEFAULT

		mov     eax, 641442h
		jmp     eax

CAMERACLEAR_DEFAULT:
		mov     eax, 6412CDh
		jmp     eax

loc_64144F:
		mov     eax, 64144Fh
		jmp     eax
	}
}

void RasterClearGeneric(RwRect *rect, RwInt32 packedColor)
{
	RwRGBA          unpackedColor;
	RwUInt32        packedColor8888;
	D3DRECT         rectClear;
	RwRaster        *topRaster;
	_rwD3D8RasterExt  *rasterExt;
	LPSURFACE       surfaceLevel;
	D3DSURFACE_DESC d3d8Desc;
	LPSURFACE       currentRenderTarget;
	LPSURFACE       currentDepthStencil;
	HRESULT         hr;

	if (_RwD3D8CurrentRasterTarget->depth != 32)
	{
		_rwD3D8PixelToRGB(&unpackedColor, &packedColor, RwRasterGetFormat(_RwD3D8CurrentRasterTarget));
		_rwD3D8RGBToPixel(&packedColor8888, &unpackedColor, rwRASTERFORMAT8888);
	}
	else
	{
		packedColor8888 = packedColor;
	}

	/* Get the top level raster as this is the only one with a valid texture */
	topRaster = _RwD3D8CurrentRasterTarget;
	while ((topRaster = RwRasterGetParent(topRaster)) != RwRasterGetParent(topRaster)) {};

	/* Get the surface level */
	rasterExt = RASTEREXTFROMRASTER(topRaster);

	MSAARasterPlugin *plg = RASTERPLUGINFROMRASTER(_RwD3D8CurrentRasterTarget);

	if ( plg->renderTarget && Settings::GetInstance()->bRwMSAASupport )
	{
		DXCHECK(IDirect3DDevice8_GetRenderTarget(_RwD3DDevice, &currentRenderTarget));
		DXCHECK(IDirect3DDevice8_GetDepthStencilSurface(_RwD3DDevice, &currentDepthStencil));

		if(currentRenderTarget != plg->renderTarget)
			hr = DXCHECK(IDirect3DDevice8_SetRenderTarget(_RwD3DDevice, plg->renderTarget, NULL));
		else
			hr = S_OK;

		/* Initialize the rectangle to clear */
		rectClear.x1 = _RwD3D8CurrentRasterTarget->nOffsetX + rect->x;
		rectClear.y1 = _RwD3D8CurrentRasterTarget->nOffsetY + rect->y;
		rectClear.x2 = rectClear.x1 + rect->w;
		rectClear.y2 = rectClear.y1 + rect->h;

		/* Clear */
		DXCHECK(IDirect3DDevice8_Clear(_RwD3DDevice, 1, &rectClear, D3DCLEAR_TARGET, packedColor8888, 1.0f, 0));

		/* If the set render target fails, we need to copy from the back buffer */
		if (FAILED(hr))
		{
			RECT rectSrc;

			rectSrc.left = _RwD3D8CurrentRasterTarget->nOffsetX;
			rectSrc.top = _RwD3D8CurrentRasterTarget->nOffsetY;
			rectSrc.right = rectSrc.left + _RwD3D8CurrentRasterTarget->width;
			rectSrc.bottom = rectSrc.top + _RwD3D8CurrentRasterTarget->height;

			hr = DXCHECK(IDirect3DDevice8_CopyRects(_RwD3DDevice, _RwD3D8RenderSurface, &rectSrc, 1, plg->renderTarget, NULL));
		}
		else
		{
			if (rasterExt->cube)
			{
				IDirect3DCubeTexture8_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE8)rasterExt->texture,
														(D3DCUBEMAP_FACES)rasterExt->face,
														0,
														&surfaceLevel);
			}
			else
			{
				IDirect3DTexture8_GetSurfaceLevel(rasterExt->texture, 0,
												&surfaceLevel);
			}

			RECT                rectSrc;

			rectSrc.left = _RwD3D8CurrentRasterTarget->nOffsetX;
			rectSrc.top = _RwD3D8CurrentRasterTarget->nOffsetY;
			rectSrc.right = rectSrc.left + _RwD3D8CurrentRasterTarget->width;
			rectSrc.bottom = rectSrc.top + _RwD3D8CurrentRasterTarget->height;

			DXCHECK(IDirect3DDevice8_CopyRects
						(_RwD3DDevice, plg->renderTarget, &rectSrc,
						1, surfaceLevel, NULL));

			IDirect3DSurface8_Release(surfaceLevel);

			/* Restore the render & depth/stencil surface */
			if(currentRenderTarget != plg->renderTarget)
				DXCHECK(IDirect3DDevice8_SetRenderTarget(_RwD3DDevice, currentRenderTarget, currentDepthStencil));
		}

		IDirect3DSurface8_Release(currentDepthStencil);
		IDirect3DSurface8_Release(currentRenderTarget);
	}
	else
	{
		if (rasterExt->cube)
		{
			IDirect3DCubeTexture8_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE8)rasterExt->texture,
													(D3DCUBEMAP_FACES)rasterExt->face,
													0,
													&surfaceLevel);
		}
		else
		{
			IDirect3DTexture8_GetSurfaceLevel(rasterExt->texture, 0, &surfaceLevel);
		}

		DXCHECK(IDirect3DDevice8_GetRenderTarget(_RwD3DDevice, &currentRenderTarget));
		DXCHECK(IDirect3DDevice8_GetDepthStencilSurface(_RwD3DDevice, &currentDepthStencil));

		if(currentRenderTarget != surfaceLevel)
		{
			IDirect3DSurface8_GetDesc(surfaceLevel, &d3d8Desc);

			if (d3d8Desc.Usage == D3DUSAGE_RENDERTARGET)
				hr = DXCHECK(IDirect3DDevice8_SetRenderTarget(_RwD3DDevice, surfaceLevel, NULL));
			else
				hr = E_FAIL;
		}
		else
			hr = S_OK;

		/* Initialize the rectangle to clear */
		rectClear.x1 = _RwD3D8CurrentRasterTarget->nOffsetX + rect->x;
		rectClear.y1 = _RwD3D8CurrentRasterTarget->nOffsetY + rect->y;
		rectClear.x2 = rectClear.x1 + rect->w;
		rectClear.y2 = rectClear.y1 + rect->h;

		/* Clear */
		DXCHECK(IDirect3DDevice8_Clear(_RwD3DDevice, 1, &rectClear, D3DCLEAR_TARGET, packedColor8888, 1.0f, 0));

		/* If the set render target fails, we need to copy from the back buffer */
		if (FAILED(hr))
		{
			RECT rectSrc;

			rectSrc.left = _RwD3D8CurrentRasterTarget->nOffsetX;
			rectSrc.top = _RwD3D8CurrentRasterTarget->nOffsetY;
			rectSrc.right = rectSrc.left + _RwD3D8CurrentRasterTarget->width;
			rectSrc.bottom = rectSrc.top + _RwD3D8CurrentRasterTarget->height;

			hr = DXCHECK(IDirect3DDevice8_CopyRects(_RwD3DDevice, _RwD3D8RenderSurface, &rectSrc, 1, surfaceLevel, NULL));
		}
		else
		{
			/* Restore the render & depth/stencil surface */
			if(currentRenderTarget != surfaceLevel)
				DXCHECK(IDirect3DDevice8_SetRenderTarget(_RwD3DDevice, currentRenderTarget, currentDepthStencil));
		}

		IDirect3DSurface8_Release(currentDepthStencil);
		IDirect3DSurface8_Release(currentRenderTarget);

		IDirect3DSurface8_Release(surfaceLevel);
	}
}

void NAK patch__RasterClearGeneric()
{
	__asm
	{
		mov     ecx, dword ptr [esp+78h+4] // rect
		mov     eax, dword ptr [esp+78h+8] // packedColor

		push    eax
		push    ecx
		call    RasterClearGeneric
		pop     eax
		pop     eax

		pop     edi
		pop     esi
		pop     ebp
		pop     ebx
		mov     eax, 1
		add     esp, 68h
		retn
	}
}

RwBool RasterRenderQuad(RwInt32 state)
{
	MSAARasterPlugin *plg = RASTERPLUGINFROMRASTER(_RwD3D8CurrentRasterTarget);

	if ( plg->renderTarget && Settings::GetInstance()->bRwMSAASupport )
	{
		static HRESULT                 hr;
		static LPSURFACE               currentRenderTarget;
		static LPSURFACE               currentDepthStencil;

		if ( state == 0 )
		{
			hr = D3D_OK;

			DXCHECK(IDirect3DDevice8_GetRenderTarget(_RwD3DDevice, &currentRenderTarget));
			DXCHECK(IDirect3DDevice8_GetDepthStencilSurface(_RwD3DDevice, &currentDepthStencil));

			if(currentRenderTarget != plg->renderTarget)
			{
				/* Set the raster as the current target */
				hr = DXCHECK(IDirect3DDevice8_SetRenderTarget(_RwD3DDevice, plg->renderTarget, NULL));
			}
			else
				hr = S_OK;
		}
		else if ( state == 1 )
		{
			/* If the set render target fails, we need to copy from the back buffer */
			if (FAILED(hr))
			{
				RECT rectSrc;

				rectSrc.left = _RwD3D8CurrentRasterTarget->nOffsetX;
				rectSrc.top = _RwD3D8CurrentRasterTarget->nOffsetY;
				rectSrc.right = rectSrc.left + _RwD3D8CurrentRasterTarget->width;
				rectSrc.bottom = rectSrc.top + _RwD3D8CurrentRasterTarget->height;

				hr = DXCHECK(IDirect3DDevice8_CopyRects(_RwD3DDevice, _RwD3D8RenderSurface, &rectSrc, 1, plg->renderTarget, NULL));
			}
			else
			{
				RwRaster                *topRasterDest;
				_rwD3D8RasterExt        *rasExtDest;
				LPSURFACE               surfaceLevel;

				/* Get the top level destination raster as this is the only one with a valid texture */
				topRasterDest = _RwD3D8CurrentRasterTarget;
				while ((topRasterDest = RwRasterGetParent(topRasterDest)) != RwRasterGetParent(topRasterDest)) {};

				rasExtDest = RASTEREXTFROMRASTER(topRasterDest);

				/* Get the surface level */
				if (rasExtDest->cube)
				{
					IDirect3DCubeTexture8_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE8)rasExtDest->texture,
															(D3DCUBEMAP_FACES)rasExtDest->face,
															0,
															&surfaceLevel);
				}
				else
				{
					IDirect3DTexture8_GetSurfaceLevel(rasExtDest->texture, 0, &surfaceLevel);
				}

				RECT                rectSrc;

				rectSrc.left = _RwD3D8CurrentRasterTarget->nOffsetX;
				rectSrc.top = _RwD3D8CurrentRasterTarget->nOffsetY;
				rectSrc.right = rectSrc.left + _RwD3D8CurrentRasterTarget->width;
				rectSrc.bottom = rectSrc.top + _RwD3D8CurrentRasterTarget->height;

				DXCHECK(IDirect3DDevice8_CopyRects
							(_RwD3DDevice, plg->renderTarget, &rectSrc,
							1, surfaceLevel, NULL));

				IDirect3DSurface8_Release(surfaceLevel);

				/* Restore the render & depth/stencil surface */
				if(currentRenderTarget != plg->renderTarget)
				{
					DXCHECK(IDirect3DDevice8_SetRenderTarget(_RwD3DDevice,
											currentRenderTarget, currentDepthStencil));
				}
			}

			IDirect3DSurface8_Release(currentDepthStencil);
			IDirect3DSurface8_Release(currentRenderTarget);


			hr = E_FAIL;
			currentRenderTarget = NULL;
			currentDepthStencil = NULL;
		}

		return TRUE;
	}

	return FALSE;
}

void NAK patch__RasterRenderQuadBegin()
{
	__asm
	{
		cmp     cl, 5
		jnz     loc_65D6B2

		push    eax
		push    ecx

		push    0
		call    RasterRenderQuad
		pop     ecx

		test    eax, eax
		pop     ecx
		pop     eax

		jz      RasterRenderQuadBegin_DEFAULT

		jmp     loc_65D6B2

RasterRenderQuadBegin_DEFAULT:
		mov     edx, 65D609h
		jmp     edx

loc_65D6B2:
		mov     ecx, 65D6B2h
		jmp     ecx
	}
}

void NAK patch__RasterRenderQuadEnd()
{
	__asm
	{
		cmp     byte ptr [eax+20h], 5
		jnz     loc_65DB39

		push    eax
		push    ecx

		push    1
		call    RasterRenderQuad
		pop     ecx

		test    eax, eax
		pop     ecx
		pop     eax

		jz      RasterRenderQuadEnd_DEFAULT

		jmp     loc_65DB39

RasterRenderQuadEnd_DEFAULT:
		mov     edx, 65DAB6h
		jmp     edx

loc_65DB39:
		mov     ecx, 65DB39h
		jmp     ecx
	}
}

void NAK patch__rwD3D8CreateZBufferRaster1()
{
	__asm
	{
		push    ebx
		mov     edx, dword ptr ds: [8284A0h]	// Present.MultiSampleType
		push    edx
		push    esi
		push    ebp
		push    ecx
		push    eax
		mov     edx, [eax]
		call    dword ptr [edx+68h]
		mov     edx, 65F75Bh
		jmp     edx
	}
}

void NAK patch__rwD3D8CreateZBufferRaster2()
{
	__asm
	{
		push    ebx
		mov     edx, dword ptr ds: [8284A0h]	// Present.MultiSampleType
		push    edx
		push    esi
		push    ecx
		mov     ecx, [edi+0Ch]
		push    ecx
		push    eax
		mov     edx, [eax]
		call    dword ptr [edx+68h]
		mov     edx, 65F787h
		jmp     edx
	}
}

void NAK patch__rxD3D8VideoMemoryRasterListRestore()
{
	__asm
	{
		mov     ebx, dword ptr ds: [8284A0h]	// Present.MultiSampleType
		push    ebx
		push    eax
		mov     esi, [ecx]

		mov     ebx, 65EB3Ch
		jmp     ebx
	}
}

void UpdateMSAA()
{
	SelectedMultisamplingLevels = Clamp((RwUInt32)Settings::GetInstance()->nMSAA, 0, MaxMultisamplingLevels);
	Settings::GetInstance()->nMSAA = SelectedMultisamplingLevels;
}

void NAK patch_MSAA()
{
	__asm
	{
		pushad
		call UpdateMSAA
		popad

		mov     eax, dword ptr ds: [7EB808h] // SelectedMultisamplingLevels
		retn
	}
}

void SetIngameInfoRenderStates(long mode)
{
	if ( !(Settings::GetInstance()->nMSAA > 1 || Settings::GetInstance()->bRwMSAASupport) ) return;

	int aa = Settings::GetInstance()->bEnableMSAA;

	if ( mode == 0 ) // 2d
		RwD3D8SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

	if ( mode == 1 ) // 3d
		RwD3D8SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, aa ? TRUE : FALSE);
}

void NAK patch__CRenderer_SetIngameInfoRenderStates()
{
	__asm
	{
		mov    eax, dword ptr [esp+8+4]

		push   eax
		call   SetIngameInfoRenderStates
		pop    eax

		pop     ebp
		pop     ebx

		retn
	}
}

STARTPATCH
	{
		DBG("RW MSAA Support\n");

		//_rxD3D8VideoMemoryRasterListRestore
		CPatch::SetChar(0x65EC1D+1, 0x4C); // mov     eax, [esp+78h+var_64] -> mov     ecx, [esp+78h+var_64]
		CPatch::Nop(0x65EC25, 4);
		CPatch::RedirectJump(0x65EC29, patch__VideoMemoryRasterListRestore);

		// _rxD3D8VideoMemoryRasterListRelease
		CPatch::RedirectJump(0x65E901, VideoMemoryRasterListRelease);

		// _rwD3D8CameraBeginUpdate
		CPatch::RedirectJump(0x64281D, patch__CameraBeginUpdate);

		// _rwD3D8CameraEndUpdate
		CPatch::RedirectJump(0x64296B, patch__CameraEndUpdate);

		// _rwD3D8CameraClear
		CPatch::RedirectJump(0x6412C7, patch__CameraClear);

		// rwD3D8RasterRenderQuad	(RwRasterRender RwRasterRenderFast RwRasterRenderScaled)
		CPatch::RedirectJump(0x65D603, patch__RasterRenderQuadBegin);
		CPatch::RedirectJump(0x65DAB0, patch__RasterRenderQuadEnd);

		// rwD3D8RasterCopyQuad		(RwRasterRender RwRasterRenderFast RwRasterRenderScaled)

		// D3D8RasterClearGeneric	(RwRasterClear)
		CPatch::RedirectJump(0x65E04A, patch__RasterClearGeneric);

		// zbuf AA d3d9 backport
		CPatch::RedirectJump(0x65F74F, patch__rwD3D8CreateZBufferRaster1);
		CPatch::RedirectJump(0x65F778, patch__rwD3D8CreateZBufferRaster2);
		CPatch::RedirectJump(0x65EB37, patch__rxD3D8VideoMemoryRasterListRestore);

		// set level
		CPatch::RedirectCall(0x63F894, patch_MSAA);

		// let the game ability to switch AA
		CPatch::RedirectJump(0x5F5C2B, patch__CRenderer_SetIngameInfoRenderStates);
	}
ENDPATCH