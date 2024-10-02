#include "pch.h"

BitmapFile::BitmapFile() {
	_fileHeader = new BITMAPFILEHEADER;
	_infoHeader = new BITMAPINFOHEADER;
	_pixelData = nullptr;
}

BitmapFile::~BitmapFile() {

}

void BitmapFile::LoadFile(const char* addr)  {

	RawFile::LoadFile(addr);

	//copying parts of it into headers
	memcpy(_fileHeader, _buffer, sizeof(BITMAPFILEHEADER));

	memcpy(_infoHeader, _buffer + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

	//getting adress of the begining of the color values
	_pixelData = _buffer + _fileHeader->bfOffBits;
}

