#include "common.h"
#include "game.h"
#include "render\render.h"
#include "render\rendermatfx.h"
#include "render\rwmatfx.h"
#include "render\helpers.h"
#include "Render.h"
#include "settings.h"
#include "CPatch.h"


void SwitchableRenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
	if ( Settings::GetInstance()->bNewRender )
		_rxD3D8VSRenderCallback(repEntry, object, type, flags);
	else
		_rxD3D8DefaultRenderCallback(repEntry, object, type, flags);
}

void SwitchableMatFXDualPassRender(RxD3D8InstanceData *instancedData, int flags, RwTexture *texture, RwTexture *dualTexture)
{
	if ( Settings::GetInstance()->bMatFxDualPass )
		return MatFXDualPassRender(instancedData, flags, texture, dualTexture);
	else
		return _rpMatFXD3D8AtomicMatFXDualPassRender(instancedData, flags, texture, dualTexture);
}

void ZPush(RwRenderState state, void *value)
{
	if ( !Settings::GetInstance()->bPS2AlphaTest )
		CRenderer::PushAndSetRenderState(state, value);
}

void ZPop()
{
	if ( !Settings::GetInstance()->bPS2AlphaTest )
		CRenderer::PopRenderState();
}

static void Patch()
{
	// ZWrite For Alpha. (R* turned it off because pc has issues with alpha)
	CPatch::RedirectCall(0x475B30, ZPush);
	CPatch::RedirectCall(0x475B3B, ZPush);
	CPatch::RedirectCall(0x475B4D, ZPop);
	CPatch::RedirectCall(0x475B52, ZPop);

	// _rxD3D8DefaultRenderCallback
	CPatch::SetPointer(0x668C3B + 3, SwitchableRenderCallback);
	CPatch::SetPointer(0x669AEB + 3, SwitchableRenderCallback);
	CPatch::SetPointer(0x66BC87 + 2, SwitchableRenderCallback);

	//_rpMatFXD3D8AtomicMatFXDefaultRender
	CPatch::RedirectJump(0x662C60, _rpMatFXD3D8AtomicMatFXDefaultRender_fixed);

	//_rpMatFXD3D8AtomicMatFXRenderBlack
	CPatch::RedirectJump(0x662DE0, _rpMatFXD3D8AtomicMatFXRenderBlack_fixed);

	// _rpMatFXD3D8AtomicMatFXEnvRender
	CPatch::RedirectCall(0x664713, _rpMatFXD3D8AtomicMatFXEnvRender_ps2);
	CPatch::RedirectCall(0x665228, _rpMatFXD3D8AtomicMatFXEnvRender_ps2);

	// _rpMatFXD3D8AtomicMatFXDualPassRender
	CPatch::RedirectCall(0x665251, SwitchableMatFXDualPassRender);
	CPatch::RedirectCall(0x665264, SwitchableMatFXDualPassRender);

	// dual pass patch for default pipelines
	struct{unsigned int address;bool isindexed;} DualPassPatches[] =
	{
		// _rxD3D8DefaultRenderCallback
		{0x66B2AA, true},{0x66B2C0, false},
		// _rpMatFXD3D8AtomicMatFXDefaultRender
		{0x662D90, true},{0x662DA6, false},
		// _rpMatFXD3D8AtomicMatFXRenderBlack
		{0x662E72, true},{0x662E88, false},
		// _rpMatFXD3D8AtomicMatFXEnvRender
		{0x663C1F,true},{0x663E6F,true},{0x663D07,false},{0x663D74,true},{0x663D87,false},{0x663DE6,true},{0x663DF9,false},{0x664048,true},{0x66405B,false},{0x663F64,true},{0x663F77,false}
	};

	for ( int i = 0; i < ARRAY_SIZE(DualPassPatches); i++ )
		CPatch::RedirectCall(DualPassPatches[i].address, ((DualPassPatches[i].isindexed) ? (void*)RwD3D8DrawIndexedPrimitive_DualPass : (void*)RwD3D8DrawPrimitive_DualPass));
}

STARTPATCH
	{
		DBG("Render\n");

		Patch();
	}
ENDPATCH