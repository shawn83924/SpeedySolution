//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SeqSettingForm.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSequenceForm *SequenceForm;
//---------------------------------------------------------------------------
__fastcall TSequenceForm::TSequenceForm(TComponent* Owner, AnsiString InSeq, AnsiString OutSeq)
    : TForm(Owner)
{
    InSeqEdit->Text = InSeq;
    OutSeqEdit->Text = OutSeq;
}
//---------------------------------------------------------------------------
void __fastcall TSequenceForm::OKBtnClick(TObject *Sender)
{
    try
    {
        FInSeq  = InSeqEdit->Text.ToInt();
        FOutSeq = OutSeqEdit->Text.ToInt();
		if( FInSeq <= 0 || FOutSeq <=0 )
			throw Exception( "Seq can not less than 1" );
		ModalResult = mrOk;
	}
	catch(...)
	{
		ShowMessage( Scstrings_MAIN_FIX_SEQ_NUM_MUST_LARGER_ZERO );
    }
}
//---------------------------------------------------------------------------
