#pragma once

#define CUSTOMHEADERSIZE 10
#define CHEADER_SIZE 8
#define CHEADER_EXTENSION 2

struct CustomHeader {
	uint64_t size;
	uint16_t extension;
};


class InfectedBitmap : public BitmapAtHome
{
public:
	~InfectedBitmap();

	static InfectedBitmap* InfectBmp(BitmapAtHome* src, BYTE* data, int dataLengh);
	static InfectedBitmap* InfectBmp(BitmapAtHome* src);

	void HideData(BYTE* data, int dataLengh);
	BYTE* ReadHiddenData();

private:
	InfectedBitmap();

	void HideDataHeader(int dataLengh);
	void InternalHideData(BYTE* data, int dataLengh, int startingPoint = 0);

	void ReadCustomHeader();
	BYTE* InternalReadHiddenData(int dataLengh, int startingPoint = 0);
	

	CustomHeader* _customInfoHeader;
};

