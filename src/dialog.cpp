/*
 * $Id: dialog.cpp,v 1.19 2005/04/26 05:29:09 woods Exp $
 * dialog.cpp : ダイアログ用のプロシージャ定義
 */

static char id[] = "$Id: dialog.cpp,v 1.19 2005/04/26 05:29:09 woods Exp $";

#include <stdio.h>
#include "resource.h"
#include "dialog.h"
#include "bkshhdr.h"
#include "hdrwin.h"

// グローバル変数:

/// 現在のインスタンス
static HINSTANCE hInstance;

/// 設定ダイアログの表示ヘッダ設定文字列バッファ
static char *setstr;
/// 設定ダイアログの表示ヘッダ設定文字列バッファの大きさ
static int setsize;
/// ウィンドウ自動調整有無
static int adjustwindow;

// このコード モジュールに含まれる関数の前宣言:
extern "C" {
static LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK Setting(HWND, UINT, WPARAM, LPARAM);
}

static BOOL SelectFont(void);

/**
 * @brief 設定ダイアログ表示関数
 * @param hInst ダイアログが含まれるインスタンスのハンドル
 * @param hWnd 親ウィンドウのウィンドウハンドル
 * @param str 表示するヘッダ文字列の初期値(カンマ区切り)
 * @param size strバッファの大きさ
 * @param adjust ウィンドウサイズ自動調節フラグ
 *
 * 設定ダイアログを表示する。
 */
int DoSetting(HINSTANCE hInst, HWND hWnd, char *str, int size, UINT *adjust)
{
    setstr = str;
    setsize = size;
    adjustwindow = *adjust;
    hInstance = hInst;
    ::DialogBox(hInst, (LPCTSTR) IDD_SETTING, hWnd, (DLGPROC) Setting);

    *adjust = (adjustwindow != BST_UNCHECKED) ? 1 : 0;

    SaveWindowPos(szIni);
    return 0;
}

/**
 * @brief Aboutダイアログ表示関数
 * @param hInst ダイアログが含まれるインスタンスのハンドル
 * @param hWnd 親ウィンドウのウィンドウハンドル
 *
 * Aboutダイアログを表示する。
 */
int DoAbout(HINSTANCE hInst, HWND hWnd)
{
    ::DialogBox(hInst, (LPCTSTR) IDD_ABOUTBOX, hWnd, (DLGPROC) About);
    return 0;
}

/**
 * @brief 設定ダイアログ用メッセージ ハンドラ
 * @param hDlg : ウィンドウハンドル
 * @param message : メッセージ
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * 設定ダイアログのウィンドウメッセージを処理する。
 */
static LRESULT CALLBACK
Setting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT length;
    UINT chk;

    switch (message) {
    case WM_INITDIALOG:
        chk = (adjustwindow != 0) ? BST_CHECKED : BST_UNCHECKED;
        ::CheckDlgButton(hDlg, IDC_ADJUSTWINDOW, chk);
        ::SetDlgItemText(hDlg, IDC_HEADERS, setstr);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            length =
                ::GetDlgItemText(hDlg, IDC_HEADERS, setstr, setsize);
            adjustwindow = ::IsDlgButtonChecked(hDlg, IDC_ADJUSTWINDOW);
            ::EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
            break;
        case IDCANCEL:
            ::EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
            break;
        case IDC_ABOUT:
            DoAbout(hInstance, hDlg);
            break;
        case IDC_FONT:
            SelectFont();
            break;
        }
        break;
    default:
        break;
    }
    return FALSE;
}


/**
 * @brief Aboutダイアログメッセージ ハンドラ
 * @param hDlg : ウィンドウハンドル
 * @param message : メッセージ
 * @param wParam : メッセージのパラメータ
 * @param lParam : メッセージのパラメータ
 *
 * Aboutダイアログのウィンドウメッセージを処理する。
 */
static LRESULT CALLBACK
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            ::EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

/**
 * @brief フォント選択ダイアログ表示
 *
 * iniファイルからフォント情報を取得して、
 * それを初期値としてフォント選択コモンダイアログを表示し、
 * コモンダイアログで設定された情報をiniファイルに設定する。
 */
static BOOL SelectFont(void)
{
    BOOL ret;
    CHOOSEFONT font;
    LOGFONT logfont;

    memset(&font, 0, sizeof(font));

    LoadFont(&logfont, szIni);

    font.lStructSize = sizeof(font);
    font.lpLogFont = &logfont;
    font.hInstance = hInstance;
    font.Flags = CF_SCREENFONTS | CF_NOVERTFONTS | CF_INITTOLOGFONTSTRUCT ;
    ret = ::ChooseFont(&font);
    if (ret) {
        SaveFont(&logfont, szIni);
    }
    return ret;
}
