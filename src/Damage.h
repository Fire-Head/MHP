#pragma once

class CPed;

class CCharacterDamageManagerEx : public CCharacterDamageManager
{
public:
	// Hacky way to simulate palettes since almost all GPUs can't do them
#ifndef MH_PS2
	enum
	{
		NUM_PLAYER_TEXTURES = 1,
		NUM_DAMAGE_TEXTURES = NUM_PLAYER_TEXTURES+NUM_TEXTURES
	};

	static RwTexDictionary *ms_txd;

	static const char *ms_damageFilenames[NUM_DAMAGE_TEXTURES];
	static RwTexture *ms_damageTextures[NUM_DAMAGE_TEXTURES];

	static _rwD3D8Palette *ms_damageTexturePalettes[NUM_DAMAGE_TEXTURES];
	static RwTexture *ms_damageSWTextures[NUM_DAMAGE_TEXTURES];

	static bool ms_bHWPalette;

	static RwTexture *CreateBlankTexture(const RwTexture *texture);
	static void ConvertPalettedTexture(const RwTexture *palettedTexture, RwTexture *texture);
	static void LoadTextures();
	static void Shutdown();
	static RwTexture *GetSWTexture(const RwTexture *texture);
	static void SetSWPalette(bool enable);
#endif

	// Original PS2 code
	static int ms_paletteMap[256];

	static void InitTextures();
	static void InitPaletteMap();
	static void ApplyAplha(RwTexture *texture, unsigned char *alphaPalette);
	static void SetPedTexture(CPed *ped, RwTexture *pedtexture);
	static void UpdatePedTexture(CPed *ped);
	static void SetPlayerWet(float value);
	static void SetPlayerBlood(float value);
	static void SetPlayerBruise(float value);
};