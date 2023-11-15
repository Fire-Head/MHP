#include "common.h"
#include "game.h"
#include "render\helpers.h"
#include "Misc.h"
#include "settings.h"
#include "CPatch.h"


void CUseableFix::RenderLit()
{
	if ( !Settings::GetInstance()->bUseableLightFix )
	{
		CUseable::RenderLit();
		return;
	}

	if ( m_bUpdateLight )
	{
		if ( (CScene::ms_pPlayer->GetPosition() - GetPosition()).MagnitudeSqr() < 16.0f )
		{
			float alpha;

			float time = (float)CGameTime::ms_currGameTime / 1000.0f;
			float t = time - (float)(int)time;
			if ( t > 0.5f )
				t = 1.0f - t;

			float color = powf(2.0f, 4.0f) * powf(t, 4.0f) * 0.75f + 0.25f;

			RwRGBAReal light;
			light.red = light.green = light.blue = color;
			light.alpha = alpha;

			RwRGBARealAssign(&m_LightColor, &light);
		}
	}

	m_bUpdateLight = false;

	CEntity::RenderLit();
}

void CPedFix::RenderLit()
{
	if ( !Settings::GetInstance()->bSkinFallbackFix ) return;
	
	if ( m_pSkins[0] )
	{
		RpAtomic *atomic = NULL;
		RpClumpForAllAtomics(m_pClump, GetFirstAtomicCallback, &atomic);
		RpMaterialSetTexture(RpGeometryGetMaterial(RpAtomicGetGeometry(atomic), 0), m_pSkins[0]);
	}
}

void CPedHeadFix::RenderLit()
{
	if ( !Settings::GetInstance()->bSkinFallbackFix ) return;
	
	if ( m_pSkins[0] )
	{
		RpAtomic *atomic = NULL;
		RpClumpForAllAtomics(m_pClump, GetFirstAtomicCallback, &atomic);
		RpMaterialSetTexture(RpGeometryGetMaterial(RpAtomicGetGeometry(atomic), 0), m_pSkins[0]);
	}
}

void NAK patch_CPed_RenderLit()
{
	__asm
	{
		cmp     dword ptr [esi+15Ch], 1
		jbe     loc_4A8B66
		
		mov     eax, 4A8B25h
		jmp     eax
		
loc_4A8B66:
		mov     ecx, ebx
		call    CPedFix::RenderLit
		
		mov     eax, 4A8B66h
		jmp     eax
	}
}

void NAK patch_CPedHead_RenderLit()
{
	__asm
	{
		cmp     dword ptr [ecx+144h], 1
		jbe     loc_4F151C
		mov     edx, [ebx+13Ch]
		mov     ecx, [edx+7Ch]
		cmp     dword ptr [ecx+15Ch], 1
		jbe     loc_4F151C
		cmp     dword ptr [ebx+1A4h], 0
		jnz     loc_4F151C
		
		mov     eax, 4F14DDh
		jmp     eax

loc_4F151C:
		mov     ecx, ebx
		call    CPedHeadFix::RenderLit
		
		mov     eax, 4F151Ch
		jmp     eax
	}
}

void patch_DrawQuad3d()
{
	if ( !Settings::GetInstance()->bFixHalos ) return;
	// on ps2 it's always null here but not on pc, so let's simulate it
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
}

RwBool patch_HaloSrcBlend(RwRenderState state, void *value)
{
	return RwRenderStateSet(state, Settings::GetInstance()->bOldHalosStyle ? (void *)rwBLENDONE : (void *)rwBLENDSRCALPHA);
}

void NAK patch_crosshair()
{
	__asm
	{
		mov     ecx, ebx
		mov     eax, 45BA60h // CPlayer::IAmSniping
		call    eax
		test    eax, eax
		jnz     loc_45F2E4

		mov     dword ptr [ebp+910h], 0
		mov     eax, 45F2C4h
		jmp     eax

loc_45F2E4:
		mov     eax, 45F2E4h
		jmp     eax
	}
}

STARTPATCH
	{
		DBG("Misc\n");

		// pickup wire fix
		CPatch::RedirectJump(0x5FB217, patch_DrawQuad3d);

		// old Halo Style
		CPatch::Nop(0x5FB56B, 0x5FB574-0x5FB56B);
		CPatch::RedirectCall(0x5FB56B, patch_HaloSrcBlend);

		// useablelight fix
		CPatch::SetPointer(0x7372A4, FUNC2PTR(&CUseableFix::RenderLit));
		CPatch::SetPointer(0x738860, FUNC2PTR(&CUseableFix::RenderLit));
		CPatch::SetPointer(0x738EEC, FUNC2PTR(&CUseableFix::RenderLit));
		CPatch::SetPointer(0x739254, FUNC2PTR(&CUseableFix::RenderLit));
		
		// skin fallback fix
		CPatch::RedirectJump(0x4A8B1C, patch_CPed_RenderLit);
		CPatch::RedirectJump(0x4F14B9, patch_CPedHead_RenderLit);

		if ( Settings::GetInstance()->bPS2Brightness )
		{
			// remove DirectX SetGammaRamp
			CPatch::SetChar(0x5DB0D0, 0xC3);

			// restore ps2 behavior
			CPatch::SetChar(0x5D550A + 6, 11);
			CPatch::SetFloat(0x5D5514 + 6, 0.02f);
			CPatch::Nop(0x5DAA6A, 6);
			CPatch::SetChar(0x604D08 + 3, 1);
		}

		// sniper rifle scope fix
		if ( Settings::GetInstance()->bCrosshairFix )
			CPatch::RedirectJump(0x45F2BA, patch_crosshair);
	}
ENDPATCH