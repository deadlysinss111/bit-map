#include "pch.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	AllocConsole();
	FILE* consoleOut;
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);
	freopen_s(&consoleOut, "CONOUT$", "w", stderr);


	CImageToolbox tool;
	BitmapToolbox toolbox;

	WindowHandler handler(hInstance);
	RawFile bmp;
	bmp.LoadFile("smile.bmp");
	//toolbox.Upscale(&bmp);

	CImage* img = tool.BitmapToCImage(&bmp);

	//tool.Upscale(img, 3);

	RawFile* rf = tool.CImageToBitmap(img);

	//toolbox.Upscale(rf);

	BitmapFile bmpFile(&bmp);



	// Partial setup of Window Handler's custom parameter
	HBITMAP hBmp = handler.CreateBmpHandler(&bmpFile);	// TODO: Creation of the HBITMAP should be in the WindowHandler class
	handler._cParam.hBmp = &hBmp;

	handler.CallRedraw();

	handler.RunWindow();
}