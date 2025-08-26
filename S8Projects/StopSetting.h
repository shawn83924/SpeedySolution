//---------------------------------------------------------------------------

#ifndef StopSettingH
#define StopSettingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "OrderMessageDefine.h"
#include "RoundFormEx.h"
//---------------------------------------------------------------------------
class TOrderBookList;
class TCandleStickChart;
//---------------------------------------------------------------------------
class PACKAGE TStopSettingForm : public TForm
{
__published:	// IDE-managed Components
	TRadioGroup *BuyRadioGroup;
	TRadioGroup *SellRadioGroup;
	TComboBox *BuyStopTickComboBox;
	TComboBox *SellStopTickComboBox;
	TButton *OKButton;
	TButton *CancelButton;
	TRoundFormEx *RoundFormEx;
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
	static int TickSteps;
public:		// User declarations
	__fastcall TStopSettingForm(TComponent* Owner );
	void __fastcall ApplySetting( TOrderBookList* Depth );
	void __fastcall ApplySetting( TCandleStickChart* Chart );
public:
	nsOrderMessageDefine::OrderTypeEnum __fastcall GetBuyOrderType( void );
	nsOrderMessageDefine::OrderTypeEnum __fastcall GetSellOrderType( void );
	int __fastcall GetBuyTick( void );
	int __fastcall GetSellTick( void );

};
//---------------------------------------------------------------------------
//extern PACKAGE TStopOrderSettingForm *StopOrderSettingForm;
//---------------------------------------------------------------------------
#endif
