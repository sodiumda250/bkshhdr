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
__published:	// IDE �Ǘ��̃R���|�[�l���g
private:	// ���[�U�[�錾
protected:
    virtual void __fastcall CreateParams(TCreateParams &Params);
public:		// ���[�U�[�錾
    __fastcall TDummyForm(TComponent* Owner);
    virtual void __fastcall SetWindowStatus(void);
};
//---------------------------------------------------------------------------
extern TDummyForm *DummyForm;
//---------------------------------------------------------------------------
#endif
