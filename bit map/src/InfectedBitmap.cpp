#include "pch.h"

InfectedBitmap::InfectedBitmap() {

}

InfectedBitmap::~InfectedBitmap() {

}

InfectedBitmap* InfectedBitmap::InfectBmp(BitmapAtHome* src, BYTE* data, int dataLengh) {
	InfectedBitmap* infected = new InfectedBitmap();
	infected->_originAddr = (BYTE*)malloc(src->_size);
	
	infected->_size = src->_size;
	memcpy(infected->_originAddr, src->_originAddr, src->_size);
	/*infected->_fileHeader = src->_fileHeader;
	infected->_infoHeader = src->_infoHeader;
	infected->_colorTable = src->_colorTable;*/
	infected->_fileHeader = (BITMAPFILEHEADER*)infected->_originAddr;
	infected->_infoHeader = (BITMAPINFOHEADER*)(infected->_originAddr + sizeof(BITMAPFILEHEADER));
	infected->_colorTable = (BYTE*)(infected->_originAddr + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
	
	//infected->CustomInfoHeader = data;

	return infected;
}

void InfectedBitmap::WriteDataInBmp(BYTE* data, int dataLengh) {
	for (int i = 0; i < dataLengh; i++) {
		BYTE* targ = _colorTable + 4 * i;
		*targ >>= 2;
		*targ <<= 2;
		*targ |= ((data[i] & 0b11000000)>>6);

		targ = _colorTable + 4 * i + 1;
		*targ >>= 2;
		*targ <<= 2;
		*targ |= ((data[i] & 0b00110000)>>4);

		targ = _colorTable + 4 * i + 2;
		*targ >>= 2;
		*targ <<= 2;
		*targ |= ((data[i] & 0b00001100)>>2);

		targ = _colorTable + 4 * i + 3;
		*targ >>= 2;
		*targ <<= 2;
		*targ |= (data[i] & 0b00000011);
	}
}

BYTE* InfectedBitmap::ReadDataInBmp(int dataLengh) {
	BYTE* data = (BYTE*)malloc(dataLengh);
	for (int i = 0; i < dataLengh; i++) {
		BYTE* targ = _colorTable + 4 * i;
		data[i] = (*targ & 0b00000011);
		data[i] <<= 2;

		targ = _colorTable + 4 * i + 1;
		data[i] |= (*targ & 0b00000011);
		data[i] <<= 2;

		targ = _colorTable + 4 * i + 2;
		data[i] |= (*targ & 0b00000011);
		data[i] <<= 2;

		targ = _colorTable + 4 * i + 3;
		data[i] |= (*targ & 0b00000011);

		// ptite boucle intended
	}
	return data;
}