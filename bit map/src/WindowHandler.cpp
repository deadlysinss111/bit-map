#include "pch.h"

WindowHandler::WindowHandler(HINSTANCE hInstance) {
    _hInstance = hInstance;

    RegisterWindowClass();

    _hWnd = CreateMainWindow();
    if (_hWnd == NULL) throw new std::exception("Failed to crete the main Window !");

    _hDc = GetDC(_hWnd);
    if (_hDc == NULL) throw new std::exception("Failed to retrieve the DC !");

    /*
       Prepares the static WindowCustomParam structure, used to call functions when message pumping
    */
    //SetCustomParam(&_cParam);
    _cParam.windowHandler = this;
    // TODO: Set the other members accordingly when merging
    _cParam.bitmapAtHome = NULL;
    _cParam.hBmp = NULL;

    /*
       Initalizes common controls
    */
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES; // Initialize tab control class
    InitCommonControlsEx(&icex);

    /*
       Initializes UI Elements
       - Creates the Tab Control object
       - Creates both Tab Items
       - Inserts both Tab Items
       - Creates the Static Window object
    */
    RECT rcApp;
    GetClientRect(_hWnd, &rcApp);
    _hTabControl = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, _globalPad, _globalPad, rcApp.right, rcApp.bottom, _hWnd, NULL, _hInstance, NULL);
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

    _hStatic = CreateWindow(WC_STATIC, L"", WS_CHILD | WS_BORDER | WS_VISIBLE, _globalPad + 50, _globalPad + 50, 100, 100, _hWnd, NULL, _hInstance, NULL);
    if (_hStatic == NULL) std::cerr << "Creation of the Static Window failed and is NULL !";
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

LRESULT WindowHandler::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //_cParam = *(WindowCustomParam*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    //WindowCustomParam* customParam = (WindowCustomParam*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

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
                // Creates into the window every UI Elements, and calls RedrawWindow
                break;
            case DECODE_TAB_INDEX:
                // Creates into the window every UI Elements, and calls RedrawWindow
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
