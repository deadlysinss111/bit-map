#pragma once

struct CustomHeader {

};


class InfectedBitmap : public BitmapAtHome
{
public:
	~InfectedBitmap();

	static InfectedBitmap* InfectBmp(BitmapAtHome* src, BYTE* data, int dataLengh);

	void WriteDataInBmp(BYTE* data, int dataLengh);
	BYTE* ReadDataInBmp(int dataLengh);

private:
	InfectedBitmap();

	

	CustomHeader CustomInfoHeader;
};

