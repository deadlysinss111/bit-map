#pragma once

class WindowHandler
{
public:
	WindowHandler(HINSTANCE hInstance);
	~WindowHandler();

	void RunWindow();

	HBITMAP CreateBmpHandler(BitmapAtHome* bmp);
	void SetCustomParam(WindowCustomParam* param);
	void CallRedraw();

private:
	HINSTANCE _hInstance;
	HWND _hWnd;
	// TODO: hDc

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void RegisterWindowClass();
	HWND CreateMainWindow();

	/*
	   UI Elements : Tabs
	*/
	HWND _hTabControl;		// Handle to the Tab Control

	TCITEM _TCIEncode;		// Handle to the Tab Item for the encoding display
	TCITEM _TCIDecode;		// Handle to the Tab Item for the decoding display
	HWND _hStatic;			// Handle to the Static controle (where the displaying takes place)

	/*
	   UI Elements : Padding constants
	*/
	short _globalPad = 5;	// Padding away from the top-left (in both directions)
};

struct WindowCustomParam {
	BitmapAtHome* bmp;
	HBITMAP* hBmp;
};