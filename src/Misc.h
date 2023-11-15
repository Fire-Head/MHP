#pragma once
#include "game.h"

class CUseableFix : public CUseable
{
public:
	void RenderLit();
};

class CPedFix : public CPed
{
public:
	void RenderLit();
};

class CPedHeadFix : public CPedHead
{
public:
	void RenderLit();
};