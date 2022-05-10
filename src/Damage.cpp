#include "common.h"
#include "rw.h"
#include "game.h"
#include "render\helpers.h"
#include "Damage.h"
#include "settings.h"
#include "CPatch.h"

#ifndef MH_PS2
RwTexDictionary *CCharacterDamageManagerEx::ms_txd;
const char *CCharacterDamageManagerEx::ms_damageFilenames[NUM_DAMAGE_TEXTURES] = { "damageplayer", "damage01", "damage02", "damage03", "damage04", "damage05", "damage06", "damage07", "damage08", "damage09", "damage10", "damage11", "damage12", "damage13", "damage14", "damage15", "damage16" };
RwTexture *CCharacterDamageManagerEx::ms_damageTextures[NUM_DAMAGE_TEXTURES];
_rwD3D8Palette *CCharacterDamageManagerEx::ms_damageTexturePalettes[NUM_DAMAGE_TEXTURES];
RwTexture *CCharacterDamageManagerEx::ms_damageSWTextures[NUM_DAMAGE_TEXTURES];
bool CCharacterDamageManagerEx::ms_bHWPalette;

RwTexture *CCharacterDamageManagerEx::CreateBlankTexture(const RwTexture *texture)
{
	if ( texture )
	{
		RwRaster *srcRaster = RwTextureGetRaster(texture);
		if ( srcRaster )
		{
			RwRaster *raster = RwRasterCreate(srcRaster->width, srcRaster->height, 0, rwRASTERFORMAT8888 | rwRASTERTYPETEXTURE);
			if ( raster )
			{
				RwTexture *result = RwTextureCreate(raster);
				if ( result )
				{
					RwTextureSetFilterMode(result, RwTextureGetFilterMode(texture));
					RwTextureSetAddressingU(result, RwTextureGetAddressingU(texture));
					RwTextureSetAddressingV(result, RwTextureGetAddressingV(texture));
					RwTextureSetName(result, RwTextureGetName(texture));
					RwTextureSetMaskName(result, RwTextureGetMaskName(texture));
					return result;
				}
			}
		}
	}

	return NULL;
}

void CCharacterDamageManagerEx::ConvertPalettedTexture(const RwTexture *palettedTexture, RwTexture *texture)
{
	if ( palettedTexture && texture )
	{
		RwRaster *praster = RwTextureGetRaster(palettedTexture);
		RwRaster *nraster = RwTextureGetRaster(texture);

		RwRGBA *ppalette  = (RwRGBA *)RwRasterLockPalette(praster, rwRASTERLOCKREAD);
		RwUInt8 *pindexes  = (RwUInt8 *)RwRasterLock(praster, 0, rwRASTERLOCKREAD);

		praster->privateFlags &= ~rwRASTERPALETTELOCKEDWRITE;

		if ( ppalette && pindexes )
		{
			RwRGBA *data  = (RwRGBA *)RwRasterLock(nraster, 0, rwRASTERLOCKWRITE);
			if ( data )
			{
				for ( int i = 0; i < praster->width*praster->height; i++ )
				{
					data[i].red = ppalette[pindexes[i]].blue;
					data[i].green = ppalette[pindexes[i]].green;
					data[i].blue = ppalette[pindexes[i]].red;
					data[i].alpha = ppalette[pindexes[i]].alpha;
				}
				RwRasterUnlock(nraster);
			}
		}
		RwRasterUnlock(praster);
		RwRasterUnlockPalette(praster);
	}
}

RwTexDictionary *PalettedTexDictLoad(const char * path)
{
	RwTexDictionary *result;

	{
		RwStandardFunc oldread = RWSRCGLOBAL(stdFunc[rwSTANDARDNATIVETEXTUREREAD]);

		RWSRCGLOBAL(stdFunc[rwSTANDARDNATIVETEXTUREREAD]) = PalettedTextureRead;
		result = TexDictLoad(path);
		RWSRCGLOBAL(stdFunc[rwSTANDARDNATIVETEXTUREREAD]) = oldread;
	}

	return result;
}

void CCharacterDamageManagerEx::LoadTextures()
{
	char path[MAX_PATH];

	GetFilePath("MHP\\damage.txd", path, MAX_PATH);

	DBG("%s\n", path);

	//ms_txd = ms_bHWPalette ? TexDictLoad(path) : PalettedTexDictLoad(path);
	ms_txd = PalettedTexDictLoad(path);

	if ( ms_txd )
	{
		for ( int i = 0; i < NUM_DAMAGE_TEXTURES; i++ )
		{
			ms_damageTextures[i] = RwTexDictionaryFindNamedTexture(ms_txd, ms_damageFilenames[i]);
			if ( !ms_bHWPalette )
			{
				ms_damageTexturePalettes[i] = RASTEREXTFROMRASTER(RwTextureGetRaster(ms_damageTextures[i]))->palette;
				ms_damageSWTextures[i] = CreateBlankTexture(ms_damageTextures[i]);
			}
		}
	}
	else
		ErrorMsg("Can't load damage.txd");
}

void CCharacterDamageManagerEx::Shutdown()
{
	for ( int i = 0; i < NUM_DAMAGE_TEXTURES; i++ )
	{
		if ( !ms_bHWPalette )
			RASTEREXTFROMRASTER(RwTextureGetRaster(ms_damageTextures[i]))->palette = ms_damageTexturePalettes[i];

		ms_damageTextures[i] = NULL;

		if ( ms_damageSWTextures[i] )
		{
			RwTextureDestroy(ms_damageSWTextures[i]);
			ms_damageSWTextures[i] = NULL;
		}
	}

	if ( ms_txd )
	{
		RwTexDictionaryDestroy(ms_txd);
		ms_txd = NULL;
	}
}

RwTexture *CCharacterDamageManagerEx::GetSWTexture(const RwTexture *texture)
{
	if ( texture )
	{
		for ( int i = 0; i < NUM_DAMAGE_TEXTURES; i++ )
		{
			if ( ms_damageTextures[i] == texture )
				return ms_damageSWTextures[i];
		}
	}

	return NULL;
}

void CCharacterDamageManagerEx::SetSWPalette(bool enable)
{
	for ( int i = 0; i < NUM_DAMAGE_TEXTURES; i++ )
		RASTEREXTFROMRASTER(RwTextureGetRaster(ms_damageTextures[i]))->palette = enable ? ms_damageTexturePalettes[i] : NULL;
}

#endif // #ifndef MH_PS2

int CCharacterDamageManagerEx::ms_paletteMap[256];

void CCharacterDamageManagerEx::InitTextures()
{
#ifdef MH_PS2
	for ( int i = 0; i < NUM_TEXTURES; i++ )
		ms_textures[i] = CFrontend::ms_damageTextures[i];
#else

	ms_bHWPalette = _rwD3D8CheckValidTextureFormat(D3DFMT_P8) ? true : false;

	LoadTextures();

	for ( int i = 0; i < NUM_TEXTURES; i++ )
		ms_textures[i] = ms_damageTextures[NUM_PLAYER_TEXTURES+i];
#endif
}

void CCharacterDamageManagerEx::InitPaletteMap()
{
	static bool Init = false;

	int a = 0;

	if ( !Init )
	{
		int n = 0;
		int m = 0;

		for ( int i = 0; i < 16; i++ )
		{
			for ( int j = 0; j < 2; j++ )
			{
				for ( int k = 0; k < 8; k++ )
				{
					int value = k + a;
					if ( j == 1 )
						value += 16;

					ms_paletteMap[n++] = value;

					if ( ++m == 32 )
					{
						m = 0;
						a += 16;
					}
				}
			}
			a += 8;
		}

		Init = true;
	}
}

void CCharacterDamageManagerEx::ApplyAplha(RwTexture *texture, unsigned char *alphaPalette)
{
#ifdef DEVBUILD
	if ( Settings::GetInstance()->bDebugDamageEffects )
	{
		SetPlayerBlood(Settings::GetInstance()->fDamageEffect1);
		SetPlayerBruise(Settings::GetInstance()->fDamageEffect2);
		SetPlayerWet(Settings::GetInstance()->fDamageEffect3);
	}

	if ( Settings::GetInstance()->bDebugDamageDemo )
	{
		int TIME = Settings::GetInstance()->nDebugDamageDemoTime;
		int t = CGameTime::ms_currGameTime % (TIME*2+1);
		float time = (float)CGameTime::ms_currGameTime / float(TIME);
		float v = time - (float)(int)time;
		if ( v >= 0.5f )
			v = 1.0f - v;

		if ( t < TIME )
		{
			float val = v*2;
			SetPlayerWet(0.0f);
			SetPlayerBlood(val);
			SetPlayerBruise(val);
		}
		else
		{
			float val = v*2;
			SetPlayerWet(val);
			SetPlayerBlood(0.0f);
			SetPlayerBruise(0.0f);
		}
	}
#endif

	RwRaster *raster = RwTextureGetRaster(texture);

#ifndef MH_PS2
	if ( !ms_bHWPalette )
		SetSWPalette(true);
#endif

	RwRGBA *palette  = (RwRGBA *)RwRasterLockPalette(raster, rwRASTERLOCKWRITE);

#ifndef MH_PS2
	if ( !ms_bHWPalette )
		raster->privateFlags &= ~rwRASTERPALETTELOCKEDWRITE;
#endif

	for ( int i = 0; i < 256; i++ )
	{
#ifdef MH_PS2
		palette[ms_paletteMap[i]].alpha = alphaPalette[i];
#else
		palette[i].alpha = alphaPalette[i];
#endif
	}
	RwRasterUnlockPalette(raster);

#ifndef MH_PS2
	if ( !ms_bHWPalette )
	{
		ConvertPalettedTexture(texture, GetSWTexture(texture));
		SetSWPalette(false);
	}
#endif
}

void CCharacterDamageManagerEx::SetPedTexture(CPed *ped, RwTexture *pedtexture)
{
	if ( !ms_iShowPlayerDamageMap )
		return;

#ifdef MH_PS2
	RwTexture  *texture = pedtexture;
#else
	RwTexture  *texture = ms_bHWPalette ? pedtexture : GetSWTexture(pedtexture);
#endif

	RpMaterial *material = RpGeometryGetMaterial(RpAtomicGetGeometry(GetFirstAtomic(ped->m_pClump)), 0);
	if ( material )
	{
		RpMatFXMaterialSetDualBlendModes(material, /*srcBlendMode*/rwBLENDSRCALPHA, /*dstBlendMode*/rwBLENDINVSRCALPHA);
		RpMatFXMaterialSetDualTexture(material, texture);
	}

	RpMaterial *material2 = RpGeometryGetMaterial(RpAtomicGetGeometry(GetFirstAtomic(ped->m_pClump)), 1);
	if ( material2 )
	{
		RpMatFXMaterialSetDualBlendModes(material2, /*srcBlendMode*/rwBLENDSRCALPHA, /*dstBlendMode*/rwBLENDINVSRCALPHA);
		RpMatFXMaterialSetDualTexture(material2, texture);
	}
}

void CCharacterDamageManagerEx::UpdatePedTexture(CPed *ped)
{
	bool bDualTex = false;

	RpMaterial *material = RpGeometryGetMaterial(RpAtomicGetGeometry(GetFirstAtomic(ped->m_pClump)), 0);

#ifdef MH_PS2
	RwTexture  *texture = ped->m_pDamageTex;
#else
	RwTexture  *texture = ms_bHWPalette ? ped->m_pDamageTex : GetSWTexture(ped->m_pDamageTex);
#endif

	if ( RpMatFXAtomicQueryEffects(GetFirstAtomic(ped->m_pClump)) && RpMatFXMaterialGetDualTexture(material) != NULL )
		bDualTex = true;

	if ( material && bDualTex )
	{
		RpMatFXMaterialSetDualBlendModes(material, /*srcBlendMode*/rwBLENDSRCALPHA, /*dstBlendMode*/rwBLENDINVSRCALPHA);
		RpMatFXMaterialSetDualTexture(material, texture);
	}
}

void CCharacterDamageManagerEx::SetPlayerWet(float value)
{
	value = Clamp(value, 0.0f, 1.0f);

	unsigned char *alphaPalette = CScene::ms_pPlayer->m_aAlphaPalette;
	float alpha = value * 81.6f;

	for ( int i = 224; i < 232; i++ )
		alphaPalette[i] = (unsigned int)alpha;

	ms_iUpdatePlayerDamgeMapAlpha = true;
}

void CCharacterDamageManagerEx::SetPlayerBlood(float value)
{
	value = Clamp(value, 0.0f, 1.0f);

	unsigned char *alphaPalette = CScene::ms_pPlayer->m_aAlphaPalette;
	float alpha = value * 127.5f;

	for ( int i = 16; i < 168; i++ )
		alphaPalette[i] = (unsigned int)alpha;

	ms_iUpdatePlayerDamgeMapAlpha = true;
}

void CCharacterDamageManagerEx::SetPlayerBruise(float value)
{
	value = Clamp(value, 0.0f, 1.0f);

	unsigned char *alphaPalette = CScene::ms_pPlayer->m_aAlphaPalette;
	float alpha = value * 25.5f;

	for ( int i = 176; i < 216; i++ )
		alphaPalette[i] = (unsigned int)alpha;

	ms_iUpdatePlayerDamgeMapAlpha = true;
}

void patch__CPed_RenderLit(CPed *ped, void *unk)
{
	CCharacterDamageManagerEx::ApplyAplha(ped->m_pDamageTex, ped->m_aAlphaPalette);
	CCharacterDamageManagerEx::UpdatePedTexture(ped);
}

STARTPATCH
	{
		DBG("Damage\n");

		if ( Settings::GetInstance()->bPS2Damage )
		{
			CPatch::RedirectJump(0x5A95B0, CCharacterDamageManagerEx::InitTextures);

			CPatch::RedirectCall(0x5A9510, CCharacterDamageManagerEx::InitPaletteMap);
			CPatch::RedirectJump(0x5A9510+5, (void *)0x5A9523);

			CPatch::RedirectJump(0x5A96F0, CCharacterDamageManagerEx::SetPedTexture);

			CPatch::RedirectJump(0x5A96A0, CCharacterDamageManagerEx::ApplyAplha);

			CPatch::RedirectJump(0x5A97A0, CCharacterDamageManagerEx::SetPlayerWet);
			CPatch::RedirectJump(0x5A97B0, CCharacterDamageManagerEx::SetPlayerBlood);
			CPatch::RedirectJump(0x5A9820, CCharacterDamageManagerEx::SetPlayerBruise);

			CPatch::Nop(0x4A8C60, 6);
			CPatch::RedirectCall(0x4A8C6D, patch__CPed_RenderLit);

			// fild    [esp+1BBCh+src] -> fld     [esp+1BBCh+var_20]
			unsigned char patchb[] = {0xD9, 0x84, 0x24, 0x9C, 0x1B, 0x00, 0x00};
			CPatch::Set(0x5F10D0, patchb, sizeof(patchb));
#ifndef MH_PS2
			CPatch::RedirectJump(0x5E5150, CCharacterDamageManagerEx::Shutdown);

			CPatch::SetPointer(0x49C697 + 2, &CCharacterDamageManagerEx::ms_damageTextures[0]);
#endif
		}
	}
ENDPATCH