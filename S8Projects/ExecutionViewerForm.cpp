//---------------------------------------------------------------------------
#include <vcl.h>
#include "main.h"
#pragma hdrstop
#include "FMTConfig.h"
#include "ExecutionViewerForm.h"
#include "ContractViewer.h"
#include "UnifyDlg.h"
#include "MD5.h"
#include "LeaderBoard.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphButton"
#pragma link "GraphPanel"
#pragma link "OrderListView"
#pragma link "RoundFormEx"
#pragma link "cgauges"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
extern TCMarketDataStore* gMarketDataStore;
extern TOrderStore*       gOrderStore;
extern bool               GIsTestingMode;
extern bool               GSimMatch;
extern TUnifyUser         gUser;
extern bool               GVIPServer;
//---------------------------------------------------------------------------
const int EXEC_FONT_SIZE_MIN = 9;
const int EXEC_FONT_SIZE_DEF = 11;
const int EXEC_FONT_SIZE_MAX = 28;
//---------------------------------------------------------------------------
NewBuyColor[2]        = {RGB(187,187,187),RGB(51,51,51)};
NewBuyBkColor[2]      = {RGB(48,0,0),RGB(253,233,234)};
FillBuyColor[2]       = {RGB(187,187,187),RGB(51,51,51)};
FillBuyBkColor[2]     = {RGB(58,0,0),RGB(253,223,224)};
PartialBuyColor[2]    = {RGB(187,187,187),RGB(51,51,51)};
PartialBuyBkColor[2]  = {RGB(53,0,0),RGB(253,228,229)};
NewSellColor[2]       = {RGB(187,187,187),RGB(51,51,51)};
NewSellBkColor[2]     = {RGB(22,50,1),RGB(243,255,235)};
FillSellColor[2]      = {RGB(187,187,187),RGB(51,51,51)};
FillSellBKColor[2]    = {RGB(22,60,1),RGB(233,255,225)};
PartialSellColor[2]   = {RGB(187,187,187),RGB(51,51,51)};
PartialSellBKColor[2] = {RGB(22,55,1),RGB(238,255,230)};
PendingColor[2]       = {RGB(187,187,187),RGB(51,51,51)};
PendingBKColor[2]     = {RGB(1,52,53),RGB(255,255,203)};
RejectColor[2]        = {RGB(187,187,187),RGB(51,51,51)};
RejectBKColor[2]      = {RGB(128,0,0),RGB(255,128,128)};
CancelColor[2]        = {RGB(187,187,187),RGB(51,51,51)};
CancelBkColor[2]      = {RGB(76,76,76),RGB(229,229,229)};
//---------------------------------------------------------------------------
const char* DEF_FIELDS = "1=56,2=56,3=92,28=56,16=44,25=56,29=56,19=56,30=92,26=56,21=74,27=56,24=56,33=56,";
//---------------------------------------------------------------------------
__fastcall TExecutionForm::TExecutionForm(TComponent* Owner )
:TForm(Owner)
,FColumns( DEF_FIELDS )
,FFontSize( EXEC_FONT_SIZE_DEF )
,FFilter( ftActive )
,FProfile( L"")
{
	InitTimer->Tag = 0; ///< Not init
	FBtns[0] = ActiveButton;
	FFilters[0] = ftActive;
	FBtns[1] = AllButton;
	FFilters[1] = ftAll;
	FBtns[2] = CancelButton;
	FFilters[2] = ftCanceled;
	FBtns[3] = FillButton;
	FFilters[3] = ftFill;
	FBtns[4] = StopPxButton;
	FFilters[4] = ftStopOrder;
	DefaultMonitor = dmMainForm;
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_MOUSEACTIVATE:
			 this->BringToFront();
			 ContractViewerForm->ActiveClient( this );
			 Msg.Result = MA_ACTIVATE;
			 return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
OrderFilterEnum __fastcall TExecutionForm::SelectBtn(TObject *Sender)
{
	OrderFilterEnum SelFilter = ftActive;
	for( int i = 0; i < TABS; i++)
	{
		if( FBtns[i] == Sender )
		{
			WebBrowser->Visible = false;
			PageControl->ActivePageIndex = 0;
			SelFilter = FFilters[i];
			FBtns[i]->Selected  = true;
		}
		else
			FBtns[i]->Selected  = false;
	}
	return  SelFilter;
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::FilterButtonClick(TObject *Sender)
{
	FFilter = SelectBtn( Sender );
	Application->ProcessMessages();
	if( FFilter == ftNone )
		return;

	OrderListView->OrderStore = gOrderStore;
	OrderListView->Filter = FFilter;
}
//---------------------------------------------------------------------------
OrderFilterEnum	__fastcall TExecutionForm::GetFilterType( void )
{
	if( ActiveButton->Selected == true )
		return ftActive;
	else if( CancelButton->Selected == true )
		return ftCanceled;
	else if( FillButton->Selected   == true )
    	return ftFill;
	else if( StopPxButton->Selected == true )
    	return ftStopOrder;
    else
		return ftAll;
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::SetFilterType( OrderFilterEnum Filter )
{
	switch( Filter )
    {
		case ftActive:   FilterButtonClick( ActiveButton );break;
		case ftCanceled: FilterButtonClick( CancelButton );break;
		case ftFill:     FilterButtonClick( FillButton );break;
		case ftStopOrder:FilterButtonClick( StopPxButton );break;
		case ftAll:      FilterButtonClick( AllButton );break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::FormShow(TObject *Sender)
{
	if( InitTimer->Tag != 0 ) ///< Not init or change parent.
		return;

	InitTimer->Tag = 1;
	OrderListView->SetColumns( FColumns );
	InitTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::FormHide(TObject *Sender)
{
	String ColumnSettings,RegKey;

	OrderListView->OrderStore = NULL;
	RegKey.printf( L"%s\\ExecForm%d", FProfile.c_str(), GetGroup() );
	OrderListView->GetColumns(  ColumnSettings );
	g_Config.SetDesktopString(  RegKey, "Columns", ColumnSettings  );
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::OrderListViewProgress(TObject *Sender, int CurrentItemNo,
		  int TotalItemCount)
{
	if( TotalItemCount > 0 )
	{
		int Percent = CurrentItemNo*100 / TotalItemCount;

		if( CurrentItemNo >= TotalItemCount ) ///< finished.
		{
			ProgressPanel->Visible = false;
			ProgressBar->Progress  = Percent;
		}
		else
		{
			if( ProgressPanel->Visible == false )
			{
				ProgressBar->MinValue  = 0;
				ProgressBar->MaxValue  = 100;
				ProgressBar->Progress  = 0;
				ProgressPanel->Visible = true;
			}
			else if( Percent - ProgressBar->Progress >= 5 )
				ProgressBar->Progress  = Percent;
		}
		Application->ProcessMessages();
	}
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::InitTimerTimer(TObject *Sender)
{
	if( RoundFormEx->Lock ==false )
	{
		if( Left > MainForm->PageControl->ActivePage->Width )
			Left = MainForm->PageControl->ActivePage->Width - 50;
		if( Left < -1* (Width -50) )
			Left = -1* (Width -50);
		if( Top > MainForm->PageControl->ActivePage->Height )
			Top = MainForm->PageControl->ActivePage->Height - 50;
		if( Top < -1* (Height -50) )
			Top = -1* (Height -50);
	}
	InitTimer->Enabled = false;
	OrderListView->Visible = false;
	OrderListView->OrderStore = gOrderStore;
	if( FFilter != ftActive )
		OrderListView->Filter = ftActive;
	else
		OrderListView->Filter = ftStopOrder;
	OrderListView->FilterMarket = fltmAll;
	SetFilterType( FFilter );
    OrderListView->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::LoadProperties( void )
{
	String RegKey;
	int DefTop = MainForm->PageControl->Height - Height - 10;

	RegKey.printf( L"ExecForm%d", GetGroup() );
	///< Load columns
	FColumns  = g_Config.GetDesktopString( RegKey, "Columns", DEF_FIELDS );
	FFontSize = g_Config.GetDesktopInteger( RegKey, "FontSize", EXEC_FONT_SIZE_DEF );
	OrderListView->Font->Size = FFontSize;
	OrderListView->FitAllColumnWidth();
	OrderListView->SetColumns( FColumns );
	FFilter = (OrderFilterEnum)g_Config.GetDesktopInteger( RegKey, "Filter", (int)ftActive );
	FilterPanel->Height = 1;
	g_Config.GetDesktopFormPos( RegKey, "Form", RoundFormEx ,true, Left, DefTop, Width, Height );
	LoadColor();
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::SaveProperties( void )
{
	String RegKey;

	RegKey.printf( L"ExecForm%d", GetGroup() );
	///< Save columns
	OrderListView->GetColumns(  FColumns );
	g_Config.SetDesktopString(  RegKey, "Columns", FColumns  );
	g_Config.SetDesktopInteger( RegKey, "Filter", (int)GetFilterType() );
	g_Config.SetDesktopFormPos( RegKey, "Form", RoundFormEx );
	g_Config.SetDesktopInteger( RegKey, "FontSize", FFontSize );
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::OrderListViewOrderRightClick(TObject *Sender,
															 int OrderNID,
															 UnicodeString OrderID,
															 nsOrderMessageDefine::MarketEnum Market,
															 UnicodeString ExchangeCode,
															 UnicodeString SymbolCode,
															 nsOrderMessageDefine::SideEnum Side,
															 double Price, int LeavesQty,
															 nsOrderMessageDefine::OrderStatusEnum OrderStatus)
{
	ExchangeInfo*     ExInfo;

	FSelectedNID      = OrderNID;
	FSelectedMarket   = Market;
	FSelectedOrderID  = OrderID;
	FSelectedExchange = ExchangeCode;
	FSelectedSymbol   = SymbolCode;
	FSelectedSide     = Side;
	FSelectedPx       = Price;
	FSelectedQty      = LeavesQty;
	FPartialFill      = true;
	switch( OrderStatus )
	{
		case nsOrderMessageDefine::osNone:
		case nsOrderMessageDefine::osPendingCancel:
		case nsOrderMessageDefine::osPendingReplace:
		case nsOrderMessageDefine::osCanceled:
		case nsOrderMessageDefine::osFilled:
		case nsOrderMessageDefine::osRejected:
			QueryMenuItem->Enabled   = false;
			DeleteMenuItem->Enabled  = false;
			break;
		case nsOrderMessageDefine::osNew:
			FPartialFill = false;
		case nsOrderMessageDefine::osReplaced:
		case nsOrderMessageDefine::osPartiallyFilled:
			QueryMenuItem->Enabled   = true;
			DeleteMenuItem->Enabled  = true;
			break;
		case nsOrderMessageDefine::osPendingNew:
			QueryMenuItem->Enabled   = true;
			DeleteMenuItem->Enabled  = false;
	}
	PopupMenu->Popup( Mouse->CursorPos.x, Mouse->CursorPos.y );
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::DeleteMenuItemClick(TObject *Sender)
{
	try
	{
		gOrderStore->CancelOrder( FSelectedMarket, FSelectedExchange, FSelectedOrderID );
	}
	catch( UFC::Exception& execp )
	{
		String Msg;
		Msg.printf( L"刪單失敗[%s]", execp.what() );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, Msg );///< 刪單失敗"
	}
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::QueryMenuItemClick(TObject *Sender)
{
	try
	{
		gOrderStore->RequestOrderStatus(FSelectedNID, FSelectedMarket, FSelectedExchange, FSelectedOrderID, FSelectedSymbol, FSelectedSide, FSelectedPx);
	}
	catch( UFC::Exception& execp )
	{
		String Msg;
		Msg.printf( L"查詢失敗[%s]", execp.what() );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, Msg );///< 查詢失敗"
	}
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::WidthFitButtonClick(TObject *Sender)
{
	OrderListView->FitAllColumnWidth();
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::OrderListViewOrderKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift, int OrderNID, UnicodeString OrderID, nsOrderMessageDefine::MarketEnum Market,
		  UnicodeString SymbolCode, nsOrderMessageDefine::SideEnum Side, double Price, int LeavesQty,
		  nsOrderMessageDefine::OrderStatusEnum OrderStatus)
{
	if( Key == 'R' )
	{
		FSelectedMarket  = Market;
		FSelectedOrderID = OrderID;
		FSelectedSymbol  = SymbolCode;
		FSelectedPx      = Price;
		FSelectedQty     = LeavesQty;
		FPartialFill     = true;
		switch( OrderStatus )
		{
			case nsOrderMessageDefine::osNew:
				FPartialFill = false;
			case nsOrderMessageDefine::osReplaced:
			case nsOrderMessageDefine::osPartiallyFilled:
				break;
			default : break;
		}
	}
	else if( Key == VK_DELETE )
	{
		FSelectedMarket  = Market;
		FSelectedOrderID = OrderID;
		switch( OrderStatus )
		{
			case nsOrderMessageDefine::osNew:
			case nsOrderMessageDefine::osReplaced:
			case nsOrderMessageDefine::osPartiallyFilled:
				DeleteMenuItemClick( this );
				break;
			default : break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::LoadColor( void )
{
	int      DorL = (bool)g_Config.GetBoolProperty( "Setting","Skin", true );
	OrderListView->BuyNewColor             = NewBuyColor[DorL];
	OrderListView->BuyNewBKColor           = NewBuyBkColor[DorL];
	OrderListView->BuyFillColor            = FillBuyColor[DorL];
	OrderListView->BuyFillBKColor          = FillBuyBkColor[DorL];
	OrderListView->BuyPartialFillColor     = PartialBuyColor[DorL];
	OrderListView->BuyPartialFillBKColor   = PartialBuyBkColor[DorL];
	OrderListView->SellNewColor            = NewSellColor[DorL];
	OrderListView->SellNewBKColor          = NewSellBkColor[DorL];
	OrderListView->SellFillColor           = FillSellColor[DorL];
	OrderListView->SellFillBKColor         = FillSellBKColor[DorL];
	OrderListView->SellPartialFillColor    = PartialSellColor[DorL];
	OrderListView->SellPartialFillBKColor  = PartialSellBKColor[DorL];
	OrderListView->PendingColor            = PendingColor[DorL];
	OrderListView->PendingBKColor          = PendingBKColor[DorL];
	OrderListView->RejectColor             = RejectColor[DorL];
	OrderListView->RejectBKColor           = RejectBKColor[DorL];
	OrderListView->CanceledColor           = CancelColor[DorL];
	OrderListView->CanceledBKColor         = CancelBkColor[DorL];
	OrderListView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::QueryButtonClick(TObject *Sender)
{
	MarketFilterEnum CurMarketFilter;
	SideFilterEnum   CurSideFilter;

	switch( MarketRadioGroup->ItemIndex )
	{
		case 1: CurMarketFilter = fltmFutures;
				break;
		case 2: CurMarketFilter = fltmOptions;
				break;
		case 0:  //All
		default: CurMarketFilter = fltmAll;break;
	}
	switch (SideRadioGroup->ItemIndex)
	{
		case 1:	CurSideFilter = fltsBuy;break;
		case 2:	CurSideFilter = fltsSell;break;
		case 0:  //All
		default: CurSideFilter = fltsAll;break;
	}

	String CurSymbolFilter = edtSymbol->Text;
	double CurPriceFilter;

	if (edtPrice->Text.Length() > 0)
	{
		try
		{
			CurPriceFilter = edtPrice->Text.ToDouble();
		}
		catch( ... )
		{
			edtPrice->Text = L"";
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"回報查詢錯誤[不正確的價格]" );
			return;
		}
	}
	else
		CurPriceFilter = 0.0;
	OrderListView->SetFilters( CurMarketFilter, CurSideFilter, CurSymbolFilter, CurPriceFilter);
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::ClearButtonClick(TObject *Sender)
{
	MarketFilterEnum CurMarketFilter;
	SideFilterEnum   CurSideFilter;

	switch( MarketRadioGroup->ItemIndex )
	{
		case 1: CurMarketFilter = fltmFutures;
				break;
		case 2: CurMarketFilter = fltmOptions;
				break;
		case 0:  //All
		default: CurMarketFilter = fltmAll;break;
	}
	switch (SideRadioGroup->ItemIndex)
	{
		case 1:	CurSideFilter = fltsBuy;break;
		case 2:	CurSideFilter = fltsSell;break;
		case 0:  //All
		default: CurSideFilter = fltsAll;break;
	}
	edtSymbol->Text = L"";
	edtPrice->Text  = L"";
	OrderListView->SetFilters( CurMarketFilter, CurSideFilter, L"", 0.0 );
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::MarketRadioGroupClick(TObject *Sender)
{
	switch( MarketRadioGroup->ItemIndex )
	{
		case 1: OrderListView->FilterMarket = fltmFutures;
				break;
		case 2: OrderListView->FilterMarket = fltmOptions;
				break;
		case 0:  //All
		default: OrderListView->FilterMarket = fltmAll; break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::SideRadioGroupClick(TObject *Sender)
{
	switch( SideRadioGroup->ItemIndex )
	{
		case 1:	OrderListView->FilterSide = fltsBuy;break;
		case 2:	OrderListView->FilterSide = fltsSell;break;
		case 0:  //All
		default:OrderListView->FilterSide = fltsAll;break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::edtSymbolKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if( Key == VK_RETURN )
		QueryButtonClick( this );
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::edtPriceKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if( Key == VK_RETURN )
		QueryButtonClick( this );
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::FormActivate(TObject *Sender)
{
	edtSymbol->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::LargeButtonClick(TObject *Sender)
{
	FFontSize++;
	if( FFontSize > EXEC_FONT_SIZE_MAX )
	{
		FFontSize = EXEC_FONT_SIZE_MAX;
		return;
	}
	OrderListView->Font->Size = FFontSize;
	OrderListView->FitAllColumnWidth();
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::SmallButtonClick(TObject *Sender)
{
	FFontSize--;
	if( FFontSize < EXEC_FONT_SIZE_MIN )
	{
		FFontSize = EXEC_FONT_SIZE_MIN;
		return;
	}
	OrderListView->Font->Size = FFontSize;
	OrderListView->FitAllColumnWidth();
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::Release( void )
{
	SaveProperties();
	OrderListView->OrderStore = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::TAIFEXBalanceURL( String& URL )
{
	String     Acc( gOrderStore->Account );
	AnsiString ChkSumStr = L"098" + Acc + "megasecu";
	UFC::MD5   ChkSum( (const UFC::UInt8*)ChkSumStr.c_str(), ChkSumStr.Length() );
	String     Key( ChkSum.ToString().c_str() );

	URL.printf( L"%s?branch_id=098&cust_id=%s&key=%s", g_Config.GetMegaBackendURL(), Acc, Key );
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::TWSEBalanceURL( String& URL )
{
	String     Acc( gOrderStore->TWSEAccount );
	AnsiString ChkSumStr = L"7000" + Acc + "megasecu";
	UFC::MD5   ChkSum( (const UFC::UInt8*)ChkSumStr.c_str(), ChkSumStr.Length() );
	String     Key( ChkSum.ToString().c_str() );

	URL.printf( L"https://rayin.emega.com.tw/bking.jsp?branch_id=7000&cust_id=%s&key=%s", Acc, Key );
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::ShowBalanceWeb( int i  )
{
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::SetVisible( bool IsVisible )
{
	this->Visible = IsVisible;
}
//---------------------------------------------------------------------------
void __fastcall TExecutionForm::RoundFormExLockIconClick(TObject *Sender)
{
	String ColumnSettings,RegKey;

	RegKey.printf( L"%s\\ExecForm%d", FProfile.c_str(), GetGroup() );
	OrderListView->GetColumns(  FColumns );
	g_Config.SetDesktopString(  RegKey, "Columns", FColumns  );
	InitTimer->Tag = 0;
	if( PageControl->ActivePage != ExecTabSheet )
		FilterButtonClick( ActiveButton );
}
//---------------------------------------------------------------------------

