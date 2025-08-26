//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "OrderConfirm.h"
#include "FMTConfig.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphButton"
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TOrdConfirmForm *OrdConfirmForm;
extern TCMarketDataStore* gMarketDataStore;
//---------------------------------------------------------------------------
__fastcall TOrdConfirmForm::TOrdConfirmForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::Init( const String& Ex, const String& Sym, nsOrderMessageDefine::SideEnum side, double Px )
{
	double     TickPx;
	String     PxText;

	FTickList = (TTickList*)gMarketDataStore->GetTickList( Ex, Sym );
	if( Ex == L"TAIFEX" )
	{
		FUnit = L"口";
		NormalButton->Visible    = false;
		LoanButton->Visible      = false;
		LoanShareButton->Visible = false;
		NormalButtonClick( NormalButton );
	}
	else if( Ex == L"TWSE" || Ex == L"OTC" )
	{
		FUnit = L"張";
		NormalButton->Visible    = true;
		LoanButton->Visible      = true;
		LoanShareButton->Visible = true;
		NormalButtonClick( NormalButton );
	}
	if( FLastEx != Ex && FLastSym != Sym )
	{
		FLastEx  = Ex;
		FLastSym = Sym;
		PxComboBox->Clear();
		for( int i = 0; i < FTickList->ItemCount(); i++ )
		{
			TickPx = FTickList->GetPxFromIndex( i, false );
			FTickList->ToTNFormat( TickPx, PxText );
			PxComboBox->Items->Add( PxText );
		}
	}
	PxComboBox->ItemIndex = FTickList->PxToTickIndex( Px, false );
	LotsUpDown->Position  = g_Config.GetIntegerProperty("Setting","ChartQty", 1 );
	if( side == sBuy )
		BuyGraphButtonClick( NULL );
	else
		SellGraphButtonClick( NULL );
}
//---------------------------------------------------------------------------
double __fastcall TOrdConfirmForm::GetPrice( void )
{
	return FTickList->GetPxFromIndex( PxComboBox->ItemIndex, false );
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::BuildBtnText( int Qty )
{
	String     BtnText;
	String     PxStr = PxComboBox->Text;

	BtnText.printf( L"%s%d%s@%s", FSideStr, Qty,FUnit, PxStr );
	OKButton->Caption = BtnText;
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::NormalButtonClick(TObject *Sender)
{
	if( Sender == NormalButton )
	{
		NormalButton->Selected    = true;
		LoanButton->Selected      = false;
		LoanShareButton->Selected = false;
	}
	else if( Sender == LoanButton )
	{
		NormalButton->Selected    = false;
		LoanButton->Selected      = true;
		LoanShareButton->Selected = false;
	}
	else
	{
		NormalButton->Selected    = false;
		LoanButton->Selected      = false;
		LoanShareButton->Selected = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::DayTradeButtonClick(TObject *Sender)
{
	if( DayTradeButton->Selected == true )
	{
		DayTradeButton->Selected = false;
		DayTradeButton->Hint = L"停用當沖";
	}
	else
	{
		DayTradeButton->Selected = true;
		DayTradeButton->Hint = L"啟用當沖";
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::LotsUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	AllowChange = true;
	BuildBtnText( NewValue );
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::PxComboBoxChange(TObject *Sender)
{
	BuildBtnText( LotsUpDown->Position );
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::OKButtonClick(TObject *Sender)
{
	g_Config.SetIntegerProperty("Setting","ChartQty", LotsUpDown->Position );
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::CancelButtonClick(TObject *Sender)
{
	g_Config.SetIntegerProperty("Setting","ChartQty", LotsUpDown->Position );
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------
PositionEffectEnum   __fastcall TOrdConfirmForm::GetPositionEffect( void )
{
	if( DayTradeButton->Selected == true )
		return nsOrderMessageDefine::peDayTrade;
	return nsOrderMessageDefine::peAuto;
}
//---------------------------------------------------------------------------
char __fastcall TOrdConfirmForm::GetTWSEOrderType( SideEnum side )
{
	if( LoanButton->Selected   == true )
		return '3';  ///< trade margin
	else if( LoanShareButton->Selected   == true )
		return '4';  ///< sell short
	else
		return '0'; ///< Normal or Day Trade
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::BuyGraphButtonClick(TObject *Sender)
{
	BuyGraphButton->Selected = true;
	SellGraphButton->Selected = false;
	FSide = sBuy;
	OKButton->Font->Color = clRed;
	FSideStr = L"買";
	BuildBtnText( LotsUpDown->Position );
}
//---------------------------------------------------------------------------
void __fastcall TOrdConfirmForm::SellGraphButtonClick(TObject *Sender)
{
	BuyGraphButton->Selected = false;
	SellGraphButton->Selected = true;
	FSide = sSell;
	OKButton->Font->Color = clGreen;
	FSideStr = L"賣";
	BuildBtnText( LotsUpDown->Position );
}
//---------------------------------------------------------------------------

