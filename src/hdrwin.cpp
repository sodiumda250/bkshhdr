/*
 * $Id: hdrwin.cpp,v 1.73 2005/04/26 05:14:27 woods Exp $
 */

static char id[] = "$Id: hdrwin.cpp,v 1.73 2005/04/26 05:14:27 woods Exp $";

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include "resource.h"
#include "bkshhdr.h"
#include "charbuf.h"
#include "hdrlist.h"
#include "hdrwin.h"

// ソース内グローバル
/// ヘッダ表示ウィンドウのWindow Class
static TCHAR szWindowClass[100] = "";

int headerWindow::c_addx = 0;
int headerWindow::c_addy = 0;
HFONT headerWindow::c_hFont = 0;
UINT headerWindow::c_adjust = 0;

/// ヘッダ表示用のウィンドウ定義構造体の配列
static headerWindow *window;
/// ウィンドウ定義構造体の個数
static int windowNum = 0;


// このコード モジュールに含まれる関数の前宣言:
extern "C" {
static ATOM MyRegisterClass(HINSTANCE hInstance);
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
}

static int GetHeaderIndex(HWND hwnd);

static void Redraw(void);
static LRESULT STDCALL OnChar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnActivate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnLButtonDblClk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnNCLButtonDblClk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT STDCALL OnKillFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/**
 * @brief ヘッダ表示ウィンドウ情報の初期化
 * @param szIniFile iniファイルのパス名
 * @retval TRUE 初期化成功
 * @retval FALSE 初期化失敗
 *
 * ヘッダ表示ウィンドウ情報を初期化する。
 */
BOOL InitHeader(const char *szIniFile)
{
    char buf[1024];
    int max;
    char *p = buf;
    char *pp = buf + 1;
    int len = 0;
    char* plist[512];
    int i;

    memset(plist, 0, sizeof(plist));

    headerWindow::setadjust(LoadAdjust(szIniFile));

    // iniファイルのセクション名が表示ヘッダ名なのでセクション名を全て取得する
    max = GetPrivateProfileSectionNames(buf, sizeof(buf), szIni);

    if (strlen(buf) == 0) {
        memset(buf, 0, sizeof(buf));
        strcpy(buf, "X-Mailer");
    }

    // セクション名(ヘッダ名)ごとの処理
    p = buf, pp = buf;
    while ((*p != 0) && (*pp != 0) && (p < buf + max) && (pp < buf + max)) {
        int show;

        while (*pp != 0) {
            pp++;
        }
        // "Show"キーが0なら非表示
        show = GetPrivateProfileInt(p, "Show", 1, szIni);
        if (show != 0) {
            plist[len++] = p;
        }
        pp++;
        p = pp;
    }
    window = new headerWindow[len];
    windowNum = len;

    for (i = 0; i < windowNum; i++) {
        int cx, cy;

        window[i].settitle(plist[i]);
        window[i].body() = plist[i]/* 本当はヘッダの内容 */;
        window[i].rect().top =
                 GetPrivateProfileInt(plist[i], "Top", i * 10, szIni);
        window[i].rect().left =
                 GetPrivateProfileInt(plist[i], "Left", i * 10, szIni);
        cy =
                 GetPrivateProfileInt(plist[i], "Height", 45, szIni);
        cx =
                 GetPrivateProfileInt(plist[i], "Width", 200, szIni);
        window[i].rect().bottom = window[i].rect().top + cy;
        window[i].rect().right = window[i].rect().left + cx;
    }

    return TRUE;
}

/**
 * @brief ヘッダ表示ウィンドウ情報のデストラクタ
 * @retval TRUE 破壊成功
 * @retval FALSE 破壊失敗
 *
 * ヘッダ表示ウィンドウ情報を破棄する。
 */
BOOL DestroyHeader(void)
{
    delete[] window;
    window = NULL;
    windowNum = 0;

    headerWindow::deleteFont();

    return TRUE;
}

/**
 * @brief ヘッダ情報の表示
 * @param lpMailID : メールID
 * @return 表示ウィンドウの個数
 *
 * 指定されたメールのヘッダ情報からヘッダ表示ウィンドウを表示する。
 */
int ShowHeader(LPCTSTR lpMailID)
{
    int i;

    if (headerWindow::adjust()) {
        headerWindow::setadd(
               GetSystemMetrics(SM_CXSIZEFRAME) * 2 + 1,
               GetSystemMetrics(SM_CYSMCAPTION) +
               GetSystemMetrics(SM_CXSIZEFRAME) * 2 + 1
        );
    }
    for (i = windowNum - 1; i >= 0; i--) {
        window[i].ShowHeader(lpMailID);
    }
    
    return windowNum;
}

/**
 * @brief ヘッダ情報の表示
 * @param lpMailID : メールID
 *
 * 指定されたメールのヘッダ情報からヘッダ表示ウィンドウを表示する。
 */
void headerWindow::ShowHeader(LPCTSTR lpMailID)
{
    int len;
    char buf[1024];
    POINT pt;
    HDC hdc;

    if (lpMailID != NULL && lpMailID[0] != 0) {
        bka.GetSpecifiedHeader(title(), buf, sizeof(buf));
    } else {
        buf[0] = 0;
    }
    len = strlen(buf);
    body() = buf;

    // 文字列の幅と高さを計算します。
    hdc = GetDC(hwnd());
    SelectObject(hdc, font());
    pt.x = 1;
    pt.y = 1;
    body().setrect(hdc, pt);
    ReleaseDC(hwnd(), hdc);
    if (adjust() && (len > 0)) {
        rect().right = rect().left + body().rect().right + headerWindow::addx();
        rect().bottom = rect().top + body().rect().bottom + headerWindow::addy();
    }

    SetWindowPos(hwnd(),
                 HWND_TOP,
                 rect().left,
                 rect().top,
                 rect().right - rect().left,
                 rect().bottom - rect().top,
                 ((len == 0) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW) | SWP_NOACTIVATE);
    redraw();
}

/**
 * @brief ヘッダ表示ウィンドウの表示
 *
 * 非アクティブ化で非表示になっているヘッダ表示ウィンドウを表示する。
 */
void ShowWindow(void)
{
    int i;

    for (i = windowNum - 1; i >= 0; i--) {
        window[i].ShowWindow();
    }
}

/**
 * @brief ヘッダ表示ウィンドウの非表示化
 *
 * メインウィンドウの非アクティブ化に伴ってヘッダ表示ウィンドウを非表示にする。
 */
void HideWindow(void)
{
    int i;

    for (i = windowNum - 1; i >= 0; i--) {
        window[i].HideWindow();
    }
}

/**
 * @brief ヘッダ表示ウィンドウクラスの登録
 * @param hInstance : Window Procedureのあるモジュールのインスタンス
 * @return ウィンドウクラスを識別するアトム
 *
 * ヘッダ表示ウィンドウのウィンドウクラスを登録する。
 */
static ATOM MyRegisterClass(HINSTANCE hInstance)
{
    ATOM ret;
    WNDCLASSEX wcex;

    if (strcmp(szWindowClass, "BKSHHDR") == 0) {
        return 1;
    }
    strcpy(szWindowClass, "BKSHHDR");

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
    wcex.lpfnWndProc = (WNDPROC) WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_IBEAM);
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIcon = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
    wcex.hIconSm = NULL;//LoadIcon(wcex.hInstance, (LPCTSTR) IDI_SMALL);

    ret = RegisterClassEx(&wcex);
    if (ret == 0) {
        LPVOID lpMsgBuf;

        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
        );
        MessageBox( NULL, (char *)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
        LocalFree( lpMsgBuf );
    }
    return ret;
}

/**
 * @brief ヘッダ表示用ウィンドウの作成
 * @param hInstance : Window Procedureのあるモジュールのインスタンス
 * @param hParentWnd : 親ウィンドウ
 * @retval TRUE 成功
 * @retval FALSE 失敗
 *
 * ヘッダ表示ウィンドウを作成する。
 */
BOOL
InitHeaederInstance(HINSTANCE hInstance, HWND hParentWnd)
{
    int i;
    HWND hWnd;
    LOGFONT logfont;
    HDC hDC;

    MyRegisterClass(hInstance);

    LoadFont(&logfont, szIni);
    headerWindow::setFont(logfont);

    for (i = 0; i < windowNum; i++) {
        hWnd = CreateWindowEx(WS_EX_TOOLWINDOW,
                            szWindowClass,
                            window[i].title(),
                            WS_POPUP | WS_BORDER | WS_CAPTION | WS_THICKFRAME,
                            window[i].rect().left,
                            window[i].rect().top,
                            window[i].rect().right - window[i].rect().left,
                            window[i].rect().bottom - window[i].rect().top,
                            hParentWnd, NULL, hInstance, NULL);

        if (!hWnd) {
            LPVOID lpMsgBuf;

            FormatMessage( 
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM | 
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
                (LPTSTR) &lpMsgBuf,
                0,
                NULL 
            );
            MessageBox( NULL, (char *)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
            LocalFree( lpMsgBuf );

            return FALSE;
        }
        window[i].sethwnd(hWnd);

        hDC = GetDC(hWnd);
        SelectObject(hDC, headerWindow::font());

        ShowWindow(hWnd, SW_HIDE);
        UpdateWindow(hWnd);
        ReleaseDC(hWnd, hDC);

        GetWindowRect(hWnd, &(window[i].rect()));
    }

    return TRUE;
}

/**
 * @brief ヘッダ表示ウィンドウメッセージハンドラ
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのウィンドウメッセージを処理する。
 */
static LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;

    switch (message) {
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // メニュー選択の解析:
        switch (wmId) {
        case IDM_COPY:
        case IDM_CUT:
            window[GetHeaderIndex(hWnd)].SetClipBoardText();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_CHAR:
        OnChar(hWnd, message, wParam, lParam);
        break;
    case WM_PAINT:
        OnPaint(hWnd, message, wParam, lParam);
        break;
    case WM_SIZE:
    case WM_MOVE:
        // ウィンドウの大きさ、位置をセーブする。
        OnMove(hWnd, message, wParam, lParam);
        break;
    case WM_ACTIVATE:
        OnActivate(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:
        //PostQuitMessage( 0 );
        break;
    case WM_LBUTTONDOWN:
        OnLButtonDown(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONUP:
        OnLButtonUp(hWnd, message, wParam, lParam);
        break;
    case WM_MOUSEMOVE:
        OnMouseMove(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONDBLCLK:
        OnLButtonDblClk(hWnd, message, wParam, lParam);
        break;
    case WM_NCLBUTTONDBLCLK:
        OnNCLButtonDblClk(hWnd, message, wParam, lParam);
        break;
    case WM_KILLFOCUS:
        OnKillFocus(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/**
 * @brief WM_CHARイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_CHAR)
 * @param wParam : メッセージのパラメータ(character code)
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_CHARメッセージを処理する。
 * Ctrl-Cが入力されたら指定された文字列をクリップボードにコピーする。
 */
static LRESULT STDCALL
OnChar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if ((((TCHAR) wParam) == 'c') || (((TCHAR) wParam) == 'C')) {
        LRESULT ret = SendMessage(hWnd, WM_GETHOTKEY, (WPARAM)0, (LPARAM)0); 
        if (ret & HOTKEYF_CONTROL) {
            window[GetHeaderIndex(hWnd)].SetClipBoardText();
        }
    } else if (((TCHAR) wParam) == 3/* ctrl-C */) {
        window[GetHeaderIndex(hWnd)].SetClipBoardText();
    }
    return 0;
}

/**
 * @brief WM_MOVE/WM_SIZEイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_MOVE/WM_SIZE)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_MOVE/WM_SIZEメッセージを処理する。
 * 移動／サイズ変更後のウィンドウ位置範囲を記録する。
 */
static LRESULT STDCALL
OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ::GetWindowRect(hWnd, &(window[GetHeaderIndex(hWnd)].rect()));

    return 0;
}

/**
 * @brief WM_ACTIVATEイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_ACTIVATE)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_ACTIVATEメッセージを処理する。
 */
static LRESULT STDCALL
OnActivate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    unsigned long pid_before = 0;
    unsigned long pid_after = 0;

    ::GetWindowThreadProcessId((HWND)(lParam), &pid_before);
    ::GetWindowThreadProcessId(hWnd, &pid_after);

    if (pid_before != pid_after && (BOOL)HIWORD(wParam) == false) {
        switch (LOWORD(wParam)) {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            ShowWindow();
            break;
        case WA_INACTIVE:
            HideWindow();
            break;
        default:
           break;
        }
    }

    return 0;
}

/**
 * @brief WM_PAINTイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_PAINT)
 * @param wParam : メッセージのパラメータ(the device context to draw in)
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_PAINTメッセージを処理し、
 * ヘッダ表示ウィンドウを描画する。
 */
static LRESULT STDCALL
OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int j;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT urect;

    COLORREF ctextn, cbackn;

    headerWindow& hw = window[GetHeaderIndex(hWnd)];
    hdc = ::BeginPaint(hWnd, &ps);
    ctextn = ::GetTextColor(hdc);
    cbackn = ::GetBkColor(hdc);
    urect = ps.rcPaint;
    ::SelectObject(hdc, headerWindow::font());
    for (j = 0; j < hw.body().length(); j++) {
        if (hw.body(j).in_rect(urect)) {
            if (hw.body(j).reverse() != 0) {
                ::SetTextColor(hdc, cbackn);
                ::SetBkColor(hdc, ctextn);
            } else {
                ::SetTextColor(hdc, ctextn);
                ::SetBkColor(hdc, cbackn);
            }
            ::DrawText(hdc, hw.body(j).str(),
                     hw.body(j).length(),
                     &(hw.body(j).rect()), DT_LEFT);
        }
    }
    ::EndPaint(hWnd, &ps);

    return 0;
}

/**
 * @brief WM_LBUTTONDOWNイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_LBUTTONDOWN)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ(mouse clicked point)
 *
 * ヘッダ表示ウィンドウのWM_LBUTTONDOWNイベントを処理する。
 * マウスクリックフラグをセットし、クリック位置を記録し、選択文字列をクリアする。
 */
static LRESULT STDCALL
OnLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int j;
    RECT rect;

    ::SetActiveWindow(hWnd);
    headerWindow& hw = window[GetHeaderIndex(hWnd)];
    hw.setclicked(1);
    hw.setclickpoint(MAKEPOINTS(lParam));

    rect.top = rect.bottom = MAKEPOINTS(lParam).y;
    rect.left = rect.right = MAKEPOINTS(lParam).x;
    for (j = 0; j < hw.body().length(); j++) {
        if (hw.body(j).reverse() != 0) {
            hw.body(j).setreverse(0);
            addrect(rect, hw.body(j).rect());
        }
    }
    hw.redraw(rect);
    ::SetCapture(hWnd);

    return 0;
}

/**
 * @brief WM_LBUTTONUPイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_LBUTTONUP)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ(mouse clicked point)
 *
 * ヘッダ表示ウィンドウのWM_LBUTTONUPイベントを処理する。
 * クリックフラグをリセットする。
 */
static LRESULT STDCALL
OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ::ReleaseCapture();
    window[GetHeaderIndex(hWnd)].setclicked(0);

    return 0;
}

/**
 * @brief WM_MOUSEMOVEイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_MOUSEMOVE)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ(point of mouse cursor)
 *
 * ヘッダ表示ウィンドウのWM_MOUSEMOVEイベントを処理し、
 * マウスがクリックされているときは文字列選択処理を行う。
 */
static LRESULT STDCALL
OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int j;
    RECT mrect;
    RECT crect;

    headerWindow& hw = window[GetHeaderIndex(hWnd)];
    if (hw.clicked() && DragDetect(hWnd, hw.clickpoint())) {
        // ドラッグ時処理
        mrect.left = (MAKEPOINTS(lParam).x < hw.clickpoint().x) ?
                         MAKEPOINTS(lParam).x : hw.clickpoint().x;
        mrect.right = (MAKEPOINTS(lParam).x > hw.clickpoint().x) ?
                         MAKEPOINTS(lParam).x : hw.clickpoint().x;
        mrect.top = (MAKEPOINTS(lParam).y < hw.clickpoint().y) ?
                         MAKEPOINTS(lParam).y : hw.clickpoint().y;
        mrect.bottom = (MAKEPOINTS(lParam).y > hw.clickpoint().y) ?
                         MAKEPOINTS(lParam).y : hw.clickpoint().y;
        crect = mrect;

        for (j = 0; j < hw.body().length(); j++) {
            if (hw.body(j).in_rect(mrect)) {
                if (hw.body(j).reverse() == 0) {
                    hw.body(j).setreverse(1);
                    addrect(crect, hw.body(j).rect());
                }
            } else {
                if (hw.body(j).reverse() != 0) {
                    hw.body(j).setreverse(0);
                    addrect(crect, hw.body(j).rect());
                }
            }
        }
        hw.redraw(crect);
    }
    return 0;
}

/**
 * @brief WM_LBUTTONDBLCLKイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_LBUTTONDBLCLK)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_LBUTTONDBLCLKイベントを処理し、
 * 全文字を選択状態にする。
 */
static LRESULT STDCALL
OnLButtonDblClk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int j;

    headerWindow& hw = window[GetHeaderIndex(hWnd)];
    for (j = 0; j < hw.body().length(); j++) {
        hw.body(j).setreverse(1);
    }
    hw.redraw();

    return 0;
}

/**
 * @brief WM_NCLBUTTONDBLCLKイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_NCLBUTTONDBLCLK)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_NCLBUTTONDBLCLKイベントを処理し、
 * ウィンドウの大きさを調整する。
 */
static LRESULT STDCALL
OnNCLButtonDblClk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    window[GetHeaderIndex(hWnd)].AdjustWindow();

    return 0;
}

/**
 * @brief WM_KILLFOCUSイベント処理
 * @param hWnd : ウィンドウハンドル
 * @param message : メッセージ(WM_KILLFOCUS)
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * ヘッダ表示ウィンドウのWM_KILLFOCUSイベントを処理し、
 * 文字の選択状態を解除する。
 */
static LRESULT STDCALL
OnKillFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int j;

    ::ReleaseCapture();
    headerWindow& hw = window[GetHeaderIndex(hWnd)];
    for (j = 0; j < hw.body().length(); j++) {
        hw.body(j).setreverse(0);
    }
    hw.redraw();

    return 0;
}

/**
 * @brief クリップボードへコピー
 * @param hwnd コピー対象のウィンドウのハンドル
 *
 * ヘッダ表示ウィンドウのテキストをクリップボードにコピーする。
 */
void headerWindow::SetClipBoardText()
{
    int j, pt;
    HANDLE hText;
    int len;
    int blen;
    char *pText;
    char buf[128];

    len = body().length();
    blen = 0;
    for (j = 0; j < len; j++) {
        if (body(j).reverse() != 0) {
            blen += body(j).length();
        }
    }

    if (blen == 0) {
        return;
    }

    hText = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, blen + 1);
    if (hText == NULL) {
        return;
    }

    pText = (char *) ::GlobalLock(hText);
    pt = 0;
    for (j = 0; j < len; j++) {
        if (body(j).reverse() != 0) {
            memcpy(pText + pt, body(j).str(),
                   body(j).length());
            pt += body(j).length();
        }
    }
    pText[pt] = 0;
    ::GlobalUnlock(hText);

    /* Open clipboard and clear it of it's contents */
    if (! ::OpenClipboard(hwnd())) {
        return;
    }
    EmptyClipboard();

    if (! ::SetClipboardData(CF_TEXT, hText)) {
        ::GlobalFree(hText);
    }
    ::CloseClipboard();

    strcpy(buf, "copied to clipboard");
    ::LoadString(g_hInstance, IDS_COPYDONE, buf, sizeof(buf));
    bka.SetMessageText(NULL, buf);
    return;
}

/**
 * @brief フォント情報ロード
 * @param lf フォント情報をロードするためのLOGFONT構造体を示すポインタ
 * @param szIni iniファイルのパス名
 * @retval TRUE フォント情報ロード成功
 * @retval FALSE フォント情報ロード失敗
 *
 * フォント情報をiniファイルからロードする。
 */
BOOL LoadFont(LOGFONT *lf, const char *szIni)
{
    char buffer[128];

    memset(lf, 0, sizeof(LOGFONT));

    lf->lfHeight =
        ::GetPrivateProfileInt("BkShHdr", "FontSize", -9, szIni);
    lf->lfCharSet = DEFAULT_CHARSET;
    lf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf->lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf->lfPitchAndFamily = DEFAULT_PITCH;
    ::GetPrivateProfileString("BkShHdr", "FontName", "System",
                              buffer, sizeof(buffer), szIni);
    strncpy(lf->lfFaceName, buffer, sizeof(lf->lfFaceName));
    lf->lfWeight = ::GetPrivateProfileInt("BkShHdr", "FontWeight", 0, szIni);
    lf->lfItalic = ::GetPrivateProfileInt("BkShHdr", "FontItalic", 0, szIni);

    return TRUE;
}

/**
 * @brief フォント情報セーブ
 * @param lf フォント情報にセーブするLOGFONT構造体を示すポインタ
 * @param szIni iniファイルのパス名
 * @retval TRUE フォント情報セーブ成功
 * @retval FALSE フォント情報セーブ失敗
 *
 * フォント情報をiniファイルにセーブする。
 */
BOOL SaveFont(LOGFONT *lf, const char *szIni)
{
    char buffer[128];

    ::WritePrivateProfileString("BkShHdr", "FontName",
                                lf->lfFaceName, szIni);
    sprintf(buffer, "%ld", lf->lfHeight);
    ::WritePrivateProfileString("BkShHdr", "FontSize",
                                buffer, szIni);
    sprintf(buffer, "%ld", lf->lfWeight);
    ::WritePrivateProfileString("BkShHdr", "FontWeight", buffer, szIni);

    sprintf(buffer, "%d", lf->lfItalic);
    ::WritePrivateProfileString("BkShHdr", "FontItalic", buffer, szIni);

    headerWindow::setFont(lf);
    Redraw();
    return TRUE;
}

/**
 * @brief ウィンドウ定義構造体インデックス取得
 * @param hwnd インデックスを取得するウィンドウのウィンドウハンドル
 * @return ウィンドウ定義構造体のインデックス
 *
 * ウィンドウハンドルからウィンドウ定義構造体のインデックスを取得する。
 */
static int GetHeaderIndex(HWND hwnd)
{
    int i;

    for (i = 0; i < windowNum; i++) {
        if (hwnd == window[i].hwnd()) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief ヘッダ表示ウィンドウの再描画
 *
 * ヘッダ表示ウィンドウを全て再描画する。
 */
static void Redraw(void)
{
    int i;

    for (i = 0; i < windowNum; i++) {
        window[i].redraw();
    }
}

/**
 * @brief ヘッダ表示ウィンドウ位置のセーブ
 * @param szIni iniファイルのパス名
 *
 * ヘッダ表示ウィンドウの位置をiniファイルにセーブする。
 */
void SaveWindowPos(const char *szIni)
{
    int i;
    char buf[128];

    for (i = 0; i < windowNum; i++) {
        // 今あるウィンドウは必ず非表示でない
        ::WritePrivateProfileString(window[i].title(), "Show",
                                    "1", szIni);

        sprintf(buf, "%ld", window[i].rect().top);
        ::WritePrivateProfileString(window[i].title(), "Top",
                                    buf, szIni);
        sprintf(buf, "%ld", window[i].rect().left);
        ::WritePrivateProfileString(window[i].title(), "Left",
                                    buf, szIni);
        sprintf(buf, "%ld", window[i].rect().bottom - window[i].rect().top);
        ::WritePrivateProfileString(window[i].title(), "Height",
                                    buf, szIni);
        sprintf(buf, "%ld", window[i].rect().right - window[i].rect().left);
        ::WritePrivateProfileString(window[i].title(), "Width",
                                    buf, szIni);
    }
}

/**
 * @brief ウィンドウ自動調整情報ロード
 * @param szIni iniファイルのパス名
 *
 * ウィンドウの大きさを自動調整するかどうかの情報をロードする。
 */ 
UINT LoadAdjust(const char *szIni)
{
    UINT ret = ::GetPrivateProfileInt("BkShHdr", "AdjustWindow", 0, szIni);

    headerWindow::setadjust(ret);
    return ret;
}

/**
 * @brief ウィンドウ自動調整情報セーブ
 * @param pAdjust 自動調整フラグ(0:自動調整しない、それ以外:自動調整する)
 * @param szIni iniファイルのパス名
 *
 * ウィンドウの大きさを自動調整するかどうかの情報をiniファイルにセーブする。
 */ 
void SaveAdjust(UINT pAdjust, const char *szIni)
{
    char buf[32];

    headerWindow::setadjust(pAdjust);
    sprintf(buf, "%d", pAdjust);
    ::WritePrivateProfileString("BkShHdr", "AdjustWindow", buf, szIni);
}

/**
 * @brief RECTの範囲の合成
 * @param dst 合成対象のRECT構造体ポインタ
 * @param src 合成するRECT構造体ポインタ
 * @return 合成したRECT(dstと同じもの)
 *
 * RECTの範囲を合成し、２つのRECTがともに含まれるRECTを作成しdstに設定する。
 */
RECT& addrect(RECT& dst, const RECT& src)
{
    dst.top = (dst.top > src.top) ? src.top : dst.top;
    dst.bottom = (dst.bottom < src.bottom) ? src.bottom : dst.bottom;
    dst.left = (dst.left > src.left) ? src.left : dst.left;
    dst.right = (dst.right < src.right) ? src.right : dst.right;

    return dst;
}
