#pragma once

class RawFile
{
public:
	RawFile();
	~RawFile();

	void LoadFile(const char* addr);
	void SaveAsFile(const char* name);

	BYTE* _buffer;
	int _size;
};

