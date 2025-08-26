//---------------------------------------------------------------------------

#ifndef ContractListSettingFormH
#define ContractListSettingFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RoundFormEx.h"
#include <ExtCtrls.hpp>
#include "GraphButton.h"
#include "GraphPanel.h"
//---------------------------------------------------------------------------
class TContractListColorSettingForm : public TForm
{
__published:	// IDE-managed Components
	TRoundFormEx *RoundFormEx;
	TColorBox *TextColorBox;
	TColorBox *OddBKColorBox;
	TColorBox *EvenBKColorBox;
	TColorBox *RiseBKColorBox;
	TColorBox *FallBKColorBox;
	TGraphButton *OKButton;
	TGraphButton *CancelButton;
	TGraphPanel *GraphPanel;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
private:	// User declarations
	TColor FTextColor;
	TColor FOddBKColor;
	TColor FEvenBKColor;
	TColor FRiseBKColor;               // 上漲顏色
	TColor FFallBKColor;               // 跌顏色
	void __fastcall LoadColor( void );
	void __fastcall SaveColor( void );
	void __fastcall DefaultColor( void );
	void __fastcall SetColor( void );
public:		// User declarations
	__fastcall TContractListColorSettingForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TContractListColorSettingForm *ContractListColorSettingForm;
//---------------------------------------------------------------------------
#endif
