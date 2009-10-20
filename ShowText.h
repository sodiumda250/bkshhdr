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
__published:	// IDE �Ǘ��̃R���|�[�l���g
	 TMemo *ShowText;
	 void __fastcall FormResize(TObject *Sender);
    void __fastcall DblClick(TObject *Sender);
private:	// ���[�U�[�錾
    char * Header;
    TIniFile *IniFile; // Ini�t�@�C���N���X�̃G�C���A�X�A���̂�TShowFormList�ŊǗ�����B
    bool HideIfNoHeader;  // ���ꂪtrue�̂Ƃ��A�w�b�_���Ȃ��ꍇWindow���B���B
public:		// ���[�U�[�錾
    TShowForm *Next;  // ���̃��X�g�v�f�������B
	__fastcall TShowForm::TShowForm(TComponent* Owner, TIniFile *IniFile, const char *PrmHeader);
	__fastcall TShowForm::~TShowForm();
	void __fastcall Initialize();  // �R���X�g���N�^�ōs��Ȃ����������s���B
	inline const char * GetHeader(){return Header;};  // �\�����ׂ��w�b�_����ԋp����B
    void __fastcall SetHeader(const char *);  // �\�����ׂ��w�b�_����ݒ肷��B
    inline int Length(){return ShowText->Text.Length();};  // �\��������̒�����ԋp����B
    HWND SetParentWin(HWND);  // �ewindow���w�肷��B���󂤂܂����삵�Ă��Ȃ��B
    inline void SetHideIfNoHeader(bool flag){HideIfNoHeader = flag;};  // HideIfNoHeader�t���O��ݒ肷��B
    inline bool IsMyWindowsHandle(HWND handle)  // �w�肳�ꂽ�n���h����������WindowHandle���ǂ�����ԋp����B
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
    int listcount;  // �\���w�b�_���X�g�̗v�f��
    TShowForm *List; // �\���w�b�_���X�g�̐擪�v�f
    TShowForm *Last;  // �\���w�b�_���X�g�̍ŏI�v�f
    TIniFile *IniFile; // ���̃v���O�C���Ŏg�p���Ă���ini�t�@�C���̃I�u�W�F�N�g
    bool AlwaysOnTop;  // �u��Ɉ�ԏ�ɕ\���v�I�v�V�����̒l
    void __fastcall NewList();
    void __fastcall DeleteList();
    HWND hMain;  // Becky!�̃��C���E�B���h�E�n���h��
    HWND hTree;  // Becky!�̃c���[�r���[�E�B���h�E�n���h��
    HWND hList;  // Becky!�̃��X�g�r���[���C���E�B���h�E�n���h��
    HWND hView;  // Becky!�̃��[���r���[���C���E�B���h�E�n���h��
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
