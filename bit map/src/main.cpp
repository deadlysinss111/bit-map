#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>
#include <wingdi.h>

using namespace std;


void SwapRnB(BYTE* stream, int offset, int size);

int main(int argc, int argv) {
	FILE* file;

	fopen_s(&file, "red.bmp", "rb");
	
	if (file == 0) {
		std::cout << "osijbosghnsgoj";
		return 0;
	}

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	BYTE* buffer = new BYTE[size];
	fread(buffer, 1, size, file);
	fclose(file);

	BITMAPFILEHEADER fileHeader;
	memcpy(&fileHeader, buffer, sizeof(BITMAPFILEHEADER));

	BITMAPINFOHEADER infoHeader;
	memcpy(&infoHeader, buffer+ sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

	BYTE* bgr = buffer + fileHeader.bfOffBits;

	cout << "there is the blue: " << (int)bgr[0 * infoHeader.biSize] << endl;
	cout << "there is the green: " << (int)bgr[1 * infoHeader.biSize] << endl;
	cout << "there is the red: " << (int)bgr[2 * infoHeader.biSize] << endl;

	for (int i = 2; i < 100; i += 3) {
		buffer[fileHeader.bfOffBits + i] = 0;
	}

	SwapRnB(buffer, fileHeader.bfOffBits, size);
	
	
	FILE* empty;
	fopen_s(&empty, "textFile.bmp", "wb");
	fwrite(buffer, 1, size, empty);
	fclose(empty);
	



	//HBITMAP hbmp = CreateBitmap(infoHeader.biWidth, infoHeader.biHeight, infoHeader.biPlanes, infoHeader.biBitCount, NULL);
	//HGDIOBJ bmpObj = SelectObject(, hbmp)
}


void SwapRnB(BYTE* stream, int offset, int size) {
	BYTE* cursor;
	cursor = stream + offset;
	while (size > offset) {
		BYTE buf = *(cursor + size);
		*(cursor + size) = *(cursor + size - 2);
		*(cursor + size - 2) = buf;
		size -= 3;
	}
}