//---------------------------------------------------------------------------
#ifndef TAboutFormH
#define TAboutFormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Dialogs.hpp>
//---------------------------------------------------------------------------
class TAboutForm : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TButton *OK;
	TButton *Cancel;
	TLabel *ProgramName;
   TLabel *Version;
   TLabel *HeaderCaption;
   TEdit *HeaderList;
   TCheckBox *isAlwaysOnTop;
   TCheckBox *HideIfNoHeader;
   TButton *Fonts;
   TFontDialog *FontDialog;
   void __fastcall FontsClick(TObject *Sender);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
	__fastcall TAboutForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TAboutForm *AboutForm;
//---------------------------------------------------------------------------
#endif
