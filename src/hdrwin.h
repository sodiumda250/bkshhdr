/* $Id: hdrwin.h,v 1.27 2005/04/26 05:34:58 woods Exp $ */

#ifndef HDRWIN_H
#define HDRWIN_H

#include <windows.h>
#include "charbuf.h"

/// �E�B���h�E��`�\����
class headerWindow {
    /// �E�B���h�E�̑傫���̉��̍ŏ��l(�g�̑傫���Ȃ�)
    static int c_addx;
    /// �E�B���h�E�̑傫���̏c�̍ŏ��l(�^�C�g���o�[�A�g�̑傫���Ȃ�)
    static int c_addy;

    /// �\���p�t�H���g�n���h��
    static HFONT c_hFont;

    /// �\���̈撲���t���O 0:�������Ȃ� ����ȊO:��������
    static UINT c_adjust;

    /// �E�B���h�E�n���h��
    HWND mhwnd;
    /// �^�C�g��������
    charbuf mtitle;
    /// �E�B���h�E���\������
    rectstr mbody;
    /// �E�B���h�E�̕\���̈�
    RECT mrect;
    /// ���N���b�N�ʒu
    POINT mclickpoint;
    /// �}�E�X�J�[�\���̈ʒu
    POINT mmousepoint;
    /// �h���b�O���t���O
    int mclicked;
    /// �\���t���O
    int mshow;
public:
    /** @brief �E�B���h�E�̑傫���̉��̍ŏ��l(�g�̑傫���Ȃ�) */
    static const int addx(){return c_addx;};
    /** @brief �E�B���h�E�̑傫���̏c�̍ŏ��l(�^�C�g���o�[�A�g�̑傫���Ȃ�) */
    static const int addy(){return c_addy;};

    /// @brief �ݒ肳��Ă���t�H���g���폜����
    static void deleteFont(void) {
        if (c_hFont != NULL) {
            ::DeleteObject(c_hFont);
            c_hFont = NULL;
        }
    }

    /**
     * @brief �t�H���g��ݒ肷��
     * @param f �ݒ肷��t�H���g�n���h��
     */
    static void setFont(const HFONT f) {
        deleteFont();
        c_hFont = f;
    }
    /**
     * @brief �t�H���g��ݒ肷��
     * @param logfont �ݒ肷��t�H���g�̃t�H���g�\����
     */
    static void setFont(const LOGFONT& logfont) {
        setFont(CreateFontIndirect(&logfont));
    }
    /**
     * @brief �t�H���g��ݒ肷��
     * @param lp �ݒ肷��t�H���g�̃t�H���g�\���̂̃|�C���^
     */
    static void setFont(const LOGFONT* const lp) {
        setFont(CreateFontIndirect(lp));
    }
    /// @brief �ݒ肳�ꂽ�t�H���g�n���h����ԋp����
    static const HFONT font(void) { return c_hFont;};

    /// @brief �E�B���h�E�T�C�Y���������L����ԋp����
    static UINT adjust(){return c_adjust;};
    /**
     * @brief �E�B���h�E�T�C�Y���������L����ݒ肷��
     * @param pAdjust ���������̗L��
     */
    static void setadjust(const UINT pAdjust) {c_adjust = pAdjust;};

    /**
     * @brief �E�B���h�E�̑傫���̍ŏ��l��ݒ肷��
     * @param x ���̍ŏ��l(�g�̑傫���Ȃ�)
     * @param y ���̍ŏ��l(�^�C�g���o�[�A�g�̑傫���Ȃ�)
     */
    static void setadd(const int x, const int y) {
        c_addx = x;
        c_addy = y;
    }

    /// @brief �E�B���h�E�n���h����ԋp����
    HWND hwnd(void){return mhwnd;};
    /// @brief �E�B���h�E�n���h����ݒ肷��
    void sethwnd(const HWND hWnd){mhwnd = hWnd;};
    /// @brief �E�B���h�E�̃^�C�g����ԋp����
    const char* title(void){return mtitle.textbuf();};
    /// @brief �E�B���h�E�̃^�C�g����ݒ肷��
    void settitle(const char *str) {mtitle = str;};
    /// @brief �E�B���h�E�̕\��������C���X�^���X��ԋp����
    rectstr& body(void){return mbody;};

    /**
     * @brief �E�B���h�E�̕\�������C���X�^���X��ԋp����
     * @param index �����̃C���f�b�N�X
     */
    rectchar& body(const int index){return mbody[index];};
    /// @brief �E�B���h�E�\���ʒu��ԋp����
    RECT& rect(void){return mrect;};
    /// @brief �}�E�X�N���b�N���ꂽ�ʒu��ԋp����
    POINT& clickpoint(void){return mclickpoint;};
    /// @brief �}�E�X�N���b�N���ꂽ�ʒu��ۑ�����
    void setclickpoint(const POINTS& p) {
        mclickpoint.x = p.x;
        mclickpoint.y = p.y;
    };
    const POINT& mousepoint(void){return mmousepoint;};
    /// @brief �N���b�N�����ǂ�����ԋp����
    int clicked(void){return mclicked;};
    /// @brief �N���b�N�����ǂ�����ݒ肷��
    void setclicked(const int f){mclicked = f;};
    int show(void){return mshow;};
    void setshow(const int f){mshow = f;};
    /// @brief ���������s��
    void init(void) {
        mhwnd = NULL;
        mrect.top = 0;
        mrect.left = 0;
        mrect.bottom = 200;
        mrect.right = 50;
        memset(&(mclickpoint), 0, sizeof(mclickpoint));
        mclicked = 0;
    };
    /// @brief ������s��
    void free(void) {
        if (mhwnd != NULL) {
            ::DestroyWindow(mhwnd);
            mhwnd = NULL;
        }
    }
    headerWindow(void){init();};
    ~headerWindow(void){free();};
    /// @brief �ĕ`����s��
    void redraw(void) {
        ::InvalidateRect(mhwnd, NULL, TRUE);
        ::UpdateWindow(mhwnd);
    };
    /**
     * @brief �w��͈͂̍ĕ`����s��
     * @param rect �ĕ`��͈�
     */
    void redraw(RECT& rect) {
        ::InvalidateRect(mhwnd, &rect, FALSE);
        ::UpdateWindow(mhwnd);
    }

    /**
     * @brief �w�b�_�\���E�B���h�E�̃T�C�Y����
     *
     * �w�肳�ꂽ�E�B���h�E�̕\���ɕK�v�ȑ傫�����v�Z���āA
     * �E�B���h�E�̑傫���𒲐�����B
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
     * @brief �w�肳�ꂽ���[���̓��e�ŃE�B���h�E��\������
     * @param lpMailID ���[��ID
     *
     * �w�肳�ꂽ���[��ID�̃��[���w�b�_�����E�B���h�E�ɐݒ肵�ĕ\������B
     */
    void ShowHeader(LPCTSTR lpMailID);

    /**
     * @brief �E�B���h�E�̍ĕ\��
     *
     * ��\���ɂȂ��Ă���w�b�_�\���E�B���h�E���ĕ\������B
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
     * @brief �E�B���h�E�̔�\����
     *
     * �E�B���h�E���\���ɂ���B
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
