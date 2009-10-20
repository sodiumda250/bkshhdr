//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "TAboutForm.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutForm *AboutForm;
//---------------------------------------------------------------------------
__fastcall TAboutForm::TAboutForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::FontsClick(TObject *Sender)
{
    FontDialog->Execute();
}
//---------------------------------------------------------------------------
