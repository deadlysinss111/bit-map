#include "pch.h"

InfectedBitmap::InfectedBitmap() {
	_customInfoHeader = nullptr;
}

InfectedBitmap::~InfectedBitmap() {

}

InfectedBitmap* InfectedBitmap::InfectBmp(BitmapAtHome* src, BYTE* data, int dataLengh) {
	InfectedBitmap* target = InfectedBitmap::InfectBmp(src);
	target->HideData(data, dataLengh);
	return target;
}

InfectedBitmap* InfectedBitmap::InfectBmp(BitmapAtHome* src) {
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

void InfectedBitmap::HideData(BYTE* data, int dataLengh) {
	HideDataHeader(dataLengh);
	//InternalHideData(data, _customInfoHeader->size, CUSTOMHEADERSIZE);
	InternalHideData(data, _customInfoHeader->size, 80);
}

void InfectedBitmap::HideDataHeader(int dataLengh) {
	if (_customInfoHeader != nullptr) {
		free(_customInfoHeader);
	}
	_customInfoHeader = new CustomHeader;
	_customInfoHeader->size = dataLengh;

	int* convertedData = (int*)malloc(64);
	*convertedData = dataLengh;
	InternalHideData((BYTE*)convertedData, 8);

	// just filling with useless value for now
	InternalHideData((BYTE*)convertedData, 16);

	//free(convertedData);
	// TODO hide extension as well
}

void InfectedBitmap::InternalHideData(BYTE* data, int dataLengh, int startingPoint) {
	for (int i = startingPoint; i < dataLengh + startingPoint; i++) {
		BYTE* targ = _colorTable + 4 * i;
		*targ >>= 2;
		*targ <<= 2;
		*targ |= ((data[i] & 0b11000000) >> 6);

		targ = _colorTable + 4 * i + 1;
		*targ >>= 2;
		*targ <<= 2;
		*targ |= ((data[i] & 0b00110000) >> 4);

		targ = _colorTable + 4 * i + 2;
		*targ >>= 2;
		*targ <<= 2;
		*targ |= ((data[i] & 0b00001100) >> 2);

		targ = _colorTable + 4 * i + 3;
		*targ >>= 2;
		*targ <<= 2;
		*targ |= (data[i] & 0b00000011);
	}
}

BYTE* InfectedBitmap::ReadHiddenData() {
	ReadCustomHeader();
	BYTE* data = InternalReadHiddenData(_customInfoHeader->size, 80);
	return data;
	//lets just treat the data as txt for now
}

void InfectedBitmap::ReadCustomHeader() {
	// TODO ask Sylvain if that works and which option is better
	/*CustomHeader* header;
	memcpy(header, InternalReadHiddenData(CUSTOMHEADERSIZE), CUSTOMHEADERSIZE);*/

	//BYTE* addr = InternalReadHiddenData(CUSTOMHEADERSIZE);

	BYTE* data = InternalReadHiddenData(80);
	if (_customInfoHeader != nullptr) {
		free(_customInfoHeader);
	}
	_customInfoHeader = new CustomHeader;
	_customInfoHeader->size = (uint64_t)data;
	_customInfoHeader->extension = (uint16_t)(data + 64);
}

BYTE* InfectedBitmap::InternalReadHiddenData(int dataLengh, int startingPoint) {
	BYTE* data = (BYTE*)malloc(dataLengh);
	for (int i = startingPoint; i < dataLengh + startingPoint; i++) {
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