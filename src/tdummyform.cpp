//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "TDummyForm.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TDummyForm *DummyForm;
//---------------------------------------------------------------------------
__fastcall TDummyForm::TDummyForm(TComponent* Owner)
   : TForm(Owner)
{
    CreateHandle();
}
//---------------------------------------------------------------------------
void __fastcall TDummyForm::CreateParams(TCreateParams &Params)
{
    TForm::CreateParams(Params);
    //Params.WndParent = Parent->Handle;
    Params.Style = WS_VISIBLE;
    Params.ExStyle = WS_EX_TOOLWINDOW;
    Params.X = 0;
    Params.Y = CW_USEDEFAULT;
    Params.Width = 0;
    Params.Height = CW_USEDEFAULT;
}
//---------------------------------------------------------------------------
void __fastcall TDummyForm::SetWindowStatus(void)
{
    long lStyle;

    lStyle = ::GetWindowLong(Handle, GWL_STYLE);
    lStyle &= ~(WS_CLIPCHILDREN);
    lStyle &= ~(WS_CLIPSIBLINGS);
     ::SetWindowLong(Handle, GWL_STYLE, lStyle);
}
//---------------------------------------------------------------------------
