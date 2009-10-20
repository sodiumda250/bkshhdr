/*
 * $Id: hdrwin.cpp,v 1.76 2005/05/25 03:55:21 woods Exp $
 */

static char id[] = "$Id: hdrwin.cpp,v 1.76 2005/05/25 03:55:21 woods Exp $";

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include "resource.h"
#include "bkshhdr.h"
#include "charbuf.h"
#include "hdrlist.h"
#include "hdrwin.h"

// �\�[�X���O���[�o��
/// �w�b�_�\���E�B���h�E��Window Class
static TCHAR szWindowClass[100] = "";

int headerWindow::c_addx = 0;
int headerWindow::c_addy = 0;
HFONT headerWindow::c_hFont = 0;
UINT headerWindow::c_adjust = 0;

/// �w�b�_�\���p�̃E�B���h�E��`�\���̂̔z��
static headerWindow *window;
/// �E�B���h�E��`�\���̂̌�
static int windowNum = 0;


// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̑O�錾:
extern "C" {
static ATOM MyRegisterClass(HINSTANCE hInstance);
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
}

static void Redraw(void);

/**
 * @brief �E�B���h�E��`�\���̃C���X�^���X�擾
 * @param hwnd �C���X�^���X���擾����E�B���h�E�n���h��
 * @return �E�B���h�E��`�\���̃C���X�^���X
 *
 * �E�B���h�E�n���h������E�B���h�E��`�\���̂̃C���X�^���X���擾����B
 */
headerWindow& headerWindow::getWindow(const HWND hwnd)
{
    int i;

    for (i = 0; i < windowNum; i++) {
        if (hwnd == window[i].hwnd()) {
            return window[i];
        }
    }
    return window[i];
}

/**
 * @brief �w�b�_�\���E�B���h�E���̏�����
 * @param szIniFile ini�t�@�C���̃p�X��
 * @retval TRUE ����������
 * @retval FALSE ���������s
 *
 * �w�b�_�\���E�B���h�E��������������B
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

    // ini�t�@�C���̃Z�N�V���������\���w�b�_���Ȃ̂ŃZ�N�V��������S�Ď擾����
    max = GetPrivateProfileSectionNames(buf, sizeof(buf), szIni);

    if (strlen(buf) == 0) {
        memset(buf, 0, sizeof(buf));
        strcpy(buf, "X-Mailer");
    }

    // �Z�N�V������(�w�b�_��)���Ƃ̏���
    p = buf, pp = buf;
    while ((*p != 0) && (*pp != 0) && (p < buf + max) && (pp < buf + max)) {
        int show;

        while (*pp != 0) {
            pp++;
        }
        // "Show"�L�[��0�Ȃ��\��
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
        window[i].body() = plist[i]/* �{���̓w�b�_�̓��e */;
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
 * @brief �w�b�_�\���E�B���h�E���̃f�X�g���N�^
 * @retval TRUE �j�󐬌�
 * @retval FALSE �j�󎸔s
 *
 * �w�b�_�\���E�B���h�E����j������B
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
 * @brief �w�b�_���̕\��
 * @param lpMailID : ���[��ID
 * @return �\���E�B���h�E�̌�
 *
 * �w�肳�ꂽ���[���̃w�b�_��񂩂�w�b�_�\���E�B���h�E��\������B
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
 * @brief �w�b�_���̕\��
 * @param lpMailID : ���[��ID
 *
 * �w�肳�ꂽ���[���̃w�b�_��񂩂�w�b�_�\���E�B���h�E��\������B
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

    // ������̕��ƍ������v�Z���܂��B
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
 * @brief �w�b�_�\���E�B���h�E�̕\��
 *
 * ��A�N�e�B�u���Ŕ�\���ɂȂ��Ă���w�b�_�\���E�B���h�E��\������B
 */
void ShowWindow(void)
{
    int i;
    WINDOWPLACEMENT wplace;
    HWND hMain, hTree, hList, hView;

    bka.GetWindowHandles(&hMain, &hTree, &hList, &hView);

    wplace.length = sizeof(WINDOWPLACEMENT);
    wplace.showCmd = SW_SHOWNORMAL;
    ::GetWindowPlacement(hMain, &wplace);
    if (wplace.showCmd != SW_SHOWMAXIMIZED) {
        for (i = windowNum - 1; i >= 0; i--) {
            window[i].ShowWindow();
        }
    } else {
        for (i = windowNum - 1; i >= 0; i--) {
            window[i].HideWindow();
        }
    }
}

/**
 * @brief �w�b�_�\���E�B���h�E�̔�\����
 *
 * ���C���E�B���h�E�̔�A�N�e�B�u���ɔ����ăw�b�_�\���E�B���h�E���\���ɂ���B
 */
void HideWindow(void)
{
    int i;

    for (i = windowNum - 1; i >= 0; i--) {
        window[i].HideWindow();
    }
}

/**
 * @brief �w�b�_�\���E�B���h�E�N���X�̓o�^
 * @param hInstance : Window Procedure�̂��郂�W���[���̃C���X�^���X
 * @return �E�B���h�E�N���X�����ʂ���A�g��
 *
 * �w�b�_�\���E�B���h�E�̃E�B���h�E�N���X��o�^����B
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
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // �f�t�H���g����
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
 * @brief �w�b�_�\���p�E�B���h�E�̍쐬
 * @param hInstance : Window Procedure�̂��郂�W���[���̃C���X�^���X
 * @param hParentWnd : �e�E�B���h�E
 * @retval TRUE ����
 * @retval FALSE ���s
 *
 * �w�b�_�\���E�B���h�E���쐬����B
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
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // �f�t�H���g����
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
 * @brief �w�b�_�\���E�B���h�E���b�Z�[�W�n���h��
 * @param hWnd : �E�B���h�E�n���h��
 * @param message : ���b�Z�[�W
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E�̃E�B���h�E���b�Z�[�W����������B
 */
static LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    LRESULT ret = 0;

    headerWindow& hw = headerWindow::getWindow(hWnd);

    switch (message) {
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // ���j���[�I���̉��:
        switch (wmId) {
        case IDM_COPY:
        case IDM_CUT:
            hw.SetClipBoardText();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_CHAR:
        ret = hw.OnChar(wParam, lParam);
        break;
    case WM_PAINT:
        ret = hw.OnPaint(wParam, lParam);
        break;
    case WM_SIZE:
    case WM_MOVE:
        // �E�B���h�E�̑傫���A�ʒu���Z�[�u����B
        ret = hw.OnMove(wParam, lParam);
        break;
    case WM_ACTIVATE:
        ret = hw.OnActivate(wParam, lParam);
        break;
    case WM_DESTROY:
        //PostQuitMessage( 0 );
        break;
    case WM_LBUTTONDOWN:
        ret = hw.OnLButtonDown(wParam, lParam);
        break;
    case WM_LBUTTONUP:
        ret = hw.OnLButtonUp(wParam, lParam);
        break;
    case WM_MOUSEMOVE:
        ret = hw.OnMouseMove(wParam, lParam);
        break;
    case WM_LBUTTONDBLCLK:
        ret = hw.OnLButtonDblClk(wParam, lParam);
        break;
    case WM_NCLBUTTONDBLCLK:
        ret = hw.OnNCLButtonDblClk(wParam, lParam);
        break;
    case WM_KILLFOCUS:
        ret = hw.OnKillFocus(wParam, lParam);
        break;
    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
    return ret;
}

/**
 * @brief WM_CHAR�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^(character code)
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_CHAR���b�Z�[�W����������B
 * Ctrl-C�����͂��ꂽ��w�肳�ꂽ��������N���b�v�{�[�h�ɃR�s�[����B
 */
LRESULT headerWindow::OnChar(WPARAM wParam, LPARAM lParam)
{
    if ((((TCHAR) wParam) == 'c') || (((TCHAR) wParam) == 'C')) {
        LRESULT ret = SendMessage(hwnd(), WM_GETHOTKEY, (WPARAM)0, (LPARAM)0); 
        if (ret & HOTKEYF_CONTROL) {
            this->SetClipBoardText();
        }
    } else if (((TCHAR) wParam) == 3/* ctrl-C */) {
        this->SetClipBoardText();
    }
    return 0;
}

/**
 * @brief WM_MOVE/WM_SIZE�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_MOVE/WM_SIZE���b�Z�[�W����������B
 * �ړ��^�T�C�Y�ύX��̃E�B���h�E�ʒu�͈͂��L�^����B
 */
LRESULT headerWindow::OnMove(WPARAM wParam, LPARAM lParam)
{
    ::GetWindowRect(hwnd(), &(rect()));

    return 0;
}

/**
 * @brief WM_ACTIVATE�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_ACTIVATE���b�Z�[�W����������B
 */
LRESULT headerWindow::OnActivate(WPARAM wParam, LPARAM lParam)
{
    unsigned long pid_before = 0;
    unsigned long pid_after = 0;

    ::GetWindowThreadProcessId((HWND)(lParam), &pid_before);
    ::GetWindowThreadProcessId(hwnd(), &pid_after);

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
 * @brief WM_PAINT�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^(the device context to draw in)
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_PAINT���b�Z�[�W���������A
 * �w�b�_�\���E�B���h�E��`�悷��B
 */
LRESULT headerWindow::OnPaint(WPARAM wParam, LPARAM lParam)
{
    int j;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT urect;

    COLORREF ctextn, cbackn;

    hdc = ::BeginPaint(hwnd(), &ps);
    ctextn = ::GetTextColor(hdc);
    cbackn = ::GetBkColor(hdc);
    urect = ps.rcPaint;
    ::SelectObject(hdc, headerWindow::font());
    for (j = 0; j < body().length(); j++) {
        if (body(j).in_rect(urect)) {
            if (body(j).reverse() != 0) {
                ::SetTextColor(hdc, cbackn);
                ::SetBkColor(hdc, ctextn);
            } else {
                ::SetTextColor(hdc, ctextn);
                ::SetBkColor(hdc, cbackn);
            }
            ::DrawText(hdc, body(j).str(),
                     body(j).length(),
                     &(body(j).rect()), DT_LEFT);
        }
    }
    ::EndPaint(hwnd(), &ps);

    return 0;
}

/**
 * @brief WM_LBUTTONDOWN�C�x���g����
 * @param hWnd : �E�B���h�E�n���h��
 * @param message : ���b�Z�[�W(WM_LBUTTONDOWN)
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^(mouse clicked point)
 *
 * �w�b�_�\���E�B���h�E��WM_LBUTTONDOWN�C�x���g����������B
 * �}�E�X�N���b�N�t���O���Z�b�g���A�N���b�N�ʒu���L�^���A�I�𕶎�����N���A����B
 */
LRESULT headerWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    int j;
    RECT rect;

    ::SetActiveWindow(hwnd());
    setclicked(1);
    setclickpoint(MAKEPOINTS(lParam));

    rect.top = rect.bottom = MAKEPOINTS(lParam).y;
    rect.left = rect.right = MAKEPOINTS(lParam).x;
    for (j = 0; j < body().length(); j++) {
        if (body(j).reverse() != 0) {
            body(j).setreverse(0);
            addrect(rect, body(j).rect());
        }
    }
    redraw(rect);
    ::SetCapture(hwnd());

    return 0;
}

/**
 * @brief WM_LBUTTONUP�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^(mouse clicked point)
 *
 * �w�b�_�\���E�B���h�E��WM_LBUTTONUP�C�x���g����������B
 * �N���b�N�t���O�����Z�b�g����B
 */
LRESULT headerWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    ::ReleaseCapture();
    setclicked(0);

    return 0;
}

/**
 * @brief WM_MOUSEMOVE�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^(point of mouse cursor)
 *
 * �w�b�_�\���E�B���h�E��WM_MOUSEMOVE�C�x���g���������A
 * �}�E�X���N���b�N����Ă���Ƃ��͕�����I���������s���B
 */
LRESULT headerWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    int j;
    RECT mrect;
    RECT crect;

    if (clicked() && DragDetect(hwnd(), clickpoint())) {
        // �h���b�O������
        mrect.left = (MAKEPOINTS(lParam).x < clickpoint().x) ?
                         MAKEPOINTS(lParam).x : clickpoint().x;
        mrect.right = (MAKEPOINTS(lParam).x > clickpoint().x) ?
                         MAKEPOINTS(lParam).x : clickpoint().x;
        mrect.top = (MAKEPOINTS(lParam).y < clickpoint().y) ?
                         MAKEPOINTS(lParam).y : clickpoint().y;
        mrect.bottom = (MAKEPOINTS(lParam).y > clickpoint().y) ?
                         MAKEPOINTS(lParam).y : clickpoint().y;
        crect = mrect;

        for (j = 0; j < body().length(); j++) {
            if (body(j).in_rect(mrect)) {
                if (body(j).reverse() == 0) {
                    body(j).setreverse(1);
                    addrect(crect, body(j).rect());
                }
            } else {
                if (body(j).reverse() != 0) {
                    body(j).setreverse(0);
                    addrect(crect, body(j).rect());
                }
            }
        }
        redraw(crect);
    }
    return 0;
}

/**
 * @brief WM_LBUTTONDBLCLK�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_LBUTTONDBLCLK�C�x���g���������A
 * �S������I����Ԃɂ���B
 */
LRESULT headerWindow::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
    int j;

    for (j = 0; j < body().length(); j++) {
        body(j).setreverse(1);
    }
    redraw();

    return 0;
}

/**
 * @brief WM_NCLBUTTONDBLCLK�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_NCLBUTTONDBLCLK�C�x���g���������A
 * �E�B���h�E�̑傫���𒲐�����B
 */
LRESULT headerWindow::OnNCLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
    AdjustWindow();

    return 0;
}

/**
 * @brief WM_KILLFOCUS�C�x���g����
 * @param wParam : ���b�Z�[�W�̃p�����[�^
 * @param lParam : ���b�Z�[�W�̃p�����[�^
 *
 * �w�b�_�\���E�B���h�E��WM_KILLFOCUS�C�x���g���������A
 * �����̑I����Ԃ���������B
 */
LRESULT headerWindow::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
    int j;

    ::ReleaseCapture();
    for (j = 0; j < body().length(); j++) {
        body(j).setreverse(0);
    }
    redraw();

    return 0;
}

/**
 * @brief �N���b�v�{�[�h�փR�s�[
 * @param hwnd �R�s�[�Ώۂ̃E�B���h�E�̃n���h��
 *
 * �w�b�_�\���E�B���h�E�̃e�L�X�g���N���b�v�{�[�h�ɃR�s�[����B
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
 * @brief �t�H���g��񃍁[�h
 * @param lf �t�H���g�������[�h���邽�߂�LOGFONT�\���̂������|�C���^
 * @param szIni ini�t�@�C���̃p�X��
 * @retval TRUE �t�H���g��񃍁[�h����
 * @retval FALSE �t�H���g��񃍁[�h���s
 *
 * �t�H���g����ini�t�@�C�����烍�[�h����B
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
 * @brief �t�H���g���Z�[�u
 * @param lf �t�H���g���ɃZ�[�u����LOGFONT�\���̂������|�C���^
 * @param szIni ini�t�@�C���̃p�X��
 * @retval TRUE �t�H���g���Z�[�u����
 * @retval FALSE �t�H���g���Z�[�u���s
 *
 * �t�H���g����ini�t�@�C���ɃZ�[�u����B
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
 * @brief �w�b�_�\���E�B���h�E�̍ĕ`��
 *
 * �w�b�_�\���E�B���h�E��S�čĕ`�悷��B
 */
static void Redraw(void)
{
    int i;

    for (i = 0; i < windowNum; i++) {
        window[i].redraw();
    }
}

/**
 * @brief �w�b�_�\���E�B���h�E�ʒu�̃Z�[�u
 * @param szIni ini�t�@�C���̃p�X��
 *
 * �w�b�_�\���E�B���h�E�̈ʒu��ini�t�@�C���ɃZ�[�u����B
 */
void SaveWindowPos(const char *szIni)
{
    int i;
    char buf[128];

    for (i = 0; i < windowNum; i++) {
        // ������E�B���h�E�͕K����\���łȂ�
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
 * @brief �E�B���h�E����������񃍁[�h
 * @param szIni ini�t�@�C���̃p�X��
 *
 * �E�B���h�E�̑傫���������������邩�ǂ����̏������[�h����B
 */ 
UINT LoadAdjust(const char *szIni)
{
    UINT ret = ::GetPrivateProfileInt("BkShHdr", "AdjustWindow", 0, szIni);

    headerWindow::setadjust(ret);
    return ret;
}

/**
 * @brief �E�B���h�E�����������Z�[�u
 * @param pAdjust ���������t���O(0:�����������Ȃ��A����ȊO:������������)
 * @param szIni ini�t�@�C���̃p�X��
 *
 * �E�B���h�E�̑傫���������������邩�ǂ����̏���ini�t�@�C���ɃZ�[�u����B
 */ 
void SaveAdjust(UINT pAdjust, const char *szIni)
{
    char buf[32];

    headerWindow::setadjust(pAdjust);
    sprintf(buf, "%d", pAdjust);
    ::WritePrivateProfileString("BkShHdr", "AdjustWindow", buf, szIni);
}

/**
 * @brief RECT�͈̔͂̍���
 * @param dst �����Ώۂ�RECT�\���̃|�C���^
 * @param src ��������RECT�\���̃|�C���^
 * @return ��������RECT(dst�Ɠ�������)
 *
 * RECT�͈̔͂��������A�Q��RECT���Ƃ��Ɋ܂܂��RECT���쐬��dst�ɐݒ肷��B
 */
RECT& addrect(RECT& dst, const RECT& src)
{
    dst.top = (dst.top > src.top) ? src.top : dst.top;
    dst.bottom = (dst.bottom < src.bottom) ? src.bottom : dst.bottom;
    dst.left = (dst.left > src.left) ? src.left : dst.left;
    dst.right = (dst.right < src.right) ? src.right : dst.right;

    return dst;
}
