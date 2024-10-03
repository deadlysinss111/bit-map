#include "pch.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	AllocConsole();
	FILE* consoleOut;
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);
	freopen_s(&consoleOut, "CONOUT$", "w", stderr);


	WindowHandler handler(hInstance);
	BitmapFile bmp;

	// B�tise time
	bmp.LoadFile("long.bmp");
	//bmp.Upscale(2);

	std::cout << bmp._infoHeader->biXPelsPerMeter << " || " << bmp._infoHeader->biYPelsPerMeter << std::endl;

	BitmapToolbox toolbox;

	RawFile test;
	test.LoadFile("ff.png");

	toolbox.HideData(&bmp, test._buffer, test._size, "png");
	


	handler.CallRedraw();

	CustomHeader header;
	BYTE* result = toolbox.ReadHiddenData(&bmp, &header);


	char* appended = new char[100];
	strcpy(appended, "jvjvj.");
	strcat(appended, (char*)header.extension);
	
	FILE* target;
	fopen_s(&target, appended, "wb");
	fwrite(result, 1, header.size, target);
	fclose(target);

	//delete result;

	//std::cout << "result is : "<< (char*)result << "." << (char*)extension << std::endl;

	// Partial setup of Window Handler's custom parameter
	HBITMAP hBmp = handler.CreateBmpHandler(&bmp);	// TODO: Creation of the HBITMAP should be in the WindowHandler class
	handler._cParam.hBmp = &hBmp;

	handler.RunWindow();
}