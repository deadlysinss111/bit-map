#pragma once
#define ENCODE_TAB_INDEX 0
#define DECODE_TAB_INDEX 1

struct WindowCustomParam {
	WindowHandler* windowHandler;
	BitmapAtHome* bitmapAtHome;
	HBITMAP* hBmp;
};

class WindowHandler
{
public:
	WindowHandler(HINSTANCE hInstance);
	~WindowHandler();

	void RunWindow();

	HBITMAP CreateBmpHandler(BitmapAtHome* bmp);
	void SetCustomParam(WindowCustomParam* param);
	void CallRedraw();

	// Public since we need main.cpp to fill some of its fields
	static WindowCustomParam _cParam;

private:
	HINSTANCE _hInstance;
	HWND _hWnd;
	HDC _hDc;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void RegisterWindowClass();
	HWND CreateMainWindow();

	/*
	   UI Elements : Tabs
	*/
	HWND _hTabControl;							// Handle to the Tab Control
	wchar_t _encodeTabName[50] = L"Encode";		// duh
	wchar_t _decodeTabName[50] = L"Decode";		// duh
	HWND _hStatic;								// Handle to the Static controle (where the displaying takes place)

	HRESULT ResizeTabControl(LPARAM ARGlParam);

	/*
	   UI Elements : File Selectors, Bitmap Outlines, Buttons, Texts
	*/

	/*
	   UI Elements : Padding constants
	*/
	short _globalPad = 5;	// Padding away from the top-left (in both directions)
};