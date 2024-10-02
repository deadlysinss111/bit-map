#include "pch.h"

/* Static fields definition */
WindowCustomParam WindowHandler::_cParam;
HWND** WindowHandler::_hENCODEarrElements = new HWND*[ENCODE_SCREEN_HWND_COUNT];
HWND** WindowHandler::_hDECODEarrElements = new HWND*[DECODE_SCREEN_HWND_COUNT];

WindowHandler::WindowHandler(HINSTANCE hInstance) {
    _hInstance = hInstance;

    RegisterWindowClass();

    _hWnd = CreateMainWindow();
    if (_hWnd == NULL) throw new std::exception("Failed to crete the main Window !");

    _hDc = GetDC(_hWnd);
    if (_hDc == NULL) throw new std::exception("Failed to retrieve the DC !");

    /*
       Prepares the static WindowCustomParam structure, used to call functions when message pumping
       - Sets a pointer to its own home class
       - The other 2 fields are set in main.cpp
    */
    SetCustomParam(&_cParam);
    _cParam.windowHandler = this;

    /*
       Initalizes common controls
    */
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES; // Initialize tab control class
    InitCommonControlsEx(&icex);

    /*
       Initializes UI Elements
    */
    CreateUIElements();
}

WindowHandler::~WindowHandler() {

}

void WindowHandler::RunWindow() {
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HWND WindowHandler::CreateMainWindow() {
    HWND hwnd = CreateWindowEx(
        0,
        L"MyWindowClass",
        L"My Win32 Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, 
        NULL,
        _hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return NULL;
    }

    ShowWindow(hwnd, SW_SHOW);
    return hwnd;
}

void WindowHandler::RegisterWindowClass() {
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = _hInstance;
    wc.lpszClassName = L"MyWindowClass";

    RegisterClass(&wc);
}

LRESULT WindowHandler::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //if (_cParam.windowHandler == nullptr) std::cerr << "winH  ";
    //if (_cParam.bitmapAtHome == nullptr) std::cerr << "bmpH  ";
    //if (_cParam.hBmp == nullptr) std::cerr << "hBmp  ";

    //if (_cParam.windowHandler == nullptr || _cParam.bitmapAtHome == nullptr || _cParam.hBmp == nullptr)
    //{
    //    std::cerr << "are nullptr\n";
    //    return E_FAIL;
    //}
    
    if (_cParam.windowHandler == nullptr || _cParam.bitmapAtHome == nullptr || _cParam.hBmp == nullptr)
    {
        std::cout << "It's too soon...\n";
        return E_FAIL;
    }

    switch (uMsg)
    {
    case WM_SIZE:
        _cParam.windowHandler->ResizeTabControl(lParam);
        break;

    case WM_NOTIFY:
        // Switching on the code of the WM_NOTIFY Message
        switch (((LPNMHDR)lParam)->code)
        {
        // We let the user change tab
        case TCN_SELCHANGING:
            return FALSE;
            break;
            
        // We load in the correct UI elements to show the correct screen
        case TCN_SELCHANGE:
            switch (TabCtrl_GetCurSel(_cParam.windowHandler->_hTabControl))
            {
            case ENCODE_TAB_INDEX:
                SendMessage(_cParam.windowHandler->_hStaticAppScreen, WM_SETTEXT, 0, (LPARAM) &(_cParam.windowHandler->_encodeTabName));
                // Make the UI elements of the Encode screen visible
                for (int i = (int)_hENCODEarrElements; i < (int)_hENCODEarrElements + ENCODE_SCREEN_HWND_COUNT; ++i)
                    ShowWindow(*_hENCODEarrElements[i], SW_SHOW);

                // Hides the UI elements of the Decode screen
                // TODO
                break;
            case DECODE_TAB_INDEX:
                SendMessage(_cParam.windowHandler->_hStaticAppScreen, WM_SETTEXT, 0, (LPARAM) &(_cParam.windowHandler->_decodeTabName));
                // Make the UI elements of the Decode screen visible
                // TODO

                // Hides the UI elements of the Encode screen
                for (int i = (int)_hENCODEarrElements;  i < (int)_hENCODEarrElements + ENCODE_SCREEN_HWND_COUNT;  ++i)
                    ShowWindow(*_hENCODEarrElements[i], SW_HIDE);
            }
            break;
        }
        break;
    
    case WM_PAINT:
        // Need to define a scope otherwise compiler isn't happy with variable definition
        {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hWnd, &ps);

            HDC memDC = CreateCompatibleDC(hDC);
            SelectObject(memDC, *(_cParam.hBmp));

            //StretchBlt(hDC, 10, 10, 40, 40, memDC, 0, 0, customParam->bmp->_infoHeader->biHeight, customParam->bmp->_infoHeader->biWidth, SRCCOPY);
            BitBlt(hDC, 10, 10, _cParam.bitmapAtHome->_infoHeader->biWidth, _cParam.bitmapAtHome->_infoHeader->biHeight, memDC, 0, 0, SRCCOPY);
            
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);

            //HBITMAP hBmp = *customParam->hBmp;
            //HDC memDC = CreateCompatibleDC(hdc);
            //SelectObject(memDC, hBmp);

            ////StretchBlt(hdc, 10, 10, 40, 40, memDC, 0, 0, customParam->bmp->_infoHeader->biHeight, customParam->bmp->_infoHeader->biWidth, SRCCOPY);
            //BitBlt(hdc, 10, 10, customParam->bitmapAtHome->_infoHeader->biWidth, customParam->bitmapAtHome->_infoHeader->biHeight, memDC, 0, 0, SRCCOPY);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

HBITMAP WindowHandler::CreateBmpHandler(BitmapAtHome* bmp) {
    // Set that in it's own function rather than directly in the WinProc for performences reasons
    return  CreateDIBitmap(_hDc, bmp->_infoHeader, CBM_INIT, bmp->_colorTable, (BITMAPINFO*)bmp->_infoHeader, DIB_RGB_COLORS);
}

void WindowHandler::SetCustomParam(WindowCustomParam* param) {
    SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(param));
}

void WindowHandler::CallRedraw() {
    RedrawWindow(_hWnd, 0, 0, RDW_INTERNALPAINT | RDW_INVALIDATE);
}

/* UI specific functions */
// Resizes the Tab Control to match the new window's size
HRESULT WindowHandler::ResizeTabControl(LPARAM ARGlParam)
{
    RECT rcApp;
    GetClientRect(_hWnd, &rcApp);
    if (!SetWindowPos(_hTabControl, HWND_TOP/*TODO : topmost ?*/, _globalPad, _globalPad, LOWORD(ARGlParam), HIWORD(ARGlParam), SWP_SHOWWINDOW)) {
        std::cerr << "Faild to resize the Tab Control !" << std::endl;
        return E_FAIL;
    }

    return S_OK;
}

// Creates every UI elements (should be called on construction)
void WindowHandler::CreateUIElements()
{
    /*
       Initializes UI Elements : Tabs
       - Creates the Tab Control object
       - Creates both Tab Items
       - Inserts both Tab Items
       - Creates the Static Window object
    */
    RECT rcTemp;    // Used to positon windows created heere relatively to each other
    GetClientRect(_hWnd, &rcTemp);
    _hTabControl = CreateWindow(
        WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        0, 0, rcTemp.right, rcTemp.bottom,
        _hWnd, NULL, _hInstance, NULL);
    if (_hTabControl == NULL) std::cerr << "Creation of the Tab Control failed and is NULL !";

    TCITEM TCIEncode;
    TCIEncode.mask = TCIF_TEXT;
    TCIEncode.pszText = _encodeTabName;
    TCIEncode.cchTextMax = sizeof(_encodeTabName);
    TCITEM TCIDecode;
    TCIDecode.mask = TCIF_TEXT;
    TCIDecode.pszText = _decodeTabName;
    TCIDecode.cchTextMax = sizeof(_decodeTabName);

    if (TabCtrl_InsertItem(_hTabControl, ENCODE_TAB_INDEX, &TCIEncode) == -1) {
        std::cerr << "Insertion of TCIEncode failed ! Destroying Tab Control..." << std::endl;
        DestroyWindow(_hTabControl);
    }
    if (TabCtrl_InsertItem(_hTabControl, DECODE_TAB_INDEX, &TCIDecode) == -1) {
        std::cerr << "Insertion of TCIDecode failed ! Destroying Tab Control..." << std::endl;
        DestroyWindow(_hTabControl);
    }

    _hStaticAppScreen = CreateWindow(
        WC_STATIC, L"", STATIC_STYLE,
        _globalPad, _globalPad + 20, 100, 100,
        _hWnd, NULL, _hInstance, NULL);
    if (_hStaticAppScreen == NULL) std::cerr << "Creation of the Static Window failed and is NULL !";

    /*
       Initializes UI Elements : Encoding Screen
       - Creates the button for the host
       - Creates the static window for the .bmp preview
       - Creates the button for the parasite
       - Creates an edit control to type a message to inject in
       - Creates a static text more to the right
       - Creates the static window for the parsited .bmp preview
       - Creates the button to inject the parasite file
       - Creates the button to inject the parasite message
    */

    GetWindowRect(_hStaticAppScreen, &rcTemp);
    _hENCODEbtnOpenHostBMP = CreateWindow(
        WC_BUTTON, L"Open a host bitmap image...", BUTTON_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, 30, 5,
        _hStaticAppScreen, (HMENU)IDB_HOST, _hInstance, NULL);
    if (_hENCODEbtnOpenHostBMP == NULL) std::cerr << "Creation of the Host Button failed and is NULL !";
    _hENCODEarrElements[0] = &_hENCODEbtnOpenHostBMP;

    GetWindowRect(_hENCODEbtnOpenHostBMP, &rcTemp);
    _hENCODEstaticHostPreview = CreateWindow(
        WC_STATIC, L"", STATIC_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, BMP_PREVIEW_DIMS, BMP_PREVIEW_DIMS,
        _hStaticAppScreen, NULL, _hInstance, NULL);
    if (_hENCODEstaticHostPreview == NULL) std::cerr << "Creation of the Static Host Preview failed and is NULL !";
    _hENCODEarrElements[1] = &_hENCODEstaticHostPreview;

    GetWindowRect(_hENCODEstaticHostPreview, &rcTemp);
    _hENCODEbtnOpenParasiteFile = CreateWindow(
        WC_BUTTON, L"Open a file to inject in...", BUTTON_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, 30, 5,
        _hStaticAppScreen, (HMENU)IDB_PARASITE, _hInstance, NULL);
    if (_hENCODEbtnOpenParasiteFile == NULL) std::cerr << "Creation of the Parasite Button failed and is NULL !";
    _hENCODEarrElements[2] = &_hENCODEbtnOpenParasiteFile;

    GetWindowRect(_hENCODEbtnOpenParasiteFile, &rcTemp);
    _hENCODEeditParasiteMessage = CreateWindow(
        WC_EDIT, L"If not a file, type your injected message here.", EDIT_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, 60, 40,
        _hStaticAppScreen, (HMENU)IDE_PARASITE, _hInstance, NULL);
    if (_hENCODEeditParasiteMessage == NULL) std::cerr << "Creation of the Edit Parasite failed and is NULL !";
    _hENCODEarrElements[3] = &_hENCODEeditParasiteMessage;

    GetWindowRect(_hENCODEbtnOpenHostBMP, &rcTemp);
    _hENCODEstaticResultTitle = CreateWindow(
        WC_STATIC, L"Preview of the result of the injection :", STATIC_STYLE,
        rcTemp.right + 2 * _globalPad, rcTemp.top, 60, 10,
        _hStaticAppScreen, NULL, _hInstance, NULL);
    if (_hENCODEstaticResultTitle == NULL) std::cerr << "Creation of the Static Result Title failed and is NULL !";
    _hENCODEarrElements[4] = &_hENCODEstaticResultTitle;

    GetWindowRect(_hENCODEstaticResultTitle, &rcTemp);
    _hENCODEstaticResultPreview = CreateWindow(
        WC_STATIC, L"", STATIC_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, BMP_PREVIEW_DIMS * 2, BMP_PREVIEW_DIMS * 2,
        _hStaticAppScreen, NULL, _hInstance, NULL);
    if (_hENCODEstaticResultPreview == NULL) std::cerr << "Creation of the Static Result Preview failed and is NULL !";
    _hENCODEarrElements[5] = &_hENCODEstaticResultPreview;

    GetWindowRect(_hENCODEstaticResultPreview, &rcTemp);
    _hENCODEbtnInjectFile = CreateWindow(
        WC_BUTTON, L"Inject file", BUTTON_STYLE,
        rcTemp.left + 10, rcTemp.bottom + 3 * _globalPad, 50, 10,
        _hStaticAppScreen, (HMENU)IDB_INJECT_FILE, _hInstance, NULL);
    if (_hENCODEbtnInjectFile == NULL) std::cerr << "Creation of the Injectf File Button failed and is NULL !";
    _hENCODEarrElements[6] = &_hENCODEbtnInjectFile;

    GetWindowRect(_hENCODEbtnInjectFile, &rcTemp);
    _hENCODEbtnInjectMessage = CreateWindow(
        WC_BUTTON, L"Inject message", BUTTON_STYLE,
        rcTemp.right + 3 * _globalPad, rcTemp.top, 50, 10,
        _hStaticAppScreen, (HMENU)IDB_INJECT_TEXT, _hInstance, NULL);
    if (_hENCODEbtnInjectMessage == NULL) std::cerr << "Craation of the Inject Message Button failed and is NULL !";
    _hENCODEarrElements[7] = &_hENCODEbtnInjectMessage;
}