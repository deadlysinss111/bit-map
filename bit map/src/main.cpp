#include "pch.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	AllocConsole();
	FILE* consoleOut;
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);
	freopen_s(&consoleOut, "CONOUT$", "w", stderr);


	WindowHandler handler(hInstance);
	

	BitmapAtHome bmp;

	bmp.LoadFile("titre.bmp");
	bmp.Upscale();

	//int rgb[3] = { 0, 0, 0 };
	//bmp.ChangePixelAt(12, 2, rgb);
	//bmp.WriteInFile("textFile.bmp");

	WindowCustomParam param;
	param.bmp = &bmp;
	HBITMAP hBmp = handler.CreateBmpHandler(&bmp);
	param.hBmp = &hBmp;
	handler.SetCustomParam(&param);
	handler.CallRedraw();

	handler.RunWindow();
}