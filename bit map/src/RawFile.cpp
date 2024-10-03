#include "pch.h"

RawFile::RawFile() {
	_buffer = nullptr;
	_size = 0;
}

RawFile::~RawFile(){
	if (_buffer != nullptr) {
		delete _buffer;
	}
}

void RawFile::LoadFile(const char* addr) {

	FILE* file;
	fopen_s(&file, addr, "rb");

	if (file == 0) {
		std::cerr << "error in reading file : " << addr << std::endl;
		return;
	}

	//get file size in bytes
	fseek(file, 0, SEEK_END);
	_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	//reading file
	_buffer = new BYTE[_size];
	fread(_buffer, 1, _size, file);
	fclose(file);
}

void RawFile::SaveAsFile(const char* name) {
	FILE* target;
	fopen_s(&target, name, "wb");
	fwrite(_buffer, 1, _size, target);
	fclose(target);
}