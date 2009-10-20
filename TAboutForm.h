//---------------------------------------------------------------------------
#ifndef TAboutFormH
#define TAboutFormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TAboutForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TButton *OK;
	TButton *Cancel;
	TLabel *ProgramName;
   TLabel *Version;
   TLabel *HeaderCaption;
   TEdit *HeaderList;
   TCheckBox *isAlwaysOnTop;

    TCheckBox *HideIfNoHeader;
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TAboutForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TAboutForm *AboutForm;
//---------------------------------------------------------------------------
#endif
