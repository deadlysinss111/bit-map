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
		free(_originAddr);
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
	_originAddr = (BYTE*)malloc(_size);
	fread(_originAddr, 1, _size, file);
	fclose(file);

	//copying parts of it into headers
	//ask sylvain how to get ride of that memory
	memcpy(_fileHeader, _originAddr, sizeof(BITMAPFILEHEADER));

	memcpy(_infoHeader, _originAddr + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

	//getting adress of the begining of the color values
	_colorTable = _originAddr + _fileHeader->bfOffBits;

	//we swap to rgb so that it's easier to work on the file
	//SwapRnB(_size - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)));
}

void BitmapAtHome::WriteInFile(const char* name) {
	//dont forget to turn back to bgr
	//SwapRnB(_size - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)));

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

void BitmapAtHome::Upscale(int iterations = 1) {
	for (int i = 0; i < iterations; i++) {
		InternalUpscale();
	}
}

// Double the size of the bmp
void BitmapAtHome::InternalUpscale() {
	int newWidth = _infoHeader->biWidth * 2;
	int newHeight = _infoHeader->biHeight * 2;
	int bytesPerPixel = _infoHeader->biBitCount / 8;

	int newRowSize = ((newWidth * _infoHeader->biBitCount + 31) / 32) * 4;

	BYTE* addr = (BYTE*)malloc(newRowSize * newHeight + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

	memcpy(addr, _originAddr, sizeof(BITMAPFILEHEADER));
	memcpy(_fileHeader, addr, sizeof(BITMAPFILEHEADER));
	memcpy(addr + sizeof(BITMAPFILEHEADER), _originAddr + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	memcpy(_infoHeader, _originAddr + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	BYTE* colorAddr = addr + _fileHeader->bfOffBits;

	for (int j = 0; j < _infoHeader->biHeight; j++) {
		for (int i = 0; i < _infoHeader->biWidth; i++) {

			// Let's retrieve the pixel we want to copy
			int srcIndex = (i + j * _infoHeader->biWidth) * bytesPerPixel;
			BYTE* pixelData = _colorTable + srcIndex;

			// We are looking for the base destination in our copy bmp
			int destIndex = (i*2 + j*2* newWidth) * bytesPerPixel;

			// For each of the r, g and b value
			for (int b = 0; b < bytesPerPixel; b++) {
				// We copy it into a square of 4 pixels (the first one being the base destination we searched for just before) into our copy bmp
				colorAddr[destIndex + b] = pixelData[b];
				colorAddr[destIndex + b + bytesPerPixel] = pixelData[b];
				colorAddr[destIndex + b + newWidth * bytesPerPixel] = pixelData[b];
				colorAddr[destIndex + b + newWidth * bytesPerPixel + bytesPerPixel] = pixelData[b];
			}
		}
	}

	_infoHeader->biWidth = newWidth;
	_infoHeader->biHeight = newHeight;
	_infoHeader->biSizeImage = newRowSize * newHeight;
	_size = newRowSize * newHeight + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	free(_originAddr);
	_originAddr = addr;
	_colorTable = colorAddr;
	_fileHeader->bfSize = newRowSize * newHeight + sizeof(BITMAPINFOHEADER);
}