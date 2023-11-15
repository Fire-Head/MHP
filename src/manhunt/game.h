#pragma once
#include "rw.h"

class CVector
{
public:
	float x, y, z;
	CVector(void) {}
	CVector(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	float Magnitude(void) const { return (float)sqrt(x*x + y * y + z * z); }
	float MagnitudeSqr(void) const { return x * x + y * y + z * z; }
	float Magnitude2D(void) const { return (float)sqrt(x*x + y * y); }
	float MagnitudeSqr2D(void) const { return x * x + y * y; }
	void Normalise(void) {
		float sq = MagnitudeSqr();
		if (sq > 0.0f) {
			float invsqrt = 1.0f/sqrt(sq);
			x *= invsqrt;
			y *= invsqrt;
			z *= invsqrt;
		}
		else
			x = 1.0f;
	}

	void Normalise2D(void) {
		float sq = MagnitudeSqr2D();
		float invsqrt = 1.0f/sqrt(sq);
		x *= invsqrt;
		y *= invsqrt;
	}

	const CVector &operator+=(CVector const &right) {
		x += right.x;
		y += right.y;
		z += right.z;
		return *this;
	}

	const CVector &operator+=(float const &right) {
		x += right;
		y += right;
		z += right;
		return *this;
	}

	const CVector &operator-=(CVector const &right) {
		x -= right.x;
		y -= right.y;
		z -= right.z;
		return *this;
	}

	const CVector &operator*=(float right) {
		x *= right;
		y *= right;
		z *= right;
		return *this;
	}

	const CVector &operator/=(float right) {
		x /= right;
		y /= right;
		z /= right;
		return *this;
	}

	CVector operator-() const {
		return CVector(-x, -y, -z);
	}

	const bool operator==(CVector const &right) {
		return x == right.x && y == right.y && z == right.z;
	}

	const bool operator!=(CVector const &right) {
		return x != right.x || y != right.y || z != right.z;
	}
};

inline CVector operator+(const CVector &left, const CVector &right)
{
	return CVector(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline CVector operator-(const CVector &left, const CVector &right)
{
	return CVector(left.x - right.x, left.y - right.y, left.z - right.z);
}

inline CVector operator*(const CVector &left, float right)
{
	return CVector(left.x * right, left.y * right, left.z * right);
}

inline CVector operator*(float left, const CVector &right)
{
	return CVector(left * right.x, left * right.y, left * right.z);
}

inline CVector operator/(const CVector &left, float right)
{
	return CVector(left.x / right, left.y / right, left.z / right);
}

inline float
DotProduct(const CVector &v1, const CVector &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

struct Immediate
{
	int field_0;
	int bPerPixelCoords;
	float fAngle;
	float fScreenZ;
	float fRHW;
	int bDontScaleCoords;
	RwIm2DVertex *Im2DVertices;
	RwIm2DVertex *pVerts2d;
	RwIm3DVertex *Im3DVertices;
	RwIm3DVertex *pVerts3d;
	int Im2DTriIndices;
	int Im2DQuadIndices;
	int Im3dTriIndices;
	RwImVertexIndex *Im3dQuadIndices;
	int pRaster;
	int Im3dMatrix;
	int Im3dFlags;
	float _ax;
	float _ay;
	float _bx;
	float _by;
	int field_54;
	float x1;
	float y1;
	float x2;
	float y2;
	float x3;
	float y3;
	float x4;
	float y4;
	float field_78_x;
	float field_7C_y;
	char field_80[4];
	int field_84;
	int field_88;
	int field_8C;
	int field_90;
	float field_94;
	float field_98;
	int field_9C;
	int field_A0;
	int field_A4;
	char field_A8[8];
	float field_B0;
	int NumTris;
	int NumQuads;
	float f2dHorizScale;
	float f2dVertScale;
	int PrimType;
};

class CRenderer
{
public:
	static Immediate &ms_im;

	static void SetIngameInfoRenderStates(long mode);
	static void RenderStateSetBlend(int src, int dst);
	static void PopRenderState();
	static void PushAndSetRenderState(RwRenderState state, void *value);
	static void RenderStateAlphaSet();
	static void RenderStateAlphaCombine_ONE();
	static void SetAngle(float angle);
	static void DrawQuad2dBegin(RwRaster *raster);
	static void DrawQuad2dEnd();
	static void DrawQuad2d(float posx, float posy, float scalex, float scaley, long R, long G, long B, long A, RwRaster *raster);
	static bool DrawQuad2dSetAll(float posx, float posy, float scalex, float scaley, float U1, float V1, float U2, float V2, float U3, float V3, float U4, float V4, long R1, long G1, long B1, long A1, long R2, long G2, long B2, long A2, long R3, long G3, long B3, long A3, long R4, long G4, long B4, long A4);
	// PS2
	static void FlushTextures();
	static void BlackBoxDraw(int alpha);
};

class CGameTime
{
public:
	static unsigned int& ms_currGameTime;
};

class CEntity
{
public:
char unk0[0x80];
	RpClump *m_pClump;
char unk1[0x44];
	int m_iEntityFlags;
	RwRGBAReal m_LightColor;
char unk2[0x1C];
	int m_iLOD;
char unk3[0x14];
	char m_bAnimate;
char unk4[0x2B];

	CVector &GetPosition();
	void RenderLit();
};

class CUseable : public CEntity
{
public:
char unk5[0x1C];
	char m_bUpdateLight;

	void RenderLit();
};

class CCharacter : public CEntity
{
public:
char unk5[0x80];
};

class CPed : public CCharacter
{
public:
char _unk5[0x10];
	RwTexture *m_pDamageTex;
	unsigned char m_aAlphaPalette[256];
char _unk6[0x140];
	RwTexture *m_pSkins[4];
};

class CPlayer : public CPed
{
public:

};

class CPedHead : public CEntity
{
public:
char _unk5[0x8C];
	RwTexture *m_pSkins[4];
};

class CScene
{
public:
	static int &ms_stepMode;
	static CPlayer* &ms_pPlayer;
	static RwCamera* &ms_pCamera;
};

class CCharacterDamageManager
{
public:
	enum
	{
		NUM_TEXTURES = 16
	};

	static RwTexture *(&ms_textures)[NUM_TEXTURES];
	static int &ms_iShowPlayerDamageMap;
	static int &ms_iUpdatePlayerDamgeMapAlpha;
};

class CScreen
{
public:
	float fWidth;
	float fHeight;
	float fWidthRatio;
	float fHeightRatio;
	float fWidthScale;
	float fHeightScale;
	float HudStretch;
	int nDepth;
	int pCamera;
	RwRaster *pFrameBuffer;
	int pZBuffer;
	int field_2C;
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	int field_40;
};

class CFrontend
{
public:
	static int &ms_FXModeMenu;
	static int &ms_FXArtFilter;
	static int &ms_EnableZClearOnly;
	static int &ms_Nightvision;
	static int &ms_FXModeFade;
	static RwTexture *(&ms_damageTextures)[CCharacterDamageManager::NUM_TEXTURES];
	static CScreen &ms_scrn;
	static RwCamera* &ms_pCamera;

	static bool IsCameraFXActive();
};

class CFrontendMenu
{
public:	
	struct menuMouse
	{
		int field_0;
		int bSaveGameSel;
		int bDBoxSelectOnClickInput;
		int nControlsSliderID;
		int bOption;
		int bMouseUpdated;
		int bSliderMinusSel;
		int bSliderPlusSel;
		int bLeftOnClickInput;
		int bRightOnClickInput;
		int bPrintfInfo;
		int bPrintInfoButton1Input;
		int bPrintInfoButton2Input;
		int nLastOptionSel;
		int nLastSubOptionSel;
		int nOptionSel;
		int nSubOptionSel;
		int bOptionSel;
		int nLastIconSel;
		int nIconSel;
		float fPointerX;
		float fPointerY;
		float fPointerSizeX;
		float fPointerSizeY;
		float fX;
		float fY;
		int bLMB;
		int bMMB;
		int bRMB;
		int bLMBOnce;
	};
	
	struct dialogBox
	{
		int enabled;
		int draw;
		int select;
		int field_C;
		wchar_t *text;
		wchar_t *option1;
		wchar_t *option2;
		int optionID;
		int positiveID;
	};


	static menuMouse &Mouse;
	static dialogBox &ms_dBox;
	static int &m_menuCurrent;
};

class CGameInfo
{
public:
	static int &m_gameInfo;
};

class CCamGlobals
{
public:
char unk0[0x26C];
	char bExecutionStatus;
char unk1[0x14E];
	char bHandCamSetVideoEffectScrollBar;
	char bCutsceneCamera;
};

extern CCamGlobals &gCameraGlobals;

RpAtomic* GetFirstAtomicCallback(RpAtomic *atm, void *data);
RwTexDictionary *TexDictLoad(const char * path);