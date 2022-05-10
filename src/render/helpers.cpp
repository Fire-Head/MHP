#include "common.h"
#include "game.h"
#include "settings.h"
#include "rw.h"


void
_rwD3D8EnableClippingIfNeeded(void *object, RwUInt8 type)
{
	int clip;
	if(type == rpATOMIC)
		clip = !RwD3D8CameraIsSphereFullyInsideFrustum(RwCameraGetCurrentCamera(),
		                                               RpAtomicGetWorldBoundingSphere((RpAtomic*)object));
	else
		clip = !RwD3D8CameraIsBBoxFullyInsideFrustum(RwCameraGetCurrentCamera(),
		                                             &((RpWorldSector*)object)->tightBoundingBox);
	RwD3D8SetRenderState(D3DRS_CLIPPING, clip);
}

void RwD3D8DrawIndexedPrimitive_DualPass(RwUInt32 primitiveType, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 numIndices)
{
	if (!Settings::GetInstance()->bPS2AlphaTest)
	{
		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);
		return;
	}
	int hasAlpha, alphafunc, alpharef, zwrite;
	RwD3D8GetRenderState(D3DRS_ALPHABLENDENABLE, &hasAlpha);
	RwD3D8GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);

	if (hasAlpha && zwrite)
	{
		RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
		RwD3D8GetRenderState(D3DRS_ALPHAREF, &alpharef);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		RwD3D8SetRenderState(D3DRS_ALPHAREF, 128);

		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
		RwD3D8SetRenderState(D3DRS_ALPHAREF, alpharef);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, zwrite);
	}
	else if ( !zwrite )
	{
		RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);

		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
	}
	else
		RwD3D8DrawIndexedPrimitive(primitiveType, minIndex, numVertices, startIndex, numIndices);
}

void RwD3D8DrawPrimitive_DualPass(RwUInt32 primitiveType, RwUInt32 startVertex, RwUInt32 numVertices)
{
	if (!Settings::GetInstance()->bPS2AlphaTest)
	{
		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);
		return;
	}

	int hasAlpha, alphafunc, alpharef, zwrite;
	RwD3D8GetRenderState(D3DRS_ALPHABLENDENABLE, &hasAlpha);
	RwD3D8GetRenderState(D3DRS_ZWRITEENABLE, &zwrite);

	if (hasAlpha && zwrite)
	{
		RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
		RwD3D8GetRenderState(D3DRS_ALPHAREF, &alpharef);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		RwD3D8SetRenderState(D3DRS_ALPHAREF, 128);

		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
		RwD3D8SetRenderState(D3DRS_ALPHAREF, alpharef);
		RwD3D8SetRenderState(D3DRS_ZWRITEENABLE, zwrite);
	}
	else if ( !zwrite )
	{
		RwD3D8GetRenderState(D3DRS_ALPHAFUNC, &alphafunc);
		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);

		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);

		RwD3D8SetRenderState(D3DRS_ALPHAFUNC, alphafunc);
	}
	else
		RwD3D8DrawPrimitive(primitiveType, startVertex, numVertices);
}

RpAtomic *GetFirstAtomic(RpClump *clump)
{
	RpAtomic *atm;

	atm = NULL;
	RpClumpForAllAtomics(clump, GetFirstAtomicCallback, &atm);
	return atm;
}

RwUInt32
RwStreamTell(RwStream *stream)
{
    switch (stream->type)
    {
        case rwSTREAMFILENAME:
        case rwSTREAMFILE:
            {
                void               *fp = stream->Type.file.fpFile;
                RWRETURN(RwFtell(fp));
            }
            break;
        case rwSTREAMMEMORY:
            {
                RwStreamMemory     *smpMem = &stream->Type.memory;
                RWRETURN(smpMem->position);
            }
            break;
    }

	return -1;
}

RwStream *
RwStreamSeek(RwStream *stream, RwUInt32 offset)
{
    /* No point in skipping 0 bytes - it always works */
    if (offset == 0)
    {
        RWRETURN(stream);
    }

    switch (stream->type)
    {
        case rwSTREAMFILENAME:
        case rwSTREAMFILE:
            {
                void               *fp = stream->Type.file.fpFile;
                RwStream           *retVal;

                /* Handle reading from the stream */

                if (RwFseek(fp, offset, SEEK_SET))
                {
                    if (RwFeof(fp))
                    {
                        RWERROR((E_RW_ENDOFSTREAM));
                    }
                    retVal = (RwStream *)NULL;
                }
                else
                {
                    retVal = stream;
                }

                RWRETURN(retVal);
            }
            break;
        case rwSTREAMMEMORY:
            {
                RwStreamMemory     *smpMem = &stream->Type.memory;

                if ( offset > smpMem->nSize)
                {
                    /* Put at file end */
                    smpMem->position = smpMem->nSize;

                    RWERROR((E_RW_ENDOFSTREAM));
                    RWRETURN((RwStream *)NULL);
                }

                /* Move the position forward */
                smpMem->position = offset;

                /* Success */
                RWRETURN(stream);
            }
            break;
    }

	return NULL;
}

static RwBool _fakePalCreateTextureRaster(RwRaster *raster, _rwD3D8RasterExt *rasExt)
{
    RwUInt32    rasFormat;
    RwUInt32    pixelFormat;
    D3DFORMAT   colorFormat;
    HRESULT     hr;

    rasFormat = RwRasterGetFormat(raster);
    pixelFormat = rasFormat & rwRASTERFORMATPIXELFORMATMASK;

    if ((rasFormat & rwRASTERFORMATPAL4) || (rasFormat & rwRASTERFORMATPAL8))
    {
        if (!((rwRASTERFORMAT888 == pixelFormat) ||
              (rwRASTERFORMAT8888 == pixelFormat)))
        {
            RWRETURN(FALSE);
        }

        /* Create the palette */
        if (PaletteFreeList == NULL)
        {
            PaletteFreeList = RwFreeListCreate(sizeof(_rwD3D8Palette), 64, 4,
                rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);
        }

        rasExt->palette = (_rwD3D8Palette *)RwFreeListAlloc(PaletteFreeList, rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);

        if(rasExt->palette)
            rasExt->palette->globalindex = FindAvailablePaletteIndex();

        colorFormat = D3DFMT_A8;
    }

    /* Does this raster format have an alpha component */
    rasExt->alpha = _rwRasterConvTable[(rasFormat & rwRASTERFORMATPIXELFORMATMASK)>> 8].alpha;

    /* Check the size of the texture */
    _rwD3D8CheckRasterSize(&(raster->width), &(raster->height), raster->cType | rasFormat);

    {
        hr = DXCHECK(IDirect3DDevice8_CreateTexture(_RwD3DDevice, raster->width,
                                                    raster->height,
                                                    (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                                                    0,
                                                    colorFormat,
                                                    D3DPOOL_MANAGED,
                                                    &(rasExt->texture)));
    }

    if (FAILED(hr))
    {
        if (D3DFMT_P8 == colorFormat)
        {
            if(rasExt->palette)
            {
                AddAvailablePaletteIndex(rasExt->palette->globalindex);

                RwFreeListFree(PaletteFreeList, rasExt->palette);
                rasExt->palette = NULL;
            }
        }

        RWERROR((E_RW_DEVICEERROR, RWSTRING("Failed to create texture.")));
        RWRETURN(FALSE);
    }

    RWRETURN(TRUE);
}

RwBool
_fakePalRasterCreate(void *unused1 __RWUNUSED__, void *rasterIn, RwInt32 flags)
{
    RwRaster        *raster = (RwRaster *)rasterIn;
    _rwD3D8RasterExt  *rasExt = RASTEREXTFROMRASTER(raster);

    /* Initialise structure to something sensible */
    raster->cType    = (RwUInt8)(flags & rwRASTERTYPEMASK);
    raster->cFlags   = (RwUInt8)(flags & ~rwRASTERTYPEMASK);
    raster->cpPixels = NULL;
    raster->palette  = NULL;

    rasExt->texture = NULL;
    rasExt->palette = NULL;
    rasExt->alpha = 0;
    rasExt->cube = 0;
    rasExt->face = 0;
    rasExt->lockedSurface = NULL;
    rasExt->lockedMipLevel = (RwUInt8)-1;
    rasExt->dxtFormat = 0;
    raster->cType = (RwUInt8)(flags & rwRASTERTYPEMASK);
    raster->cFlags = (RwUInt8)(flags & ~rwRASTERTYPEMASK);
	raster->depth = 8;
    raster->cFormat = (RwUInt8)(((flags & rwRASTERFORMATMASK)) >> 8);
    /*
     * If it is not a camera buffer, then we need to allocate real memory for it
     */
    if ((raster->width) && (raster->height))
    {
        switch (raster->cType)
        {
            case rwRASTERTYPENORMAL:
			case rwRASTERTYPETEXTURE:
            {
                if (!(raster->cFlags & rwRASTERDONTALLOCATE))
                {
                    if (!_fakePalCreateTextureRaster(raster, rasExt))
                    {
                        RWRETURN(FALSE);
                    }

                    if (rasExt->palette != NULL)
                    {
                        rxD3D8VideoMemoryRasterListAdd(raster);
                    }
                }
            }
            break;
        }
    }

    RWRETURN(TRUE);
}

RwBool _fakePalTextureRead(void *streamIn, void *textureIn,
                       RwInt32 unused3 __RWUNUSED__)
{
    RwStream            *stream = (RwStream *)streamIn;
    RwRaster            *raster = (RwRaster *)NULL;
    RwInt32             i;
    RwInt32             rasFormat = 0;
    RwTexture           *texture;
    RwUInt32            length, version;
    _rwD3D8NativeRaster   nativeRaster;
    _rwD3D8NativeTexture  nativeTexture;
    _rwD3D8RasterExt      *rasExt;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version))
    {
        RWRETURN(FALSE);
    }

    if (!((version >= rwLIBRARYBASEVERSION) &&
         (version <= rwLIBRARYCURRENTVERSION)))
    {
        RWRETURN(FALSE);
    }

    /*
     * Don't have to worry about endianness as this is platform specific
     */

    /* ID, filter & addressing modes */
    if (RwStreamRead(stream, (void *)&nativeTexture,
        sizeof(_rwD3D8NativeTexture)) != sizeof(_rwD3D8NativeTexture))
    {
        RWRETURN(FALSE);
    }

    if (rwID_PCD3D8 != nativeTexture.id)
    {
        RWRETURN(FALSE);
    }

    /* Native raster data structure */
    if (RwStreamRead(stream, (void *)&nativeRaster,
        sizeof(_rwD3D8NativeRaster)) != sizeof(_rwD3D8NativeRaster))
    {
        RWRETURN(FALSE);
    }

    /* Create a raster */
    //rasFormat = _rwD3D8FindCorrectRasterFormat((RwRasterType)nativeRaster.type, nativeRaster.format);
    rasFormat = nativeRaster.format & rwRASTERFORMATMASK;

    rasFormat |= (nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP));

	RwStandardFunc oldcreate = NULL;
	oldcreate = RWSRCGLOBAL(stdFunc[rwSTANDARDRASTERCREATE]);
	RWSRCGLOBAL(stdFunc[rwSTANDARDRASTERCREATE]) = _fakePalRasterCreate;
    raster = RwRasterCreate(nativeRaster.width,
                            nativeRaster.height,
                            (RwInt32)nativeRaster.depth,
                            nativeRaster.type | rasFormat);


	RWSRCGLOBAL(stdFunc[rwSTANDARDRASTERCREATE]) = oldcreate;
	printf("rasFormat %d %d\n", rasFormat, nativeRaster.type);

    if (!raster)
    {
        RWRETURN(FALSE);
    }

    /* Get the raster extension */
    rasExt = RASTEREXTFROMRASTER(raster);

    rasExt->dxtFormat = 0;

    /* This raster has an alpha component */
    /*
    rasExt->alpha = nativeRaster.alpha;
    */

    rasFormat = RwRasterGetFormat(raster);
    if (rasFormat == nativeRaster.format &&
        nativeRaster.width == raster->width &&
        nativeRaster.height == raster->height )
    {
        RwUInt32    autoMipmap;

        /* Load the palette if palletized */
        if (nativeRaster.format & rwRASTERFORMATPAL4)
        {
            RwUInt8     *palette;
            RwUInt32    size;

            palette = RwRasterLockPalette(raster, rwRASTERLOCKWRITE);
            RWASSERT(NULL != palette);

            size = sizeof(PALETTEENTRY) * 32;
            if (RwStreamRead(stream, (void *)palette, size) != size)
            {
                RWRETURN(FALSE);
            }

            RwRasterUnlockPalette(raster);
        }
        else if (nativeRaster.format & rwRASTERFORMATPAL8)
        {
            RwUInt8     *palette;
            RwUInt32    size;

            palette = RwRasterLockPalette(raster, rwRASTERLOCKWRITE);
            RWASSERT(NULL != palette);

            size = sizeof(PALETTEENTRY) * 256;
            if (RwStreamRead(stream, (void *)palette, size) != size)
            {
                RWRETURN(FALSE);
            }

            RwRasterUnlockPalette(raster);
        }

        /* Remove AUTOMIPMAP flag to avoid unlock invoking mipmap create */
        autoMipmap = raster->cFormat & (rwRASTERFORMATAUTOMIPMAP >> 8);
        raster->cFormat &= ~autoMipmap;

        /* Load mips into raster */
        for (i = 0; i < nativeRaster.numMipLevels; i++)
        {
            RwUInt8     *pixels;
            RwUInt32    size;

            pixels = RwRasterLock(raster, (RwUInt8)i, rwRASTERLOCKWRITE);
            RWASSERT(NULL != pixels);

            /* Size in bytes of mip to load */
            if (RwStreamRead(stream, (void *)&size, sizeof(RwUInt32)) != sizeof(RwUInt32))
            {
                RWRETURN(FALSE);
            }

            /* Read the mip level */
            if (RwStreamRead(stream, (void *)pixels, size) != size)
            {
                RWRETURN(FALSE);
            }

            RwRasterUnlock(raster);
        }

        /* Restore auto mip flag */
        raster->cFormat |= autoMipmap;
    }
    else
    {
		// Texture format or dimensions not supported

        RwRasterDestroy(raster);

        RWRETURN(FALSE);
    }

    texture = RwTextureCreate(raster);
    if (!texture)
    {
        RwRasterDestroy(raster);

        RWRETURN(FALSE);
    }

    RwTextureSetFilterMode(texture, nativeTexture.filterAndAddress & 0xFF);
    RwTextureSetAddressingU(texture, (nativeTexture.filterAndAddress >> 8) & 0x0F);
    RwTextureSetAddressingV(texture, (nativeTexture.filterAndAddress >> 12) & 0x0F);
    RwTextureSetName(texture, nativeTexture.name);
    RwTextureSetMaskName(texture, nativeTexture.mask);

    *((RwTexture **)textureIn) = texture;

    RWRETURN(TRUE);
}

RwBool PalettedTextureRead(void *streamIn, void *textureIn, RwInt32 unused3 __RWUNUSED__)
{
	RwStream            *stream = (RwStream *)streamIn;
	RwBool result;
	RwInt32 pos;

	pos = RwStreamTell(stream);
	result = _rwD3D8NativeTextureRead(streamIn, textureIn, unused3);
	if ( !result )
	{
		RwStreamSeek(stream, pos);
		result = _fakePalTextureRead(streamIn, textureIn, unused3);
	}
	return result;
}