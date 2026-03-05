//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "DepthTrade.h"
#include "main.h"
#include "UnifyDlg.h"
#include "ContractForm.h"
#include "ContractViewer.h"
#include "FMTConfig.h"
#include "StopSetting.h"
#include "SearchUnit.h"
#include "dsaudio.h"
#include "UnifyUtility.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OrderBookList"
#pragma link "ChartsStore"
#pragma link "MarketDataStore"
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma resource "*.dfm"
TDepthForm *DepthForm;
//---------------------------------------------------------------------------
extern TOrderStore*       gOrderStore;
extern TCMarketDataStore* gMarketDataStore;
extern DSAudio            gDSAudio;
extern bool               GSimMatch;
extern bool               gNuclear;
extern bool               gTFT;  ///< Enable TFT functions
extern TUnifyUser         gUser;
//---------------------------------------------------------------------------
const int FONT_SIZE_MIN = 9;
const int FONT_SIZE_DEF = 10;
const int FONT_SIZE_MAX = 15;
const int CUSTOM_LIST_COUNT = 5;
//---------------------------------------------------------------------------
TColor FixedRowBidColor[2]  = {RGB(239,239,239),RGB(84,84,84)};
TColor FixedRowAskColor[2]  = {RGB(239,239,239),RGB(84,84,84)};
TColor FixedRowColor[2]     = {clBlack,clWhite};
TColor FixedRowBKColor[2]   = {RGB(239,239,239),RGB(84,84,84)};
TColor GridColor[2]	        = {RGB(224,224,224),RGB(104,104,104)};
///< Tick row colors
TColor TickBKColor[2]       = {RGB(249,249,249),RGB(8,16,27)};
TColor TickUpperColor[2]    = {RGB(230,0,0),RGB(230,0,0)};
TColor TickLowerColor[2]    = {RGB(33,140,4),RGB(33,140,4)};
TColor FillColor[2]         = {clWhite,clWhite};
TColor FillBKColor[2]       = {RGB(50,195,190),RGB(9,60,62)};
TColor BetterBuyColor[2]    = {clWhite,clWhite};
TColor BetterBuyBKColor[2]  = {RGB(255,109,135),RGB(74,19,25)};
TColor BetterSellColor[2]   = {clWhite,clWhite};
TColor BetterSellBKColor[2] = {RGB(72,137,193),RGB(10,32,73)};
TColor DayHiColor[2]        = {clRed,clRed};
TColor DayLoColor[2]        = {clGreen,clGreen};
TColor AvgPriceColor[2]     = {clWhite,clWhite};
///< Buy side color
TColor BidDepth5BKColor[2]       = {clWhite,RGB(53,53,53)};
TColor BidDepth5Color[2]         = {RGB(248,152,154),RGB(255,166,166)};
TColor BuyBKColor[2]             = {clWhite,RGB(53,53,53)};
TColor BuyColor[2]               = {clBlack,clWhite};
TColor BuyFillBKColor[2]         = {clWhite,RGB(53,53,53)};
TColor BuyFillColor[2]           = {clBlack,clWhite};
TColor BuyConditionColBKColor[2] = {clWhite,RGB(53,53,53)};
TColor BuyConditionColColor[2]   = {clBlack,clWhite};
TColor BuyOCOColBKColor[2]       = {clWhite,RGB(53,53,53)};
TColor BuyOCOColColor[2]         = {clBlack,clWhite};
TColor BuyOCODelColBKColor[2]    = {clWhite,RGB(53,53,53)};
///<Sell side color
TColor AskDepth5BKColor[2]        = {clWhite,RGB(53,53,53)};
TColor AskDepth5Color[2]          = {RGB(124,186,63),RGB(149,255,116)};
TColor SellBKColor[2]             = {clWhite,RGB(53,53,53)};
TColor SellColor[2]               = {clBlack,clWhite};
TColor SellFillBKColor[2]         = {clWhite,RGB(53,53,53)};
TColor SellFillColor[2]           = {clBlack,clWhite};
TColor SellConditionColBKColor[2] = {clWhite,RGB(53,53,53)};
TColor SellConditionColColor[2]   = {clBlack,clWhite};
TColor SellOCOColBKColor[2]       = {clWhite,RGB(53,53,53)};
TColor SellOCOColColor[2]         = {clBlack,clWhite};
TColor SellOCODelColBKColor[2]    = {clWhite,RGB(53,53,53)};
//---------------------------------------------------------------------------
UFC::PHashMap<UFC::AnsiString, UFC::List<TDepthForm*>*> FStopCloseMap;
UFC::List<BasicInformation*> TDepthForm::FCustomMDList[CUSTOM_LIST_COUNT];
//---------------------------------------------------------------------------
__fastcall TDepthForm::TDepthForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
__fastcall TDepthForm::TDepthForm(TWinControl* Owner, int Page , String Ex, String Sym )
:TForm(Owner)
,FParent( Owner )
,FEx( Ex )
,FSym( Sym )
,FOldEx( Ex )
,FOldSym( Sym )
,FProfile( L"" )
,FRegOrderStore( false )
,FLastKeyDownTick( GetTickCount() )
,FKeyCxlStop( 0 )
,FKeyCxlAndClose( 0 )
,FKeyNuclearBuy( 0 )
,FKeyNuclearSell( 0 )
,FGroup( Page )
,FNetPos( 0 )
,FFontSize( FONT_SIZE_DEF )
,FPostSizeOffset( 7 )
,FLeft(0)
,FTop(0)
,FFateTesting( false )
,RegCount( 0 )
{
	OrderBookList->Store = gMarketDataStore;
	Parent = FParent;
	ToolSV->UseAnimation = false;
	ToolSV->Opened = false;
	ToolSV->UseAnimation = true;
	PageControl->ActivePage = SettingTabSheet;
	this->DefaultMonitor = dmDesktop;
	OrderBookList->MaxFPS = 10;
	EnableStopAfterFilled( false );
	TIFComboBox->ItemIndex = 0;

	FExList = new TStringList();
	FSymList = new TStringList();

	FExpBtn2Panel.Add(SplitExpBtn,SplitPanel);
	FExpBtn2Panel.Add(BullInExpBtn,BullInPanel);
	FExpBtn2Panel.Add(BullOutExpBtn,BullOutPanel);
	FExpBtn2Panel.Add(DepthExpBtn,DepthPanel);
}
//---------------------------------------------------------------------------
__fastcall TDepthForm::~TDepthForm( void )
{
	delete FExList;
	delete FSymList;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SetWidth( int Add )
{
	int NewWidth = OrderBookList->BestFitWidth + 2 + Add;

	Constraints->MaxWidth = 9999;   ///< Reset Constraints
	Constraints->MinWidth = 0;
	Width = NewWidth;
	Constraints->MaxWidth  = NewWidth;
	Constraints->MinWidth  = NewWidth;
	Constraints->MinHeight = 500;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ShowDepth( int L,int T, const String& Profile  )
{
	ChangeSymbol( );
	LoadHotkey( );
	LoadColor();
	LoadStopSetting();
	OrderBookList->ShowDerivedPx = false;
	OrderBookList->ShowFillQty = false;
	OrderBookList->ShowFilled = false;
	OrderBookList->CenterFillPrice = false;
	Left = L;
	Top = T;
	SetWidth( 0 );
	ToolSV->Opened = false;
	AdjuestFont( );
	SetEditTag();
	LoadProperty( Profile );
	Show();
	BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ShowDepth( int L,int T )
{
	ChangeSymbol( );
	LoadHotkey( );
	LoadColor();
	LoadStopSetting();
	OrderBookList->ShowDerivedPx = false;
	OrderBookList->ShowFillQty = false;
	OrderBookList->ShowFilled = false;
	OrderBookList->CenterFillPrice = false;
	Left = L;
	Top = T;
	SetWidth( 0 );
	ToolSV->Opened = false;
	AdjuestFont( );
	SetEditTag();
	Show();
	BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::UpdateCaption( void )
{
	if( FMarket == nsOrderMessageDefine::mTSE || FMarket == nsOrderMessageDefine::mOTC )
	{
		String StockName;

		StockName.printf( L"%s(%s)", OrderBookList->ChineseName , FSym );
		Caption = StockName;
	}
	else
		Caption = OrderBookList->ChineseName;

}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::FormShow(TObject *Sender)
{
	UpdateCaption();
	NuclearGroupBox->Visible = gNuclear;
	TFTTabSheet->TabVisible = gTFT;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:
			 Msg.Result = 1;
			 return;
		case WM_MOUSEACTIVATE:
			 BringToFront();
			 ContractViewerForm->ActiveClient( this );
			 OrderBookList->SetFocus();
			 Msg.Result = MA_ACTIVATE;
			 return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::AutoCenterSwitchClick(TObject *Sender)
{
	if( AutoCenterSwitch->State == tssOff )
		OrderBookList->CenterFillPrice = false;
	else
		OrderBookList->CenterFillPrice = true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::FillToggleSwitchClick(TObject *Sender)
{
	if( FillToggleSwitch->State == tssOff )
		OrderBookList->ShowFilled = false;
	else
		OrderBookList->ShowFilled = true;
	SetWidth( ToolSV->OpenedWidth );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::StopToggleSwitchClick(TObject *Sender)
{
	if( StopToggleSwitch->State == tssOff )
		OrderBookList->ConditionOrder = false;
	else
		OrderBookList->ConditionOrder = true;
	SetWidth( ToolSV->OpenedWidth );
}
//---------------------------------------------------------------------------

void __fastcall TDepthForm::DrivedToggleSwitchClick(TObject *Sender)
{
	if( DrivedToggleSwitch->State == tssOff )
		OrderBookList->ShowDerivedPx = false;
	else
		OrderBookList->ShowDerivedPx = true;
	SetWidth( ToolSV->OpenedWidth );
}
//---------------------------------------------------------------------------

void __fastcall TDepthForm::FillQtyToggleSwitchClick(TObject *Sender)
{
	if( FillQtyToggleSwitch->State == tssOff )
		OrderBookList->ShowFillQty = false;
	else
		OrderBookList->ShowFillQty = true;
	AdjuestFont( );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::CxlBeforeNewToggleSwitchClick(TObject *Sender)
{
	if( CxlBeforeNewToggleSwitch->State == tssOff )
		OrderBookList->CancelBeforeNew = false;
	else
		OrderBookList->CancelBeforeNew = true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::imgMenuClick(TObject *Sender)
{
  if( ToolSV->Opened )
  {
	  SetWidth( 0 );
	  ToolSV->Close();
  }
  else
  {
	  if( RoundFormEx->Lock == true )
	  {
		if( PageControl->ActivePage == ColorTabSheet )
			PageControl->ActivePage = SettingTabSheet;
		ColorTabSheet->TabVisible = false;
	  }
	  else
		ColorTabSheet->TabVisible = true;
	  SetWidth( ToolSV->OpenedWidth );
	  FixRowComboBoxChange( NULL );
	  TickComboBoxChange( NULL );
	  BuyComboBoxChange( NULL );
	  SellComboBoxChange( NULL );
	  ExchangeComboBoxChange( NULL );
	  ToolSV->Open();
  }
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::Release( void )
{
	if( FProfile.Length() <=  0 )
		FProfile = L"SpeedyUnify\\Depth" + IntToStr( Tag );
	SaveProperty( FProfile );
	EnableStopAfterFilled( false );
	OrderBookList->Unsubscribe();
	OrderBookList->Sync = false;
	OrderBookList->Store = NULL;
	UnregisterOrderStore();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::AdjustByExchange( const String& Ex )
{
	if( Ex == L"TAIFEX" )
	{
		DrivedPxLabel->Enabled = true;
		DrivedToggleSwitch->Enabled = true;
		NormalButton->Visible    = false;
		LoanButton->Visible      = false;
		LoanShareButton->Visible = false;
		NormalButtonClick( NormalButton );
	}
	else if( Ex == L"TWSE" || Ex == L"OTC" )
	{
		DrivedPxLabel->Enabled = false;
		DrivedToggleSwitch->Enabled = false;
		DrivedToggleSwitch->State = tssOff;
		NormalButton->Visible    = true;
		LoanButton->Visible      = true;
		LoanShareButton->Visible = true;
		NormalButtonClick( NormalButton );
    }
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ChangeSymbol( String Ex, String Sym )
{
	FEx = Ex;
	FSym = Sym;
	ChangeSymbol();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ChangeSymbol( void )
{
	BasicInformation* NewSymInfo = OrderBookList->Store->GetBasicInformation( FEx, FSym, false );

	if( NewSymInfo == NULL )
    	return;

	FSymInfo = NewSymInfo;
	EnableStopAfterFilled( false );
	UnregisterOrderStore();
	for( int i = 0;i < 20; i++)
	{
		Application->ProcessMessages();
		UFC::SleepMS(10);
	}
	OrderBookList->SetSymbol( FEx, FSym );
	FMarket   = FSymInfo->GetMarket();
	FSym      = FSymInfo->GetSymbol().c_str();
	FCallPut  = FSymInfo->GetCallPut();
	FStrikePX = FSymInfo->GetStrikePrice();
	FBullPx   = FSymInfo->GetBullPrice();
	FDigit    = FSymInfo->GetDigit();
	UpdateCaption();
	RegisterOrderStore();
	ContractViewerForm->ChangeSymbol( FOldEx, FOldSym, FEx, FSym, this );
	FOldEx = FEx;
	FOldSym = FSym;
	OnSymbolChanged();

}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OnSymbolChanged( void )
{
	FBullInBeginTick = 0;
	FBullInAccuQty   = 0;
	FBullOutBeginTick = 0;
	FBullOutAccuQty   = 0;
	AdjustByExchange( FEx );
	if( FilledStopToggleSwitch->State == tssOn || FilledStopProfitToggleSwitch->State == tssOn )
		EnableStopAfterFilled( true );
	else
		EnableStopAfterFilled( false );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::RoundFormExIconClick(TObject *Sender)
{
	if(  OrderBookList->Hotkey == true )
	{
		TPoint ScrLT = ClientToScreen( TPoint( 0, 0 ));

		SearchSymbolForm->Top = ScrLT.Y - RoundFormEx->CaptionHeight;
		SearchSymbolForm->Left = ScrLT.X - RoundFormEx->LeftFrameWidth;
		SearchSymbolForm->Width = Width;
		ContractViewerForm->OnSelectSymbol = SelectSymbol;
		if( RoundFormEx->Lock == true )
		{
			Application->NormalizeTopMosts();
			SetWindowPos( SearchSymbolForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
		}
		else
			SetWindowPos( SearchSymbolForm->Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
		SearchSymbolForm->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SelectSymbol( System::TObject* Sender, TSelectActionType Type, const String& Ex, const String& Sym )
{
	switch( Type )
	{
		case satSymbol: if( FOldEx != Ex || FOldSym != Sym )
						{
							FEx  = Ex;
							FSym = Sym;
							ChangeSymbol( );
						}
						break;
		case satDepth:  ContractViewerForm->OpenOrderBookForm( Ex, Sym, true );
						break;
		case satKChart: ContractViewerForm->OpenKBarForm( Ex, Sym );
						break;
	}
	ContractViewerForm->OnSelectSymbol = NULL;
	ContractViewerForm->Hide();
}
//---------------------------------------------------------------------------
bool __fastcall TDepthForm::CheckOrder( int Qty )
{
	if( Qty == 0 )
	{
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"口數不能為0" );
		return false;
	}
	if( FMarket == nsOrderMessageDefine::mTSE || FMarket == nsOrderMessageDefine::mOTC )
	{
		if( gUser.AccountType == hatTAIFEX )
		{
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"證券帳號不存在" );
			return false;
		}
	}
	else if( FMarket == nsOrderMessageDefine::mTWFutures || FMarket == nsOrderMessageDefine::mTWOptions )
	{
		if( gUser.AccountType == hatTWSE )
		{
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"期貨帳號不存在" );
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TDepthForm::PlaceOrder( SideEnum side, double Price, int Qty,OrderTypeEnum OrderType, bool EnableStop )
{
	TimeInForceEnum     TIF = GetTimeInForce( false );
	PositionEffectEnum  PE = GetPositionEffect();

	if( CheckOrder( Qty ) == false )
		return false;
	try
	{
		AnsiString Sym( FSym );
		bool Rtn;
		int StopTick = 0;
		char TWSEOrdType = GetTWSEOrderType( side );

		if( FilledStopToggleSwitch->State == tssOn || FilledStopProfitToggleSwitch->State == tssOn )
			StopTick =  10;

		if( side == sBuy )
			UFC::BufferedLog::Printf( " [TDepthForm::PlaceOrder]Buy Symbol[%s] %d@%0.2f", Sym.c_str(),Qty, Price );
		else
			UFC::BufferedLog::Printf( " [TDepthForm::PlaceOrder]Sell Symbol[%s] %d@%0.2f", Sym.c_str(),Qty, Price );
		if( gTFT == true && SplitToggleSwitch->State == tssOn )
		{
			int SpiltQty = SplitUpDown->Position;
			int OrdCount = Qty / SpiltQty;
			int LastQty  = Qty % SpiltQty;

			for( int i = 0; i < OrdCount; i ++ )
				Rtn = gOrderStore->NewOrder( FMarket, FEx, FSym, side, Price, SpiltQty, OrderType, TIF, PE, FCallPut, FStrikePX, StopTick, 0.0, TWSEOrdType );
			if( LastQty > 0 )
				Rtn = gOrderStore->NewOrder( FMarket, FEx, FSym, side, Price, LastQty, OrderType, TIF, PE, FCallPut, FStrikePX, StopTick, 0.0, TWSEOrdType );
		}
		else
		{
			Rtn = gOrderStore->NewOrder(  FMarket, ///< Market
										  FEx,
										  FSym,  ///< symbol
										  side,     ///< side
										  Price,    ///< Price
										  Qty,      ///< Qty
										  OrderType,
										  TIF,      ///< time in force.
										  PE,       ///< Position effect.
										  FCallPut, ///< Call or put
										  FStrikePX,///< Options Strike price
										  StopTick, ///< Tick Count
										  0.0,      ///< Stop Pricr
										  TWSEOrdType );
		}
		return Rtn;
	}
	catch( UFC::Exception& execp )
	{
		String Msg;
		Msg.printf( L"委託失敗[%s]", execp.what() );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, Msg ); ///< 委託失敗
		return false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::PlaceMarketOrder( SideEnum side, int Qty,TimeInForceEnum TIF )
{
	PositionEffectEnum  PE = GetPositionEffect();

	try
	{
		if( CheckOrder( Qty ) == false )
			return;
		gOrderStore->NewOrder( FMarket, ///< Market
							   FEx,
							   FSym, ///< symbol
							   side, ///< side
							   0,    ///< Market order don't need Price.
							   Qty,  ///Qty
							   nsOrderMessageDefine::otMarket,
							   TIF,  ///< time in force.
							   PE,   ///< Position effect.
							   FCallPut,
							   FStrikePX,
							   0,0.0, GetTWSEOrderType( side ) );
	}
	catch( UFC::Exception& execp )
	{
		String Msg;
		Msg.printf( L"委託失敗[%s]", execp.what() );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, Msg );
	}
}
//---------------------------------------------------------------------------
void TDepthForm::OrderQtyChanged( MarketEnum Market, const String& Symbol, SideEnum Side, double Px, int Qty)
{
	if( Symbol == FSym )
		OrderBookList->UpdateQty( Side, Px, Qty );
}
//---------------------------------------------------------------------------
void TDepthForm::StopOrderQtyChanged( nsOrderMessageDefine::MarketEnum Market,
									  const String& Symbol, double StopPx,
									  nsOrderMessageDefine::SideEnum Side,
									  double Px, int Qty,
									  nsOrderMessageDefine::OrderTypeEnum OrderType )
{
	if( Symbol == FSym )
		OrderBookList->UpdateStopOrderQty( Side, StopPx, Qty );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::RegisterOrderStore( void )
{
	if( FRegOrderStore == false )
	{
		FRegOrderStore = true;
		gOrderStore->AddOrderQtyListener(FMarket, FEx, FSym, this );
		gOrderStore->AddPositionChangeListener( FMarket, FEx, FSym, OrderBookList );
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::UnregisterOrderStore( void )
{
	if( FRegOrderStore == true )
	{
		FRegOrderStore = false;
		gOrderStore->RemoveOrderQtyListener( this );
		gOrderStore->RemovePositionChangeListener( OrderBookList );
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListNewOrders(TObject *Sender, SideEnum side,
		  TDoubleDynArray &PriceArray, int Qty)
{
	if( gOrderStore->IsReady() == true )
	{
		AnsiString Sym( FSym );

		UFC::BufferedLog::Printf( " -----[OrderBookList::OnNewOrders] Symbol[%s] %d orders.", Sym.c_str(), PriceArray.Length );
		for( register int i = 0; i < PriceArray.Length; i++ )
			 PlaceOrder( side, PriceArray[i], LotsUpDown->Position, nsOrderMessageDefine::otLimit, false );
	}
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListNewOrder(TObject *Sender, SideEnum side,
		  double Price, int Qty)
{
	if( gOrderStore->IsReady() == true )
	{
		AnsiString Sym( FSym );

		UFC::BufferedLog::Printf( " -----[OrderBookList::OnNewOrder] Symbol[%s] Qty[%d]", Sym.c_str(), Qty );
		PlaceOrder( side, Price, LotsUpDown->Position, nsOrderMessageDefine::otLimit, true );
	}
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListNewMarketOrder(TObject *Sender, SideEnum side,
		  int Qty)
{
	if( gOrderStore->IsReady() == true )
		PlaceMarketOrder( side, LotsUpDown->Position, GetTimeInForce( true ) );
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );

}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListNewConditionOrder(TObject *Sender, SideEnum side,
		  double Px, OrderTypeEnum Type, double OrderPrice)
{
	PositionEffectEnum  PE = GetPositionEffect();

	if( gOrderStore->IsReady() == true )
	{
		UFC::BufferedLog::Printf( " -----[OrderBookList::OnStopOrder]" );
		gOrderStore->NewStopOrderRequest( FEx,                            ///< Stop Exchange
										  FSym,                           ///< Stop symbol
										  Px,                             ///< Stop price
										  FMarket,
										  FEx,                            ///< Execute Exchange
										  FSym,                           ///< Execute symbol
										  side,                           ///< Buy/Sell
										  OrderPrice,                     ///< Execute price
										  LotsPerOrderEdit->Text.ToInt(), ///< Qty
										  Type,
										  (Type == nsOrderMessageDefine::otMarket)?nsOrderMessageDefine::tifIOC:nsOrderMessageDefine::tifROD,
										  PE,                             ///< Position effect. ///<nsOrderMessageDefine::peAuto, ///< Position effect.
										  FCallPut,
										  FStrikePX,
										  true );                         ///< client side stop order.
	}
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------//
// Private function for Nuclear BUY/Sell
// to place stop order
//
//---------------------------------------------------------------------------
void __fastcall TDepthForm::NewStopOrder( SideEnum side, double Px, int Qty )
{
	PositionEffectEnum  PE = GetPositionEffect();
	OrderTypeEnum       Type;
	double              ExecPx = OrderBookList->GetStopExecutePrice( side, Px );

	if( side == sBuy )
		Type = 	OrderBookList->BuyStopOrderType;
	else
		Type = 	OrderBookList->SellStopOrderType;
	gOrderStore->NewStopOrderRequest( FEx,                            ///< Stop Exchange
									  FSym,                           ///< Stop symbol
									  Px,                             ///< Stop price
									  FMarket,
									  FEx,                            ///< Execute Exchange
									  FSym,                           ///< Execute symbol
									  side,                           ///< Buy/Sell
									  ExecPx,                     ///< Execute price
									  Qty, ///< Qty
									  Type,
									  (Type == nsOrderMessageDefine::otMarket)?nsOrderMessageDefine::tifIOC:nsOrderMessageDefine::tifROD,
									  PE,                             ///< Position effect. ///<nsOrderMessageDefine::peAuto, ///< Position effect.
									  FCallPut,
									  FStrikePX,
									  true );                         ///< client side stop order.
}

//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListDeleteStopOrder(TObject *Sender, SideEnum side,
		  double StopPx)
{
	if( gOrderStore->IsReady() == true )
		gOrderStore->CancelStopOrder( FEx, FSym, StopPx, side );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListReduceQty(TObject *Sender, SideEnum side,
		  double Price, int Qty)
{
	try
	{
		if( gOrderStore->IsReady() == true )
			gOrderStore->CancelOrder( FMarket, FEx, FSym, side, Price);
		else
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
	}
	catch( UFC::Exception& execp )
	{
		String Msg;
		Msg.printf( L"刪單失敗[%s]", execp.what() );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, Msg ); ///< 刪單失敗
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListDeleteAllOrders(TObject *Sender, SideEnum side,
		  const TDoubleDynArray &PriceArray, int &DelCount)
{
	int CxCount = 0;
	int PxCount = PriceArray.Length;

	try
	{	///< Cancel all limit orders.
		for( register int i = 0; i < PxCount; i ++ )
		{
			 CxCount += gOrderStore->CancelOrder( FMarket, FEx, FSym, side, PriceArray[i] );
			 Application->ProcessMessages();
		}
		///< Cancel all Market Orders.( Px = 0.0 )
		CxCount += gOrderStore->CancelOrder( FMarket, FEx, FSym, side, 0.0, true );
	}
	catch( UFC::Exception& execp )
	{
		String Msg;
		Msg.printf( L"刪單失敗[%s]", execp.what() );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, Msg ); ///< 刪單失敗
	}
	DelCount = CxCount;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListReplacePx(TObject *Sender, SideEnum side,
          double Price, double NewPrice)
{
	String Msg;
	ExchangeInfo*  ExInfo;

	try
	{
		if( (ExInfo = OrderBookList->Store->GetExchangeInfo( FEx ))!= NULL )
		{
			if( ExInfo->CanReplacePx() == true )
			{
				gOrderStore->ReplaceOrderPrice( FMarket, FEx, FSym, side, Price, NewPrice, true );
				return;
			}
			else
				Msg.printf( L"改價失敗,交易所[%s]不支援改價", FEx );
		}
		else
			Msg.printf( L"改價失敗,交易所[%s]不存在", FEx );
	}
	catch( UFC::Exception& execp )
	{
		Msg.printf( L"改價失敗,原因[%s]", execp.what() );
	}
	TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, Msg ); ///< 改價失敗
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListNetPositionUpdate(TObject *Sender, int NetPosition,
		  double Profitint, int BuyOpenQty, int SellOpenQty)
{
	String PosStr;
	bool  PosClosed = false;

	if( FNetPos != 0 && NetPosition == 0 )
		PosClosed = true;
	FNetPos = NetPosition;
	PosStr.printf( L"%d", NetPosition );
	if( FNetPos > 0 )
		PosLabel->Color = OrderBookList->TickUpperColor;
	else if( FNetPos < 0 )
		PosLabel->Color = OrderBookList->TickLowerColor;
	else
		PosLabel->Color = clBlack;
	PosLabel->Caption = PosStr;
	if( PosClosed == true )
		OnPositionClosed();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListProfitAndAvgPxUpdate(TObject *Sender, double NetPositionProfit,
		  double AvgPx)
{
	Currency Balance( NetPositionProfit );
	String   DspStr;

	OrderBookList->AvgFillPx = AvgPx;
	ProfitLabel->Caption = CurrToStrF( Balance, ffNumber, 2 );
	DspStr.printf( L"%.*lf", FDigit + 2, AvgPx );
	AvgPxLabel->Caption = DspStr;
	UpdateSymbolListCount();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::EditKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
	TEdit *edit 	= static_cast<TEdit*>(Sender);
	TUpDown *updown = reinterpret_cast<TUpDown*>(edit->Tag);
	String ErrMsg;
	try
	{
		int Qty;
		if( !TryStrToInt(edit->Text, Qty) )
		{
			edit->Text = String( updown->Min );
			return;
		}

		if( Qty > updown->Max )
		{
			edit->Text = String( updown->Min );
			ErrMsg.printf( L"輸入的數量[%d],超過數量上限[%d]", Qty, updown->Max );
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName,  ErrMsg );
			return;
		}
		if( Qty < updown->Min  )
		{
			edit->Text = String( updown->Min );
			ErrMsg.printf( L"輸入的數量[%d],超過數量下限[%d]", Qty, updown->Min );
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName,  ErrMsg );
			return;
		}

		edit->Text = String( Qty );
	}
	catch(...)
	{
		edit->Text = String( updown->Min );
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::CancelAllAndClose( bool UsingStep )
{
	double ClosePx;
	int    dir;
	nsOrderMessageDefine::SideEnum Side;

	if( FNetPos < 0 )
	{
		dir   = -1;
		Side  = nsOrderMessageDefine::sBuy;
	}
	else
	{
		dir   = 1;
		Side  = nsOrderMessageDefine::sSell;
	}
	int step = 0;
	if(UsingStep)
	{
		step = dir* MainForm->CloseBetterTick();
	}
	ClosePx = OrderBookList->GetTickPrice( step ); ///< Get price.
	OrderBookList->DeleteAllOrders( false );
	///< Place new order to close positions.
	if( FNetPos != 0 )
		PlaceOrder( Side, ClosePx, dir * FNetPos, nsOrderMessageDefine::otLimit , false  );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::NuclearBuy( void )
{
	if( gOrderStore->IsReady() == true )
	{
		CancelAllAndClose(true);
		OrderBookList->DeleteAllStopOrders();
		TDoubleDynArray UpPrices,DownPrices,ProfitPrices;
		int Lots,AddLots;

		///< Better price stop order.
		Lots = FNuclearUpperLots/UpCountUpDown->Position;
		AddLots = FNuclearUpperLots - (UpCountUpDown->Position*Lots);
		OrderBookList->GetPrices( true,  UpPrices, UpCountUpDown->Position, UpStepUpDown->Position, 0  );
		for( register int i = 0; i < UpPrices.Length; i++ )
		{
			if( i == 0 )
				NewStopOrder( sBuy, UpPrices[i], Lots + AddLots );
			else
				NewStopOrder( sBuy, UpPrices[i], Lots );
		}
		///< Lower price New Order.
		Lots = FNuclearLowerLots/DownCountUpDown->Position;
		AddLots = FNuclearLowerLots - (DownCountUpDown->Position*Lots);
		OrderBookList->GetPrices( false, DownPrices, DownCountUpDown->Position, DownStepUpDown->Position, 0  );
		for( register int i = 0; i < DownPrices.Length; i++ )
		{
			if( i == 0 )
				PlaceOrder( sBuy, DownPrices[i], Lots + AddLots, nsOrderMessageDefine::otLimit, false );
			else
				PlaceOrder( sBuy, DownPrices[i], Lots, nsOrderMessageDefine::otLimit, false );
		}
		///< Stop profit orders.
		Lots = NuclearLotsUpDown->Position/ProfitCountUpDown->Position;
		AddLots = NuclearLotsUpDown->Position - (ProfitCountUpDown->Position*Lots);
		OrderBookList->GetPrices( true,  ProfitPrices, ProfitCountUpDown->Position, ProfitStepUpDown->Position, ProfitTickUpDown->Position  );
		for( register int i = 0; i < ProfitPrices.Length; i++ )
		{
			if( i == 0 )
				NewStopOrder( sSell, ProfitPrices[i], Lots + AddLots );
			else
				NewStopOrder( sSell, ProfitPrices[i], Lots );
		}
	}
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::NuclearSell( void )
{
	if( gOrderStore->IsReady() == true )
	{
		CancelAllAndClose(true);
		OrderBookList->DeleteAllStopOrders();
		TDoubleDynArray UpPrices,DownPrices,ProfitPrices;
		int Lots,AddLots;

		///< Better price stop order.
		Lots = FNuclearUpperLots/UpCountUpDown->Position;
		AddLots = FNuclearUpperLots - (UpCountUpDown->Position*Lots);
		OrderBookList->GetPrices( false, UpPrices, UpCountUpDown->Position, UpStepUpDown->Position, 0  );
		for( register int i = 0; i < UpPrices.Length; i++ )
		{
			if( i == 0 )
				NewStopOrder( sSell, UpPrices[i], Lots + AddLots );
			else
				NewStopOrder( sSell, UpPrices[i], Lots );
		}
		///< Lower price New Order.
		Lots = FNuclearLowerLots/DownCountUpDown->Position;
		AddLots = FNuclearLowerLots - (DownCountUpDown->Position*Lots);
		OrderBookList->GetPrices( true, DownPrices, DownCountUpDown->Position, DownStepUpDown->Position, 0  );
		for( register int i = 0; i < DownPrices.Length; i++ )
		{
			if( i == 0 )
				PlaceOrder( sSell, DownPrices[i], Lots + AddLots, nsOrderMessageDefine::otLimit, false );
			else
				PlaceOrder( sSell, DownPrices[i], Lots, nsOrderMessageDefine::otLimit, false );
		}
		///< Stop profit orders.
		Lots = NuclearLotsUpDown->Position/ProfitCountUpDown->Position;
		AddLots = NuclearLotsUpDown->Position - (ProfitCountUpDown->Position*Lots);
		OrderBookList->GetPrices( false, ProfitPrices, ProfitCountUpDown->Position, ProfitStepUpDown->Position, ProfitTickUpDown->Position  );
		for( register int i = 0; i < ProfitPrices.Length; i++ )
		{
			if( i == 0 )
				NewStopOrder( sBuy, ProfitPrices[i], Lots + AddLots );
			else
				NewStopOrder( sBuy, ProfitPrices[i], Lots );
		}
	}
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	int PressNum;

	if( Key == FKeyCxlStop ) ///< Cancel all Stop orders.
	{
		UFC::BufferedLog::Printf( " -----[KeyDown::Cancel all Stop orders] Begin" );
		OrderBookList->DeleteAllStopOrders();
		UFC::BufferedLog::Printf( " -----[KeyDown::Cancel all Stop orders] End" );
		return;
	}
	else if( Key == FKeyCxlAndClose && gNuclear == true ) ///< Cancel all and close it.
	{
		UFC::BufferedLog::Printf( " -----[KeyDown::Cancel all and Close] Begin" );
		CancelAllAndClose(true);
		UFC::BufferedLog::Printf( " -----[KeyDown::Cancel all and Close] End" );
		return;
	}
	else if( Key == FKeyNuclearBuy && gNuclear == true  )
	{
		UFC::BufferedLog::Printf( " -----[KeyDown::Nuclear Buy] Begin" );
		NuclearBuy();
		UFC::BufferedLog::Printf( " -----[KeyDown::Nuclear Buy] End" );
		return;
	}
	else if( Key == FKeyNuclearSell )
	{
		UFC::BufferedLog::Printf( " -----[KeyDown::Nuclear Sell] Begin" );
		NuclearSell();
		UFC::BufferedLog::Printf( " -----[KeyDown::Nuclear Sell] End" );
		return;
	}
	else if( Key == VK_ADD || Key == VK_RETURN ) ///< key in lots finished.
	{
		FLastKeyDownTick -= 1000;
	}
	else if( Key >= '0' && Key <= '9'  )///< Number key
	{
		PressNum = Key - '0';
	}
	else if( Key >= VK_NUMPAD0 && Key <= VK_NUMPAD9  )///< Number key in keypad
	{
		PressNum = Key - VK_NUMPAD0;
	}
	else
		return; ///< Unhandled hotkey.
	DWORD TickNow = GetTickCount();

	if( TickNow - FLastKeyDownTick  > 1000 )
	{
		if( PressNum >= 0 && PressNum <= 9 )
			LotsUpDown->Position = PressNum;
	}
	else
	{
		int NewQty = LotsUpDown->Position*10 + PressNum;

		if( NewQty > LotsUpDown->Max )
		{
			String ErrMsg;

			LotsUpDown->Position = 1;
			ErrMsg.printf( L"輸入的口數[%d],超過口數上限[%d]",NewQty,LotsUpDown->Max );
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName,  ErrMsg );
		}
		else
			LotsUpDown->Position = NewQty;
	}
	FLastKeyDownTick = TickNow;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::EditMouseActivate(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y, int HitTest, TMouseActivate &MouseActivate)
{
	TEdit *edit = static_cast<TEdit*>(Sender);
	edit->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::EditMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	TEdit *edit = static_cast<TEdit*>(Sender);
	edit->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SaveProperty( const String& Profile )
{
	g_Config.SetIntegerProperty( Profile,"Lots", LotsUpDown->Position );
	g_Config.SetBoolProperty( Profile,"Fill", OrderBookList->ShowFilled );
	g_Config.SetBoolProperty( Profile,"AutoCenter", OrderBookList->CenterFillPrice );
	g_Config.SetBoolProperty( Profile,"Stop", OrderBookList->ConditionOrder );
	g_Config.SetBoolProperty( Profile,"DerivedPx", OrderBookList->ShowDerivedPx );
	g_Config.SetBoolProperty( Profile,"FillQty", OrderBookList->ShowFillQty );
	g_Config.SetBoolProperty( Profile,"OrderByOneClick", OrderBookList->OrderByOneClick );
	g_Config.SetBoolProperty( Profile,"CancelByRightClick", OrderBookList->CancelByRightClick );
	g_Config.SetBoolProperty( Profile,"CxBeforeNew", OrderBookList->CancelBeforeNew );
	g_Config.SetBoolProperty( Profile,"EnableSlice", OrderBookList->SetEnable );
	g_Config.SetBoolProperty( Profile,"MKTIOC", ( MarketToggleSwitch->State == tssOn)? true:false );
	g_Config.SetBoolProperty( Profile,"DelOnClosed", (DelOnCloseToggleSwitch->State == tssOn)? true:false );

	g_Config.SetBoolProperty( Profile,"StopUsePercent", AutoStopRadioGroup->ItemIndex );
	g_Config.SetIntegerProperty( Profile,"StopAfterFilledTick", StopTickUpDown->Position );
	g_Config.SetIntegerProperty( Profile,"StopAfterFilledPercent", StopPercentComboBox->ItemIndex );

	g_Config.SetBoolProperty( Profile,"ProfitUsePercent", AutoStopProfitRadioGroup->ItemIndex );
	g_Config.SetIntegerProperty( Profile,"ProfitAfterFilledTick", StopProfitTickUpDown->Position );
	g_Config.SetIntegerProperty( Profile,"ProfitAfterFilledPercent", StopProfitPercentComboBox->ItemIndex );


	g_Config.SetIntegerProperty( Profile,"Step", StepUpDown->Position );
	g_Config.SetIntegerProperty( Profile,"StepCount", StepCountUpDown->Position );
	g_Config.SetIntegerProperty( Profile,"Font", FFontSize );
	g_Config.SetIntegerProperty( Profile ,"BetterStep", OrderBookList->BetterStep );
	g_Config.SetIntegerProperty( Profile ,"SettingPage", PageControl->ActivePageIndex );

	g_Config.SetIntegerProperty( Profile ,"NuclearLots", NuclearLotsUpDown->Position );
	g_Config.SetIntegerProperty( Profile ,"NuclearUCount", UpCountUpDown->Position );
	g_Config.SetIntegerProperty( Profile ,"NuclearUStep", UpStepUpDown->Position );
	g_Config.SetIntegerProperty( Profile ,"NuclearRatio", RatioComboBox->ItemIndex );
	g_Config.SetIntegerProperty( Profile ,"NuclearDCount", DownCountUpDown->Position );
	g_Config.SetIntegerProperty( Profile ,"NuclearDStep", DownStepUpDown->Position );
	g_Config.SetIntegerProperty( Profile ,"NuclearPTicks", ProfitTickUpDown->Position );
	g_Config.SetIntegerProperty( Profile ,"NuclearPCount", ProfitCountUpDown->Position );
	g_Config.SetIntegerProperty( Profile ,"NuclearPStep", ProfitStepUpDown->Position );

	g_Config.SetIntegerProperty( Profile ,"ExchangeComboBox", ExchangeComboBox->ItemIndex);
	SaveTFT();
	SaveOCOSetting();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ApplyStopTick( int StopTick, int ProfitTick )
{
	double StopPercent   = StopPercentComboBox->Text.ToDouble();
	double ProfitPercent = StopProfitPercentComboBox->Text.ToDouble();

	if( FilledStopToggleSwitch->State == tssOn )
		OrderBookList->AutoStopLoss = true;
	else
		OrderBookList->AutoStopLoss = false;
	if( AutoStopRadioGroup->ItemIndex == 1 )
		OrderBookList->StopLossUseTick = false;
	else
		OrderBookList->StopLossUseTick = true;
	OrderBookList->StopLossTick = StopTick;
	OrderBookList->StopLossPercent = StopPercent;


	if( FilledStopProfitToggleSwitch->State == tssOn )
		OrderBookList->AutoTakeProfit = true;
	else
		OrderBookList->AutoTakeProfit = false;
	if( AutoStopProfitRadioGroup->ItemIndex == 1 )
		OrderBookList->TakeProfitUseTick = false;
	else
		OrderBookList->TakeProfitUseTick = true;
	OrderBookList->TakeProfitTick  = ProfitTick;
	OrderBookList->TakeProfitPercent = ProfitPercent;
}
//---------------------------------------------------------------------------
bool __fastcall TDepthForm::LoadProperty( const String& Profile )
{
	FProfile = Profile;
	LotsUpDown->Position = g_Config.GetIntegerProperty( Profile,"Lots", 1 );
	OrderBookList->ShowFilled      = g_Config.GetBoolProperty( Profile,"Fill", false );
	OrderBookList->CenterFillPrice = g_Config.GetBoolProperty( Profile,"AutoCenter", true );
	OrderBookList->ConditionOrder  = g_Config.GetBoolProperty( Profile,"Stop", false );
	OrderBookList->ShowDerivedPx   = g_Config.GetBoolProperty( Profile,"DerivedPx", false );
	OrderBookList->ShowFillQty     = g_Config.GetBoolProperty( Profile,"FillQty", false );
	OrderBookList->OrderByOneClick = g_Config.GetBoolProperty( Profile,"OrderByOneClick", true );
	OrderBookList->CancelByRightClick = g_Config.GetBoolProperty( Profile,"CancelByRightClick", true );
	OrderBookList->CancelBeforeNew = g_Config.GetBoolProperty( Profile,"CxBeforeNew", false );
	OrderBookList->SetEnable       = g_Config.GetBoolProperty( Profile,"EnableSlice", false );
	MarketToggleSwitch->State      = (g_Config.GetBoolProperty( Profile,"MKTIOC", true ) == true)? tssOn:tssOff;
	FilledStopToggleSwitch->State  = tssOff;
	FilledStopProfitToggleSwitch->State  = tssOff;
	DelOnCloseToggleSwitch->State  = (g_Config.GetBoolProperty( Profile,"DelOnClosed", false ) == true)? tssOn:tssOff;

	AutoStopRadioGroup->ItemIndex  = g_Config.GetBoolProperty( Profile,"StopUsePercent", false ); ///< Use Tick
	StopTickUpDown->Position       = g_Config.GetIntegerProperty( Profile, "StopAfterFilledTick", 10  ); ///< 10 Ticks
	StopPercentComboBox->ItemIndex = g_Config.GetIntegerProperty( Profile,"StopAfterFilledPercent", 5 ); ///< 3%

	AutoStopProfitRadioGroup->ItemIndex  = g_Config.GetBoolProperty( Profile,"ProfitUsePercent", false ); ///< Use Tick
	StopProfitTickUpDown->Position       = g_Config.GetIntegerProperty( Profile, "ProfitAfterFilledTick", 10  ); ///< 10 Ticks
	StopProfitPercentComboBox->ItemIndex = g_Config.GetIntegerProperty( Profile,"ProfitAfterFilledPercent", 5 ); ///< 3%

	OrderBookList->Step            = g_Config.GetIntegerProperty( Profile ,"Step", 1 );
	OrderBookList->StepCount       = g_Config.GetIntegerProperty( Profile ,"StepCount", 0 );
	FFontSize                      = g_Config.GetIntegerProperty( Profile, "Font", FONT_SIZE_DEF );
	OrderBookList->BetterStep      = g_Config.GetIntegerProperty( Profile ,"BetterStep", 1 );
	PageControl->ActivePageIndex   = g_Config.GetIntegerProperty( Profile ,"SettingPage", 0 );

	NuclearLotsUpDown->Position  = g_Config.GetIntegerProperty( Profile ,"NuclearLots", 10 );
	UpCountUpDown->Position      = g_Config.GetIntegerProperty( Profile ,"NuclearUCount", 4 );
	UpStepUpDown->Position       = g_Config.GetIntegerProperty( Profile ,"NuclearUStep", 2 );
	RatioComboBox->ItemIndex     = g_Config.GetIntegerProperty( Profile ,"NuclearRatio", 1 );
	DownCountUpDown->Position    = g_Config.GetIntegerProperty( Profile ,"NuclearDCount", 2 );
	DownStepUpDown->Position     = g_Config.GetIntegerProperty( Profile ,"NuclearDStep", 2 );
	ProfitTickUpDown->Position   = g_Config.GetIntegerProperty( Profile ,"NuclearPTicks", 30 );
	ProfitCountUpDown->Position  = g_Config.GetIntegerProperty( Profile ,"NuclearPCount", 2 );
	ProfitStepUpDown->Position   = g_Config.GetIntegerProperty( Profile ,"NuclearPStep", 3 );

	FillToggleSwitch->State = ( OrderBookList->ShowFilled == true)?tssOn:tssOff;
	AutoCenterSwitch->State = ( OrderBookList->CenterFillPrice == true)?tssOn:tssOff;
	StopToggleSwitch->State = ( OrderBookList->ConditionOrder == true)?tssOn:tssOff;
	DrivedToggleSwitch->State = ( OrderBookList->ShowDerivedPx == true)?tssOn:tssOff;
	FillQtyToggleSwitch->State = ( OrderBookList->ShowFillQty == true)?tssOn:tssOff;
	OrderByOneClickSwitch->State = (OrderBookList->OrderByOneClick == true)?tssOn:tssOff;
	CancelByRightClickSwitch->State = (OrderBookList->CancelByRightClick == true)?tssOn:tssOff;
	CxlBeforeNewToggleSwitch->State = ( OrderBookList->CancelBeforeNew == true)?tssOn:tssOff;
	SliceOrderSwitch->State = ( OrderBookList->SetEnable == true)?tssOn:tssOff;
	StepUpDown->Position = OrderBookList->Step;
	StepCountUpDown->Position = OrderBookList->StepCount;
	LoadCustomList();
    int ExchangeComboBoxIdx = g_Config.GetIntegerProperty( Profile ,"ExchangeComboBox", 0 );
	InitExchangeComboBox(ExchangeComboBoxIdx);
	InitSmartOrderTabs(0);
	ApplyStopTick( StopTickUpDown->Position, StopProfitTickUpDown->Position );
	RatioComboBoxChange( NULL );
	FilledStopToggleSwitchClick( NULL );
	SliceOrderSwitchClick( NULL );
	LoadColor();
	LoadStopSetting();
	LoadTFT();
	LoadOCOSetting();
	AdjuestFont( );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LoadTFT( void )
{
	///< Split Qty
	SplitExpBtn->ImageIndex    = g_Config.GetIntegerProperty( FProfile,"SplitExp", 1 );
	SplitToggleSwitch->State   = (g_Config.GetBoolProperty( FProfile,"Split", false ) == true)? tssOn:tssOff;
	SplitUpDown->Position      = g_Config.GetIntegerProperty( FProfile ,"SplitQty", 2 );
	SetExpBtnPanel( SplitExpBtn );
	///< Bull in
	BullInExpBtn->ImageIndex    = g_Config.GetIntegerProperty( FProfile,"BullInExp", 1 );
	BullInToggleSwitch->State   = tssOff;
	BullInOnceCheckBox->Checked = g_Config.GetBoolProperty( FProfile,"BullInOnce", false );
	BullInOnceUpDown->Position  = g_Config.GetIntegerProperty( FProfile,"BullInOnceQty", 199 );
	BullInAccumulateCheckBox->Checked = g_Config.GetBoolProperty( FProfile,"BullInAccumulate", true );
	BullInAccumulateTimeComboBox->ItemIndex = g_Config.GetIntegerProperty( FProfile,"BullInAccumulateTime", 4 );
	BullInAccumulateUpDown->Position = g_Config.GetIntegerProperty( FProfile,"BullInAccumulateQty", 499 );
	SetExpBtnPanel( BullInExpBtn );
	///< Bull cancel
	BullOutExpBtn->ImageIndex  = g_Config.GetIntegerProperty( FProfile,"BullOutExp", 1 );
	BullOutToggleSwitch->State = tssOff;
	BullOutOnceCheckBox->Checked = g_Config.GetBoolProperty( FProfile,"BullOutOnce", false );
	BullOutOnceUpDown->Position  = g_Config.GetIntegerProperty( FProfile,"BullOutOnceQty", 199 );
	BullOutAccumulateCheckBox->Checked = g_Config.GetBoolProperty( FProfile,"BullOutAccumulate", true );
	BullOutAccumulateTimeComboBox->ItemIndex = g_Config.GetIntegerProperty( FProfile,"BullInAccumulateTime", 4 );
	BullOutAccumulateUpDown->Position = g_Config.GetIntegerProperty( FProfile,"BullOutAccumulateQty", 499 );
	BullOutTickUpDown->Position = g_Config.GetIntegerProperty( FProfile,"BullOutTick", 2 );
	SetExpBtnPanel( BullOutExpBtn );
	///< Auto depth
	DepthExpBtn->ImageIndex    = g_Config.GetIntegerProperty( FProfile,"DepthExp", 1 );
	DepthCancelToggleSwitch->State = tssOff;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SaveTFT( void )
{
	///< Split Qty
	g_Config.SetIntegerProperty( FProfile,"SplitExp", SplitExpBtn->ImageIndex );
	g_Config.SetBoolProperty( FProfile,"Split", ( SplitToggleSwitch->State == tssOn)? true:false );
	g_Config.SetIntegerProperty( FProfile ,"SplitQty", SplitUpDown->Position );
	///< Bull in
	g_Config.SetIntegerProperty( FProfile,"BullInExp", BullInExpBtn->ImageIndex );
	g_Config.SetBoolProperty( FProfile,"BullInOnce", BullInOnceCheckBox->Checked );
	g_Config.SetIntegerProperty( FProfile,"BullInOnceQty", BullInOnceUpDown->Position );
	g_Config.SetBoolProperty( FProfile,"BullInAccumulate", BullInAccumulateCheckBox->Checked );
	g_Config.SetIntegerProperty( FProfile,"BullInAccumulateTime", BullInAccumulateTimeComboBox->ItemIndex );
	g_Config.SetIntegerProperty( FProfile,"BullInAccumulateQty", BullInAccumulateUpDown->Position );
	///< Bull cancel
	g_Config.SetIntegerProperty( FProfile,"BullOutExp", BullOutExpBtn->ImageIndex );
	g_Config.SetBoolProperty( FProfile,"BullOutOnce", BullOutOnceCheckBox->Checked );
	g_Config.SetIntegerProperty( FProfile,"BullOutOnceQty", BullOutOnceUpDown->Position );
	g_Config.SetBoolProperty( FProfile,"BullOutAccumulate", BullOutAccumulateCheckBox->Checked );
	g_Config.SetIntegerProperty( FProfile,"BullOutAccumulateTime", BullOutAccumulateTimeComboBox->ItemIndex );
	g_Config.SetIntegerProperty( FProfile,"BullOutAccumulateQty", BullOutAccumulateUpDown->Position );
	g_Config.SetIntegerProperty( FProfile,"BullOutTick", BullOutTickUpDown->Position );
	///< Auto depth
	g_Config.SetIntegerProperty( FProfile,"DepthExp", DepthExpBtn->ImageIndex );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SetExpBtnPanel( TButton* ExpBtn)
{
	TPanel*  FuncPanel;

	if( FExpBtn2Panel.GetObjectByKey( ExpBtn,FuncPanel ) == true )
	{
		if( ExpBtn->ImageIndex == 0 ) ///< Opened ( - icon)
			 FuncPanel->Height = ExpBtn->Tag;
		else ///< Minimize ( + icon)
			 FuncPanel->Height = 55;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LoadHotkey( void )
{
	OrderBookList->KeyBuy  = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKBuy", 0 )];
	OrderBookList->KeySell = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKSell", 0 )];
	OrderBookList->KeyBuyMarket     = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKBuyMkt", 0 )];
	OrderBookList->KeySellMarket    = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKSellMkt", 0 )];
	OrderBookList->KeyBetterDecrese = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKDeccBetter", 0 )];
	OrderBookList->KeyBetterIncrese = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKIncBetter", 0 )];
	OrderBookList->KeyCancelAll     = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKDelAll", 0 )];
	OrderBookList->KeyCenterFillPx  = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKCenterFill", 0 )];
	FKeyCxlStop                     = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKCancelAllStop", 0 )];
	FKeyCxlAndClose                 = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKCancelAllClose", 0 )];
	FKeyNuclearBuy                  = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKNuclearBuy", 0 )];
	FKeyNuclearSell                 = HotkeyCode[g_Config.GetIntegerProperty("Setting","HKNuclearSell", 0 )];

	String Lots( g_Config.GetIntegerProperty("Setting","LotsMax", 200 ) );
	int index = LotsComboBox->Items->IndexOf( Lots );
	if( index == -1 )
		LotsComboBox->ItemIndex = LotsComboBox->Items->Count -1;
	else
		LotsComboBox->ItemIndex = index;
	LotsUpDown->Max = LotsComboBox->Text.ToInt();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SmallButtonClick(TObject *Sender)
{
	FFontSize--;
	AdjuestFont( );
	AlignLabels();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LargeButtonClick(TObject *Sender)
{
	FFontSize++;
	AdjuestFont( );
	AlignLabels();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ShowKBarButtonClick(TObject *Sender)
{
	ContractViewerForm->OpenKBarForm( FEx, FSym );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::AdjuestFont( void )
{
	TFont* Font = new TFont();
	if( FFontSize < FONT_SIZE_MIN )
		FFontSize = FONT_SIZE_MIN;
	if( FFontSize > FONT_SIZE_MAX )
		FFontSize = FONT_SIZE_MAX;
	Font->Name  = OrderBookList->Font->Name;
	Font->Style = OrderBookList->Font->Style;
	Font->Size  = FFontSize;
	OrderBookList->Font = Font;
	Canvas->Font = Font;
	RoundFormEx->CaptionFont->Size  = FFontSize;
	NetPosText->Font->Size  = FFontSize + FPostSizeOffset;
	Canvas->Font->Size = FFontSize + FPostSizeOffset;
	NetPosText->Width  = Canvas->TextWidth( L"淨部位   ");
	NetPosText->Height = Canvas->TextHeight( L"淨部位   ");
	Canvas->Font->Size = FFontSize;
	NetPosText->Top = ((pnlToolbar->Height - Bevel4->Height) - NetPosText->Height )/2;
	PosLabel->Font->Size  = FFontSize;
	PosLabel->SetBounds( NetPosText->Left + NetPosText->Width,
						 NetPosText->Top,
						 Canvas->TextWidth( L"1000"),
						 NetPosText->Height );
	ProfitPanel->Height = NetPosText->Height;
	SmallButton->Left = PosLabel->Left + PosLabel->Width + 5;
	LargeButton->Left = SmallButton->Left + SmallButton->Width + 2;

	AvgPxTextLabel->Font->Size  = FFontSize;
	AvgPxLabel->Font->Size  = FFontSize;
	AvgPxLabel->Width = Canvas->TextWidth( L"00000.0000");
	ProfitTextLabel->Font->Size  = FFontSize;
	ProfitLabel->Font->Size  = FFontSize;
	ProfitLabel->Width = Canvas->TextWidth( L"00000000.00");
	TIFComboBox->Font->Size  = FFontSize - 2;
	TIFComboBox->Width = Canvas->TextWidth( L"ROD  ") + NetPosText->Height;
	MarketBuyLabel->Font->Size  = FFontSize;
	CenterLabel->Font->Size  = FFontSize;
	MarketSellLabel->Font->Size  = FFontSize;
	delete Font;
	if( ToolSV->Opened )
		SetWidth( ToolSV->OpenedWidth );
	else
		SetWidth( 0 );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SetEditTag( void )
{
	LotsPerOrderEdit	->Tag = reinterpret_cast<NativeInt>(LotsUpDown);
	StepEdit			->Tag =	reinterpret_cast<NativeInt>(StepUpDown);
	StepCountEdit		->Tag = reinterpret_cast<NativeInt>(StepCountUpDown);
	Edit1				->Tag =	reinterpret_cast<NativeInt>(NuclearLotsUpDown);
	Edit2				->Tag =	reinterpret_cast<NativeInt>(UpCountUpDown);
	Edit3				->Tag =	reinterpret_cast<NativeInt>(UpStepUpDown);
	Edit4				->Tag = reinterpret_cast<NativeInt>(DownCountUpDown);
	Edit5				->Tag = reinterpret_cast<NativeInt>(DownStepUpDown);
	Edit6				->Tag = reinterpret_cast<NativeInt>(ProfitTickUpDown);
	Edit7				->Tag = reinterpret_cast<NativeInt>(ProfitStepUpDown);
	Edit8				->Tag = reinterpret_cast<NativeInt>(ProfitCountUpDown);
	Edit9				->Tag = reinterpret_cast<NativeInt>(SplitUpDown);
	Edit10				->Tag = reinterpret_cast<NativeInt>(BullOutTickUpDown);
	Edit12				->Tag = reinterpret_cast<NativeInt>(BullInAccumulateUpDown);
	Edit13				->Tag = reinterpret_cast<NativeInt>(BullInOnceUpDown);
	Edit15				->Tag = reinterpret_cast<NativeInt>(BullOutAccumulateUpDown);
	Edit16          	->Tag = reinterpret_cast<NativeInt>(BullOutOnceUpDown);
	StopTickEdit    	->Tag = reinterpret_cast<NativeInt>(StopTickUpDown);
	StopProfitTickEdit	->Tag =	reinterpret_cast<NativeInt>(StopProfitTickUpDown);
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::UpdateBalance( double Net )
{
	//Currency Balance( Net );
	//NetBalanceText->Caption = CurrToStrF( Balance, ffNumber , g_Config.GetBalanceDisplayDigi() );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListFocus(TObject *Sender)
{
	 ContractViewerForm->ActiveClient( this );
	 BringToFront();
	 if( FMarket ==	nsOrderMessageDefine::mTWFutures || FMarket == nsOrderMessageDefine::mTWOptions )
		 MainForm->UpdateAccount( true );
	 else
		 MainForm->UpdateAccount( false );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SliceOrderSwitchClick(TObject *Sender)
{
	if( SliceOrderSwitch->State == tssOff )
	{
		OrderBookList->SetEnable = false;
		StepCountEdit->Enabled = false;
		StepCountUpDown->Enabled = false;
		StepEdit->Enabled = false;
		StepUpDown->Enabled = false;
		CountLabel->Enabled = false;
		TickLabel->Enabled = false;
	}
	else
	{
		OrderBookList->SetEnable = true;
		StepCountEdit->Enabled = true;
		StepCountUpDown->Enabled = true;
		StepEdit->Enabled = true;
		StepUpDown->Enabled = true;
		CountLabel->Enabled = true;
		TickLabel->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::StepCountUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	OrderBookList->StepCount = NewValue;
	AllowChange = true;
}
//---------------------------------------------------------------------------

void __fastcall TDepthForm::StepUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	OrderBookList->Step = NewValue;
	AllowChange = true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::StopTickUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	ApplyStopTick( NewValue, StopProfitTickUpDown->Position );
	AllowChange = true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::StopProfitTickUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	ApplyStopTick( StopTickUpDown->Position, NewValue );
	AllowChange = true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::StopPercentComboBoxChange(TObject *Sender)
{
	ApplyStopTick( StopTickUpDown->Position, StopProfitTickUpDown->Position );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::AutoStopRadioGroupClick(TObject *Sender)
{
	ApplyStopTick( StopTickUpDown->Position, StopProfitTickUpDown->Position );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LoadColor( void )
{
	int DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );

	LoadDefColor( DorL );
	///< Fix row colors
	OrderBookList->FixedRowBidColor  = FixedRowBidColor[DorL];
	OrderBookList->FixedRowAskColor  = FixedRowAskColor[DorL];
	OrderBookList->FixedRowColor     = FixedRowColor[DorL];
	OrderBookList->FixedRowBKColor   = FixedRowBKColor[DorL];
	OrderBookList->GridColor	     = GridColor[DorL];
	///< Tick row colors
	OrderBookList->TickBKColor       = TickBKColor[DorL];
	OrderBookList->TickUpperColor    = TickUpperColor[DorL];
	OrderBookList->TickLowerColor    = TickLowerColor[DorL];
	OrderBookList->FillColor         = FillColor[DorL];
	OrderBookList->FillBKColor       = FillBKColor[DorL];
	OrderBookList->BetterBuyColor    = BetterBuyColor[DorL];
	OrderBookList->BetterBuyBKColor  = BetterBuyBKColor[DorL];
	OrderBookList->BetterSellColor   = BetterSellColor[DorL];
	OrderBookList->BetterSellBKColor = BetterSellBKColor[DorL];
	OrderBookList->DayHColor         = DayHiColor[DorL];
	OrderBookList->DayLColor         = DayLoColor[DorL];
	OrderBookList->AvgPxColor        = AvgPriceColor[DorL];
	///< Buy side color
	OrderBookList->BidDepth5BKColor  = BidDepth5BKColor[DorL];
	OrderBookList->BidDepth5Color    = BidDepth5Color[DorL];
	OrderBookList->BuyBKColor        = BuyBKColor[DorL];
	OrderBookList->BuyColor          = BuyColor[DorL];
	OrderBookList->BuyFillBKColor    = BuyFillBKColor[DorL];
	OrderBookList->BuyFillColor      = BuyFillColor[DorL];
	OrderBookList->BuyConditionColBKColor = BuyConditionColBKColor[DorL];
	OrderBookList->BuyConditionColColor   = BuyConditionColColor[DorL];
	OrderBookList->BuyOCOColColor    = BuyOCOColColor[DorL];
	OrderBookList->BuyOCOColBKColor  = BuyOCOColBKColor[DorL];
	OrderBookList->BuyOCODelColBKColor=BuyOCODelColBKColor[DorL];
	///<Sell side color
	OrderBookList->AskDepth5BKColor  = AskDepth5BKColor[DorL];
	OrderBookList->AskDepth5Color    = AskDepth5Color[DorL];
	OrderBookList->SellBKColor       = SellBKColor[DorL];
	OrderBookList->SellColor         = SellColor[DorL];
	OrderBookList->SellFillBKColor   = SellFillBKColor[DorL];
	OrderBookList->SellFillColor     = SellFillColor[DorL];
	OrderBookList->SellConditionColBKColor = SellConditionColBKColor[DorL];
	OrderBookList->SellConditionColColor   = SellConditionColColor[DorL];
	OrderBookList->SellOCOColColor   = SellOCOColColor[DorL];
	OrderBookList->SellOCOColBKColor = SellOCOColBKColor[DorL];
	OrderBookList->SellOCODelColBKColor = SellOCODelColBKColor[DorL];
	OrderBookList->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LoadOCOSetting( void )
{
	FOCOType = g_Config.GetIntegerProperty( "Setting", "OCOType", 0 );
	FLimitOrderTick = g_Config.GetIntegerProperty( "Setting", "LimitOrderTick", 0 );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SaveDefColor( void  )
{
	String Name;
	int    DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );

	if( DorL == true )
		Name = L"DepthDarkColor";
	else
		Name = L"DepthLightColor";
	SaveColorToConfig( Name, DorL );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SaveColorToConfig( const String& Name, bool DorL )
{
	///< Fix row colors
    if (FixedRowBidColor[DorL] != OrderBookList->FixedRowBidColor) {
        g_Config.SetIntegerProperty(Name, "FixRowBColor", OrderBookList->FixedRowBidColor);
        FixedRowBidColor[DorL] = OrderBookList->FixedRowBidColor;
    }
    if (FixedRowAskColor[DorL] != OrderBookList->FixedRowAskColor) {
        g_Config.SetIntegerProperty(Name, "FixRowSColor", OrderBookList->FixedRowAskColor);
        FixedRowAskColor[DorL] = OrderBookList->FixedRowAskColor;
    }
    if (FixedRowColor[DorL] != OrderBookList->FixedRowColor) {
        g_Config.SetIntegerProperty(Name, "FixRowColor", OrderBookList->FixedRowColor);
        FixedRowColor[DorL] = OrderBookList->FixedRowColor;
    }
    if (FixedRowBKColor[DorL] != OrderBookList->FixedRowBKColor) {
        g_Config.SetIntegerProperty(Name, "FixRowBKColor", OrderBookList->FixedRowBKColor);
        FixedRowBKColor[DorL] = OrderBookList->FixedRowBKColor;
    }
    if (GridColor[DorL] != OrderBookList->GridColor) {
        g_Config.SetIntegerProperty(Name, "GridLineColor", OrderBookList->GridColor);
        GridColor[DorL] = OrderBookList->GridColor;
    }

    ///< Tick row colors
    if (TickBKColor[DorL] != OrderBookList->TickBKColor) {
        g_Config.SetIntegerProperty(Name, "TickColBKColor", OrderBookList->TickBKColor);
        TickBKColor[DorL] = OrderBookList->TickBKColor;
    }
    if (TickUpperColor[DorL] != OrderBookList->TickUpperColor) {
        g_Config.SetIntegerProperty(Name, "TickUpperColor", OrderBookList->TickUpperColor);
        TickUpperColor[DorL] = OrderBookList->TickUpperColor;
    }
    if (TickLowerColor[DorL] != OrderBookList->TickLowerColor) {
        g_Config.SetIntegerProperty(Name, "TickLowerColor", OrderBookList->TickLowerColor);
        TickLowerColor[DorL] = OrderBookList->TickLowerColor;
    }
    if (FillColor[DorL] != OrderBookList->FillColor) {
        g_Config.SetIntegerProperty(Name, "FillCellColor", OrderBookList->FillColor);
        FillColor[DorL] = OrderBookList->FillColor;
    }
    if (FillBKColor[DorL] != OrderBookList->FillBKColor) {
        g_Config.SetIntegerProperty(Name, "FillCellBKColor", OrderBookList->FillBKColor);
        FillBKColor[DorL] = OrderBookList->FillBKColor;
    }
    if (BetterBuyColor[DorL] != OrderBookList->BetterBuyColor) {
        g_Config.SetIntegerProperty(Name, "BetterBuyColor", OrderBookList->BetterBuyColor);
        BetterBuyColor[DorL] = OrderBookList->BetterBuyColor;
    }
    if (BetterBuyBKColor[DorL] != OrderBookList->BetterBuyBKColor) {
        g_Config.SetIntegerProperty(Name, "BetterBuyBKColor", OrderBookList->BetterBuyBKColor);
        BetterBuyBKColor[DorL] = OrderBookList->BetterBuyBKColor;
    }
    if (BetterSellColor[DorL] != OrderBookList->BetterSellColor) {
        g_Config.SetIntegerProperty(Name, "BetterSellColor", OrderBookList->BetterSellColor);
        BetterSellColor[DorL] = OrderBookList->BetterSellColor;
    }
    if (BetterSellBKColor[DorL] != OrderBookList->BetterSellBKColor) {
        g_Config.SetIntegerProperty(Name, "BetterSellBKColor", OrderBookList->BetterSellBKColor);
        BetterSellBKColor[DorL] = OrderBookList->BetterSellBKColor;
    }
    if (DayHiColor[DorL] != OrderBookList->DayHColor) {
        g_Config.SetIntegerProperty(Name, "DHColor", OrderBookList->DayHColor);
        DayHiColor[DorL] = OrderBookList->DayHColor;
    }
    if (DayLoColor[DorL] != OrderBookList->DayLColor) {
        g_Config.SetIntegerProperty(Name, "DLColor", OrderBookList->DayLColor);
        DayLoColor[DorL] = OrderBookList->DayLColor;
    }
    if (AvgPriceColor[DorL] != OrderBookList->AvgPxColor) {
        g_Config.SetIntegerProperty(Name, "AvgPxColor", OrderBookList->AvgPxColor);
        AvgPriceColor[DorL] = OrderBookList->AvgPxColor;
    }

    ///< Buy side color
    if (BidDepth5BKColor[DorL] != OrderBookList->BidDepth5BKColor) {
        g_Config.SetIntegerProperty(Name, "BuyDepthColBKColor", OrderBookList->BidDepth5BKColor);
        BidDepth5BKColor[DorL] = OrderBookList->BidDepth5BKColor;
    }
    if (BidDepth5Color[DorL] != OrderBookList->BidDepth5Color) {
        g_Config.SetIntegerProperty(Name, "BuyDepthColColor", OrderBookList->BidDepth5Color);
        BidDepth5Color[DorL] = OrderBookList->BidDepth5Color;
    }
    if (BuyBKColor[DorL] != OrderBookList->BuyBKColor) {
        g_Config.SetIntegerProperty(Name, "BuyColBKColor", OrderBookList->BuyBKColor);
        BuyBKColor[DorL] = OrderBookList->BuyBKColor;
    }
    if (BuyColor[DorL] != OrderBookList->BuyColor) {
        g_Config.SetIntegerProperty(Name, "BuyColColor", OrderBookList->BuyColor);
        BuyColor[DorL] = OrderBookList->BuyColor;
    }
    if (BuyFillBKColor[DorL] != OrderBookList->BuyFillBKColor) {
        g_Config.SetIntegerProperty(Name, "BuyFillBKColor", OrderBookList->BuyFillBKColor);
        BuyFillBKColor[DorL] = OrderBookList->BuyFillBKColor;
    }
    if (BuyFillColor[DorL] != OrderBookList->BuyFillColor) {
        g_Config.SetIntegerProperty(Name, "BuyFillColor", OrderBookList->BuyFillColor);
        BuyFillColor[DorL] = OrderBookList->BuyFillColor;
	}
	if (BuyConditionColColor[DorL] != OrderBookList->BuyConditionColColor) {
		g_Config.SetIntegerProperty(Name, "BuyStopColor", OrderBookList->BuyConditionColColor);
		BuyConditionColColor[DorL] = OrderBookList->BuyConditionColColor;
	}
	if (BuyConditionColBKColor[DorL] != OrderBookList->BuyConditionColBKColor) {
		g_Config.SetIntegerProperty(Name, "BuyStopBKColor", OrderBookList->BuyConditionColBKColor);
		BuyConditionColBKColor[DorL] = OrderBookList->BuyConditionColBKColor;
	}
	if (BuyOCOColColor[DorL] != OrderBookList->BuyOCOColColor)
	{
		g_Config.SetIntegerProperty(Name, "BuyOCOColColor", OrderBookList->BuyOCOColColor);
		BuyOCOColColor[DorL] = OrderBookList->BuyOCOColColor;
	}
	if (BuyOCOColBKColor[DorL] != OrderBookList->BuyOCOColBKColor)
	{
		g_Config.SetIntegerProperty(Name, "BuyOCOColBKColor", OrderBookList->BuyOCOColBKColor);
		BuyOCOColBKColor[DorL] = OrderBookList->BuyOCOColBKColor;
	}
	if( BuyOCODelColBKColor[DorL] != OrderBookList->BuyOCODelColBKColor)
	{
		g_Config.SetIntegerProperty(Name, "BuyOCODelColBKColor", OrderBookList->BuyOCODelColBKColor);
		BuyOCODelColBKColor[DorL] = OrderBookList->BuyOCODelColBKColor;
	}

    ///< Sell side color
    if (AskDepth5BKColor[DorL] != OrderBookList->AskDepth5BKColor) {
        g_Config.SetIntegerProperty(Name, "SellDepthColBKColor", OrderBookList->AskDepth5BKColor);
        AskDepth5BKColor[DorL] = OrderBookList->AskDepth5BKColor;
    }
    if (AskDepth5Color[DorL] != OrderBookList->AskDepth5Color) {
        g_Config.SetIntegerProperty(Name, "SellDepthColColor", OrderBookList->AskDepth5Color);
        AskDepth5Color[DorL] = OrderBookList->AskDepth5Color;
    }
    if (SellBKColor[DorL] != OrderBookList->SellBKColor) {
        g_Config.SetIntegerProperty(Name, "SellColBKColor", OrderBookList->SellBKColor);
        SellBKColor[DorL] = OrderBookList->SellBKColor;
    }
    if (SellColor[DorL] != OrderBookList->SellColor) {
        g_Config.SetIntegerProperty(Name, "SellColColor", OrderBookList->SellColor);
        SellColor[DorL] = OrderBookList->SellColor;
    }
    if (SellFillBKColor[DorL] != OrderBookList->SellFillBKColor) {
        g_Config.SetIntegerProperty(Name, "SellFillBKColor", OrderBookList->SellFillBKColor);
        SellFillBKColor[DorL] = OrderBookList->SellFillBKColor;
    }
    if (SellFillColor[DorL] != OrderBookList->SellFillColor) {
        g_Config.SetIntegerProperty(Name, "SellFillColor", OrderBookList->SellFillColor);
        SellFillColor[DorL] = OrderBookList->SellFillColor;
    }
    if (SellConditionColBKColor[DorL] != OrderBookList->SellConditionColBKColor) {
        g_Config.SetIntegerProperty(Name, "SellStopBKColor", OrderBookList->SellConditionColBKColor);
        SellConditionColBKColor[DorL] = OrderBookList->SellConditionColBKColor;
    }
    if (SellConditionColColor[DorL] != OrderBookList->SellConditionColColor) {
        g_Config.SetIntegerProperty(Name, "SellStopColor", OrderBookList->SellConditionColColor);
        SellConditionColColor[DorL] = OrderBookList->SellConditionColColor;
    }
	if (SellOCOColColor[DorL] != OrderBookList->SellOCOColColor) {
		g_Config.SetIntegerProperty(Name, "SellOCOColColor", OrderBookList->SellOCOColColor);
		SellOCOColColor[DorL] = OrderBookList->SellOCOColColor;
	}
	if (SellOCOColBKColor[DorL] != OrderBookList->SellOCOColBKColor) {
		g_Config.SetIntegerProperty(Name, "SellOCOColBKColor", OrderBookList->SellOCOColBKColor);
		SellOCOColBKColor[DorL] = OrderBookList->SellOCOColBKColor;
	}
	if (SellOCODelColBKColor[DorL] != OrderBookList->SellOCODelColBKColor)
	{
		g_Config.SetIntegerProperty(Name, "SellOCODelColBKColor", OrderBookList->SellOCODelColBKColor);
		SellOCODelColBKColor[DorL] = OrderBookList->SellOCODelColBKColor;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SaveOCOSetting( void )
{
	g_Config.SetIntegerProperty( "Setting", "OCOType", FOCOType);
	g_Config.SetIntegerProperty( "Setting", "LimitOrderTick", FLimitOrderTick);
}
//---------------------------------------------------------------------------
TColor __fastcall TDepthForm::GetFixColor( int Index )
{
	switch( Index )
	{
		case 0: return OrderBookList->FixedRowBidColor;
		case 1: return OrderBookList->FixedRowAskColor;
		case 2: return OrderBookList->FixedRowColor;
		case 3: return OrderBookList->GridColor;
		default: return clWhite;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SetFixColor( int Index, TColor Color )
{
	switch( Index )
	{
		case 0: OrderBookList->FixedRowAskColor = Color;break;
		case 1: OrderBookList->FixedRowBidColor = Color;break;
		case 2: OrderBookList->FixedRowColor = Color;break;
		case 3: OrderBookList->GridColor = Color;break;
		default: break;
	}
}
//---------------------------------------------------------------------------
TColor __fastcall TDepthForm::GetTickColor( int Index )
{
	switch( Index )
	{
		case 0: return OrderBookList->TickBKColor;
		case 1: return OrderBookList->TickUpperColor;
		case 2: return OrderBookList->TickLowerColor;
		case 3: return OrderBookList->FillBKColor;
		case 4: return OrderBookList->FillColor;
		case 5: return OrderBookList->BetterBuyBKColor;
		case 6: return OrderBookList->BetterBuyColor;
		case 7: return OrderBookList->BetterSellBKColor;
		case 8: return OrderBookList->BetterSellColor;
		case 9: return OrderBookList->DayHColor;
		case 10: return OrderBookList->DayLColor;
		case 11: return OrderBookList->AvgPxColor;
		default: return clWhite;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SetTickColor( int Index, TColor Color )
{
	switch( Index )
	{
		case 0: OrderBookList->TickBKColor = Color;break;
		case 1: OrderBookList->TickUpperColor = Color;break;
		case 2: OrderBookList->TickLowerColor = Color;break;
		case 3: OrderBookList->FillBKColor = Color;break;
		case 4: OrderBookList->FillColor = Color;break;
		case 5: OrderBookList->BetterBuyBKColor = Color;break;
		case 6: OrderBookList->BetterBuyColor = Color;break;
		case 7: OrderBookList->BetterSellBKColor = Color;break;
		case 8: OrderBookList->BetterSellColor = Color;break;
		case 9: OrderBookList->DayHColor = Color;break;
		case 10: OrderBookList->DayLColor = Color;break;
		case 11: OrderBookList->AvgPxColor = Color;break;

		default: break;
	}
}
//---------------------------------------------------------------------------
TColor __fastcall TDepthForm::GetBuyColor( int Index )
{
	switch( Index )
	{
		case 0: return OrderBookList->BidDepth5BKColor;
		case 1: return OrderBookList->BidDepth5Color;
		case 2: return OrderBookList->BuyBKColor;
		case 3: return OrderBookList->BuyColor;
		case 4: return OrderBookList->BuyFillBKColor;
		case 5: return OrderBookList->BuyFillColor;
		case 6: return OrderBookList->BuyConditionColBKColor;
		case 7: return OrderBookList->BuyConditionColColor;
		case 8: return OrderBookList->BuyOCOColBKColor;
		case 9: return OrderBookList->BuyOCOColColor;
		case 10:return OrderBookList->BuyOCODelColBKColor;
		default: return clWhite;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SetBuyColor( int Index, TColor Color )
{
	switch( Index )
	{
		case 0: OrderBookList->BidDepth5BKColor = Color;break;
		case 1: OrderBookList->BidDepth5Color = Color;break;
		case 2: OrderBookList->BuyBKColor = Color;break;
		case 3: OrderBookList->BuyColor = Color;break;
		case 4: OrderBookList->BuyFillBKColor = Color;break;
		case 5: OrderBookList->BuyFillColor = Color;break;
		case 6: OrderBookList->BuyConditionColBKColor = Color;break;
		case 7: OrderBookList->BuyConditionColColor = Color;break;
		case 8: OrderBookList->BuyOCOColBKColor = Color;break;
		case 9: OrderBookList->BuyOCOColColor = Color;break;
		case 10:OrderBookList->BuyOCODelColBKColor = Color;break;
		default: break;
	}
}
//---------------------------------------------------------------------------
TColor __fastcall TDepthForm::GetSellColor( int Index )
{
	switch( Index )
	{
		case 0: return OrderBookList->AskDepth5BKColor;
		case 1: return OrderBookList->AskDepth5Color;
		case 2: return OrderBookList->SellBKColor;
		case 3: return OrderBookList->SellColor;
		case 4: return OrderBookList->SellFillBKColor;
		case 5: return OrderBookList->SellFillColor;
		case 6: return OrderBookList->SellConditionColBKColor;
		case 7: return OrderBookList->SellConditionColColor;
		case 8: return OrderBookList->SellOCOColBKColor;
		case 9: return OrderBookList->SellOCOColColor;
		case 10:return OrderBookList->SellOCODelColBKColor;
		default: return clWhite;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SetSellColor( int Index, TColor Color )
{
	switch( Index )
	{
		case 0: OrderBookList->AskDepth5BKColor = Color;break;
		case 1: OrderBookList->AskDepth5Color = Color;break;
		case 2: OrderBookList->SellBKColor = Color;break;
		case 3: OrderBookList->SellColor = Color;break;
		case 4: OrderBookList->SellFillBKColor = Color;break;
		case 5: OrderBookList->SellFillColor = Color;break;
		case 6: OrderBookList->SellConditionColBKColor = Color;break;
		case 7: OrderBookList->SellConditionColColor = Color;break;
		case 8: OrderBookList->SellOCOColBKColor = Color;break;
		case 9: OrderBookList->SellOCOColColor = Color;break;
		case 10:OrderBookList->SellOCODelColBKColor = Color;break;
		default: break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::FixRowComboBoxChange(TObject *Sender)
{
	FixRowColorBox->Selected = FixRowColors[ FixRowComboBox->ItemIndex ];
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::FixRowColorBoxChange(TObject *Sender)
{
	FixRowColors[ FixRowComboBox->ItemIndex ] = FixRowColorBox->Selected;
	//ContractViewerForm->LoadDepthColor( this );
	OrderBookList->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::TickComboBoxChange(TObject *Sender)
{
	TickColorBox->Selected = TickColors[ TickComboBox->ItemIndex ];
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::TickColorBoxChange(TObject *Sender)
{
	TickColors[ TickComboBox->ItemIndex ] = TickColorBox->Selected;
	//ContractViewerForm->LoadDepthColor( this );
	OrderBookList->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::BuyComboBoxChange(TObject *Sender)
{
	BuyTextColorBox->Selected = BuyColors[ BuyComboBox->ItemIndex ];
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::BuyTextColorBoxChange(TObject *Sender)
{
	BuyColors[ BuyComboBox->ItemIndex ] = BuyTextColorBox->Selected;
	//ContractViewerForm->LoadDepthColor( this );
	OrderBookList->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SellComboBoxChange(TObject *Sender)
{
	SellTextColorBox->Selected = SellColors[ SellComboBox->ItemIndex ];
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SellTextColorBoxChange(TObject *Sender)
{
	SellColors[ SellComboBox->ItemIndex ] = SellTextColorBox->Selected;
	//ContractViewerForm->LoadDepthColor( this );
	OrderBookList->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::DefColorButtonClick(TObject *Sender)
{
	int DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );
	///< Fix row colors
	OrderBookList->FixedRowBidColor  = FixedRowBidColor[DorL];
	OrderBookList->FixedRowAskColor  = FixedRowAskColor[DorL];
	OrderBookList->FixedRowColor     = FixedRowColor[DorL];
	OrderBookList->FixedRowBKColor   = FixedRowBKColor[DorL];
	OrderBookList->GridColor	     = GridColor[DorL];
	///< Tick row colors
	OrderBookList->TickBKColor       = TickBKColor[DorL];
	OrderBookList->TickUpperColor    = TickUpperColor[DorL];
	OrderBookList->TickLowerColor    = TickLowerColor[DorL];
	OrderBookList->FillColor         = FillColor[DorL];
	OrderBookList->FillBKColor       = FillBKColor[DorL];
	OrderBookList->BetterBuyColor    = BetterBuyColor[DorL];
	OrderBookList->BetterBuyBKColor  = BetterBuyBKColor[DorL];
	OrderBookList->BetterSellColor   = BetterSellColor[DorL];
	OrderBookList->BetterSellBKColor = BetterSellBKColor[DorL];
	OrderBookList->DayHColor         = DayHiColor[DorL];
	OrderBookList->DayLColor         = DayLoColor[DorL];
	OrderBookList->AvgPxColor        = AvgPriceColor[DorL];
	///< Buy side color
	OrderBookList->BidDepth5BKColor  = BidDepth5BKColor[DorL];
	OrderBookList->BidDepth5Color    = BidDepth5Color[DorL];
	OrderBookList->BuyBKColor        = BuyBKColor[DorL];
	OrderBookList->BuyColor          = BuyColor[DorL];
	OrderBookList->BuyFillBKColor    = BuyFillBKColor[DorL];
	OrderBookList->BuyFillColor      = BuyFillColor[DorL];
	OrderBookList->BuyConditionColBKColor = BuyConditionColBKColor[DorL];
	OrderBookList->BuyConditionColColor   = BuyConditionColColor[DorL];
	OrderBookList->BuyOCOColBKColor  = BuyOCOColBKColor[DorL];
	OrderBookList->BuyOCOColColor    = BuyOCOColColor[DorL];
	OrderBookList->BuyOCODelColBKColor=BuyOCODelColBKColor[DorL];
	///<Sell side color
	OrderBookList->AskDepth5BKColor  = AskDepth5BKColor[DorL];
	OrderBookList->AskDepth5Color    = AskDepth5Color[DorL];
	OrderBookList->SellBKColor       = SellBKColor[DorL];
	OrderBookList->SellColor         = SellColor[DorL];
	OrderBookList->SellFillBKColor   = SellFillBKColor[DorL];
	OrderBookList->SellFillColor     = SellFillColor[DorL];
	OrderBookList->SellConditionColBKColor = SellConditionColBKColor[DorL];
	OrderBookList->SellConditionColColor   = SellConditionColColor[DorL];
	OrderBookList->SellOCOColBKColor = SellOCOColBKColor[DorL];
	OrderBookList->SellOCOColColor   = SellOCOColColor[DorL];
	OrderBookList->SellOCODelColBKColor = SellOCODelColBKColor[DorL];

	OrderBookList->Repaint();
	FixRowComboBoxChange( NULL );
	TickComboBoxChange( NULL );
	BuyComboBoxChange( NULL );
	SellComboBoxChange( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LoadDefColor( bool DorL )
{
	String Name;
	if( DorL == true )
		Name = L"DepthDarkColor";
	else
		Name = L"DepthLightColor";
	///< Fix row colors
	FixedRowBidColor[DorL]  = (TColor)g_Config.GetIntegerProperty(Name,"FixRowBColor",FixedRowBidColor[DorL] );
	FixedRowAskColor[DorL]  = (TColor)g_Config.GetIntegerProperty(Name,"FixRowSColor",FixedRowAskColor[DorL] );
	FixedRowColor[DorL]     = (TColor)g_Config.GetIntegerProperty(Name,"FixRowColor",FixedRowColor[DorL] );
	FixedRowBKColor[DorL]   = (TColor)g_Config.GetIntegerProperty(Name,"FixRowBKColor",FixedRowBKColor[DorL] );
	GridColor[DorL]	        = (TColor)g_Config.GetIntegerProperty(Name,"GridLineColor",GridColor[DorL] );
	///< Tick row colors
	TickBKColor[DorL]       = (TColor)g_Config.GetIntegerProperty(Name,"TickColBKColor" ,TickBKColor[DorL]);
	TickUpperColor[DorL]    = (TColor)g_Config.GetIntegerProperty(Name,"TickUpperColor" ,TickUpperColor[DorL]);
	TickLowerColor[DorL]    = (TColor)g_Config.GetIntegerProperty(Name,"TickLowerColor" ,TickLowerColor[DorL]);
	FillColor[DorL]         = (TColor)g_Config.GetIntegerProperty(Name,"FillCellColor", FillColor[DorL]);
	FillBKColor[DorL]       = (TColor)g_Config.GetIntegerProperty(Name,"FillCellBKColor",FillBKColor[DorL] );
	BetterBuyColor[DorL]    = (TColor)g_Config.GetIntegerProperty(Name,"BetterBuyColor", BetterBuyColor[DorL]);
	BetterBuyBKColor[DorL]  = (TColor)g_Config.GetIntegerProperty(Name,"BetterBuyBKColor", BetterBuyBKColor[DorL]);
	BetterSellColor[DorL]   = (TColor)g_Config.GetIntegerProperty(Name,"BetterSellColor" ,BetterSellColor[DorL]);
	BetterSellBKColor[DorL] = (TColor)g_Config.GetIntegerProperty(Name,"BetterSellBKColor" ,BetterSellBKColor[DorL]);
	DayHiColor[DorL]        = (TColor)g_Config.GetIntegerProperty(Name,"DHColor" ,DayHiColor[DorL]);
	DayLoColor[DorL]        = (TColor)g_Config.GetIntegerProperty(Name,"DLColor" ,DayLoColor[DorL]);
	AvgPriceColor[DorL]     = (TColor)g_Config.GetIntegerProperty(Name,"AvgPxColor" ,AvgPriceColor[DorL]);
	///< Buy side color
	BidDepth5BKColor[DorL]  = (TColor)g_Config.GetIntegerProperty(Name,"BuyDepthColBKColor", BidDepth5BKColor[DorL]);
	BidDepth5Color[DorL]    = (TColor)g_Config.GetIntegerProperty(Name,"BuyDepthColColor", BidDepth5Color[DorL]);
	BuyBKColor[DorL]        = (TColor)g_Config.GetIntegerProperty(Name,"BuyColBKColor", BuyBKColor[DorL]);
	BuyColor[DorL]          = (TColor)g_Config.GetIntegerProperty(Name,"BuyColColor",BuyColor[DorL] );
	BuyFillBKColor[DorL]    = (TColor)g_Config.GetIntegerProperty(Name,"BuyFillBKColor" ,BuyFillBKColor[DorL]);
	BuyFillColor[DorL]      = (TColor)g_Config.GetIntegerProperty(Name,"BuyFillColor" ,BuyFillColor[DorL]);
	BuyConditionColBKColor[DorL] = (TColor)g_Config.GetIntegerProperty(Name,"BuyStopBKColor" ,BuyConditionColBKColor[DorL]);
	BuyConditionColColor[DorL]   = (TColor)g_Config.GetIntegerProperty(Name,"BuyStopColor" ,BuyConditionColColor[DorL]);
	BuyOCOColColor[DorL]    = (TColor)g_Config.GetIntegerProperty(Name,"BuyOCOColColor" ,BuyOCOColColor[DorL]);
	BuyOCOColBKColor[DorL]  = (TColor)g_Config.GetIntegerProperty(Name,"BuyOCOColBKColor" ,BuyOCOColBKColor[DorL]);
	BuyOCODelColBKColor[DorL]=(TColor)g_Config.GetIntegerProperty(Name,"BuyOCODelColBKColor" ,BuyOCODelColBKColor[DorL]);
	///<Sell side color
	AskDepth5BKColor[DorL]  = (TColor)g_Config.GetIntegerProperty(Name,"SellDepthColBKColor",AskDepth5BKColor[DorL] );
	AskDepth5Color[DorL]    = (TColor)g_Config.GetIntegerProperty(Name,"SellDepthColColor", AskDepth5Color[DorL]);
	SellBKColor[DorL]       = (TColor)g_Config.GetIntegerProperty(Name,"SellColBKColor",SellBKColor[DorL] );
	SellColor[DorL]         = (TColor)g_Config.GetIntegerProperty(Name,"SellColColor", SellColor[DorL] );
	SellFillBKColor[DorL]   = (TColor)g_Config.GetIntegerProperty(Name,"SellFillBKColor" ,SellFillBKColor[DorL]);
	SellFillColor[DorL]     = (TColor)g_Config.GetIntegerProperty(Name,"SellFillColor" ,SellFillColor[DorL]);
	SellConditionColBKColor[DorL] = (TColor)g_Config.GetIntegerProperty(Name,"SellStopBKColor" ,SellConditionColBKColor[DorL]);
	SellConditionColColor[DorL]   = (TColor)g_Config.GetIntegerProperty(Name,"SellStopColor" ,SellConditionColColor[DorL]);
	SellOCOColColor[DorL]   = (TColor)g_Config.GetIntegerProperty(Name,"SellOCOColColor" ,SellOCOColColor[DorL]);
	SellOCOColBKColor[DorL] = (TColor)g_Config.GetIntegerProperty(Name,"SellOCOColBKColor" ,SellOCOColBKColor[DorL]);
	SellOCODelColBKColor[DorL] = (TColor)g_Config.GetIntegerProperty(Name,"SellOCODelColBKColor" ,SellOCODelColBKColor[DorL]);
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::AsDefColorButtonClick(TObject *Sender)
{
	SaveDefColor( );
	ContractViewerForm->LoadDepthColor( this );
}
//---------------------------------------------------------------------------

void __fastcall TDepthForm::CopyDepthButtonClick(TObject *Sender)
{
	TDepthForm* NewDepth;

	if( (NewDepth = ContractViewerForm->OpenOrderBookForm(FEx,FSym,true)) != NULL )
		CopyThis(  NewDepth );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::CopyThis( TDepthForm* NewDepth )
{
	///< Copy Colors
	///< Fix row colors
	NewDepth->OrderBookList->FixedRowBidColor  = OrderBookList->FixedRowBidColor;
	NewDepth->OrderBookList->FixedRowAskColor  = OrderBookList->FixedRowAskColor;
	NewDepth->OrderBookList->FixedRowColor     = OrderBookList->FixedRowColor;
	NewDepth->OrderBookList->FixedRowBKColor   = OrderBookList->FixedRowBKColor;
	NewDepth->OrderBookList->GridColor	     = OrderBookList->GridColor;
	///< Tick row colors
	NewDepth->OrderBookList->TickBKColor       = OrderBookList->TickBKColor;
	NewDepth->OrderBookList->TickUpperColor    = OrderBookList->TickUpperColor;
	NewDepth->OrderBookList->TickLowerColor    = OrderBookList->TickLowerColor;
	NewDepth->OrderBookList->FillColor         = OrderBookList->FillColor;
	NewDepth->OrderBookList->FillBKColor       = OrderBookList->FillBKColor;
	NewDepth->OrderBookList->BetterBuyColor    = OrderBookList->BetterBuyColor;
	NewDepth->OrderBookList->BetterBuyBKColor  = OrderBookList->BetterBuyBKColor;
	NewDepth->OrderBookList->BetterSellColor   = OrderBookList->BetterSellColor;
	NewDepth->OrderBookList->BetterSellBKColor = OrderBookList->BetterSellBKColor;
	NewDepth->OrderBookList->DayHColor         = OrderBookList->DayHColor;
	NewDepth->OrderBookList->DayLColor         = OrderBookList->DayLColor;
	NewDepth->OrderBookList->AvgPxColor        = OrderBookList->AvgPxColor;
	///< Buy side color
	NewDepth->OrderBookList->BidDepth5BKColor  = OrderBookList->BidDepth5BKColor;
	NewDepth->OrderBookList->BidDepth5Color    = OrderBookList->BidDepth5Color;
	NewDepth->OrderBookList->BuyBKColor        = OrderBookList->BuyBKColor;
	NewDepth->OrderBookList->BuyColor          = OrderBookList->BuyColor;
	NewDepth->OrderBookList->BuyFillBKColor    = OrderBookList->BuyFillBKColor;
	NewDepth->OrderBookList->BuyFillColor      = OrderBookList->BuyFillColor;
	NewDepth->OrderBookList->BuyConditionColBKColor = OrderBookList->BuyConditionColBKColor;
	NewDepth->OrderBookList->BuyConditionColColor   = OrderBookList->BuyConditionColColor;
	NewDepth->OrderBookList->BuyOCOColColor    = OrderBookList->BuyOCOColColor;
	NewDepth->OrderBookList->BuyOCOColBKColor  = OrderBookList->BuyOCOColBKColor;
	NewDepth->OrderBookList->BuyOCODelColBKColor = OrderBookList->BuyOCODelColBKColor;
	///<Sell side color
	NewDepth->OrderBookList->AskDepth5BKColor  = OrderBookList->AskDepth5BKColor;
	NewDepth->OrderBookList->AskDepth5Color    = OrderBookList->AskDepth5Color;
	NewDepth->OrderBookList->SellBKColor       = OrderBookList->SellBKColor;
	NewDepth->OrderBookList->SellColor         = OrderBookList->SellColor;
	NewDepth->OrderBookList->SellFillBKColor   = OrderBookList->SellFillBKColor;
	NewDepth->OrderBookList->SellFillColor     = OrderBookList->SellFillColor;
	NewDepth->OrderBookList->SellConditionColBKColor = OrderBookList->SellConditionColBKColor;
	NewDepth->OrderBookList->SellConditionColColor   = OrderBookList->SellConditionColColor;
	NewDepth->OrderBookList->SellOCOColColor   = OrderBookList->SellOCOColColor;
	NewDepth->OrderBookList->SellOCOColBKColor = OrderBookList->SellOCOColBKColor;
	NewDepth->OrderBookList->SellOCODelColBKColor = OrderBookList->SellOCODelColBKColor;
	///< Copy Setting
	NewDepth->LotsUpDown->Position = LotsUpDown->Position;
	NewDepth->OrderBookList->ShowFilled      = OrderBookList->ShowFilled;
	NewDepth->OrderBookList->CenterFillPrice = OrderBookList->CenterFillPrice;
	NewDepth->OrderBookList->ConditionOrder  = OrderBookList->ConditionOrder;
	NewDepth->OrderBookList->ShowDerivedPx   = OrderBookList->ShowDerivedPx;
	NewDepth->OrderBookList->ShowFillQty     = OrderBookList->ShowFillQty;
	NewDepth->OrderBookList->CancelBeforeNew = OrderBookList->CancelBeforeNew;
	NewDepth->OrderBookList->SetEnable       = OrderBookList->SetEnable;
	NewDepth->OrderBookList->Step            = OrderBookList->Step;
	NewDepth->OrderBookList->StepCount       = OrderBookList->StepCount;
	NewDepth->MarketToggleSwitch->State      = MarketToggleSwitch->State;
	NewDepth->FillToggleSwitch->State = ( OrderBookList->ShowFilled == true)?tssOn:tssOff;
	NewDepth->AutoCenterSwitch->State = ( OrderBookList->CenterFillPrice == true)?tssOn:tssOff;
	NewDepth->StopToggleSwitch->State = ( OrderBookList->ConditionOrder == true)?tssOn:tssOff;
	NewDepth->DrivedToggleSwitch->State = ( OrderBookList->ShowDerivedPx == true)?tssOn:tssOff;
	NewDepth->FillQtyToggleSwitch->State = ( OrderBookList->ShowFillQty == true)?tssOn:tssOff;
	NewDepth->CxlBeforeNewToggleSwitch->State = ( OrderBookList->CancelBeforeNew == true)?tssOn:tssOff;
	NewDepth->SliceOrderSwitch->State = ( OrderBookList->SetEnable == true)?tssOn:tssOff;
	NewDepth->StepUpDown->Position = OrderBookList->Step;
	NewDepth->StepCountUpDown->Position = OrderBookList->StepCount;
	NewDepth->SliceOrderSwitchClick( NULL );
	NewDepth->OrderBookList->BetterStep = OrderBookList->BetterStep;
	///< Copy Font Size
	NewDepth->Height = Height;
	NewDepth->FFontSize = FFontSize;
	NewDepth->AdjuestFont( );
	NewDepth->RoundFormEx->Lock = RoundFormEx->Lock;
	NewDepth->OrderBookList->Repaint();
    ///< Copy Order Condiction.
	NewDepth->TIFComboBox->ItemIndex = TIFComboBox->ItemIndex;
	NewDepth->DayTradeButton->Selected = DayTradeButton->Selected;
	NewDepth->NormalButton->Selected  = NormalButton->Selected;
	NewDepth->LoanButton->Selected = LoanButton->Selected;
	NewDepth->LoanShareButton->Selected = LoanShareButton->Selected;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::StopSettingBtnClick(TObject *Sender)
{
	TStopSettingForm* StopSettingForm = new TStopSettingForm( this );

	StopSettingForm->ApplySetting( OrderBookList );
	if( RoundFormEx->Lock == true )
	{
	   Application->NormalizeTopMosts();
	   SetWindowPos( StopSettingForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
	}
	else
		SetWindowPos( Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
	if( StopSettingForm->ShowModal() == mrOk )
	{
		OrderBookList->BuyStopOrderType = StopSettingForm->GetBuyOrderType();
		if( StopSettingForm->GetBuyOrderType() == nsOrderMessageDefine::otLimit )
			OrderBookList->BuyStopTick = StopSettingForm->GetBuyTick();

		OrderBookList->SellStopOrderType = StopSettingForm->GetSellOrderType();
		if( StopSettingForm->GetSellOrderType() == nsOrderMessageDefine::otLimit )
			OrderBookList->SellStopTick = StopSettingForm->GetSellTick();
		SaveStopSetting();
		ContractViewerForm->LoadStopSetting( this );
	}
	delete StopSettingForm;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderTypeStr( nsOrderMessageDefine::OrderTypeEnum ot, AnsiString& OTStr )
{
	switch( ot )
	{
		case otNone:                 OTStr = "None";break;
		case otMarket:               OTStr = "Market";break;
		case otLimit:                OTStr = "Limit";break;
		case otMarketWithProtection: OTStr = "MarketWithProtection";break;
		case otStop:                 OTStr = "Stop";break;
		case otStopLimit:            OTStr = "StopLimit";break;
		case otMLtoLimit:            OTStr = "MLtoLimit";break;
		case otMarketOnClose:        OTStr = "MarketOnClose";break;
		case otLimitOnClose:         OTStr = "LimitOnClose";break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::PrintStopSetting( void )
{
	AnsiString OTStr;

	UFC::BufferedLog::Printf( " ----- Save Stop Order Setting -----" );
	OrderTypeStr( OrderBookList->BuyStopOrderType, OTStr );
	UFC::BufferedLog::Printf( " Stop Buy  OrderType[%s] Ticks[%d]", OTStr.c_str(), OrderBookList->BuyStopTick );
	OrderTypeStr( OrderBookList->SellStopOrderType, OTStr );
	UFC::BufferedLog::Printf( " Stop Sell OrderType[%s] Ticks[%d]", OTStr.c_str(), OrderBookList->SellStopTick );
	UFC::BufferedLog::Printf( " -------------- End ----------------" );

}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LoadStopSetting( void )
{
	OrderBookList->BuyStopOrderType  = g_Config.GetIntegerProperty("StopSetting","BuyOrdType", nsOrderMessageDefine::otLimit );
	OrderBookList->SellStopOrderType = g_Config.GetIntegerProperty("StopSetting","SellOrdType", nsOrderMessageDefine::otLimit );
	OrderBookList->BuyStopTick  = g_Config.GetIntegerProperty("StopSetting","BuyTick", 0 );
	OrderBookList->SellStopTick = g_Config.GetIntegerProperty("StopSetting","SellTick", 0 );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SaveStopSetting( void )
{
	g_Config.SetIntegerProperty("StopSetting","BuyOrdType", (int)OrderBookList->BuyStopOrderType );
	g_Config.SetIntegerProperty("StopSetting","SellOrdType", (int)OrderBookList->SellStopOrderType );
	g_Config.SetIntegerProperty("StopSetting","BuyTick", OrderBookList->BuyStopTick );
	g_Config.SetIntegerProperty("StopSetting","SellTick", OrderBookList->SellStopTick );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LotsComboBoxChange(TObject *Sender)
{
	int Lots = LotsComboBox->Text.ToInt();

	if( Lots <= 0 )
		Lots = 1;
	else if( Lots > 200 )
		Lots = 200;
	g_Config.SetIntegerProperty("Setting","LotsMax", Lots );
	LotsUpDown->Max = Lots;
	ContractViewerForm->LoadHotkey( this );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SetVisible( bool IsVisible )
{
	if( IsVisible == false )
	{
		Visible = false;
	}
	else
	{
		Visible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListCenterPx(TObject *Sender)
{
	ContractViewerForm->CenterPx( this );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OnPositionClosed( void )
{
	if( DelOnCloseToggleSwitch->State == tssOn )
	{
		OrderBookList->DeleteAllStopOrders();
		OrderBookList->DeleteAllOrders( false );
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::FilledStopToggleSwitchClick(TObject *Sender)
{
	if( FilledStopToggleSwitch->State == tssOn || FilledStopProfitToggleSwitch->State == tssOn )
		EnableStopAfterFilled( true );
	else
		EnableStopAfterFilled( false );
	ApplyStopTick( StopTickUpDown->Position, StopProfitTickUpDown->Position );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::EnableStopAfterFilled( bool Enable )
{
	AnsiString              Sym( FSym );
	UFC::List<TDepthForm*>* HandleForms;
	int                     FormIndex;

	///< Any depth form handles this auto close event?
	if( FStopCloseMap.GetObjectByKey( Sym.c_str(), HandleForms ) == false )
	{
		HandleForms = new UFC::List<TDepthForm*>();
		FStopCloseMap.Add( Sym.c_str(), HandleForms );
	}
	///< Get the index of this form
	FormIndex = HandleForms->IndexOf( this );
	if( Enable == true )
	{
		if( FormIndex == -1 ) ///< Not exists
			HandleForms->Add( this ); ///< add this form
	}
	else
	{
		if( FormIndex != -1 ) ///< Exists
			HandleForms->Delete( FormIndex ); ///< Delete this form
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::UpdateNuclearLimit( void )
{
	UpCountUpDown->Max     = FNuclearUpperLots;
	if( UpCountUpDown->Position > UpCountUpDown->Max )
		UpCountUpDown->Position = UpCountUpDown->Max;
	DownCountUpDown->Max   = FNuclearLowerLots;
	if(	DownCountUpDown->Position > DownCountUpDown->Max )
		DownCountUpDown->Position = DownCountUpDown->Max;
	ProfitCountUpDown->Max = NuclearLotsUpDown->Position;
	if( ProfitCountUpDown->Position > ProfitCountUpDown->Max )
		ProfitCountUpDown->Position = ProfitCountUpDown->Max;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::NuclearLotsUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	int TotalLots = NewValue;
	int Upper,Lower;

	switch( RatioComboBox->ItemIndex )
	{
		case 0: Upper = (int)((double)TotalLots*0.9);break;
		case 1: Upper = (int)((double)TotalLots*0.8);break;
		case 2: Upper = (int)((double)TotalLots*0.7);break;
		case 3: Upper = (int)((double)TotalLots*0.6);break;
	}
	Lower = TotalLots - Upper;
	if( Upper != 0 && Lower != 0 )
	{
		FNuclearUpperLots = Upper;
		FNuclearLowerLots = Lower;
		UpdateNuclearLimit();
	}
	else
		AllowChange = false;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::RatioComboBoxChange(TObject *Sender)
{
	int TotalLots = NuclearLotsUpDown->Position;

	switch( RatioComboBox->ItemIndex )
	{
		case 0: FNuclearUpperLots = (int)((double)TotalLots*0.9);break;
		case 1: FNuclearUpperLots = (int)((double)TotalLots*0.8);break;
		case 2: FNuclearUpperLots = (int)((double)TotalLots*0.7);break;
		case 3: FNuclearUpperLots = (int)((double)TotalLots*0.6);break;
	}
	FNuclearLowerLots = TotalLots - FNuclearUpperLots;
	UpdateNuclearLimit();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::RoundFormExLockIconClick(TObject *Sender)
{
	if( RoundFormEx->Lock == true )
	{
		if( PageControl->ActivePage == ColorTabSheet )
			PageControl->ActivePage = SettingTabSheet;
		ColorTabSheet->TabVisible = false;
	}
	else
		ColorTabSheet->TabVisible = true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::NormalButtonClick(TObject *Sender)
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
char __fastcall TDepthForm::GetTWSEOrderType( SideEnum side )
{
	if( LoanButton->Selected   == true )
		return '3';  ///< trade margin
	else if( LoanShareButton->Selected   == true )
		return '4';  ///< sell short
	else
		return '0'; ///< Normal or Day Trade
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::DayTradeButtonClick(TObject *Sender)
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
PositionEffectEnum   __fastcall TDepthForm::GetPositionEffect( void )
{
	if( DayTradeButton->Selected == true )
		return nsOrderMessageDefine::peDayTrade;
	return nsOrderMessageDefine::peAuto;
}
//---------------------------------------------------------------------------
TimeInForceEnum  __fastcall TDepthForm::GetTimeInForce( bool IsHotKey )
{
	if( IsHotKey == false )
	{
		switch(TIFComboBox->ItemIndex )
		{
			case 1: return nsOrderMessageDefine::tifFOK;
			case 2: return nsOrderMessageDefine::tifIOC;
			default: return nsOrderMessageDefine::tifROD;
		}
	}
	else
	{
		if( MarketToggleSwitch->State == tssOff )
			return nsOrderMessageDefine::tifFOK;
		else
			return nsOrderMessageDefine::tifIOC;
    }
}
//---------------------------------------------------------------------------

void __fastcall TDepthForm::CenterLabelMouseEnter(TObject *Sender)
{
	TLabel* Label = dynamic_cast< TLabel* >( Sender);
	if( Label != NULL)
		Label->Font->Color = clWhite;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::CenterLabelMouseLeave(TObject *Sender)
{
	TLabel* Label = dynamic_cast< TLabel* >( Sender);
	if( Label != NULL)
		Label->Font->Color = clSilver;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::CenterLabelClick(TObject *Sender)
{
	OrderBookList->CenterFillPx( true );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ToolSVOpened(TObject *Sender)
{
   AlignLabels();
/*   if( PageControl->ActivePage == TFTTabSheet )
   {
	   PageControl->ActivePage = SettingTabSheet;
	   PageControl->ActivePage = TFTTabSheet;
   }*/
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ToolSVClosed(TObject *Sender)
{
   AlignLabels();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::AlignLabels( void )
{
   CenterLabel->Left = OrderBookList->Left + (OrderBookList->Width - CenterLabel->Width )/2;
   MarketBuyLabel->Left = CenterLabel->Left - 20 - MarketBuyLabel->Width;
   MarketSellLabel->Left = CenterLabel->Left + 20 + CenterLabel->Width;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::MarketBuyLabelClick(TObject *Sender)
{
	if( gOrderStore->IsReady() == true )
		PlaceMarketOrder( sBuy, LotsUpDown->Position, GetTimeInForce( false ) );
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::MarketSellLabelClick(TObject *Sender)
{
	if( gOrderStore->IsReady() == true )
		PlaceMarketOrder( sSell, LotsUpDown->Position, GetTimeInForce( false ) );
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
bool __fastcall TDepthForm::CanClose( void )
{
	if( OrderBookList->HasStopOrder() == true )
	{
		if( TUnifyDlgs::AskYesNoDialog( "警告訊息", "您還有觸價單,確定要關閉視窗?" ) == false )
			return false;
	}
	if( OrderBookList->HasWorkingOrder() == true )
	{
		if( TUnifyDlgs::AskYesNoDialog( "警告訊息", "您還有委託單,確定要關閉視窗?" ) == false )
			return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ExpBtnClick(TObject *Sender)
{
	TButton* ExpBtn = dynamic_cast<TButton*>( Sender );
	TPanel*  FuncPanel;


	if( ExpBtn != NULL && FExpBtn2Panel.GetObjectByKey( ExpBtn,FuncPanel ) )
	{
		if( ExpBtn->ImageIndex == 1 )
		{
			 ExpBtn->ImageIndex = 0;
			 FuncPanel->Height = ExpBtn->Tag;
		}
		else
		{
			 ExpBtn->ImageIndex = 1;
			 FuncPanel->Height = 55;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LockUI( bool Locked )
{
	if( Locked == true )
	{
		OrderBookList->Hotkey = false;
		pnlToolbar->Enabled = false;
		ProfitPanel->Enabled = false;
		ButtonPanel->Enabled = false;
	}
	else
	{
		OrderBookList->Hotkey = true;
		pnlToolbar->Enabled = true;
		ProfitPanel->Enabled = true;
		ButtonPanel->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LockOtherPanel( bool Locked, TPanel* thisPanel )
{
	if( Locked == true )
	{
		if( SplitPanel != thisPanel )
			SplitPanel->Enabled = false;
		if( BullInPanel != thisPanel )
			BullInPanel->Enabled = false;
		if( BullOutPanel != thisPanel )
			BullOutPanel->Enabled = false;
		if( DepthPanel != thisPanel )
			DepthPanel->Enabled = false;
		if( FataOrdPanel != thisPanel )
			FataOrdPanel->Enabled = false;
	}
	else
	{
		SplitPanel->Enabled = true;
		BullInPanel->Enabled = true;
		BullOutPanel->Enabled = true;
		DepthPanel->Enabled = true;
		FataOrdPanel->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SubscribeFull( void )
{
	if( RegCount >= 0 )
	{
		if( RegCount == 0 )
			MainForm->SubscribeFilled( FEx, FSym, this );
		RegCount++;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::UnsubscribeFull( void )
{
	if( RegCount > 0 )
	{
		RegCount--;
		if( RegCount == 0 )
			MainForm->UnsubscribeFilled( FEx, FSym, this );
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::FateTestButtonClick(TObject *Sender)
{
	if( OrderBookList->HasStopOrder() == true || OrderBookList->HasWorkingOrder() == true )
	{
		TUnifyDlgs::MessageDialog( "測假單錯誤", "請先刪除委託單,觸價單再測試" );
		return;
	}
	if( gOrderStore->IsReady() == true )
	{
		if( FFateTesting == false )
		{
			FFateTesting = true;
			FateTestButton->Enabled = false;
			FateTestCancelButton->Enabled = true;
			FBid1Px = OrderBookList->BidPrice[0];
			FAsk1Px = OrderBookList->AskPrice[0];
			FBid1Qty = OrderBookList->BidQty[0];
			FAsk1Qty = OrderBookList->AskQty[0];

			LockUI( true );
			LockOtherPanel( true, FataOrdPanel );
			FateTestResultLabel->Caption = L"結果:";
			SubscribeFull();
			PlaceOrder( sBuy,  FBid1Px, 1, nsOrderMessageDefine::otLimit , false  );
			PlaceOrder( sSell, FAsk1Px, 1, nsOrderMessageDefine::otLimit , false  );
		}
	}
	else
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::FateTestCancelButtonClick(TObject *Sender)
{
	if( FFateTesting == true )
	{
		FFateTesting = false;
		FateTestButton->Enabled = true;
		FateTestCancelButton->Enabled = false;

		UnsubscribeFull();
		LockUI( false );
		LockOtherPanel( false, FataOrdPanel );
		FateTestResultLabel->Caption = L"結果: 用戶取消測試";
		if( gOrderStore->IsReady() == true )
		{
			gOrderStore->CancelOrder( FMarket, FEx, FSym, sBuy, FBid1Px );
			gOrderStore->CancelOrder( FMarket, FEx, FSym, sSell, FAsk1Px );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::PageControlChanging(TObject *Sender, bool &AllowChange)
{
	if( OrderBookList->Hotkey == false )
		AllowChange = false;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::CheckBullIn(  double MatchPx, int MatchQty )
{
	if( BullInOnceCheckBox->Checked == true ) ///< Once qty
	{
		if( MatchQty >= BullInOnceUpDown->Position ) ///< Exceed one order limit
		{
			BullInToggleSwitch->State = tssOff;
			PlaceMarketOrder( sBuy, LotsUpDown->Position, GetTimeInForce( false ) );
			return;
		}
	}
	if( BullInAccumulateCheckBox->Checked == true ) ///< Accumulate qty
	{
		unsigned int Now = UFC::GetTickCountMS();
		int MaxMS = 100*BullInAccumulateTimeComboBox->ItemIndex;

		if( FBullInBeginTick == 0 )
			FBullInBeginTick = Now;
		FBullInAccuQty += MatchQty;
		if( FBullInAccuQty >= BullInAccumulateUpDown->Position )
		{
			int Diff = Now - FBullInBeginTick;
			if( Diff < MaxMS )
			{
				BullInToggleSwitch->State = tssOff;
				PlaceMarketOrder( sBuy, LotsUpDown->Position, GetTimeInForce( false ) );
			}
			else
			{
				FBullInBeginTick = Now;
				FBullInAccuQty   = MatchQty;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::CheckBullOut(  double MatchPx, int MatchQty )
{
	if( BullOutOnceCheckBox->Checked == true ) ///< Once qty
	{
		if( MatchQty >= BullOutOnceUpDown->Position ) ///< Exceed one order limit
		{
			BullOutToggleSwitch->State = tssOff;
			PlaceMarketOrder( sSell, LotsUpDown->Position, GetTimeInForce( false ) );
			return;
		}
	}
	if( BullOutAccumulateCheckBox->Checked == true ) ///< Accumulate qty
	{
		unsigned int Now = UFC::GetTickCountMS();
		int MaxMS = 100*BullOutAccumulateTimeComboBox->ItemIndex;

		if( FBullOutBeginTick == 0 )
			FBullOutBeginTick = Now;
		FBullOutAccuQty+= MatchQty;
		if( FBullOutAccuQty >= BullOutAccumulateUpDown->Position )
		{
			int Diff = Now - FBullOutBeginTick;
			if( Diff < MaxMS )
			{
				double Price = OrderBookList->GetBullPrice( BullOutTickUpDown->Position );

				BullOutToggleSwitch->State = tssOff;
				PlaceOrder( sSell, Price, LotsUpDown->Position, otLimit, false );
			}
			else
			{
				FBullOutBeginTick = Now;
				FBullOutAccuQty   = MatchQty;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListTick(TObject *Sender, const AnsiString &time,
		  double MatchPx, int MatchQty)
{
	///< Testing fate order in Bid/Ask1.
	if( FFateTesting == true )
	{
		if( MatchPx == FBid1Px )
			FBid1Qty -= MatchQty;
		if( MatchPx == FAsk1Px )
			FAsk1Qty -= MatchQty;
	}
	///< BullPx in/out strategy.
	if( BullInToggleSwitch->State == tssOn )
	{
		if( MatchPx == FBullPx )
		{
			CheckBullIn( MatchPx, MatchQty );
			CheckBullOut( MatchPx, MatchQty );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::DepthCancelToggleSwitchClick(TObject *Sender)
{
	if(  DepthCancelToggleSwitch->State == tssOff )
	{
		FAutoCancelDepth = false;
		UnsubscribeFull();
	}
	else
	{
		FAutoCancelDepth = true;
		SubscribeFull();
	}
}
//---------------------------------------------------------------------------
void TDepthForm::OnOrderFilled( const String &Exchange,const String &Symbol, const String &OrderID,SideEnum Side, double Px,int Qty, OrderStatusEnum OrderStatus)
{
	if( FFateTesting == true ) ///< Fate order testing
	{
		String Result;

		FFateTesting = false;
		MainForm->UnsubscribeFilled( FEx, FSym, this );
		if( Side == sBuy )
		{
			gOrderStore->CancelOrder( FMarket, FEx, FSym, sSell, FAsk1Px );
			if( FBid1Qty < 0 )
				FBid1Qty = 0;
			Result.printf( L"結果: 買單假單%d張", FBid1Qty );
		}
		else
		{
			gOrderStore->CancelOrder( FMarket, FEx, FSym, sBuy, FBid1Px );
			if( FAsk1Qty < 0 )
				FAsk1Qty = 0;
			Result.printf( L"結果: 賣單假單%d張", FAsk1Qty );
		}
		FateTestResultLabel->Caption = Result;
		FateTestButton->Enabled = true;
		FateTestCancelButton->Enabled = false;
		LockUI( false );
		LockOtherPanel( false, FataOrdPanel );
	}
	else if( FAutoCancelDepth == true ) ///< Auto cancel depth order when filled.
	{
		if( gOrderStore->IsReady() == true )
			OrderBookList->DeleteAllOrders( false );
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::UpdateSymbolListCount(void)
{
	FExList->Clear();
	FSymList->Clear();
	SymbolListBox->Count = FCustomMDList[ ExchangeComboBox->ItemIndex ].ItemCount();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SymbolListBoxData(TWinControl *Control, int Index, UnicodeString &Data)
{
	if(FCustomMDList[ ExchangeComboBox->ItemIndex ].ItemCount()<=Index)
        return;
	BasicInformation* Info = FCustomMDList[ ExchangeComboBox->ItemIndex ].GetItem(Index);
	if(Info == NULL)
		return;
	if(Index == FExList->Count)
		FExList->Add(Info->GetExchange().c_str());
	if(Index == FSymList->Count)
		FSymList->Add(Info->GetSymbol().c_str());
	Data = Info->GetDisplayName();
}
//---------------------------------------------------------------------------
bool __fastcall TDepthForm::GetExSymbol( String &Ex, String &Sym )
{
	int Index = SymbolListBox->ItemIndex;

	if( Index == -1 )
		return false;

	Ex  = FExList->Strings[ Index ];
	Sym = FSymList->Strings[ Index ];
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OnOrderStoreReady( void )
{
	UpdateSymbolListCount();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::BullInToggleSwitchClick(TObject *Sender)
{
	if( BullInToggleSwitch->State == tssOn )
	{
		FBullInBeginTick = 0;
		FBullInAccuQty = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::BullOutToggleSwitchClick(TObject *Sender)
{
	if( BullOutToggleSwitch->State == tssOn )
	{
		FBullOutBeginTick = 0;
		FBullOutAccuQty = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SymbolListBoxClick(TObject *Sender)
{
	String Ex, Sym;

	if( GetExSymbol( Ex, Sym ) == true )
	{
		FEx  = Ex;
		FSym = Sym;
		ChangeSymbol( );
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListPxAlarm(TObject *Sender)
{
	if( MainForm->EnablePxAlarm() == true )
	{
		GdAudioWave* Wave = gDSAudio.GetBaseWave( "PxAlarm" );
		if( Wave != NULL  )
			Wave->Play();
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListLowerPxAlarm(TObject *Sender)
{
	if( MainForm->EnableLowPxAlarm() == true )
	{
		GdAudioWave* Wave = gDSAudio.GetBaseWave( "LowPxAlarm" );
		if( Wave != NULL  )
			Wave->Play();
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::AutoStop( SideEnum side, double Price, int Qty, const String& Msg )
{
	UFC::List<TDepthForm*>* HandleForms;
	int                     FormIndex;
	AnsiString              Sym( FSym );

	if( FNetPos != 0 && FStopCloseMap.GetObjectByKey( Sym.c_str(), HandleForms ) == true &&
		HandleForms->ItemCount() > 0 && HandleForms->GetItem( 0 ) == this )
	{
		PositionEffectEnum  PE = GetPositionEffect();
		OrderTypeEnum       OrderType;
		double              ExecPrice;

		if(	side == nsOrderMessageDefine::sSell )
		{
			OrderType = OrderBookList->SellStopOrderType;
			ExecPrice = OrderBookList->GetBetterPrice( Price, OrderBookList->SellStopTick );
			UFC::BufferedLog::Printf( " [OrderBookList::%s.Sell-%d ticks]", Msg, OrderBookList->SellStopTick );
		}
		else
		{
			OrderType = OrderBookList->BuyStopOrderType;
			ExecPrice = OrderBookList->GetBetterPrice( Price, OrderBookList->BuyStopTick );
			UFC::BufferedLog::Printf( " [OrderBookList::%d.Buy+%d ticks]", Msg, OrderBookList->BuyStopTick );
		}
		gOrderStore->NewStopOrderRequest( FEx,                            ///< Stop Exchange
										  FSym,                           ///< Stop symbol
										  Price,                   ///< Stop price
										  FMarket,
										  FEx,                            ///< Execute Exchange
										  FSym,                           ///< Execute symbol
										  side,                           ///< Buy/Sell
										  ExecPrice,                      ///< Execute price
										  Qty,                            ///< Qty
										  OrderType,
										  (OrderType == nsOrderMessageDefine::otMarket)?nsOrderMessageDefine::tifIOC:nsOrderMessageDefine::tifROD,
										  PE,                             ///< Position effect. ///<nsOrderMessageDefine::peAuto, ///< Position effect.
										  FCallPut,
										  FStrikePX,
										  true );                         ///< client side stop order.
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::InitExchangeComboBox(int Idx)
{
	for( int i = 0; i < CUSTOM_LIST_COUNT; i++ )
	{
		String KeyName,DefName, CustomName;

		DefName.printf( L"自選商品-%d", i + 1 );
		KeyName.printf( L"Name%d", i + 1 );
		CustomName = g_Config.GetStringProperty( "CustomNames",KeyName, DefName );
		ExchangeComboBox->Items->Strings[ i ] = CustomName;
	}

	if(Idx<0)
		Idx = 0;
	ExchangeComboBox->ItemIndex = Idx;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::InitSmartOrderTabs( int Idx )
{
	SmartOrderTabs->TabIndex = Idx;
	SmartOrderTabsChange(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::DisableExtraPanelControls( void )
{
	for (int i = 0; i < ExtraPanel->ControlCount; i++)
	{
		TControl *ctrl = ExtraPanel->Controls[i];
        ctrl->Visible = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::EnableQuickMode( void )
{
	ExtraPanel->Visible = false;
	OrderBookList->ShowOCO = false;
	StopLabel->Enabled = true;
	StopToggleSwitch->Enabled = true;
	OrderBookList->ConditionOrder = (StopToggleSwitch->State != tssOff);

	if( ToolSV->Opened )
		SetWidth( ToolSV->OpenedWidth );
	else
		SetWidth( 0 );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::EnableOCOMode( void )
{
	ExtraPanel->Visible = true;
	DisableExtraPanelControls();
	SettingOCOBtn->Visible = true;
	SettingOCOBtn->Align = alRight;
	OCODetailBtn->Visible = true;
	OCODetailBtn->Align = alRight;
	OrderBookList->ShowOCO = true;
	OrderBookList->ConditionOrder = false;
	StopLabel->Enabled = false;
    StopToggleSwitch->Enabled = false;

	if( ToolSV->Opened )
		SetWidth( ToolSV->OpenedWidth );
	else
		SetWidth( 0 );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::LoadCustomList(void)
{
    String Dir;

	for( int j = 0; j < CUSTOM_LIST_COUNT; j++ )
	{
		FCustomMDList[ j ].Clear();
		Dir.printf( L"ContractList\\Custom%d", j );
		int Count = g_Config.GetDesktopInteger( Dir, "SymCount", 0 );
		for( int i =0; i< Count; i ++ )
		{
			String ExchangeName = "Exchange" + IntToStr( i );
			String KeyName 	= "Symbol" + IntToStr( i );
			String Exchange	= g_Config.GetDesktopString( Dir, ExchangeName, "" );
			String Symbol  	= g_Config.GetDesktopString( Dir, KeyName, "" );
			if( Exchange.Length() > 0 && Symbol.Length() > 0 )
			{
				BasicInformation* Info = gMarketDataStore->GetBasicInformation( Exchange, Symbol, false );
				if( Info != NULL )
					FCustomMDList[ j ].Add( Info );
			}
		}
	}
	UpdateSymbolListCount();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListAutoStopLoss(TObject *Sender, SideEnum side,
		  double Price, int Qty)
{
	FStopLossSide = side;
	FStopLossPrice = Price;
	FStopLossQty = Qty;
	TakeProfitTimer->Interval = 10;
	StopLossTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListAutoTakeProfit(TObject *Sender, SideEnum side,
		  double Price, int Qty)
{
	FTakeProfitSide = side;
	FTakeProfitPrice = Price;
	FTakeProfitQty = Qty;
	TakeProfitTimer->Interval = 20;
	TakeProfitTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::StopLossTimerTimer(TObject *Sender)
{
	StopLossTimer->Enabled = false;
	AutoStop( FStopLossSide, FStopLossPrice, FStopLossQty, L"OnAutoStopLoss" );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::TakeProfitTimerTimer(TObject *Sender)
{
	TakeProfitTimer->Enabled = false;
	AutoStop( FTakeProfitSide, FTakeProfitPrice, FTakeProfitQty, L"OnAutoTakeProfit" );
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::ExchangeComboBoxChange(TObject *Sender)
{
	if( ExchangeComboBox->ItemIndex == -1 )
		return;

	UpdateSymbolListCount();
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::EscapeButtonClick(TObject *Sender)
{
	if( TUnifyDlgs::AskYesNoDialog(
		Mdcomponentstrings_MD_SpeedyUnify_AppName,
		L"確定要出清該部位? " + Caption) == false )
		return;

	CancelAllAndClose(false);
}
//---------------------------------------------------------------------------

void __fastcall TDepthForm::ScrollBoxMouseWheel(TObject *Sender, TShiftState Shift,
		  int WheelDelta, TPoint &MousePos, bool &Handled)
{
	TPoint p = ScreenToClient(MousePos);

	if( PageControl->ActivePage == SettingTabSheet &&
		SettingScrollBox->BoundsRect.Contains(p))
	{
		SettingScrollBox->VertScrollBar->Position -= WheelDelta / 10;
		Handled = true;
		return;
	}

	if( PageControl->ActivePage == StopSettingTabSheet &&
		StopSettingScrollBox->BoundsRect.Contains(p))
	{
		StopSettingScrollBox->VertScrollBar->Position -= WheelDelta / 10;
		Handled = true;
		return;
	}

	if( PageControl->ActivePage == ColorTabSheet &&
		ColorScrollBox->BoundsRect.Contains(p))
	{
		ColorScrollBox->VertScrollBar->Position -= WheelDelta / 10;
		Handled = true;
		return;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SmartOrderTabsChange(TObject *Sender)
{
	switch(SmartOrderTabs->TabIndex)
	{
		case 0:
			EnableQuickMode();
			break;
		case 1:
			EnableOCOMode();
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::SettingOCOBtnClick(TObject *Sender)
{
	TSettingOCODlgForm *dlg = new TSettingOCODlgForm(this, FOCOType, FLimitOrderTick);
	int showResult = dlg->ShowModal();
	if(showResult == mrCancel)
		return;

	FOCOType = dlg->GetOrderType();
	FLimitOrderTick = dlg->GetLimitOrderTick();
	SaveOCOSetting();
	ContractViewerForm->LoadDepthOCO(this);
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListNewOCOOrder(
	TObject *Sender,
	SideEnum side,
	double Price)
{
	OrderBookList->UpdateOCOOrderQty(side, Price, LotsPerOrderEdit->Text.ToInt(), true);
}
//---------------------------------------------------------------------------
void __fastcall TDepthForm::OrderBookListNewOCOFail(TObject *Sender, const AnsiString &ErrorMessage)

{
	TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, ErrorMessage.c_str() );
}
//---------------------------------------------------------------------------

void __fastcall TDepthForm::OrderByOneClickSwitchClick(TObject *Sender)
{
	if(OrderByOneClickSwitch->State == tssOff)
		OrderBookList->OrderByOneClick = false;
	else
        OrderBookList->OrderByOneClick = true;
}
//---------------------------------------------------------------------------

