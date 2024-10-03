#pragma once


class BitmapToolbox{
public:
	BitmapToolbox();
	~BitmapToolbox();

	void SwapRnB(BitmapFile* target,  int colorTableSize);
	bool ChangePixelAt(BitmapFile* target, int x, int y, int rgb[3]);
	void Upscale(BitmapFile* target, int iterations = 1);

	bool HideData(BitmapFile* target, BYTE* data, int dataLengh, const char* extension = "txt");
	bool HideData(RawFile* target, BYTE* data, int dataLengh, const char* extension = "txt");
	BYTE* ReadHiddenData(BitmapFile* target, CustomHeader* headerAddr = nullptr);
	BYTE* ReadHiddenData(RawFile* target, CustomHeader* headerAddr);

	
private:
	void InternalUpscale(BitmapFile* target);

	void HideDataHeader(BitmapFile* target, int dataLengh, const char* extension);
	void HideDataHeader(RawFile* target, int dataLengh, const char* extension);
	void InternalHideData(BitmapFile* target, BYTE* data, int dataLengh, int startingPoint = 0);
	void InternalHideData(RawFile* target, BYTE* data, int dataLengh, int startingPoint = 0);

	CustomHeader ReadCustomHeader(BitmapFile* target);
	CustomHeader ReadCustomHeader(RawFile* target);
	BYTE* InternalReadHiddenData(BitmapFile* target, int dataLengh, int startingPoint = 0);
	BYTE* InternalReadHiddenData(RawFile* target, int dataLengh, int startingPoint = 0);
};