#include "pch.h"



int main(int argc, int argv) {
	
	BitmapAtHome bmp;

	bmp.LoadFile("red.bmp");
	int rgb[3] = { 0, 0, 0 };
	bmp.ChangePixelAt(12, 2, rgb);
	bmp.WriteInFile("textFile.bmp");


	//HBITMAP hbmp = CreateBitmap(infoHeader.biWidth, infoHeader.biHeight, infoHeader.biPlanes, infoHeader.biBitCount, NULL);
	//HGDIOBJ bmpObj = SelectObject(, hbmp)
}


