//---------------------------------------------------------------------------

#ifndef SeqSettingFormH
#define SeqSettingFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSequenceForm : public TForm
{
__published:	// IDE-managed Components
    TEdit *InSeqEdit;
    TLabel *Label1;
    TLabel *Label2;
    TEdit *OutSeqEdit;
	TBitBtn *CancelBtn;
	TBitBtn *OKBtn;
    TBevel *Bevel1;
    void __fastcall OKBtnClick(TObject *Sender);
private:	// User declarations
    int FInSeq;
    int FOutSeq;
public:		// User declarations
    __fastcall TSequenceForm(TComponent* Owner, AnsiString InSeq, AnsiString OutSeq);
    int __fastcall GetInboundSeq(){ return FInSeq;}
    int __fastcall GetOutboundSeq(){ return FOutSeq;}
};
//---------------------------------------------------------------------------
extern PACKAGE TSequenceForm *SequenceForm;
//---------------------------------------------------------------------------
#endif
