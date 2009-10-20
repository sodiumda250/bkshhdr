/* $Id: hdrwin.h,v 1.27 2005/04/26 05:34:58 woods Exp $ */

#ifndef HDRWIN_H
#define HDRWIN_H

#include <windows.h>
#include "charbuf.h"

/// ウィンドウ定義構造体
class headerWindow {
    /// ウィンドウの大きさの横の最小値(枠の大きさなど)
    static int c_addx;
    /// ウィンドウの大きさの縦の最小値(タイトルバー、枠の大きさなど)
    static int c_addy;

    /// 表示用フォントハンドラ
    static HFONT c_hFont;

    /// 表示領域調整フラグ 0:調整しない それ以外:調整する
    static UINT c_adjust;

    /// ウィンドウハンドル
    HWND mhwnd;
    /// タイトル文字列
    charbuf mtitle;
    /// ウィンドウ内表示文字
    rectstr mbody;
    /// ウィンドウの表示領域
    RECT mrect;
    /// 左クリック位置
    POINT mclickpoint;
    /// マウスカーソルの位置
    POINT mmousepoint;
    /// ドラッグ中フラグ
    int mclicked;
    /// 表示フラグ
    int mshow;
public:
    /** @brief ウィンドウの大きさの横の最小値(枠の大きさなど) */
    static const int addx(){return c_addx;};
    /** @brief ウィンドウの大きさの縦の最小値(タイトルバー、枠の大きさなど) */
    static const int addy(){return c_addy;};

    /// @brief 設定されているフォントを削除する
    static void deleteFont(void) {
        if (c_hFont != NULL) {
            ::DeleteObject(c_hFont);
            c_hFont = NULL;
        }
    }

    /**
     * @brief フォントを設定する
     * @param f 設定するフォントハンドル
     */
    static void setFont(const HFONT f) {
        deleteFont();
        c_hFont = f;
    }
    /**
     * @brief フォントを設定する
     * @param logfont 設定するフォントのフォント構造体
     */
    static void setFont(const LOGFONT& logfont) {
        setFont(CreateFontIndirect(&logfont));
    }
    /**
     * @brief フォントを設定する
     * @param lp 設定するフォントのフォント構造体のポインタ
     */
    static void setFont(const LOGFONT* const lp) {
        setFont(CreateFontIndirect(lp));
    }
    /// @brief 設定されたフォントハンドルを返却する
    static const HFONT font(void) { return c_hFont;};

    /// @brief ウィンドウサイズ自動調整有無を返却する
    static UINT adjust(){return c_adjust;};
    /**
     * @brief ウィンドウサイズ自動調整有無を設定する
     * @param pAdjust 自動調整の有無
     */
    static void setadjust(const UINT pAdjust) {c_adjust = pAdjust;};

    /**
     * @brief ウィンドウの大きさの最小値を設定する
     * @param x 横の最小値(枠の大きさなど)
     * @param y 横の最小値(タイトルバー、枠の大きさなど)
     */
    static void setadd(const int x, const int y) {
        c_addx = x;
        c_addy = y;
    }

    /// @brief ウィンドウハンドルを返却する
    HWND hwnd(void){return mhwnd;};
    /// @brief ウィンドウハンドルを設定する
    void sethwnd(const HWND hWnd){mhwnd = hWnd;};
    /// @brief ウィンドウのタイトルを返却する
    const char* title(void){return mtitle.textbuf();};
    /// @brief ウィンドウのタイトルを設定する
    void settitle(const char *str) {mtitle = str;};
    /// @brief ウィンドウの表示文字列インスタンスを返却する
    rectstr& body(void){return mbody;};

    /**
     * @brief ウィンドウの表示文字インスタンスを返却する
     * @param index 文字のインデックス
     */
    rectchar& body(const int index){return mbody[index];};
    /// @brief ウィンドウ表示位置を返却する
    RECT& rect(void){return mrect;};
    /// @brief マウスクリックされた位置を返却する
    POINT& clickpoint(void){return mclickpoint;};
    /// @brief マウスクリックされた位置を保存する
    void setclickpoint(const POINTS& p) {
        mclickpoint.x = p.x;
        mclickpoint.y = p.y;
    };
    const POINT& mousepoint(void){return mmousepoint;};
    /// @brief クリック中かどうかを返却する
    int clicked(void){return mclicked;};
    /// @brief クリック中かどうかを設定する
    void setclicked(const int f){mclicked = f;};
    int show(void){return mshow;};
    void setshow(const int f){mshow = f;};
    /// @brief 初期化を行う
    void init(void) {
        mhwnd = NULL;
        mrect.top = 0;
        mrect.left = 0;
        mrect.bottom = 200;
        mrect.right = 50;
        memset(&(mclickpoint), 0, sizeof(mclickpoint));
        mclicked = 0;
    };
    /// @brief 解放を行う
    void free(void) {
        if (mhwnd != NULL) {
            ::DestroyWindow(mhwnd);
            mhwnd = NULL;
        }
    }
    headerWindow(void){init();};
    ~headerWindow(void){free();};
    /// @brief 再描画を行う
    void redraw(void) {
        ::InvalidateRect(mhwnd, NULL, TRUE);
        ::UpdateWindow(mhwnd);
    };
    /**
     * @brief 指定範囲の再描画を行う
     * @param rect 再描画範囲
     */
    void redraw(RECT& rect) {
        ::InvalidateRect(mhwnd, &rect, FALSE);
        ::UpdateWindow(mhwnd);
    }

    /**
     * @brief ヘッダ表示ウィンドウのサイズ調整
     *
     * 指定されたウィンドウの表示に必要な大きさを計算して、
     * ウィンドウの大きさを調整する。
     */
    void AdjustWindow(void) {
        const int len = body().length();

        headerWindow::setadd(
               ::GetSystemMetrics(SM_CXSIZEFRAME) * 2 + 1,
               ::GetSystemMetrics(SM_CYSMCAPTION) +
               ::GetSystemMetrics(SM_CXSIZEFRAME) * 2 + 1
        );

        rect().right = rect().left +
                          body().rect().right + headerWindow::addx();
        rect().bottom = rect().top +
                           body().rect().bottom + headerWindow::addy();

        SetWindowPos(hwnd(),
                     HWND_TOP,
                     rect().left,
                     rect().top,
                     rect().right - rect().left,
                     rect().bottom - rect().top,
                     ((len == 0) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));
        redraw();
    };

    /**
     * @brief 指定されたメールの内容でウィンドウを表示する
     * @param lpMailID メールID
     *
     * 指定されたメールIDのメールヘッダ情報をウィンドウに設定して表示する。
     */
    void ShowHeader(LPCTSTR lpMailID);

    /**
     * @brief ウィンドウの再表示
     *
     * 非表示になっているヘッダ表示ウィンドウを再表示する。
     */
    void ShowWindow(void) {
        SetWindowPos(hwnd(),
                     HWND_TOP,
                     rect().left,
                     rect().top,
                     rect().right - rect().left,
                     rect().bottom - rect().top,
                     ((body().length() == 0) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW) |
                         SWP_NOACTIVATE);
        redraw();
    };

    /**
     * @brief ウィンドウの非表示化
     *
     * ウィンドウを非表示にする。
     */
    void HideWindow(void)
    {
        SetWindowPos(hwnd(),
                     HWND_TOP,
                     rect().left,
                     rect().top,
                     rect().right - rect().left,
                     rect().bottom - rect().top,
                     SWP_HIDEWINDOW | SWP_NOACTIVATE);
    };

    void SetClipBoardText(void);
};

BOOL InitHeader(const char *szIni);
BOOL InitHeaederInstance(HINSTANCE hInstance, HWND hParentWnd);
int ShowHeader(LPCTSTR lpMailID);
BOOL DestroyHeader(void);

void ShowWindow(void);
void HideWindow(void);

BOOL LoadFont(LOGFONT *lf, const char *szIni);
BOOL SaveFont(LOGFONT *lf, const char *szIni);

UINT LoadAdjust(const char *szIni);
void SaveAdjust(UINT adjust, const char *szIni);

void SaveWindowPos(const char *szIni);

RECT& addrect(RECT& dst, const RECT& src);

#endif                                /* HDRWIN_H */
