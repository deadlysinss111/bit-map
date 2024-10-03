#include "pch.h"

BitmapFile::BitmapFile() {
	_fileHeader = new BITMAPFILEHEADER;
	_infoHeader = new BITMAPINFOHEADER;
	_pixelData = nullptr;
}

BitmapFile::BitmapFile(RawFile* file) {
	_buffer = new BYTE[file->_size];
	memcpy(_buffer, file->_buffer, file->_size);
	_fileHeader = (BITMAPFILEHEADER*)_buffer;
	_infoHeader = (BITMAPINFOHEADER*)(_buffer + sizeof(BITMAPFILEHEADER));
	memcpy(_infoHeader, file->_buffer + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	_pixelData = _buffer + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	_size = file->_size;
}

BitmapFile::~BitmapFile() {
	/*delete _fileHeader;
	delete _infoHeader;*/
}

void BitmapFile::LoadFile(const char* addr)  {

	RawFile::LoadFile(addr);

	//copying parts of it into headers
	memcpy(_fileHeader, _buffer, sizeof(BITMAPFILEHEADER));

	memcpy(_infoHeader, _buffer + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

	//getting adress of the begining of the color values
	_pixelData = _buffer + _fileHeader->bfOffBits;
}

