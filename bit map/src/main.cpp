#include "pch.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	AllocConsole();
	FILE* consoleOut;
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);
	freopen_s(&consoleOut, "CONOUT$", "w", stderr);


	WindowHandler handler(hInstance);
	

	BitmapAtHome bmp;

	bmp.LoadFile("red.bmp");
	//bmp.Upscale(2);

	BYTE* ch = new BYTE;
	InfectedBitmap* test = InfectedBitmap::InfectBmp(&bmp, ch, 2);


	const char* str = "test";
	BYTE* bStr = (BYTE*)str;

	test->WriteDataInBmp(bStr, 4);
	//test->WriteInFile("ratilo.bmp");

	//int rgb[3] = { 0, 0, 0 };
	//bmp.ChangePixelAt(12, 2, rgb);
	//bmp.WriteInFile("textFile.bmp");

	WindowCustomParam param;
	param.bmp = test;
	HBITMAP hBmp = handler.CreateBmpHandler(test);
	param.hBmp = &hBmp;
	handler.SetCustomParam(&param);
	handler.CallRedraw();


	BYTE* result = test->ReadDataInBmp(4);
	std::cout << (char*)result << std::endl;

	handler.RunWindow();
}