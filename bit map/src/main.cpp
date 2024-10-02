#include "pch.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	AllocConsole();
	FILE* consoleOut;
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);
	freopen_s(&consoleOut, "CONOUT$", "w", stderr);


	WindowHandler handler(hInstance);
	

	BitmapFile bmp;

	bmp.LoadFile("red.bmp");
	//bmp.Upscale(2);

	BitmapToolbox toolbox;

	const char* str = "test";
	BYTE* bStr = (BYTE*)str;
	toolbox.HideData(&bmp, bStr, 5);





	//test->SaveAsFile("ratilo.bmp");
	//int rgb[3] = { 0, 0, 0 };
	//bmp.ChangePixelAt(12, 2, rgb);
	//bmp.SaveAsFile("textFile.bmp");

	WindowCustomParam param;
	param.bmp = &bmp;
	HBITMAP hBmp = handler.CreateBmpHandler(&bmp);
	param.hBmp = &hBmp;
	handler.SetCustomParam(&param);
	handler.CallRedraw();


	BYTE* result = toolbox.ReadHiddenData(&bmp);
	std::cout << "result is : "<< (char*)result << std::endl;

	handler.RunWindow();
}