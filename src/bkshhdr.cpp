//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
//   DLL メモリマネージャについて:
//
//   パラメータや関数の戻り値として String オブジェクトまたはネストした String
//   を含む構造体/クラスを渡す関数を DLL がエクスポートする場合, DLL プロジェク
//   トと DLL を使用するすべての EXE プロジェクトの両方にライブラリ BCBMM.LIB
//   を追加しなければなりません。これにより, DLL と呼び出し側の EXE の DLL が
//   メモリマネージャとして BCBMM.DLL を使うように変更されます。この場合,
//   ファイル BCBMM.DLL を DLL と一緒に配布する必要があります。
//
//   BCBMM.DLL の使用を避けるには, "char *" または ShortString パラメータを使って
//   文字列のやり取りを行ってください。
//---------------------------------------------------------------------------
USEUNIT("..\BkCommon.cpp");
USEUNIT("..\BeckyAPI.cpp");
USEFILE("..\BkCommon.h");
USEFORM("ShowText.cpp", ShowForm);
USEFORM("TAboutForm.cpp", AboutForm);
USEDEF("..\Template.def");
USEFILE("ShowText.h");
USEFILE("TAboutForm.h");
USERES("BKSHHDR.res");
USEFORM("TDummyForm.cpp", DummyForm);
//---------------------------------------------------------------------------
#include <windows.h>
#include <vector>
#include "../BeckyAPI.h"
#include "../BkCommon.h"
#include "TAboutForm.h"
#include "ShowText.h"

CBeckyAPI bka; // You can have only one instance in a project.

HINSTANCE g_hInstance = NULL;

char szIni[_MAX_PATH+2]; // Ini file to save your plugin settings.

const char PlugInName[] = "Show-Header Plug-In";
const char Vender[] = "woods";
const char Version[] = "0.4.0";
const char Description[] = "任意のヘッダを別ウィンドウに表示する";

TShowFormList *ShowFormList = NULL;

//////////////////////////////////////////////////////////////////////////////
// User Define

/////////////////////////////////////////////////////////////////////////////
// DLL entry point
int WINAPI DllEntryPoint( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    g_hInstance = (HINSTANCE)hModule;
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            {
                if (!bka.InitAPI()) {
                    return FALSE;
                }
                GetModuleFileName((HINSTANCE)hModule, szIni, _MAX_PATH);
                LPSTR lpExt = strrchr(szIni, '.');
                if (lpExt) {
                    strcpy(lpExt, ".ini");
                } else {
                    // just in case
                    strcat(szIni, ".ini");
                }
            }
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            delete ShowFormList;
            ShowFormList = NULL;	// 00/10/04 Append by Mitsumori
            break;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks from Becky!

#ifdef __cplusplus
extern "C"{
#endif

////////////////////////////////////////////////////////////////////////
// Called when the program is started and the main window is created.
int WINAPI BKC_OnStart()
{
    // Always return 0.
    ShowFormList = new TShowFormList(szIni);
    //ShowFormList->SetParentWin();
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when the main window is closing.
int WINAPI BKC_OnExit()
{
    // Return -1 if you don't want to quit.

    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when menu is initialized.
// If you want to add your plug-in's menu to Becky!, implement this
// callback.
int WINAPI BKC_OnMenuInit(HWND hWnd, HMENU hMenu, int nType)
{
    /*
    hWnd: A window handle of the window which owns the menu
    hMenu: A menu handle
    nType: A type of the menu. see below.
    */
    switch (nType) {
    case BKC_MENU_MAIN: // Menu bar of the main window
        {
            /* Adding menu items
            HMENU hSubMenu = GetSubMenu(hMenu, 4);
            // Call API function to register your command.
            // Define CmdProc as "void WINAPI CmdProc(HWND, LPARAM)"
            UINT nID = bka.RegisterCommand("Information
                       about this Command", nType, CmdProc);
            AppendMenu(hSubMenu, MF_STRING, nID, "&Menu item");
            */

            /* If needed, you can register the command UI update callback.
            // Define CmdUIProc as "UINT WINAPI CmdUIProc(HWND, LPARAM)"
            bka.RegisterUICallback(nID, CmdUIProc);
            */

        }
        break;
    case BKC_MENU_LISTVIEW: // Context menu of ListView window
        break;
    case BKC_MENU_TREEVIEW: // Context menu of TreeView window
        break;
    case BKC_MENU_MSGVIEW: // Context menu of message view Window
        break;
    case BKC_MENU_MSGEDIT: // Context menu of message view Window when it's editable
        break;
    case BKC_MENU_COMPOSE: // Menu bar of the composing window
        break;
    case BKC_MENU_COMPEDIT: // Context menu of the editor of the composing window.
        break;
    case BKC_MENU_COMPREF: // Context menu of the reference view of the composing window
        break;
    default:
        break;
    }
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a folder is opened.
int WINAPI BKC_OnOpenFolder(LPCSTR lpFolderID)
{
    /*
    lpFolderID represents an ID of the folder which is opening.
    The folder ID is a part of an actual folder directory path.
    e.g. "1234abcd\!!!!Inbox\".
    You can pass those folder IDs to some API functions.
    */
    // Always return 0.

    if (ShowFormList != NULL) {
        ShowFormList->ShowHeader((LPCSTR)NULL);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a mail is selected.
int WINAPI BKC_OnOpenMail(LPCSTR lpMailID)
{
    /*
     lpMailID represents an ID of the mail which is opening.
     The message ID is a folder ID + "?" + an unique hexadecimal identifier.
     e.g. "1234abcd\!!!!Inbox\?5678cdef".
     You can pass those IDs to some API functions.
    */
    // Always return 0.
    if (ShowFormList != NULL) {
        ShowFormList->ShowHeader(lpMailID);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called every minute.
int WINAPI BKC_OnEveryMinute()
{
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a compose windows is opened.
int WINAPI BKC_OnOpenCompose(HWND hWnd, int nMode)
{
    /*
     hWnd: A window handle of the composing window.
     nMode: The mode of the composing window.
        COMPOSE_MODE_COMPOSE1   Compose new message.
        COMPOSE_MODE_COMPOSE2   Compose to replying address.
        COMPOSE_MODE_COMPOSE3   Compose to selected addresses.
        COMPOSE_MODE_TEMPLATE   Edit/Create a template.
        COMPOSE_MODE_REPLY1 Reply
        COMPOSE_MODE_REPLY2 Reply to All
        COMPOSE_MODE_REPLY3 Reply to selected addresses.
        COMPOSE_MODE_FORWARD1   Forward.
        COMPOSE_MODE_FORWARD2   Redirect.
        COMPOSE_MODE_FORWARD3   Forward as attachments.
    */
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when the composing message is saved.
int WINAPI BKC_OnOutgoing(HWND hWnd, int nMode)
{
    /*
    hWnd: A window handle of the composing window.
    nMode: How it will be processed:
        0: Save to outbox.
        1: Save to draft box.
        2: Save as a reminder.
    */
    // Return -1 if you want to cancel the operation.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a key is pressed.
int WINAPI BKC_OnKeyDispatch(HWND hWnd, int nKey, int nShift)
{
    /*
    hWnd: A window handle of the target window in which the key will be processed.
        or NULL for the main window.
    nKey: Windows virtual key code defined in <windows.h>
    nShift: Shift state
        0x40 = Shift+
        0x20 = Ctrl+
        0x60 = Shift+Ctrl+
        0xfe = Alt+
    */
    // Return TRUE if you want to suppress subsequent command associated to this key.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a message is retrieved and saved to a folder
int WINAPI BKC_OnRetrieve(LPCSTR lpMessage, LPCSTR lpMailID)
{
    /*
    lpMessage: A complete message source.
    lpMailID: Mail ID of the new mail.
    */
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when a message is about to be sent
int WINAPI BKC_OnSend(LPCSTR lpMessage)
{
    /*
    lpMessage: A complete mail source.
*/
    // Return BKC_ONSEND_PROCESSED, if you have processed this message
    // and don't need Becky! to send it.
    // Becky! will move this message to Sent box when the sending
    // operation is done.
    // CAUTION: You are responsible for the destination of this
    // message if you return BKC_ONSEND_PROCESSED.

    // Return BKC_ONSEND_ERROR, if you want to cancel the sending operation.
    // You are responsible for displaying an error message.

    // Return 0 to proceed the sending operation.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when all messages are retrieved in a session.
int WINAPI BKC_OnFinishRetrieve(int nNumber)
{
    /*
    nNumber: Number of retrieved messages.
    */
    // Always return 0.
    return 0;
}


////////////////////////////////////////////////////////////////////////
// Called when plug-in setup is needed.
int WINAPI BKC_OnPlugInSetup(HWND hWnd)
{
    TModalResult ret;
    // Return nonzero if you have processed.
    // return 1;

    if (ShowFormList == NULL) {
        return 0;
    }

    AnsiString *Head = new AnsiString;
    AboutForm = new TAboutForm(NULL);

    AboutForm->ProgramName->Caption = PlugInName;

    char *buf = new char[strlen("Version ")+ strlen(Version)];
    strcpy(buf, "Version ");
    strcat(buf, Version);

    AboutForm->Version->Caption = buf;

    delete[] buf;

    if (ShowFormList->ShowAlwaysOnTop() == true) {
        AboutForm->isAlwaysOnTop->State = cbChecked;
    } else {
        AboutForm->isAlwaysOnTop->State = cbUnchecked;
    }

    if (ShowFormList->isHideIfNoHeader() == true) {
        AboutForm->HideIfNoHeader->State = cbChecked;
    } else {
        AboutForm->HideIfNoHeader->State = cbUnchecked;
    }

    if (ShowFormList->isHideTaskBar() == true) {
        AboutForm->HideTaskBar->State = cbChecked;
    } else {
        AboutForm->HideTaskBar->State = cbUnchecked;
    }

    AboutForm->HeaderList->Text = ShowFormList->SetHeaderList(*Head);

    AboutForm->FontDialog->Font = ShowFormList->GetFont();

    ret = AboutForm->ShowModal();
    if (ret == mrOk) {
        ShowFormList->SetFont(*(AboutForm->FontDialog->Font));

        ShowFormList->SetProperty(AboutForm->HeaderList->Text,
                                  AboutForm->isAlwaysOnTop->State,
                                  AboutForm->HideIfNoHeader->State,
                                  AboutForm->HideTaskBar->State);
        ShowFormList->Show();
    }

    delete Head;
    delete AboutForm;
    return 1;
}


////////////////////////////////////////////////////////////////////////
// Called when plug-in information is being retrieved.
typedef struct tagBKPLUGININFO
{
	char szPlugInName[80]; // Name of the plug-in
	char szVendor[80]; // Name of the vendor
	char szVersion[80]; // Version string
	char szDescription[256]; // Short description about this plugin
} BKPLUGININFO, *LPBKPLUGININFO;

int WINAPI BKC_OnPlugInInfo(LPBKPLUGININFO lpPlugInInfo)
{
/* You MUST specify at least szPlugInName and szVendor.
   otherwise Becky! will silently ignore your plug-in.
strcpy(lpPlugInInfo->szPlugInName, "Becky! PGP plug-in");
strcpy(lpPlugInInfo->szVendor, "RimArts, Inc.");
strcpy(lpPlugInInfo->szVersion, "1.0");
strcpy(lpPlugInInfo->szDescription, "Enables Becky! to use PGP.");
*/
    strcpy(lpPlugInInfo->szPlugInName, PlugInName);
    strcpy(lpPlugInInfo->szVendor, Vender);
    strcpy(lpPlugInInfo->szVersion, Version);
    strcpy(lpPlugInInfo->szDescription, Description);
    // Always return 0.
    return 0;
}

////////////////////////////////////////////////////////////////////////
// Called when drag and drop operation occurs.
int WINAPI BKC_OnDragDrop(LPCSTR lpTgt, LPCSTR lpSrc, int nCount,
                          int dropEffect)
{
   /*
    lpTgt:  A folder ID of the target folder.
            You can assume it is a root mailbox, if the string
            contains only one '\' character.
    lpSrc:  Either a folder ID or mail IDs. Multiple mail IDs are
            separated by '\n' (0x0a).
            You can assume it is a folder ID, if the string
            doesn't contain '?' character.
    nCount  Number of items to be dropped.
            It can be more than one, if you drop mail items.
    dropEffect: Type of drag and drop operation
            1: Copy
            2: Move
            4: Link (Used for filtering setup in Becky!)
    */
    // If you want to cancel the default drag and drop action,
    // return -1;
    // Do not assume the default action (copy, move, etc.) is always
    // processed, because other plug-ins might cancel the operation.
    return 0;
}

#ifdef __cplusplus
}
#endif

