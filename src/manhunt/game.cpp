#include "common.h"
#include "game.h"


Immediate &CRenderer::ms_im = *(Immediate*)0x7D35A4;
unsigned int& CGameTime::ms_currGameTime = *(unsigned int*)0x756270;
int &CScene::ms_stepMode = *(int*)0x715BA0;
CPlayer* &CScene::ms_pPlayer = *(CPlayer**)0x715B9C;
RwTexture *(&CCharacterDamageManager::ms_textures)[CCharacterDamageManager::NUM_TEXTURES] = *(RwTexture *(*)[CCharacterDamageManager::NUM_TEXTURES])*(int *)(0x7AE950);
int &CCharacterDamageManager::ms_iShowPlayerDamageMap = *(int *)0x7AE94C;
int &CCharacterDamageManager::ms_iUpdatePlayerDamgeMapAlpha = *(int*)0x7AE948;
int &CFrontend::ms_FXModeMenu = *(int*)0x7D6B30;
int &CFrontend::ms_FXArtFilter = *(int*)0x7CF110;
int &CFrontend::ms_EnableZClearOnly = *(int*)0x7CF290;
int &CFrontend::ms_Nightvision = *(int*)0x7CF0FC;
int &CFrontend::ms_FXModeFade = *(int*)0x7CF294;
RwTexture *(&CFrontend::ms_damageTextures)[CCharacterDamageManager::NUM_TEXTURES] = *(RwTexture *(*)[CCharacterDamageManager::NUM_TEXTURES])*(int *)(0x7D3838);
CScreen &CFrontend::ms_scrn = *(CScreen*)0x7D3440;
CCamGlobals &gCameraGlobals = *(CCamGlobals*)0x7A0FAC;

void CRenderer::SetIngameInfoRenderStates(long mode)
{
	((void (__cdecl *)(long))0x5F5A80)(mode);
}

void CRenderer::RenderStateSetBlend(int src, int dst)
{
	((void (__cdecl *)(int, int))0x5F5C90)(src, dst);
}

void CRenderer::PopRenderState()
{
	((void (__cdecl *)())0x5F5DE0)();
}

void CRenderer::PushAndSetRenderState(RwRenderState state, void *value)
{
	((void (__cdecl *)(RwRenderState, void*))0x5F5E60)(state, value);
}

void CRenderer::RenderStateAlphaSet()
{
	((void (__cdecl *)())0x5F5EB0)();
}

void CRenderer::RenderStateAlphaCombine_ONE()
{
	((void (__cdecl *)())0x5F5EE0)();
}

void CRenderer::DrawQuad2dBegin(RwRaster *raster)
{
	((void (__cdecl *)(RwRaster *))0x5F83F0)(raster);
}

void CRenderer::DrawQuad2dEnd()
{
	((void (__cdecl *)())0x5F96A0)();
}

void CRenderer::DrawQuad2d(float posx, float posy, float scalex, float scaley, long R, long G, long B, long A, RwRaster *raster)
{
	((void (__cdecl *)(float, float, float, float, long, long, long, long, RwRaster *))0x5F96F0)(posx, posy, scalex, scaley, R, G, B, A, raster);
}

bool CRenderer::DrawQuad2dSetAll(float posx, float posy, float scalex, float scaley, float U1, float V1, float U2, float V2, float U3, float V3, float U4, float V4, long R1, long G1, long B1, long A1, long R2, long G2, long B2, long A2, long R3, long G3, long B3, long A3, long R4, long G4, long B4, long A4)
{
	return ((bool (__cdecl *)(float, float, float, float, float, float, float, float, float, float, float, float, long, long, long, long, long, long, long, long, long, long, long, long, long, long, long, long))0x5F8D20)(posx, posy, scalex, scaley, U1, V1, U2, V2, U3, V3, U4, V4, R1, G1, B1, A1, R2, G2, B2, A2, R3, G3, B3, A3, R4, G4, B4, A4);
}

void CRenderer::FlushTextures()
{
	;
}

void CRenderer::BlackBoxDraw(int alpha)
{
	SetIngameInfoRenderStates(0);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDONE);
	DrawQuad2d(0.0f, 0.0f, 1.0f, 1.0f, 0, 0, 0, alpha, NULL);
	SetIngameInfoRenderStates(1);
}

CVector &CEntity::GetPosition()
{
	return ((CVector & (__thiscall *)(CEntity *))0x4317E0)(this);
}

void CEntity::RenderLit()
{
	((void (__thiscall *)(CEntity *))0x435A50)(this);
}

void CUseable::RenderLit()
{
	((void (__thiscall *)(CEntity *))0x4D4D80)(this);
}

bool CFrontend::IsCameraFXActive()
{
	return ((bool (__cdecl *)())0x5EA2C0)();
}

NAK RpAtomic* GetFirstAtomicCallback(RpAtomic *atm, void *data) { EAXJMP(0x435880); }
NAK RwTexDictionary *TexDictLoad(const char * path) { EAXJMP(0x4777D0); }