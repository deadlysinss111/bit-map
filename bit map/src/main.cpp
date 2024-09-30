#include "pch.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Prepares a console because prints are pog
	AllocConsole();
	FILE* consoleOut;
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);
	freopen_s(&consoleOut, "CONOUT$", "w", stderr);

	// Instantiate classes
	WindowHandler handler(hInstance);
	BitmapAtHome bmp;						// TODO: This class needs to be PURGED

	// Testing ground to call some functions
	bmp.LoadFile("titre.bmp");
	bmp.Upscale();

	//int rgb[3] = { 0, 0, 0 };
	//bmp.ChangePixelAt(12, 2, rgb);
	//bmp.WriteInFile("textFile.bmp");


	// Partial setup of Window Handler's custom parameter
	HBITMAP hBmp = handler.CreateBmpHandler(&bmp);	// TODO: Creation of the HBITMPA should be in the WindowHandler class
	handler._cParam.bitmapAtHome = &bmp;
	handler._cParam.hBmp = &hBmp;

	handler.CallRedraw();

	handler.RunWindow();
}