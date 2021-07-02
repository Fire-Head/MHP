#include "common.h"
#include "rw.h"


LPDIRECT3DDEVICE8           &_RwD3DDevice = *(LPDIRECT3DDEVICE8*)0x824448;
RwInt32 &MatFXMaterialDataOffset = *(RwInt32*)0x824928;
RwInt32 &_RwD3D8RasterExtOffset = *(RwInt32*)0x8295A4;
RwInt32 &D3D8LightOffset = *(RwInt32*)0x827970;
RwInt32 &__rxPipelineGlobalsOffset = *(RwInt32*)0x829294;
static RwModuleInfo &rasterModule = *(RwModuleInfo*)0x8243A8;

_rxD3D8VertexShader *&VertexShaderList = *(_rxD3D8VertexShader **)0x8248E0;
RwUInt32     &LastVertexShaderUsed = *(RwUInt32*)0x7EB810;
static RwFreeList *&VertexShaderFreeList = *(RwFreeList **)0x8248E4;


NAK RwCamera *RwCameraBeginUpdate(RwCamera * camera) { EAXJMP(0x6260E0); }
NAK RwCamera *RwCameraEndUpdate(RwCamera * camera) { EAXJMP(0x6260D0); }

NAK RwRaster *RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags) { EAXJMP(0x63CED0); }
NAK RwBool RwRasterDestroy(RwRaster * raster) { EAXJMP(0x63CCC0); }
NAK const RwSphere *RpAtomicGetWorldBoundingSphere(RpAtomic *  atomic) { EAXJMP(0x061A4D0); }
NAK void   RwD3D8GetRenderState(RwUInt32 state, void *value) { EAXJMP(0x65A3A0); }
NAK RwBool RwD3D8SetRenderState(RwUInt32 state, RwUInt32 value) { EAXJMP(0x65A350); }
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
NAK RwBool _rwD3D8RenderStateIsVertexAlphaEnable(void) { EAXJMP(0x65BFE0); }
NAK RwBool _rwD3D8RenderStateVertexAlphaEnable(RwBool enable)  { EAXJMP(0x65BD70); }
NAK RwRaster * _rwD3D8RWGetRasterStage(RwUInt32 stage) { EAXJMP(0x65B920); }
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

NAK RwBool RwD3D8DynamicVertexBufferCreate(RwUInt32 fvf, RwUInt32 size, void **vertexBuffer) { EAXJMP(0x643B70); }
NAK void RwD3D8DynamicVertexBufferDestroy(void *vertexBuffer) { EAXJMP(0x643C60); }
NAK RwBool RwD3D8DynamicVertexBufferLock(RwUInt32 vertexSize, RwUInt32 numVertex, void **vertexBufferOut, void **vertexDataOut, RwUInt32 *baseIndexOut) { EAXJMP(0x643EE0); }
NAK RwBool RwD3D8DynamicVertexBufferUnlock(void *vertexBuffer) { EAXJMP(0x644060); }
NAK RwBool RwD3D8IndexBufferCreate(RwUInt32 numIndices, void **indexBuffer) { EAXJMP(0x643C90); }

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