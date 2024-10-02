#include "pch.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	AllocConsole();
	FILE* consoleOut;
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);
	freopen_s(&consoleOut, "CONOUT$", "w", stderr);


	WindowHandler handler(hInstance);
	

	BitmapFile bmp;

	bmp.LoadFile("long.bmp");
	//bmp.Upscale(2);

	BitmapToolbox toolbox;

	const char* str = "test";

	RawFile test;
	test.LoadFile("ff.png");

	

	BYTE* bStr = (BYTE*)str;
	toolbox.HideData(&bmp, test._buffer, test._size, "png");




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

	BYTE* extension;
	BYTE* result = toolbox.ReadHiddenData(&bmp, &extension);

	std::string name;

	char* appended = new char[100];
	strcpy(appended, "jvjvj");
	strcat(appended, (char*)extension);
	
	FILE* target;
	fopen_s(&target, appended, "wb");
	fwrite(result, 1, test._size, target);
	fclose(target);

	delete result;

	//std::cout << "result is : "<< (char*)result << "." << (char*)extension << std::endl;

	handler.RunWindow();
}