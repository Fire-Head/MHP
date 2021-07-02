#pragma once
#include "rw.h"

class ColorFilter
{
	RwRaster *pFrontBuffer;

	RwUInt32  PS;
	RwUInt32  debugPS;

	RwIm2DVertex    Vertex[4];
	RwImVertexIndex Index[6];

private:
	void CreateImmediateModeData(RwCamera *camera, RwRect *rect);
	void UpdateFrontBuffer(RwCamera *camera);
	void OverlayRender(RwCamera *camera, bool reset);

public:
	void Initialise();
	void Shutdown();

	void Open(RwCamera *camera);
	void Close();
	void Render(RwCamera *camera, bool reset);

	static ColorFilter *GetInstance();
};