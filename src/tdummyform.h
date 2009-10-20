//---------------------------------------------------------------------------
#ifndef TDummyFormH
#define TDummyFormH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TDummyForm : public TForm
{
__published:	// IDE 管理のコンポーネント
private:	// ユーザー宣言
protected:
    virtual void __fastcall CreateParams(TCreateParams &Params);
public:		// ユーザー宣言
    __fastcall TDummyForm(TComponent* Owner);
    virtual void __fastcall SetWindowStatus(void);
};
//---------------------------------------------------------------------------
extern TDummyForm *DummyForm;
//---------------------------------------------------------------------------
#endif
