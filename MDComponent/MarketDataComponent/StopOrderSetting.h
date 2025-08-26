//---------------------------------------------------------------------------

#ifndef StopOrderSettingH
#define StopOrderSettingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "OrderMessageDefine.h"
//---------------------------------------------------------------------------
class PACKAGE TStopOrderSettingForm : public TForm
{
__published:	// IDE-managed Components
	TRadioGroup *BuyRadioGroup;
	TRadioGroup *SellRadioGroup;
	TComboBox *ComboBox2;
	TComboBox *ComboBox4;
	TButton *Button1;
	TButton *Button2;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TStopOrderSettingForm(TComponent* Owner);
	nsOrderMessageDefine::OrderTypeEnum __fastcall GetBuyOrderType( void );
	nsOrderMessageDefine::OrderTypeEnum __fastcall GetSellOrderType( void );
	int __fastcall GetBuyTick( void );
	int __fastcall GetSellTick( void );
	static int TickSteps;
};
//---------------------------------------------------------------------------
//extern PACKAGE TStopOrderSettingForm *StopOrderSettingForm;
//---------------------------------------------------------------------------
#endif
