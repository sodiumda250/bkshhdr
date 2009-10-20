/*
 * $Id: hookproc.cpp,v 1.5 2005/05/25 03:55:21 woods Exp $
 */

static char id[] = "$Id: hookproc.cpp,v 1.5 2005/05/25 03:55:21 woods Exp $";

#include <stdio.h>
#include <windows.h>
#include "hookproc.h"
#include "hdrwin.h"

extern "C" {
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

/// 元のウィンドウプロシージャ
static LONG prevWndProc = 0;
/// サブクラス化したウィンドウのウィンドウハンドル
static HWND hookhwnd = 0;

/**
 * @brief ウィンドウのサブクラス化
 * @param hMainWnd サブクラス化するウィンドウのウィンドウハンドル
 *
 * 指定されたウィンドウをサブクラス化する。
 */
void hookMailWindow(HWND hMainWnd)
{
    prevWndProc = ::SetWindowLong(hMainWnd, GWL_WNDPROC, (LONG)WindowProc);
    if (prevWndProc != 0) {
        hookhwnd = hMainWnd;
    } else {
        hookhwnd = 0;
    }
}

/**
 * @brief サブクラス化解除
 *
 * hookMailWindow()のサブクラス化を解除する。
 */
void unhookMailWindow(void)
{
    if (prevWndProc != 0) {
        ::SetWindowLong(hookhwnd, GWL_WNDPROC, (LONG)prevWndProc);
        prevWndProc = 0;
        hookhwnd = 0;
    }
}

/**
 * @brief サブクラスウィンドウメッセージハンドラ
 * @param hwnd ウィンドウのハンドル
 * @param uMsg メッセージ
 * @param wParam メッセージの追加情報1
 * @param lParam メッセージの追加情報2
 */
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    unsigned long pid_before = 0;
    unsigned long pid_after = 0;

    ::GetWindowThreadProcessId((HWND)(lParam), &pid_before);
    ::GetWindowThreadProcessId(hookhwnd, &pid_after);

    switch (uMsg) {
    case WM_ACTIVATE:
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
        break;
    case WM_SIZE:
        if (wParam == SIZE_MAXIMIZED) {
            HideWindow();
        } else if (wParam == SIZE_RESTORED) {
            ShowWindow();
        }
        break;
    default:
        break;
    }
    return ::CallWindowProc((WNDPROC)prevWndProc, hwnd, uMsg, wParam, lParam);
}

