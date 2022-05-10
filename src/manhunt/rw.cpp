#include "common.h"
#include "rw.h"


LPDIRECT3DDEVICE8           &_RwD3DDevice = *(LPDIRECT3DDEVICE8*)0x824448;
LPDIRECT3DSURFACE8          &_RwD3D8RenderSurface = *(LPDIRECT3DSURFACE8*)0x824450;
LPDIRECT3DSURFACE8          &_RwD3D8DepthStencilSurface = *(LPDIRECT3DSURFACE8*)0x82444C;
D3DPRESENT_PARAMETERS       &Present = *(D3DPRESENT_PARAMETERS*)0x828490;
D3DCAPS8                    &_RwD3D8DeviceCaps = *(D3DCAPS8*)0x8292A4;
RwInt32                     &_RwD3D8ZBufferDepth = *(RwInt32*)0x829228;
_rwD3D8AdapterInformation   &_RwD3D8AdapterInformation = *(_rwD3D8AdapterInformation*)0x829380;
RwUInt32                    &StencilClearValue = *(RwUInt32*)0x82446C;
RwBool                      &NeedToCopyFromBackBuffer = *(RwBool*)0x824458;
RwBool                      &_RwHasStencilBuffer = *(RwBool*)0x824460;
RwRwDeviceGlobals           &dgGGlobals = *(RwRwDeviceGlobals*)0x828470;
HWND                        &WindowHandle = *(HWND*)0x824444;
RwUInt32 &SelectedMultisamplingLevels = *(RwUInt32*)0x7EB808;
RwUInt32 &MaxMultisamplingLevels = *(RwUInt32*)0x7EB804;
RwInt32 &MatFXMaterialDataOffset = *(RwInt32*)0x824928;
RwInt32 &_RwD3D8RasterExtOffset = *(RwInt32*)0x8295A4;
RwInt32 &D3D8LightOffset = *(RwInt32*)0x827970;
RwInt32 &__rxPipelineGlobalsOffset = *(RwInt32*)0x829294;
static RwModuleInfo &rasterModule = *(RwModuleInfo*)0x8243A8;
RwRaster *&_RwD3D8CurrentRasterTarget = *(RwRaster**)0x827718;

_rxD3D8VertexShader *&VertexShaderList = *(_rxD3D8VertexShader **)0x8248E0;
RwUInt32     &LastVertexShaderUsed = *(RwUInt32*)0x7EB810;
RwUInt32     &LastPixelShaderUsed = *(RwUInt32*)0x7EB814;
void        *&LastVertexBufferUsed = *(void**)0x7EB818;
RwUInt32     &LastVertexBufferStride = *(RwUInt32*)0x824480;
void        *&LastIndexBufferUsed = *(void**)0x7EB81C;
RwUInt32     &LastBaseIndex = *(RwUInt32*)0x7EB820;

RwBool       &LastWorldMatrixUsedIdentity = *(RwBool*)0x824484;

D3DMATERIAL8 &LastMaterial = *(D3DMATERIAL8*)0x824400;

static RwFreeList *&VertexShaderFreeList = *(RwFreeList **)0x8248E4;
RxD3D8VideoMemoryRaster *&VideoMemoryRasters = *(RxD3D8VideoMemoryRaster **)0x827730;
static RwFreeList *&VideoMemoryRastersFreeList = *(RwFreeList **)0x827734;
RwUInt16 *&PaletteIndexFree = *(RwUInt16 **)0x827728;
RwFreeList *&PaletteFreeList = *(RwFreeList **)0x82772C;

static RwUInt32 &NumPaletteIndexFree = *(RwUInt32*)0x82771C;
static RwUInt32 &MaxPaletteIndexFree = *(RwUInt32*)0x827720;
static RwUInt32 &MaxPaletteIndex = *(RwUInt32*)0x827724;

_rwRasterConvData (&_rwRasterConvTable)[11] = *(_rwRasterConvData (*)[11])*(int *)(0x67B108);

RwIm2DVertex (&_RwD3D8QuadVerts)[4] = *(RwIm2DVertex (*)[4])*(int *)(0x8276A8);

NAK RwV3d *RwV3dTransformVectors(RwV3d * vectorsOut, const RwV3d * vectorsIn, RwInt32 numPoints, const RwMatrix * matrix) { EAXJMP(0x615700); }
NAK RwReal RwV3dLength(const RwV3d * in) { EAXJMP(0x615430); }
NAK void _rwProcessorForceSinglePrecision(void) { EAXJMP(0x66F432); }
NAK RwInt32 RwFreeListPurgeAllFreeLists(void) { EAXJMP(0x611300); }
NAK RwUInt32 RwStreamRead(RwStream * stream, void *buffer, RwUInt32 length) { EAXJMP(0x611C80); }
NAK RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut) { EAXJMP(0x63C420); }
NAK RwCamera *RwCameraBeginUpdate(RwCamera * camera) { EAXJMP(0x6260E0); }
NAK RwCamera *RwCameraClear(RwCamera *camera, RwRGBA *colour, RwInt32 clearMode) { EAXJMP(0x626290); }
NAK RwCamera *RwCameraEndUpdate(RwCamera * camera) { EAXJMP(0x6260D0); }
NAK RwCamera *RwCameraSetViewWindow(RwCamera *camera, const RwV2d *viewWindow)  { EAXJMP(0x626360); }
NAK RwRaster *RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags) { EAXJMP(0x63CED0); }
NAK RwBool RwRasterDestroy(RwRaster * raster) { EAXJMP(0x63CCC0); }
NAK RwRaster *RwRasterUnlockPalette(RwRaster * raster) { EAXJMP(0x63CC90); }
NAK RwUInt8 *RwRasterLockPalette(RwRaster * raster, RwInt32 lockMode) { EAXJMP(0x63CD80); }
NAK RwUInt8 *RwRasterLock(RwRaster * raster, RwUInt8 level, RwInt32 lockMode)  { EAXJMP(0x63CF70); }
NAK RwRaster *RwRasterUnlock(RwRaster * raster) { EAXJMP(0x63CC70); }
NAK RwInt32 RwRasterRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) { EAXJMP(0x63CD50); }
NAK RwTexture *RwTextureSetName(RwTexture * texture, const RwChar * name)  { EAXJMP(0x62F540); }
NAK RwTexture *RwTextureSetMaskName(RwTexture * texture, const RwChar * maskName)  { EAXJMP(0x62F5B0); }
NAK RwTexture *RwTextureCreate(RwRaster * raster)  { EAXJMP(0x62F460); }
NAK RwBool RwTextureDestroy(RwTexture *texture) { EAXJMP(0x62F4C0); }
NAK RwBool RwTexDictionaryDestroy(RwTexDictionary * dict) { EAXJMP(0x62F340); }
NAK RwTexture *RwTexDictionaryFindNamedTexture(RwTexDictionary * dict, const RwChar * name) { EAXJMP(0x62F660); }
NAK const RwSphere *RpAtomicGetWorldBoundingSphere(RpAtomic *  atomic) { EAXJMP(0x061A4D0); }
NAK void   RwD3D8GetRenderState(RwUInt32 state, void *value) { EAXJMP(0x65A3A0); }
NAK RwBool RwD3D8SetRenderState(RwUInt32 state, RwUInt32 value) { EAXJMP(0x65A350); }
NAK RwBool RwD3D8CameraAttachWindow(void *camera, void *hwnd) { EAXJMP(0x641B70); }
NAK RwBool RwD3D8DrawIndexedPrimitive(RwUInt32 primitiveType, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 numIndices) { EAXJMP(0x643210); }
NAK RwBool RwD3D8DrawPrimitive(RwUInt32 primitiveType, RwUInt32 startVertex, RwUInt32 numVertices) { EAXJMP(0x6432A0); }
NAK RwBool RwD3D8SetTextureStageState(RwUInt32 stage, RwUInt32 type, RwUInt32 value) { EAXJMP(0x65A3C0); }
NAK RwBool RwD3D8SetTexture(RwTexture *texture, RwUInt32 stage) { EAXJMP(0x65B930); }
NAK RwBool RwD3D8SetPixelShader(RwUInt32 handle) { EAXJMP(0x643130); }
NAK RwBool RwD3D8SetVertexShader(RwUInt32 handle) { EAXJMP(0x6430F0); }
NAK RwBool RwD3D8SetStreamSource(RwUInt32 streamNumber, void *streamData, RwUInt32 stride) { EAXJMP(0x643170); }
NAK RwBool RwD3D8SetSurfaceProperties(const RwRGBA *color, const RwSurfaceProperties *surfaceProps, RwBool modulate) { EAXJMP(0x6435F0); }
NAK RwBool RwD3D8SetIndices(void *indexData, RwUInt32 baseVertexIndex) { EAXJMP(0x6431C0); }
NAK RwBool RwD3D8SetTransform(RwUInt32 state, const void *matrix) { EAXJMP(0x643330); }
NAK RwBool RwD3D8CameraIsSphereFullyInsideFrustum(const void *camera, const void *sphere) { EAXJMP(0x643DF0); }
NAK RwBool RwD3D8CameraIsBBoxFullyInsideFrustum(const void *camera, const void *boundingBox) { EAXJMP(0x643E50); }
NAK RwBool _rwD3D8BeginScene(void) { EAXJMP(0x641500); }
NAK RwBool _rwD3D8RenderStateIsVertexAlphaEnable(void) { EAXJMP(0x65BFE0); }
NAK RwBool _rwD3D8RenderStateVertexAlphaEnable(RwBool enable)  { EAXJMP(0x65BD70); }
NAK RwRaster * _rwD3D8RWGetRasterStage(RwUInt32 stage) { EAXJMP(0x65B920); }
NAK RwBool _rwD3D8RWSetRasterStage(RwRaster *raster, RwUInt32 stage) { EAXJMP(0x65B450); }
NAK RwBool _rwD3D8TextureHasAlpha(const RwTexture *texture) { EAXJMP(0x65CD00); }
NAK void _rwD3D8RenderStateFlushCache(void) { EAXJMP(0x65A280); }
NAK RwBool _rwD3D8RenderStateSrcBlend(RwBlendFunction srcBlend) { EAXJMP(0x65B3B0); }
NAK RwBool _rwD3D8RenderStateDestBlend(RwBlendFunction dstBlend) { EAXJMP(0x65B400); }
NAK RwBool _rwD3D8CheckValidTextureFormat(RwInt32 format) { EAXJMP(0x643880); }
NAK RwBool _rwD3D8CheckValidZBufferFormat(RwInt32 format) { EAXJMP(0x643820); }
NAK RwBool _rwD3D8CheckValidCameraTextureFormat(RwInt32 format)  { EAXJMP(0x6438C0); }
NAK RwUInt32 rwD3D8FindRwFormat(D3DFORMAT d3d8Format)  { EAXJMP(0x65F200); }
NAK RwMatrix *RwMatrixCreate(void) { EAXJMP(0x61A260); }
NAK RwBool RwMatrixDestroy(RwMatrix * mpMat) { EAXJMP(0x61A230); }
NAK RwMatrix *RwMatrixMultiply(RwMatrix * matrixOut, const RwMatrix * MatrixIn1, const RwMatrix * matrixIn2) { EAXJMP(0x6190C0); }
NAK RwMatrix *RwMatrixInvert(RwMatrix * matrixOut, const RwMatrix * matrixIn) { EAXJMP(0x619880); }
NAK RwMatrix *RwMatrixOrthoNormalize(RwMatrix * matrixOut, const RwMatrix * matrixIn) { EAXJMP(0x619130); }
NAK RwMatrix *RwMatrixOptimize(RwMatrix * matrix, const RwMatrixTolerance *tolerance) { EAXJMP(0x618FF0); }
NAK RwReal _rwInvSqrt(const RwReal num)  { EAXJMP(0x615500); }
NAK RwMatrix *RwFrameGetLTM(RwFrame * frame) { EAXJMP(0x6184C0); }
NAK RwFreeList *RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint) { EAXJMP(0x610D10); }
NAK RwReal RpLightGetConeAngle(const RpLight *light) { EAXJMP(0x621200); }
NAK RpWorldSectorCallBackRender RpWorldGetSectorRenderCallBack(const RpWorld * world)  { EAXJMP(0x6163D0); }
NAK RpClump *RpClumpForAllAtomics(RpClump * clump, RpAtomicCallBack callback, void *pData) { EAXJMP(0x61AC40); }
NAK RwBool RwD3D8CreatePixelShader(const RwUInt32 *function, RwUInt32 *handle) { EAXJMP(0x643CC0); }
NAK void RwD3D8DeletePixelShader(RwUInt32 handle) { EAXJMP(0x643D40); }
NAK RwBool RwD3D8SetPixelShaderConstant(RwUInt32 registerAddress, const void *constantData, RwUInt32  constantCount) { EAXJMP(0x643DB0); }
NAK RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor initCB, RwPluginObjectDestructor termCB) { EAXJMP(0x6125D0); }
NAK const void *RwD3D8GetCaps(void) { EAXJMP(0x643DE0); }
NAK void RwD3D8GetTransform(RwUInt32 state, void *matrix) { EAXJMP(0x643470); }
NAK RwBool D3D8AtomicDefaultInstanceCallback(void *object, RxD3D8InstanceData *instancedData, RwBool reinstance) { EAXJMP(0x669B00); }
NAK RwBool D3D8AtomicDefaultReinstanceCallback(void *object, RwResEntry *resEntry, const RpMeshHeader *meshHeader, RxD3D8AllInOneInstanceCallBack instanceCallback) { EAXJMP(0x66A670); }
NAK RxNodeDefinition *RxNodeDefinitionGetD3D8AtomicAllInOne(void) { EAXJMP(0x66ABC0); }
NAK RxPipelineNode *RxPipelineFindNodeByName(RxPipeline *pipeline, const RwChar *name, RxPipelineNode *start, RwInt32 *nodeIndex) { EAXJMP(0x668700); }
NAK void _rpMatFXD3D8AtomicMatFXEnvRender(RxD3D8InstanceData *inst, int flags, int sel, RwTexture *texture, RwTexture *envMap) { EAXJMP(0x663A30); }
NAK void _rpMatFXD3D8AtomicMatFXDualPassRender(RxD3D8InstanceData *instancedData, int flags, RwTexture *texture, RwTexture *dualTexture) { EAXJMP(0x662EA0); }
NAK RwBool RpMatFXAtomicQueryEffects( RpAtomic *atomic ) { EAXJMP(0x647C20); }
NAK RwTexture *RpMatFXMaterialGetDualTexture( const RpMaterial *material ) { EAXJMP(0x648070); }
NAK RpMaterial *RpMatFXMaterialSetDualBlendModes( RpMaterial *material, RwBlendFunction srcBlendMode, RwBlendFunction dstBlendMode ) { EAXJMP(0x648000); }
NAK RpMaterial *RpMatFXMaterialSetDualTexture( RpMaterial *material, RwTexture *texture ) { EAXJMP(0x647F70); }
NAK RwBool RwD3D8DynamicVertexBufferCreate(RwUInt32 fvf, RwUInt32 size, void **vertexBuffer) { EAXJMP(0x643B70); }
NAK void RwD3D8DynamicVertexBufferDestroy(void *vertexBuffer) { EAXJMP(0x643C60); }
NAK RwBool RwD3D8DynamicVertexBufferLock(RwUInt32 vertexSize, RwUInt32 numVertex, void **vertexBufferOut, void **vertexDataOut, RwUInt32 *baseIndexOut) { EAXJMP(0x643EE0); }
NAK RwBool RwD3D8DynamicVertexBufferUnlock(void *vertexBuffer) { EAXJMP(0x644060); }
NAK RwBool RwD3D8IndexBufferCreate(RwUInt32 numIndices, void **indexBuffer) { EAXJMP(0x643C90); }
NAK void _rwD3D8CheckRasterSize(RwInt32 *width, RwInt32 *height, RwUInt32 rasterFlags) { EAXJMP(0x65FDD0); }
NAK RwInt32 _rwD3D8FindCorrectRasterFormat(RwRasterType type, RwInt32 flags) { EAXJMP(0x65E3A0); }
NAK RwBool rwD3D8CreateTextureRaster(RwRaster *raster, _rwD3D8RasterExt *rasExt)   { EAXJMP(0x65F250); }
NAK RwBool _rwD3D8NativeTextureRead(void *streamIn, void *textureIn, RwInt32 unused3 __RWUNUSED__)  { EAXJMP(0x645F50); }
NAK RwUInt32 RpRandom(void) { EAXJMP(0x614D50); }
NAK void _rxD3D8DefaultRenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags) { EAXJMP(0x66B050); }
NAK void _rwD3D8Im2DRenderClose(void)  { EAXJMP(0x659640); }
NAK void _rwD3D8Im3DRenderClose(void)  { EAXJMP(0x661910); }
NAK void _rxD3D8VideoMemoryRasterListRelease(void) { EAXJMP(0x65E890); }
NAK RwBool _rxD3D8VideoMemoryRasterListRestore(void) { EAXJMP(0x65E910); }
NAK RwBool D3D8DynamicVertexBufferListRestore(void) { EAXJMP(0x641A00); }
NAK RwBool D3D8VertexShaderListRestore(void) { EAXJMP(0x641AC0); }
NAK RwBool D3D8PixelShaderListRestore(void) { EAXJMP(0x641B20); }
NAK void _rwD3D8RenderStateReset(void) { EAXJMP(0x65AA20); }
NAK RwBool D3D8RestoreCacheLights(void) { EAXJMP(0x641930); }
NAK RwBool _rwD3D8Im2DRenderOpen(void) { EAXJMP(0x659580); }
NAK RwBool _rwD3D8Im3DRenderOpen(void) { EAXJMP(0x661930); }
NAK RwBool D3D8DynamicVertexBufferManagerCreate(void) { EAXJMP(0x63FF10); }
NAK void D3D8ClearCacheMatrix(void) { EAXJMP(0x63FFD0); }
NAK void D3D8DynamicVertexBufferListRelease(void) { EAXJMP(0x6418A0); }
NAK void D3D8DynamicVertexBufferManagerDestroy(void) { EAXJMP(0x63FEB0); }
NAK RwBool _rwD3D8RWGetRenderState(RwRenderState nState, void *pParam) { EAXJMP(0x65B130); }
NAK RwBool _rwD3D8RWSetRenderState(RwRenderState nState, void *pParam) { EAXJMP(0x65C270); }
NAK RwBool _rwD3D8Im2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex *verts, RwInt32 numVerts) { EAXJMP(0x659AD0); }
NAK RwBool _rwD3D8RGBToPixel(void *pixel, void *col, RwInt32 format) { EAXJMP(0x65FE80); }
NAK RwBool _rwD3D8PixelToRGB(void *rgb, void *pixel, RwInt32 format) { EAXJMP(0x6600D0); }

RwBool
RwD3D8SetVertexShaderConstant(RwUInt32 registerAddress,
                              const void *constantData,
                              RwUInt32 constantCount)
{
    HRESULT             hr = D3D_OK;

    RWAPIFUNCTION(RWSTRING("RwD3D8SetVertexShaderConstant"));

    hr = DXCHECK(IDirect3DDevice8_SetVertexShaderConstant(_RwD3DDevice,
                                                          registerAddress,
                                                          constantData,
                                                          constantCount));

    RWRETURN(SUCCEEDED(hr));
}

void
RwD3D8DeleteVertexShader(RwUInt32 handle)
{
    _rxD3D8VertexShader *previous;
    _rxD3D8VertexShader *current;

    RWAPIFUNCTION(RWSTRING("RwD3D8DeleteVertexShader"));

    DXCHECK(IDirect3DDevice8_DeleteVertexShader(_RwD3DDevice, handle));

    /* remove it from the list */
    previous = NULL;
    current = VertexShaderList;
    while (current != NULL)
    {
        if (current->handle != NULL &&
            *(current->handle) == handle)
        {
            break;
        }

        previous = current;
        current = current->next;
    }

    if (current != NULL)
    {
        if (current == VertexShaderList)
        {
            VertexShaderList = VertexShaderList->next;
        }
        else
        {
            RWASSERT(previous != NULL);

            previous->next = current->next;
        }

        RwFreeListFree(VertexShaderFreeList, current);
    }

    RWRETURNVOID();
}

RwBool
RwD3D8CreateVertexShader(const RwUInt32 * declaration,
                         const RwUInt32 * function,
                         RwUInt32 * handle, RwUInt32 usage)
{
    HRESULT             hr = D3D_OK;

    RWAPIFUNCTION(RWSTRING("RwD3D8CreateVertexShader"));

    hr = DXCHECK(IDirect3DDevice8_CreateVertexShader(_RwD3DDevice,
                                                     (const DWORD *)declaration,
                                                     (const DWORD *)function,
                                                     (DWORD *)handle,
                                                     usage));

    if (SUCCEEDED(hr))
    {
        _rxD3D8VertexShader *newVertexShader;

        /* Force to reset any vertex shader again */
        LastVertexShaderUsed = 0xffffffff;

        /* Add it to the list */
        if (VertexShaderFreeList == NULL)
        {
            VertexShaderFreeList = RwFreeListCreate(sizeof(_rxD3D8VertexShader),
                                                    50, 4,
                                                    rwID_DRIVERMODULE |
                                                    rwMEMHINTDUR_EVENT); /* ~1024 Bytes */
        }

        newVertexShader =
                (_rxD3D8VertexShader *)RwFreeListAlloc(VertexShaderFreeList,
                    rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);

        newVertexShader->declaration = declaration;
        newVertexShader->function = function;
        newVertexShader->handle = handle;
        newVertexShader->usage = usage;
        newVertexShader->next = VertexShaderList;

        VertexShaderList = newVertexShader;
    }

    RWRETURN(SUCCEEDED(hr));
}

void RpD3D8LightGetAttenuationParams(const RpLight *light,
                                RpD3D8AttenuationParams *params)
{
    const rpD3D8LightData     *lightData;

    RWAPIFUNCTION(RWSTRING("RpD3D8LightGetAttenuationParams"));
    RWASSERT(light && params);

    lightData = D3D8LIGHTDATAFROMCONSTLIGHT(light);

    *params = lightData->attenuation;

    RWRETURNVOID();
}

RwRaster           *
RwRasterPushContext(RwRaster * raster)
{
    RWAPIFUNCTION(RWSTRING("RwRasterPushContext"));
    RWASSERT(rasterModule.numInstances);
    RWASSERT(raster);
    RWASSERT(RWRASTERGLOBAL(rasterSP) < (rwRASTERCONTEXTSTACKSIZE - 1));
/*
    RWASSERTM((RWSRCGLOBAL(curCamera) == NULL),
        (RWSTRING("RwRasterPushContext has been used between "
                 "RwCameraBeginUpdate and RwCameraEndUpdate.")));
*/
    /* Push onto the stack */
    if (RWSRCGLOBAL(stdFunc[rwSTANDARDSETRASTERCONTEXT])
        (NULL, (void *) raster, 0))
    {
        RwInt32             top = ++RWRASTERGLOBAL(rasterSP);
        RWRASTERGLOBAL(rasterStack[top]) = raster;
        RWRETURN(raster);
    }

    /* Device failure */
    RWRETURN((RwRaster *) NULL);
}

RwRaster           *
RwRasterPopContext(void)
{
    RwInt32             top;
    RwRaster           *raster;
    RwStandardFunc      SetRasterContextFunc;
    RwRaster           *StackTop;

    RWAPIFUNCTION(RWSTRING("RwRasterPopContext"));
    RWASSERT(rasterModule.numInstances);
    RWASSERT(RWRASTERGLOBAL(rasterSP) > 0);
	/*
    RWASSERTM((RWSRCGLOBAL(curCamera) == NULL),
        (RWSTRING("RwRasterPushContext has been used between "
                 "RwCameraBeginUpdate and RwCameraEndUpdate.")));
	*/
    top = RWRASTERGLOBAL(rasterSP)--;
    raster = RWRASTERGLOBAL(rasterStack[top]);

    SetRasterContextFunc =
        RWSRCGLOBAL(stdFunc[rwSTANDARDSETRASTERCONTEXT]);

    top = RWRASTERGLOBAL(rasterSP);
    StackTop = RWRASTERGLOBAL(rasterStack[top]);

    RWASSERT(NULL != SetRasterContextFunc);

    /* Set up the new */
    if (SetRasterContextFunc(NULL, StackTop, 0))
    {
        RWRETURN(raster);
    }

    /* Device failure */
    RWRETURN((RwRaster *) NULL);
}

RwRaster           *
RwRasterRenderFast(RwRaster * raster, RwInt32 x, RwInt32 y)
{
    RwRect              rRect;

    RWAPIFUNCTION(RWSTRING("RwRasterRenderFast"));
    RWASSERT(rasterModule.numInstances);
    RWASSERT(raster);
    RWASSERT(RWRASTERGLOBAL(rasterSP) > 0);
    RWASSERT(RWSRCGLOBAL(curCamera) == NULL);

    rRect.x = x;
    rRect.y = y;

    RWSRCGLOBAL(stdFunc[rwSTANDARDRASTERRENDERFAST]) (raster, &rRect,
                                                      0);
    RWRETURN(raster);
}

void
rxD3D8VideoMemoryRasterListAdd(RwRaster *raster)
{
    RxD3D8VideoMemoryRaster *videoRaster;

    videoRaster = (RxD3D8VideoMemoryRaster *)
                                RwFreeListAlloc(VideoMemoryRastersFreeList,
                                    rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);

    videoRaster->raster = raster;
    videoRaster->next = VideoMemoryRasters;

    VideoMemoryRasters = videoRaster;

    RWRETURNVOID();
}

RwUInt32 FindAvailablePaletteIndex(void)
{
    RwUInt32 index;

    if (NumPaletteIndexFree)
    {
        NumPaletteIndexFree--;

        RWASSERT(PaletteIndexFree != NULL);
        index = PaletteIndexFree[NumPaletteIndexFree];
    }
    else
    {
        index = MaxPaletteIndex;

        MaxPaletteIndex ++;
    }

    RWRETURN(index);
}

void
AddAvailablePaletteIndex(RwUInt32 index)
{
    RWFUNCTION(RWSTRING("AddAvailablePaletteIndex"));
    RWASSERT(index < MaxPaletteIndex);

    if (NumPaletteIndexFree >= MaxPaletteIndexFree)
    {
        MaxPaletteIndexFree += 512;

        if (PaletteIndexFree == NULL)
        {
            PaletteIndexFree = (RwUInt16 *)
                               RwMalloc(MaxPaletteIndexFree * sizeof(RwUInt16),
                                rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT |
                                rwMEMHINTFLAG_RESIZABLE);
        }
        else
        {
            PaletteIndexFree = (RwUInt16 *)
                                RwRealloc(PaletteIndexFree,
                                          MaxPaletteIndexFree * sizeof(RwUInt16),
                                          rwID_DRIVERMODULE     |
                                          rwMEMHINTDUR_EVENT    |
                                          rwMEMHINTFLAG_RESIZABLE);
        }
    }

    RWASSERT(PaletteIndexFree != NULL);
    PaletteIndexFree[NumPaletteIndexFree++] = (RwUInt16)index;

    RWRETURNVOID();
}