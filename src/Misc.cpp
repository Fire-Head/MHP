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

void NAK patch_centermouse()
{
	__asm
	{
		test    al, al
		jnz     loc_4C1890

		mov     eax, dword ptr ds: [7D2E28h] // gameIsRunning
		test    al, al
		jz      loc_4C1890

		mov     eax, dword ptr ds: [715BA0h] // ms_stepMode
		test    al, al
		jnz     loc_4C1890

		mov     ecx, dword ptr ds: [736DB8h]

		mov     eax, 4C185Ch
		jmp     eax

loc_4C1890:
		mov     eax, 4C1890h
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

#ifdef ADDWINDOWEDMODE
		{
			// allow windowed mode
			CPatch::Nop(0x4BEE85, 6);

			// don't center mouse when in-menu
			CPatch::RedirectJump(0x4C1852, patch_centermouse);
		}
#endif

		// use resolution from bink file
		{
			unsigned char push_width_ebx[]  =  { 0xFF, 0x73, 0x00 };   // push    dword ptr [ebx+0]
			unsigned char push_height_ebx[] =  { 0xFF, 0x73, 0x04 };   // push    dword ptr [ebx+4]

			unsigned char push_width_ebp[]  =  { 0xFF, 0x75, 0x00 };   // push    dword ptr [ebp+0]
			unsigned char push_height_ebp[] =  { 0xFF, 0x75, 0x04 };   // push    dword ptr [ebp+4]

			CPatch::Nop(0x4C0D2F, 5);
			CPatch::Set(0x4C0D2F, push_width_ebx, sizeof(push_width_ebx));

			CPatch::Nop(0x4C0D2A, 5);
			CPatch::Set(0x4C0D2A, push_height_ebx, sizeof(push_height_ebx));

			CPatch::Nop(0x4C0ECA, 5);
			CPatch::Set(0x4C0ECA, push_width_ebp, sizeof(push_width_ebp));

			CPatch::Nop(0x4C0EC5, 5);
			CPatch::Set(0x4C0EC5, push_height_ebp, sizeof(push_height_ebp));

			CPatch::Nop(0x4C0ED7, 5);
			CPatch::Set(0x4C0ED7, push_height_ebp, sizeof(push_height_ebp));
		}
	}
ENDPATCH