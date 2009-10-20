//---------------------------------------------------------------------------
#ifndef ShowTextH
#define ShowTextH
#include <vcl\inifiles.hpp>
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TShowForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	 TMemo *ShowText;
	 void __fastcall FormResize(TObject *Sender);
    void __fastcall DblClick(TObject *Sender);
private:	// ユーザー宣言
    char * Header;
    TIniFile *IniFile; // Iniファイルクラスのエイリアス、実体はTShowFormListで管理する。
    bool HideIfNoHeader;  // これがtrueのとき、ヘッダがない場合Windowを隠す。
public:		// ユーザー宣言
    TShowForm *Next;  // 次のリスト要素を示す。
	__fastcall TShowForm::TShowForm(TComponent* Owner, TIniFile *IniFile, const char *PrmHeader);
	__fastcall TShowForm::~TShowForm();
	void __fastcall Initialize();  // コンストラクタで行わない初期化を行う。
	inline const char * GetHeader(){return Header;};  // 表示すべきヘッダ名を返却する。
    void __fastcall SetHeader(const char *);  // 表示すべきヘッダ名を設定する。
    inline int Length(){return ShowText->Text.Length();};  // 表示文字列の長さを返却する。
    HWND SetParentWin(HWND);  // 親windowを指定する。現状うまく動作していない。
    inline void SetHideIfNoHeader(bool flag){HideIfNoHeader = flag;};  // HideIfNoHeaderフラグを設定する。
    inline bool IsMyWindowsHandle(HWND handle)  // 指定されたハンドルが自分のWindowHandleかどうかを返却する。
              {return ((Handle == handle || ::IsChild(Handle, handle) != false)?true:false);};
    inline void SetFont(TFont &SrcFont){ShowText->Font->Assign(&SrcFont);};
};
//---------------------------------------------------------------------------
extern TShowForm *ShowForm;
//---------------------------------------------------------------------------

// user define
class TShowFormList
{
private:
    int listcount;  // 表示ヘッダリストの要素数
    TShowForm *List; // 表示ヘッダリストの先頭要素
    TShowForm *Last;  // 表示ヘッダリストの最終要素
    TIniFile *IniFile; // このプラグインで使用しているiniファイルのオブジェクト
    bool AlwaysOnTop;  // 「常に一番上に表示」オプションの値
    void __fastcall NewList();
    void __fastcall DeleteList();
    HWND hMain;  // Becky!のメインウィンドウハンドル
    HWND hTree;  // Becky!のツリービューウィンドウハンドル
    HWND hList;  // Becky!のリストビューメインウィンドウハンドル
    HWND hView;  // Becky!のメールビューメインウィンドウハンドル
    long DebugNum;
    bool HideIfNoHeader;
    TFont *Font;

public:
    __fastcall TShowFormList(const char*);
    __fastcall ~TShowFormList();
    inline int count(){return listcount;};
    int __fastcall ShowHeader(LPCSTR lpMailID);
    void __fastcall SetProperty(const AnsiString&, const TCheckBoxState, const TCheckBoxState);
    void __fastcall Show();
    void __fastcall Hide();
    void __fastcall SetParentWin(HWND hMain);
    inline void SetParentWin(void){SetParentWin(hMain);};
    inline bool ShowAlwaysOnTop(){return AlwaysOnTop;};
    inline bool isHideIfNoHeader(){return HideIfNoHeader;};
    AnsiString &SetHeaderList(AnsiString &);
    inline HWND GetBeckyMain(){return hMain;};
    inline HWND GetBeckyTree(){return hTree;};
    inline HWND GetBeckyList(){return hList;};
    inline HWND GetBeckyView(){return hView;};
    bool __fastcall IsMyWindowsHandle(HWND);
    inline void SetFont(TFont &SrcFont){Font->Assign(&SrcFont);};
    inline TFont* GetFont(){return Font;};
};

#endif
