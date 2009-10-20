//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "../BeckyAPI.h"

#include "ShowText.h"
#include "stdio.h"	// 00/10/04 Append By Mitsumori
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TShowForm *ShowForm;

//GetTesxtExtentExPoint() to get size(Width) of string(memo)

extern HINSTANCE g_hInstance; // instance of this DLL(maybe(--;;;)
extern CBeckyAPI bka; // You can have only one instance in a project.

static TShowFormList *PtrShowFormList = NULL;
static HHOOK hhk_CBT;
static HHOOK hhk_GetMessage;

int CALLBACK CallBeckyWndProc(
    int nCode,	// hook code
    WPARAM wParam,	// current-process flag
    LPARAM lParam 	// address of structure with message data
   )
{
    unsigned long pid_before = 0;
    unsigned long pid_after = 0;

    if (((CWPSTRUCT *)lParam)->hwnd != PtrShowFormList->GetBeckyMain()) {
        return ::CallNextHookEx(hhk_GetMessage, nCode, wParam, lParam);
    }

    ::GetWindowThreadProcessId((HWND)(((CWPSTRUCT *)lParam)->lParam), &pid_before);
    ::GetWindowThreadProcessId(PtrShowFormList->GetBeckyMain(), &pid_after);

    switch (((CWPSTRUCT *)lParam)->message & 0xFFFF) {
    case WM_ACTIVATE:
        if (pid_before != pid_after && (BOOL)HIWORD(((CWPSTRUCT *)lParam)->wParam) == false) {
            switch (LOWORD(((CWPSTRUCT *)lParam)->wParam)) {
            case WA_ACTIVE:
            case WA_CLICKACTIVE:
                PtrShowFormList->Show();
                break;
            case WA_INACTIVE:
                if (PtrShowFormList->IsMyWindowsHandle((HWND)(((CWPSTRUCT *)lParam)->lParam)) == false) {
                    PtrShowFormList->Hide();
                }
                break;
            default:
               break;
            }
        }
        break;
    default:
        break;
    }
    return ::CallNextHookEx(hhk_GetMessage, nCode, wParam, lParam);
}

int CALLBACK CallBeckyCBTProc(
    int nCode,	// hook code
    WPARAM wParam,	// current-process flag
    LPARAM lParam 	// address of structure with message data
   )
{
    if (nCode == HCBT_MINMAX && (HWND)wParam == PtrShowFormList->GetBeckyMain()) {
        switch (lParam & 0xffff) {
        case SW_MAXIMIZE:
        case SW_RESTORE:
        case SW_SHOWDEFAULT:
        case SW_SHOWNORMAL:
            // Show Windows
            PtrShowFormList->Show();
            break;
        case SW_HIDE:
        case SW_MINIMIZE:
        case SW_SHOWMINIMIZED:
        case SW_SHOWMINNOACTIVE	:
            // Hide Windows
            PtrShowFormList->Hide();
            break;
        default:
            break;
        }
    }

    return CallNextHookEx(hhk_CBT, nCode, wParam, lParam);
}

//---------------------------------------------------------------------------
__fastcall TShowFormList::TShowFormList(const char *szIni)
{

    // initialization of member-variables;
    List = Last = NULL;
    AlwaysOnTop = false;
    HideIfNoHeader = false;
    HideTaskBar = false;
    volatile HWND Handle = NULL;

    // Dummy Window
    Dummy = new TDummyForm(NULL);
    //Dummy->CreateHandle();
    Handle = Dummy->Handle;
    Dummy->SetWindowStatus();


    DebugNum = 0;

    Font = new TFont;

    // Reading .ini file
    IniFile = new TIniFile(AnsiString(szIni));

    Font->Name = IniFile->ReadString("BkShHdr", "FontName", "System");
    Font->Size = IniFile->ReadInteger("BkShHdr", "FontSize", 14);

    NewList();

    PtrShowFormList = this;

    hhk_CBT = ::SetWindowsHookEx(WH_CBT,
                             (int (_stdcall *)())CallBeckyCBTProc,
                             NULL,
                             ::GetCurrentThreadId());
    hhk_GetMessage = ::SetWindowsHookEx(WH_CALLWNDPROC,
                             (int (_stdcall *)())CallBeckyWndProc,
                             NULL,
                             ::GetCurrentThreadId());
}

//---------------------------------------------------------------------------
void __fastcall TShowFormList::NewList()
{
    int i;
    TShowForm *ptr;
    TStringList *HeaderStr;

    if (bka.GetWindowHandles(&hMain, &hTree, &hList, &hView) == FALSE) {
        hMain = NULL;
    }

    HeaderStr = new TStringList;

    // get header list
    IniFile->ReadSections(HeaderStr);

    if (HeaderStr->Count == 0) {
	     HeaderStr->Add("X-Mailer");
    }

    AlwaysOnTop = IniFile->ReadBool("BkShHdr", "AlwaysOnTop", false);
    HideIfNoHeader = IniFile->ReadBool("BkShHdr", "HideIfNoHeader", true);
    HideTaskBar = IniFile->ReadBool("BkShHdr", "HideTaskBar", true);

    listcount = HeaderStr->Count;
    for (i = 0; i < listcount; i++) {
        if (IniFile->ReadBool(HeaderStr->Strings[i], "Show", true) == true) {
            ptr = new TShowForm(Dummy, IniFile, (HeaderStr->Strings[i]).c_str());
            ptr->Initialize();
            if (List == NULL) {
                List = Last = ptr;
            } else {
                Last->Next = ptr;
                Last = ptr;
            }
            ptr->SetFont(*Font);
            if (AlwaysOnTop == true) {
                // 常に最前列に表示
                ::SetWindowPos(ptr->Handle,	// handle of window
                               HWND_TOPMOST,	// placement-order handle
                               0,	// horizontal position
                               0,	// vertical position
                               0,	// width
                               0,	// height
                               SWP_NOMOVE + SWP_NOSIZE	 	// window-positioning flags
                              );
            } else {
                // 前列に移動するだけ(TOPMOSTを解除)
                ::SetWindowPos(ptr->Handle,	// handle of window
                               HWND_NOTOPMOST,	// placement-order handle
                               0,	// horizontal position
                               0,	// vertical position
                               0,	// width
                               0,	// height
                               SWP_NOMOVE + SWP_NOSIZE	 	// window-positioning flags
                              );
            }
            ptr->SetHideIfNoHeader(HideIfNoHeader);
            if (HideIfNoHeader) {
                ptr->Hide();
            }
            ptr->SetHideTaskBar(HideTaskBar);
        }
    }
    //SetParentWin(Dummy->Handle);

    delete HeaderStr;
}

//---------------------------------------------------------------------------
__fastcall TShowFormList::~TShowFormList()
{
    DeleteList();

    delete IniFile;
    delete Font;

    PtrShowFormList = NULL;

    if (hhk_CBT != NULL) {
        ::UnhookWindowsHookEx(hhk_CBT);
        hhk_CBT = NULL;
    }
    if (hhk_GetMessage != NULL) {
        ::UnhookWindowsHookEx(hhk_GetMessage);
        hhk_GetMessage = NULL;
    }

    delete Dummy;
}

//---------------------------------------------------------------------------
void __fastcall TShowFormList::DeleteList()
{
    TShowForm *ptr;

    IniFile->WriteString("BkShHdr", "FontName", Font->Name);
    IniFile->WriteInteger("BkShHdr", "FontSize", Font->Size);

    ptr = List;
    while (ptr != NULL) {
        ptr = ptr->Next;
        delete List;
        List = ptr;
    }
    Last = List = NULL;
}

//---------------------------------------------------------------------------
int __fastcall TShowFormList::ShowHeader(LPCSTR lpMailID)
{
    char buf[256];
    TShowForm *ptr;
    HWND preFocus;
    bool Showed = false;

    ptr = List;
    preFocus = ::GetFocus();
    while (ptr != NULL) {
        if (lpMailID != NULL && lpMailID[0] != 0) {
            bka.GetSpecifiedHeader(ptr->GetHeader(), buf, sizeof(buf));
        } else {
            buf[0] = '\0';
        }
        ptr->ShowText->Text = buf;
        if (HideIfNoHeader) {
            if (strlen(buf) == 0) {
                if (ptr->Visible == true) {
                    ptr->Hide();
                }
            } else if (ptr->Visible == false) {
                ptr->Show();
                Showed = true;
            }
        }
        ptr = ptr->Next;
    }
    if (Showed == true) {
        ::SetFocus(preFocus);
    }
    // Always return 0.
    return 0;
}

//---------------------------------------------------------------------------
void __fastcall TShowFormList::Show()
{
    TShowForm *ptr;

    ptr = List;
    while (ptr != NULL) {
        if (!isHideIfNoHeader() || ptr->Length() > 0) {
            ptr->Show();
        }
        ptr = ptr->Next;
    }
}

//---------------------------------------------------------------------------
void __fastcall TShowFormList::Hide()
{
    TShowForm *ptr;

    ptr = List;
    while (ptr != NULL) {
        ptr->Hide();
        ptr = ptr->Next;
    }
}

//---------------------------------------------------------------------------
bool __fastcall TShowFormList::IsMyWindowsHandle(HWND handle)
{
    TShowForm *ptr;

    ptr = List;
    while (ptr != NULL) {
        if (ptr->IsMyWindowsHandle(handle) != false) {
            return true;
        }
        ptr = ptr->Next;
    }
    return false;
}

//---------------------------------------------------------------------------
void __fastcall TShowFormList::SetParentWin(HWND hMain)
{
    TShowForm *ptr;

    ptr = List;
    while (ptr != NULL) {
        ptr->SetParentWin(hMain);
        ptr = ptr->Next;
    }
}

//---------------------------------------------------------------------------
void __fastcall TShowFormList::SetHideTaskBar(bool hide)
{
    TShowForm *ptr;

    ptr = List;
    while (ptr != NULL) {
        ptr->SetHideTaskBar(hide);
        ptr = ptr->Next;
    }
}

//---------------------------------------------------------------------------
void __fastcall TShowFormList::SetProperty(const AnsiString& pHead,
                                           const TCheckBoxState pState,
                                           const TCheckBoxState pHideIfNoHeader,
                                           const TCheckBoxState pHideTaskBar)
{
    char buf[512];
    int bufptr;
    int i, j;
    bool *ToShow = NULL;
    TStringList *HeaderStr;
    // int Debugint = 0;char Debugbuf[512];

    memset(buf, 0, sizeof(buf));
    bufptr = 0;
    HeaderStr = new TStringList;

    DeleteList(); // 一旦全Windowを破棄して

    // get header list
    IniFile->ReadSections(HeaderStr);

    if (pState == cbChecked) {
        AlwaysOnTop = true;
        IniFile->WriteBool("BkShHdr", "AlwaysOnTop", true);
    } else {
        AlwaysOnTop = false;
        IniFile->WriteBool("BkShHdr", "AlwaysOnTop", false);
    }

    if (pHideIfNoHeader == cbChecked) {
        HideIfNoHeader = true;
        IniFile->WriteBool("BkShHdr", "HideIfNoHeader", true);
    } else {
        HideIfNoHeader = false;
        IniFile->WriteBool("BkShHdr", "HideIfNoHeader", false);
    }
    if (pHideTaskBar == cbChecked) {
        HideTaskBar = true;
        IniFile->WriteBool("BkShHdr", "HideTaskBar", true);
    } else {
        HideTaskBar = false;
        IniFile->WriteBool("BkShHdr", "HideTaskBar", false);
    }
    IniFile->WriteString("BkShHdr", "FontName", Font->Name);
    IniFile->WriteInteger("BkShHdr", "FontSize", Font->Size);

    ToShow = new bool[HeaderStr->Count];
    for (j = 0; j < HeaderStr->Count; j++) {
        ToShow[j] = false;
    }
    for (i = 0; i < pHead.Length(); i++) {
        if (pHead.c_str()[i] != ',' && pHead.c_str()[i] != 0) { // コンマでも文字列の終端でもなければ
            if (!isspace(pHead.c_str()[i])) {  // 空白文字でもなければ
                if (bufptr < sizeof(buf)) {
                    buf[bufptr++] = pHead.c_str()[i];
                }
            }
        } else { // コンマか文字列の終端なら
            for (j = 0; j < HeaderStr->Count; j++) {
                if (stricmp(HeaderStr->Strings[j].c_str(), buf) == 0) {
                    ToShow[j] = true;
                    break;
                }
            }
            if (j == HeaderStr->Count) { // 既存ヘッダに存在しない→新規ヘッダ
                // 新規ヘッダ用のセクションを追加する。
                IniFile->WriteBool(buf, "Show", true);
            }
            memset(buf, 0, sizeof(buf));
            bufptr = 0;
        }
    }

    if (bufptr > 0) {
        for (j = 0; j < HeaderStr->Count; j++) {
             if (stricmp(HeaderStr->Strings[j].c_str(), buf) == 0) {
                ToShow[j] = true;
                break;
            }
        }
        if (j == HeaderStr->Count) { // 既存ヘッダに存在しない→新規ヘッダ
            // 新規ヘッダ用のセクションを追加する。
            IniFile->WriteBool(buf, "Show", true);
        }
    }

    // 文字列リストにないものを消去し、あるものを表示する。
    for (j = 0; j < HeaderStr->Count; j++) {
        if (ToShow[j] == true) {
            IniFile->WriteBool(HeaderStr->Strings[j], "Show", true);
        } else {
            IniFile->WriteBool(HeaderStr->Strings[j], "Show", false);
        }
    }

    NewList();    // 新たに作成する。

    ShowHeader(bka.GetCurrentMail());

    delete[] ToShow;
}

//---------------------------------------------------------------------------
AnsiString &TShowFormList::SetHeaderList(AnsiString &Head)
{
    int i;
    TStringList *HeaderStr;

    HeaderStr = new TStringList;

    // get header list
    IniFile->ReadSections(HeaderStr);

    for (i = 0; i < HeaderStr->Count; i++) {
        if (IniFile->ReadBool(HeaderStr->Strings[i], "Show", false) == true) {
            Head += HeaderStr->Strings[i] + ", ";
        }
    }

    delete HeaderStr;

    return Head;
}

//---------------------------------------------------------------------------
__fastcall TShowForm::TShowForm(TForm* Owner,
                                TIniFile *pIniFile,
                                const char *PrmHeader)
                     :TForm(Owner)
{
	Parent = Owner;

	SetHeader(PrmHeader);
	IniFile = pIniFile;

	Top    = IniFile->ReadInteger(Header, "Top",    Top);
	Height = IniFile->ReadInteger(Header, "Height", Height);
	Left   = IniFile->ReadInteger(Header, "Left",   Left);
	Width  = IniFile->ReadInteger(Header, "Width",  Width);
}

//---------------------------------------------------------------------------
__fastcall TShowForm::~TShowForm()
{
    IniFile->WriteInteger(Header, "Top",    Top );
    IniFile->WriteInteger(Header, "Height", Height );
    IniFile->WriteInteger(Header, "Left",   Left );
    IniFile->WriteInteger(Header, "Width",  Width );
    IniFile->WriteBool(Header, "Show", true);

    delete Header;
}

//---------------------------------------------------------------------------
void __fastcall TShowForm::SetHeader(const char *PrmHeader)
{
    int i;

    if (Header != NULL) {
    	delete Header;
    }
    Header = new char[strlen(PrmHeader) + 1];

    strcpy(Header, PrmHeader);
    Caption = Header;
    if (HideIfNoHeader == true) {
        i = 0;
        while (Header[i] != 0) {
            if (!isspace(Header[i])) {
                Show();
                break;
            }
            i++;
        }
        if (isspace(Header[i]) || Header[i]  == 0) {
            Hide();
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TShowForm::Initialize()
{
	ShowText->Text = "";
}

//---------------------------------------------------------------------------
HWND TShowForm::SetParentWin(HWND parenthandle)
{
    long lStyle;
    HWND Rtn;
    long rtnerrno;
    char buf[128];

    if (Handle == NULL) {
        return NULL;
    }

    // lStyle = ::GetWindowLong(Handle, GWL_STYLE);
    // lStyle &= ~(WS_POPUP);
    // lStyle |= WS_CHILD;
    // ::SetWindowLong(Handle, GWL_STYLE, lStyle);

    Rtn = ::SetParent(Handle, parenthandle);
    if (Rtn == NULL) {
        rtnerrno = ::GetLastError();

        sprintf(buf, "Parent=[0x%08X] Handle=[0x%08X] errno=%d", parenthandle, Handle, rtnerrno);
        MessageBox(Handle, buf, "SetParent", MB_OK);
        lStyle = ::GetWindowLong(Handle, GWL_STYLE);
        lStyle |= WS_VISIBLE;
        lStyle |= WS_POPUP;
        lStyle &= ~(WS_CHILD);
        ::SetWindowLong(Handle, GWL_STYLE, lStyle);
    } else {
        lStyle = ::GetWindowLong(Handle, GWL_STYLE);
        lStyle |= WS_VISIBLE;
        lStyle |= WS_CHILD;
        lStyle |= WS_SYSMENU;
        lStyle &= ~(WS_POPUP);
        lStyle &= ~(WS_CLIPSIBLINGS);
        ::SetWindowLong(Handle, GWL_STYLE, lStyle);
    }
	return Rtn;
}

//---------------------------------------------------------------------------
void __fastcall TShowForm::FormResize(TObject *Sender)
{
     ShowText->Height = ClientHeight;
     ShowText->Width  = ClientWidth;
}
//---------------------------------------------------------------------------
void __fastcall TShowForm::SetHideTaskBar(bool hide)
{
    long lStyle = ::GetWindowLong(Handle, GWL_EXSTYLE);
    if (hide) {
        lStyle |= WS_EX_TOOLWINDOW;
    } else {
        lStyle &= ~(WS_EX_TOOLWINDOW);
    }
    ::SetWindowLong(Handle, GWL_EXSTYLE, lStyle);
    if (lStyle &= WS_VISIBLE) {
        ::ShowWindow(Handle, SW_HIDE);
        ::ShowWindow(Handle, SW_SHOW);
    }

    long OWidth = Width;
    ::MoveWindow(Handle, Left, Top, Width + 1, Height, FALSE);
    ::MoveWindow(Handle, Left, Top, OWidth, Height, TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TShowForm::DblClick(TObject *Sender)
{
    HWND hMain;
    HWND hTree;
    HWND hList;
    HWND hView;

    if (bka.GetWindowHandles(&hMain, &hTree, &hList, &hView) == FALSE) {
        hMain = NULL;
    }

    ShowText->SelectAll();
    PostMessage(hMain, WM_COMMAND, 32972, 0);

}
//---------------------------------------------------------------------------

