#pragma once
#include <rwcore.h>
#include <rpworld.h>
#include <rprandom.h>
#include <d3d8.h>
#include <d3dx8.h>

#define RWAPIFUNCTION(function)
#define RWFUNCTION(function)
#define RWASSERT(condition)
#define RWRETURN(value) return(value)
#define RWRETURNVOID() return
#define RWERROR(errorcode)
#define DXCHECK(fn) (fn)
#define RWD3D8_ALIGN16 __declspec(align(16))

#define RwEngineInstance (*(RwGlobals **)0x82279C)
#define RWSRCGLOBAL(variable) \
   (((RwGlobals *)RwEngineInstance)->variable)
#define _rxPipelineGlobalsOffset __rxPipelineGlobalsOffset

#define RWRASTERGLOBAL(var)                             \
    (RWPLUGINOFFSET(rwRasterGlobals,                    \
                    RwEngineInstance,                   \
                    rasterModule.globalsOffset)->var)

#define RASTEREXTFROMRASTER(raster) \
    ((_rwD3D8RasterExt *)(((RwUInt8 *)(raster)) + _RwD3D8RasterExtOffset))

#define RASTEREXTFROMCONSTRASTER(raster)                        \
    ((const _rwD3D8RasterExt *)                                 \
     (((const RwUInt8 *)(raster)) + _RwD3D8RasterExtOffset))

#define D3D8LIGHTDATAFROMLIGHT(light)                          \
    (rpD3D8LightData*)(((RwInt8*)light) + D3D8LightOffset)

#define D3D8LIGHTDATAFROMCONSTLIGHT(light)                      \
    (const rpD3D8LightData*)(((const RwInt8*)light) + D3D8LightOffset)

#define MATFXMATERIALGETDATA(material)                               \
    ((rpMatFXMaterialData **)(((RwUInt8 *)material)+                 \
                              MatFXMaterialDataOffset))


#define FMT2TBL(_p) (((_p) & rwRASTERFORMATPIXELFORMATMASK) >> 8)

#define RwFeof   RWSRCGLOBAL(fileFuncs).rwfeof
#define RwFseek  RWSRCGLOBAL(fileFuncs).rwfseek
#define RwFtell  RWSRCGLOBAL(fileFuncs).rwftell

struct RxD3D8ResEntryHeader
{
    RwUInt16    serialNumber;   /**< The mesh headers serial number */
    RwUInt16    numMeshes;      /**< The number of meshes */
};

struct RxD3D8InstanceData
{
    RwUInt32        minVert;        /**< The vertex index where the
                                         meshes vertices begin. Needed for
                                         reinstancing, not for rendering */

    RwInt32         stride;         /**< Stride of vertex */

    RwInt32         numVertices;    /**< Number of vertices in the vertex buffer */
    RwInt32         numIndices;     /**< Number of indices to render the primitive */

    RpMaterial      *material;      /**< The material for this instanced data */

    RwUInt32        vertexShader;   /**< Vertex shader or FVF flags */

    RwUInt32        primType;       /**< Primitive type */

    void            *indexBuffer;   /**< Index buffer */
    void            *vertexBuffer;  /**< Vertex buffer */

    RwUInt32        baseIndex;      /**< Needed if we share the vertex buffer */

    RwUInt8         vertexAlpha;    /**< The prelight data contains alpha */
    RwUInt8         managed;        /**< Created by the Vertex Buffer Manager */
    RwUInt8         remapped;       /**< Vertices where remapped to save VB space */
};

#define rpMATFXALIGNMENT  rwFRAMEALIGNMENT

enum RpMatFXMaterialFlags
{
    rpMATFXEFFECTNULL       = 0,
    rpMATFXEFFECTBUMPMAP    = 1, /**<Bump mapping                 */
    rpMATFXEFFECTENVMAP     = 2, /**<Environment mapping          */
    rpMATFXEFFECTBUMPENVMAP = 3, /**<Bump and environment mapping */
    rpMATFXEFFECTDUAL       = 4, /**<Dual pass                    */

	rpMATFXEFFECTUVTRANSFORM     = 5,
	rpMATFXEFFECTDUALUVTRANSFORM = 6,

    rpMATFXEFFECTMAX,
    rpMATFXNUMEFFECTS       = rpMATFXEFFECTMAX - 1,

    rpMATFXFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpMatFXMaterialFlags  RpMatFXMaterialFlags;

enum MatFXPass
{
    rpSECONDPASS = 0,
    rpTHIRDPASS  = 1,
    rpMAXPASS    = 2
};
typedef enum MatFXPass MatFXPass;

typedef struct MatFXBumpMapData RWALIGN(MatFXBumpMapData, rpMATFXALIGNMENT);
struct MatFXBumpMapData
{
    RwFrame   *frame;
    RwTexture *texture;
    RwTexture *bumpTexture;
    RwReal    coef;
    RwReal    invBumpWidth;
};

typedef struct MatFXEnvMapData RWALIGN(MatFXEnvMapData, rpMATFXALIGNMENT);
struct MatFXEnvMapData
{
    RwFrame   *frame;
    RwTexture *texture;
    RwReal    coef;
    RwBool    useFrameBufferAlpha;
};

typedef struct MatFXDualData RWALIGN(MatFXDualData, rpMATFXALIGNMENT);
struct MatFXDualData
{
    RwTexture          *texture;
    RwBlendFunction     srcBlendMode;
    RwBlendFunction     dstBlendMode;

    /*--- device specific ---*/
};

typedef struct MatFXUVAnimData RWALIGN(MatFXUVAnimData, rpMATFXALIGNMENT);
struct MatFXUVAnimData
{
	RwMatrix           *baseTransform;
	RwMatrix           *dualTransform;
};

typedef union MatFXEffectUnion RWALIGN(MatFXEffectUnion, rpMATFXALIGNMENT);
union MatFXEffectUnion
{
    MatFXBumpMapData  bumpMap;
    MatFXEnvMapData   envMap;
    MatFXDualData     dual;
	MatFXUVAnimData   uvAnim;
};

typedef struct MatFXEffectData RWALIGN(MatFXEffectData, rpMATFXALIGNMENT);
struct MatFXEffectData
{
    MatFXEffectUnion     data;
    RpMatFXMaterialFlags flag;
};

struct rpMatFXMaterialData
{
    MatFXEffectData      data[rpMAXPASS];
    RpMatFXMaterialFlags flags;
};

typedef struct _rxD3D8VertexShader _rxD3D8VertexShader;
struct _rxD3D8VertexShader
{
    const RwUInt32  *declaration;
    const RwUInt32  *function;
    RwUInt32        *handle;
    RwUInt32        usage;
    _rxD3D8VertexShader *next;
};

typedef RwBool (*RxD3D8AllInOneInstanceCallBack)(void *object,
                                                 RxD3D8InstanceData *instancedData,
                                                 RwBool reinstance);

typedef struct _rwD3D8Palette _rwD3D8Palette;
struct _rwD3D8Palette
{
    PALETTEENTRY    entries[256];
    RwInt32     globalindex;
};

typedef LPDIRECT3DSURFACE8 LPSURFACE;
typedef LPDIRECT3DTEXTURE8 LPTEXTURE;

typedef struct _rwD3D8RasterExt _rwD3D8RasterExt;
struct _rwD3D8RasterExt
{
    LPTEXTURE               texture;
    _rwD3D8Palette          *palette;
    RwUInt8                 alpha;          /* This texture has alpha */
    RwUInt8                 cube;           /* This texture is a cube texture */
    RwUInt8                 face;           /* The face of a cube texture */
    RwUInt8                 pad;
    LPSURFACE               lockedSurface;
    D3DLOCKED_RECT          lockedRect;
    RwUInt32                lockedMipLevel;
    RwUInt32                dxtFormat;      /* DXT format */
    HWND                    window;
    LPDIRECT3DSWAPCHAIN8    swapChain;
};

struct RpD3D8AttenuationParams
{
    RwReal constant;    /**< Constant attenuation coefficient */
    RwReal linear;      /**< Linear attenuation coefficient */
    RwReal quadratic;   /**< Quadratic attenuation coefficient */
};

struct rpD3D8LightData
{
    RwInt32                 index;          /* D3D8 light index, -1 if not set before */
    RpD3D8AttenuationParams attenuation;    /* Attenuation params */
};

struct rwRasterGlobals
{
    RwRaster           *rasterStack[rwRASTERCONTEXTSTACKSIZE];
    RwInt32             rasterSP;
    RwRaster            dummyRaster;

    RwFreeList         *rasterFreeList;
};

typedef struct _rwD3D8NativeTexture _rwD3D8NativeTexture;
struct _rwD3D8NativeTexture
{
    RwInt32 id; /* RwPlatformID,(rwID_D3D8) defined in batype.h */
    RwInt32 filterAndAddress; /* Same as in babintex.c */
    RwChar  name[rwTEXTUREBASENAMELENGTH]; /* Texture name */
    RwChar  mask[rwTEXTUREBASENAMELENGTH]; /* Texture mask name */
};

typedef struct _rwD3D8NativeRaster _rwD3D8NativeRaster;
struct _rwD3D8NativeRaster
{
    RwInt32     format;         /* Raster format flags */
    RwBool      alpha;          /* This raster has an alpha component */
    RwUInt16    width;          /* Raster width */
    RwUInt16    height;         /* Raster height */
    RwUInt8     depth;          /* Raster depth */
    RwUInt8     numMipLevels;   /* The number of mip levels to load */
    RwUInt8     type;           /* The raster type */
    RwUInt8     dxtFormat;      /* 1-5 for DXT format 0 for normal */
};

typedef struct _rxD3D8VideoMemoryRaster RxD3D8VideoMemoryRaster;
struct _rxD3D8VideoMemoryRaster
{
    RwRaster                *raster;
    RxD3D8VideoMemoryRaster *next;
};

struct _rwRasterConvData
{
    D3DFORMAT   format;
    RwUInt8     depth;
    RwUInt8     alpha;
};

struct _rwD3D8AdapterInformation
{
    RwChar          name[MAX_DEVICE_IDENTIFIER_STRING];
    RwInt32         modeCount;
    D3DDISPLAYMODE  mode;
    RwInt32         displayDepth;
    RwInt32         flags;
};

struct RwRwDeviceGlobals
{
    /* Current camera */
    RwCamera           *curCamera;

    /* Memory allocators */
    RwMemoryFunctions  *memFuncs;
};

extern LPDIRECT3DDEVICE8           &_RwD3DDevice;
extern LPDIRECT3DSURFACE8          &_RwD3D8RenderSurface;
extern LPDIRECT3DSURFACE8          &_RwD3D8DepthStencilSurface;
extern D3DPRESENT_PARAMETERS       &Present;
extern D3DCAPS8                    &_RwD3D8DeviceCaps;
extern RwInt32                     &_RwD3D8ZBufferDepth;
extern _rwD3D8AdapterInformation   &_RwD3D8AdapterInformation;
extern RwUInt32                    &StencilClearValue;
extern RwBool                      &NeedToCopyFromBackBuffer;
extern RwBool                      &_RwHasStencilBuffer;
extern RwRwDeviceGlobals           &dgGGlobals;
extern HWND                        &WindowHandle;
extern RwUInt32 &SelectedMultisamplingLevels;
extern RwUInt32 &MaxMultisamplingLevels;
extern RwInt32 &MatFXMaterialDataOffset;
extern RwInt32 &_RwD3D8RasterExtOffset;
extern RwInt32 &D3D8LightOffset;
extern RwInt32 &__rxPipelineGlobalsOffset;
extern RwUInt16 *&PaletteIndexFree;
extern RwFreeList *&PaletteFreeList;
extern _rwRasterConvData (&_rwRasterConvTable)[11];
extern RwRaster *&_RwD3D8CurrentRasterTarget;
extern RwIm2DVertex (&_RwD3D8QuadVerts)[4];
extern RxD3D8VideoMemoryRaster *&VideoMemoryRasters;

void _rwProcessorForceSinglePrecision(void);
void   RwD3D8GetRenderState(RwUInt32 state, void *value);
RwBool RwD3D8SetRenderState(RwUInt32 state, RwUInt32 value);
RwBool RwD3D8CameraAttachWindow(void *camera, void *hwnd);
RwBool RwD3D8DrawIndexedPrimitive(RwUInt32 primitiveType, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 numIndices);
RwBool RwD3D8DrawPrimitive(RwUInt32 primitiveType, RwUInt32 startVertex, RwUInt32 numVertices);
RwBool RwD3D8SetTextureStageState(RwUInt32 stage, RwUInt32 type, RwUInt32 value);
RwBool RwD3D8SetTexture(RwTexture *texture, RwUInt32 stage);
RwBool RwD3D8SetPixelShader(RwUInt32 handle);
RwBool RwD3D8SetVertexShader(RwUInt32 handle);
RwBool RwD3D8SetStreamSource(RwUInt32 streamNumber, void *streamData, RwUInt32 stride);
RwBool RwD3D8SetSurfaceProperties(const RwRGBA *color, const RwSurfaceProperties *surfaceProps, RwBool modulate);
RwBool RwD3D8SetIndices(void *indexData, RwUInt32 baseVertexIndex);
RwBool RwD3D8SetTransform(RwUInt32 state, const void *matrix);
RwBool RwD3D8CameraIsSphereFullyInsideFrustum(const void *camera, const void *sphere);
RwBool RwD3D8CameraIsBBoxFullyInsideFrustum(const void *camera, const void *boundingBox);
RwBool   _rwD3D8BeginScene(void);
RwBool _rwD3D8RenderStateIsVertexAlphaEnable(void);
RwBool _rwD3D8RenderStateVertexAlphaEnable(RwBool enable);
RwRaster * _rwD3D8RWGetRasterStage(RwUInt32 stage);
RwBool _rwD3D8RWSetRasterStage(RwRaster *raster, RwUInt32 stage);
RwBool _rwD3D8TextureHasAlpha(const RwTexture *texture);
void _rwD3D8RenderStateFlushCache(void);
RwBool _rwD3D8RenderStateSrcBlend(RwBlendFunction srcBlend);
RwBool _rwD3D8RenderStateDestBlend(RwBlendFunction dstBlend);
RwBool _rwD3D8CheckValidTextureFormat(RwInt32 format);
RwBool _rwD3D8CheckValidZBufferFormat(RwInt32 format);
RwBool _rwD3D8CheckValidCameraTextureFormat(RwInt32 format);
RwUInt32 rwD3D8FindRwFormat(D3DFORMAT d3d8Format);
RwBool RwD3D8CreatePixelShader(const RwUInt32 *function, RwUInt32 *handle);
void RwD3D8DeletePixelShader(RwUInt32 handle);
RwBool RwD3D8SetPixelShaderConstant(RwUInt32 registerAddress, const void *constantData, RwUInt32  constantCount);

RwBool RwD3D8DynamicVertexBufferCreate(RwUInt32 fvf, RwUInt32 size, void **vertexBuffer);
void RwD3D8DynamicVertexBufferDestroy(void *vertexBuffer);
RwBool RwD3D8DynamicVertexBufferLock(RwUInt32 vertexSize, RwUInt32 numVertex, void **vertexBufferOut, void **vertexDataOut, RwUInt32 *baseIndexOut);
RwBool RwD3D8DynamicVertexBufferUnlock(void *vertexBuffer);
RwBool RwD3D8IndexBufferCreate(RwUInt32 numIndices, void **indexBuffer);

RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor initCB, RwPluginObjectDestructor termCB);

const void *RwD3D8GetCaps(void);
void RwD3D8GetTransform(RwUInt32 state, void *matrix);
RwBool D3D8AtomicDefaultInstanceCallback(void *object, RxD3D8InstanceData *instancedData, RwBool reinstance);
RwBool D3D8AtomicDefaultReinstanceCallback(void *object, RwResEntry *resEntry, const RpMeshHeader *meshHeader, RxD3D8AllInOneInstanceCallBack instanceCallback);

RwBool RwD3D8CreateVertexShader(const RwUInt32 * declaration, const RwUInt32 * function, RwUInt32 * handle, RwUInt32 usage);
void RwD3D8DeleteVertexShader(RwUInt32 handle);
RwBool RwD3D8SetVertexShaderConstant(RwUInt32 registerAddress, const void *constantData, RwUInt32 constantCount);

void RpD3D8LightGetAttenuationParams(const RpLight *light, RpD3D8AttenuationParams *params);

void _rpMatFXD3D8AtomicMatFXEnvRender(RxD3D8InstanceData *inst, int flags, int sel, RwTexture *texture, RwTexture *envMap);
void _rpMatFXD3D8AtomicMatFXDualPassRender(RxD3D8InstanceData *instancedData, int flags, RwTexture *texture, RwTexture *dualTexture);

RwBool RpMatFXAtomicQueryEffects( RpAtomic *atomic );
RwTexture *RpMatFXMaterialGetDualTexture( const RpMaterial *material );
RpMaterial *RpMatFXMaterialSetDualBlendModes( RpMaterial *material, RwBlendFunction srcBlendMode, RwBlendFunction dstBlendMode );
RpMaterial *RpMatFXMaterialSetDualTexture( RpMaterial *material, RwTexture *texture );

void _rxD3D8DefaultRenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags);
void rxD3D8VideoMemoryRasterListAdd(RwRaster *raster);
RwInt32 _rwD3D8FindCorrectRasterFormat(RwRasterType type, RwInt32 flags);
void _rwD3D8CheckRasterSize(RwInt32 *width, RwInt32 *height, RwUInt32 rasterFlags);
RwBool rwD3D8CreateTextureRaster(RwRaster *raster, _rwD3D8RasterExt *rasExt);
RwBool _rwD3D8NativeTextureRead(void *streamIn, void *textureIn, RwInt32 unused3 __RWUNUSED__);

RwUInt32 FindAvailablePaletteIndex(void);
void AddAvailablePaletteIndex(RwUInt32 index);

RwBool D3D8DeviceReleaseVideoMemory(void);
RwBool D3D8DeviceRestoreVideoMemory(void);

RwBool _rwD3D8RWGetRenderState(RwRenderState nState, void *pParam);
RwBool _rwD3D8RWSetRenderState(RwRenderState nState, void *pParam);
RwBool _rwD3D8Im2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex *verts, RwInt32 numVerts);

RwBool _rwD3D8RGBToPixel(void *pixel, void *col, RwInt32 format);
RwBool _rwD3D8PixelToRGB(void *rgb, void *pixel, RwInt32 format);

void RwD3D8EngineSetRefreshRate(RwUInt32 refreshRate);