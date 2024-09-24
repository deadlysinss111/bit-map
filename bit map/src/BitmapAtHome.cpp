#include "pch.h"

BitmapAtHome::BitmapAtHome() {
	_fileHeader = new BITMAPFILEHEADER;
	_infoHeader = new BITMAPINFOHEADER;
	_colorTable = nullptr;
	_originAddr = nullptr;
	_size = 0;
}

BitmapAtHome::~BitmapAtHome() {
	delete(_fileHeader);
	delete(_infoHeader);
	if (_originAddr != nullptr) {
		// memory leak for now
	}
}

void BitmapAtHome::LoadFile(const char* addr) {

	FILE* file;
	fopen_s(&file, addr, "rb");

	if (file == 0) {
		std::cerr << "error in file reading" << std::endl;
		return;
	}

	//get file size in bytes
	fseek(file, 0, SEEK_END);
	_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	//reading file
	_originAddr = new BYTE[_size];
	fread(_originAddr, 1, _size, file);
	fclose(file);

	//copying parts of it into headers
	//ask sylvain how to get ride of that memory
	memcpy(_fileHeader, _originAddr, sizeof(BITMAPFILEHEADER));

	memcpy(_infoHeader, _originAddr + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

	//getting adress of the begining of the color values
	_colorTable = _originAddr + _fileHeader->bfOffBits;

	//we swap to rgb so that it's easier to work on the file
	SwapRnB(_size - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)));
}

void BitmapAtHome::WriteInFile(const char* name) {
	//dont forget to turn back to bgr
	SwapRnB(_size - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)));

	FILE* target;
	fopen_s(&target, name, "wb");
	fwrite(_originAddr, 1, _size, target);
	fclose(target);
}

void BitmapAtHome::SwapRnB(int colorTableSize) {
	for (int i = 0; i < colorTableSize; i += 3) {
		BYTE buf = *(_colorTable + i);
		*(_colorTable + i) = *(_colorTable + i + 2);
		*(_colorTable + i + 2) = buf;
	}
}

bool BitmapAtHome::ChangePixelAt(int x, int y, int rgb[3]) {
	int width = _infoHeader->biWidth;
	int height = _infoHeader->biHeight;

	if (x < 0 || x >= width || y < 0 || y >= height) return false;

	_colorTable[3 * (x + width * (height - y))] = rgb[0];
	_colorTable[3 * (x + width * (height - y)) + 1] = rgb[1];
	_colorTable[3 * (x + width * (height - y)) + 2] = rgb[2];
	return true;
}