//---------------------------------------------------------------------------
#include <vcl.h>
#include <Vcl.Clipbrd.hpp>
#pragma hdrstop
#include "FMTConfig.h"
#include "HoldOpenInterest.h"
#include "main.h"
#include "MarketDataStore.h"
#include "TradingObjects.h"
#include "TradingObjectPool.h"
#include "TradingInvestors.h"
#include "TradingPosition.h"
#include "ContractViewer.h"
#include "UnifyDlg.h"
#include "GraphUtility.h"
#include "Roomi.h"
#include "UnifyUtility.h"
#include "AskCloseAll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma link "GraphPanel"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
extern Config             g_Config;
extern TCMarketDataStore* gMarketDataStore;
extern TOrderStore*       gOrderStore;
//---------------------------------------------------------------------------
THoldOpenInterestForm *HoldOpenInterestForm;
//---------------------------------------------------------------------------
const int CHINA_SUBITEM_COUNT = 13;
const int SUBITEM_COUNT = 14;
const int BALANCE_DISPLAY_DIGI = 2;
//---------------------------------------------------------------------------
__fastcall THoldOpenInterestForm::THoldOpenInterestForm(TComponent* Owner)
: TForm(Owner)
,FIsOpenInterestColumnDisplayed( false )
,FSubItemCount( SUBITEM_COUNT )
,FTotalProfit( 0 )
{
	FSubtotalItem = FPList->Items->Add();
	FSubtotalItem->Caption = L" ";
	for (int i = 0; i < FSubItemCount; i ++)
		 FSubtotalItem->SubItems->Add(L" ");
	RoundFormEx->Lockbox = false;
	FOrigWindowProc    = FPList->WindowProc;
	FOrigOIWindowProc  = OIList->WindowProc;
	FPList->WindowProc = ListWndProc;
	OIList->WindowProc = OIListWndProc;
	FEarseBKCanvas = new TCanvas();
	DefaultMonitor = dmDesktop;
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::EraseBK( HDC DC, TListView* ListView )
{
	HWND HeaderHandle = ListView_GetHeader( ListView->Handle );
	TRect HeaderItemRect;

	FEarseBKCanvas->Handle = DC;
	FEarseBKCanvas->Brush->Color = ListView->Color;
	Header_GetItemRect( HeaderHandle, 0, &HeaderItemRect );

	TRect RepaintRect( 0, HeaderItemRect.Height(), ListView->Width, ListView->Height );
	HRGN  MainRgn = CreateRectRgnIndirect( &RepaintRect ); ///< TList rect

	if( ListView->Items->Count > 0 )
	{
		TRect ItemRect;
		int width, height;

		///< Get the last item rect.
		ListView_GetItemRect( ListView->Handle, ListView->Items->Count - 1, &ItemRect, LVIR_BOUNDS );
		if( ItemRect.Bottom > FPList->Height )
			height = ListView->Height;
		else
			height = ItemRect.Bottom;
		width = ItemRect.Width();
		TRect TmpRect = Rect( 0, 0, width, height );
		HRGN ItemRgn = CreateRectRgnIndirect( &TmpRect );     ///< Items rect
		CombineRgn( MainRgn, MainRgn, ItemRgn, RGN_DIFF );    ///< Repaint rgn -= Items rect
		FillRgn( DC, MainRgn, FEarseBKCanvas->Brush->Handle );///< Fill Repaint rgn
		DeleteObject( (HGDIOBJ)ItemRgn );
	}
	else
		FillRgn( DC, MainRgn, FEarseBKCanvas->Brush->Handle ); ///< No items, fill Repaint rect
	DeleteObject( (HGDIOBJ)MainRgn );
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::ListWndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:
			if(  FPList->ViewStyle == vsReport )
			{
				EraseBK( (HDC) Msg.WParam, FPList );
				Msg.Result = 1;
			}
			else
				FOrigWindowProc( Msg );
			break;
		default:
			FOrigWindowProc( Msg );
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::OIListWndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:
			if(  FPList->ViewStyle == vsReport )
			{
				EraseBK( (HDC) Msg.WParam, OIList );
				Msg.Result = 1;
			}
			else
				FOrigOIWindowProc( Msg );
			break;
		default:
			FOrigOIWindowProc( Msg );
			break;
	}
}
//---------------------------------------------------------------------------
String __fastcall THoldOpenInterestForm::ToCurrencyString( double Number )
{
	Currency Balance( Number );
	return CurrToStrF( Balance, ffNumber, BALANCE_DISPLAY_DIGI );
}
//---------------------------------------------------------------------------
void THoldOpenInterestForm::RefreshSumItem()
{
	TListItem *summaryItemPtr = FPList->Items->Item[ FPList->Items->Count - 1 ]; ///< Summary Item
	double floatingProfitSum = 0.0;
	double closeProfitSum = 0.0;
	double taxSum = 0.0;
	double feeSum = 0.0;
	String tempStr;

	for( int i = 0; i < FPList->Items->Count - 1; i++ )
	{
		TListItem *curItemPtr = FPList->Items->Item[ i ];
		if( curItemPtr != NULL && curItemPtr->Data != NULL )
		{
			s888::CPositionStatisticRecord *curPosStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(curItemPtr->Data);

			curPosStatisticRecPtr->LockForRead();
			floatingProfitSum += curPosStatisticRecPtr->GetFloatingProfit(s888::rwNotLock);
			closeProfitSum    += curPosStatisticRecPtr->GetCloseProfit(s888::rwNotLock);
			feeSum += curPosStatisticRecPtr->GetFee(s888::rwNotLock);
			taxSum += curPosStatisticRecPtr->GetTax(s888::rwNotLock);
			curPosStatisticRecPtr->UnlockForRead();
		}
	}
	Currency floatingProfitCurr( floatingProfitSum );
	Currency closeProfitCurr( closeProfitSum );

	summaryItemPtr->SubItems->Strings[10] = CurrToStrF( floatingProfitCurr, ffNumber, BALANCE_DISPLAY_DIGI );
	summaryItemPtr->SubItems->Strings[11] = CurrToStrF( closeProfitCurr, ffNumber, BALANCE_DISPLAY_DIGI );
	tempStr.printf( L"%10.1lf", feeSum );
	summaryItemPtr->SubItems->Strings[12] = tempStr;
	tempStr.printf( L"%10.1lf", taxSum );
	summaryItemPtr->SubItems->Strings[13] = tempStr;
	FTotalProfit = floatingProfitSum + closeProfitSum - feeSum - taxSum;
	ProfitStaticText->Caption = ToCurrencyString( FTotalProfit );
}
//---------------------------------------------------------------------------
void THoldOpenInterestForm::CheckItemWidth( int i,  const String& ItemText, TListItem* ItemPtr )
{
	int textWidth = FPList->Canvas->TextWidth( ItemText ) + 15;

	ItemPtr->SubItems->Strings[i-1] = ItemText;
	if (FPList->Columns->Items[i]->Width < textWidth)
		FPList->Columns->Items[i]->Width = textWidth;
}
//---------------------------------------------------------------------------
void THoldOpenInterestForm::RefreshItem(const String& Exchange, const String& Symbol, TListItem* ItemPtr, void *PositionStatisticRecordPtr)
{
	if( PositionStatisticRecordPtr != NULL && ItemPtr != NULL )
	{
		s888::CPositionStatisticRecord *posStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(PositionStatisticRecordPtr);

		posStatisticRecPtr->LockForRead();
		String exchangeCode = posStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
		String symbol = posStatisticRecPtr->GetSymbol(s888::rwNotLock);
		int    buyQuantity = posStatisticRecPtr->GetBuyQuantity(s888::rwNotLock);
		double buyAmount = posStatisticRecPtr->GetBuyAmount(s888::rwNotLock);
		int    sellQuantity = posStatisticRecPtr->GetSellQuantity(s888::rwNotLock);
		double sellAmount = posStatisticRecPtr->GetSellAmount(s888::rwNotLock);
		double contractMultiplier = posStatisticRecPtr->GetContractMultiplier(s888::rwNotLock);
		double lastPrice = posStatisticRecPtr->GetLastPrice(s888::rwNotLock);
		double floatingProfit = posStatisticRecPtr->GetFloatingProfit(s888::rwNotLock);
		double closeProfit = posStatisticRecPtr->GetCloseProfit(s888::rwNotLock);
		double tax = posStatisticRecPtr->GetTax(s888::rwNotLock);
		double fee = posStatisticRecPtr->GetFee(s888::rwNotLock);
		posStatisticRecPtr->UnlockForRead();
		double buyAveragePrice = 0.0;
		double sellAveragePrice = 0.0;
		String tempStr;
		int    textWidth;
		TTickList* ticksPtr = (TTickList*)gMarketDataStore->GetTickList( Exchange, Symbol);

		if( contractMultiplier <= 0.0 )
			contractMultiplier = 1.0;
		if( buyQuantity > 0 )
			buyAveragePrice = buyAmount / (double)buyQuantity / contractMultiplier;
		if( sellQuantity > 0 )
			sellAveragePrice = sellAmount / (double)sellQuantity / contractMultiplier;

		tempStr.printf(L"%10.02f", buyAveragePrice);
		CheckItemWidth( 1, tempStr, ItemPtr );

		tempStr.printf(L"%10.02f", sellAveragePrice);
		CheckItemWidth( 2, tempStr, ItemPtr );

		tempStr.printf(L"%d", buyQuantity - sellQuantity);
		CheckItemWidth( 3, tempStr, ItemPtr );

		String formAccount = gOrderStore->ID;//g_Config.GetLogonID( FItemIndex);
		int ydBuyOI  = 0;  //未平倉買昨
		int tdBuyOI  = 0;  //未平倉買今
		int ydSellOI = 0;  //未平倉賣昨
		int tdSellOI = 0;  //未平倉賣今
		s888::CTradingAccount *accountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(formAccount, s888::rwLockForRead);
		if( accountPtr !=  NULL )
		{
			s888::CContractPosition *contractPositionPtr = accountPtr->FindContractPositionFromSymbolIndex(exchangeCode, symbol, s888::rwLockForRead);
			if (contractPositionPtr != 0)
			{
				contractPositionPtr->LockForRead();
				int buyHoldQty = contractPositionPtr->GetBuyHoldQuantity(s888::rwNotLock);
				int buyFillOpenQty = contractPositionPtr->GetBuyFillOpenQuantity(s888::rwNotLock);
				int buyFillCloseQty = contractPositionPtr->GetBuyFillCloseQuantity(s888::rwNotLock);
				int buyFillCloseTodayQty = contractPositionPtr->GetBuyFillCloseTodayQuantity(s888::rwNotLock);
				int sellHoldQty = contractPositionPtr->GetSellHoldQuantity(s888::rwNotLock);
				int sellFillOpenQty = contractPositionPtr->GetSellFillOpenQuantity(s888::rwNotLock);
				int sellFillCloseQty = contractPositionPtr->GetSellFillCloseQuantity(s888::rwNotLock);
				int sellFillCloseTodayQty = contractPositionPtr->GetSellFillCloseTodayQuantity(s888::rwNotLock);
				contractPositionPtr->UnlockForRead();

				int buyOpenQty = buyHoldQty + buyFillOpenQty - sellFillCloseQty - sellFillCloseTodayQty;
				int sellOpenQty = sellHoldQty + sellFillOpenQty - buyFillCloseQty - buyFillCloseTodayQty;

				if (buyHoldQty > sellFillCloseQty)  //若昨日持倉大於今日平倉，表示昨日持倉尚未平倉完畢
					ydBuyOI = buyHoldQty - sellFillCloseQty;

				tdBuyOI = buyFillOpenQty - sellFillCloseTodayQty;  //今日新倉 減去 賣邊平今
				if ((tdBuyOI > 0) && (ydBuyOI <= 0))  //若尚有剩餘，再減去今日平倉 平倉  昨日持倉 後的剩餘口數
					tdBuyOI = tdBuyOI - (sellFillCloseQty - buyHoldQty);

				if (sellHoldQty > buyFillCloseQty)
					ydSellOI = sellHoldQty - buyFillCloseQty;

				tdSellOI = sellFillOpenQty - buyFillCloseTodayQty;
				if ((tdSellOI > 0) && (ydSellOI <= 0))
					tdSellOI = tdSellOI - (buyFillCloseQty - sellHoldQty);
			}
		}
		if( FIsOpenInterestColumnDisplayed )
		{
			tempStr.printf(L"昨:%2d 今:%2d", ydBuyOI, tdBuyOI);
			CheckItemWidth( 4, tempStr, ItemPtr );
			tempStr.printf(L"昨:%2d 今:%2d", ydSellOI, tdSellOI);
			CheckItemWidth( 5, tempStr, ItemPtr );
		}
		if( ticksPtr != NULL )
			ticksPtr->ToTNFormat(lastPrice, tempStr);
		else
			tempStr.printf(L"%10.4lf", lastPrice);
		CheckItemWidth( 6, tempStr, ItemPtr );

		double buyMarketValue = static_cast<double>(ydBuyOI + tdBuyOI) * contractMultiplier * lastPrice;
		tempStr.printf(L"%10.2lf", buyMarketValue);
		CheckItemWidth( 7, tempStr, ItemPtr );

		double sellMarketValue = (double)(ydSellOI + tdSellOI) * contractMultiplier * lastPrice;
		tempStr.printf(L"%10.2lf", -1 * sellMarketValue );
		CheckItemWidth( 8, tempStr, ItemPtr );

		tempStr.printf(L"%d", buyQuantity);
		CheckItemWidth( 9, tempStr, ItemPtr );

		tempStr.printf(L"%d", sellQuantity);
		CheckItemWidth( 10, tempStr, ItemPtr );

		Currency floatingProfitCurr(floatingProfit);
		tempStr = CurrToStrF(floatingProfitCurr, ffNumber, BALANCE_DISPLAY_DIGI );
		CheckItemWidth( 11, tempStr, ItemPtr );

		Currency closeProfitCurr(closeProfit);
		tempStr = CurrToStrF(closeProfitCurr, ffNumber, BALANCE_DISPLAY_DIGI);
		CheckItemWidth( 12, tempStr, ItemPtr );

		tempStr.printf(L"%10.1lf", fee );
		CheckItemWidth( 13, tempStr, ItemPtr );

		tempStr.printf(L"%10.1lf", tax);
		CheckItemWidth( 14, tempStr, ItemPtr );
		RefreshSumItem();
		ItemPtr->Update();
	}
}
//---------------------------------------------------------------------------
TListItem* __fastcall THoldOpenInterestForm::FindItem( s888::CPositionStatisticRecord *posStatisticRecPtr, String& sortKey, int& insertPosition )
{
	insertPosition = FPList->Items->Count - 1;

	for( register int i = 0; i < FPList->Items->Count; i++ )
	{
		TListItem *curItemPtr = FPList->Items->Item[i];//(*(FPList->Items))[i];
		if( curItemPtr->Data != NULL )
		{
			s888::CPositionStatisticRecord *curPosStatisticRecPtr = (s888::CPositionStatisticRecord*)curItemPtr->Data;

			if( curPosStatisticRecPtr != posStatisticRecPtr )
			{
				String curSortKey;

				curPosStatisticRecPtr->LockForRead();
				curSortKey = curPosStatisticRecPtr->GetExchangeCode(s888::rwNotLock) + curPosStatisticRecPtr->GetSymbol(s888::rwNotLock);
				curPosStatisticRecPtr->UnlockForRead();
				if( (sortKey <= curSortKey) && (i < insertPosition))
					insertPosition = i;
			}
			else ///< OK! find it!
				return curItemPtr;
		}
	}
	///< Position Record not found!
	return NULL;
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::InitPositionItem( const String& ExchangeCode, const String& Symbol, TListItem *targetItemPtr, void *posStatisticRecPtr )
{
	String commodityName;

	GetDisplayName( ExchangeCode, Symbol, commodityName );
	for( int i = 0; i < SUBITEM_COUNT; i ++ )
		 targetItemPtr->SubItems->Add(L" ");
	targetItemPtr->Data = posStatisticRecPtr;
	targetItemPtr->ImageIndex = 0;
	targetItemPtr->StateIndex = 0;
	targetItemPtr->Caption = commodityName;
}
//---------------------------------------------------------------------------
void THoldOpenInterestForm::UpdateFloatingProfit(const String& ExchangeCode, const String& Symbol, void *PositionStatisticRecordPtr)
{
	if (PositionStatisticRecordPtr != NULL )
	{
		s888::CPositionStatisticRecord *posStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(PositionStatisticRecordPtr);

		posStatisticRecPtr->LockForRead();
		int buyQuantity  = posStatisticRecPtr->GetBuyQuantity(s888::rwNotLock);
		int sellQuantity = posStatisticRecPtr->GetSellQuantity(s888::rwNotLock);
		posStatisticRecPtr->UnlockForRead();
		if( buyQuantity > 0 || sellQuantity > 0 )
		{
			int        insertPosition;
			String     sortKey = ExchangeCode + Symbol;
			TListItem* targetItemPtr = FindItem( posStatisticRecPtr, sortKey, insertPosition );

			//If List Item Not Found, Create one.
			if( targetItemPtr == NULL )
			{
				targetItemPtr = FPList->Items->Insert( insertPosition );
				InitPositionItem( ExchangeCode, Symbol, targetItemPtr, posStatisticRecPtr );

				int NameWidth = FPList->Columns->Items[0]->Width;
				int DspWidth  = FPList->Canvas->TextWidth( targetItemPtr->Caption ) + 27;

				if( DspWidth > NameWidth )
					FPList->Columns->Items[0]->Width = DspWidth;
			}
			///< Update position info.
			RefreshItem( ExchangeCode, Symbol, targetItemPtr, posStatisticRecPtr);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::ProfitButtonClick(TObject *Sender)
{
	if( Sender == ProfitButton )
		SwitchPage( 0 );
	else if( Sender == HoldPositionButton )
		SwitchPage( 1 );
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::SwitchPage( int page )
{
	if( page == 0 )
	{
		ProfitButton->Selected = true;
		HoldPositionButton->Selected = false;
		PageControl->ActivePage = ProfitTabSheet;
	}
	else if( page == 1 )
	{
		ProfitButton->Selected = false;
		HoldPositionButton->Selected = true;
		PageControl->ActivePage = HoldPositionTabSheet;
		HoldPosition();
	}
}
//---------------------------------------------------------------------------
int __fastcall THoldOpenInterestForm::CurrentPage( void )
{
	if( PageControl->ActivePage == ProfitTabSheet )
		return 0;
	else if( PageControl->ActivePage == HoldPositionTabSheet )
		return 1;
	else
		return 0;
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::SaveProperties( void )
{
	String RegDir = L"Profit\\HoldOpenInterest";
	g_Config.SetDesktopBool(    RegDir, "IsColumnDisplayed", FIsOpenInterestColumnDisplayed );
	g_Config.SetDesktopInteger( RegDir, "OpenInterestPage", CurrentPage() );
	g_Config.SetDesktopFormPos( RegDir, "Form", RoundFormEx );
	g_Config.SetDesktopBool( RegDir, "Visible", Visible );
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::LoadProperties( void )
{
	String RegDir = L"Profit\\HoldOpenInterest";
	bool   IsShow = g_Config.GetDesktopBool( RegDir, "Visible", false );

	FIsOpenInterestColumnDisplayed = g_Config.GetDesktopBool( RegDir, L"IsColumnDisplayed", false );
	int pageNo = g_Config.GetDesktopInteger( RegDir, "OpenInterestPage", 0 );
	if (!TOrderStore::IsAutoPositionEffect() && (pageNo == 2))
		pageNo = 0;
	SwitchPage(pageNo);
	g_Config.GetDesktopFormPos( RegDir, "Form", RoundFormEx );
	OIMenuItemClick( NULL );
	if( IsShow == true )
		Show();
	else
		Hide();
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::FPListDrawItem(TCustomListView *Sender, TListItem *Item,
		  TRect &Rect, TOwnerDrawState State)
{
	TCustomDrawState CDState;
	bool             DefaultDraw;
	TTextFormat      Formats;
	TRect            DrawRect;

	CDState << cdsSelected;
	Sender->Canvas->Brush->Style = bsClear;
	if( Sender == OIList || Item->Index != Sender->Items->Count - 1 )
	{
		String DrawString = Item->Caption;
		bool HasIcon = false;

		if( Sender == FPList )
			HasIcon = true;
		if( Item->Selected == true )
		{
			DrawRect = Rect;
			if( HasIcon == true )
				DrawRect.left += 25;
			GDIUtility::DrawGradientBar( Sender->Canvas , DrawRect, clGray, clBlack );
			if( HasIcon == true )
			{
				DrawRect = Rect;
				DrawRect.right = DrawRect.left + 25;
				GDIUtility::DrawGradientBar( Sender->Canvas , DrawRect, clWhite, clSilver );
			}
			SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
		}
		else
		{
			GDIUtility::DrawGradientBar( Sender->Canvas , Rect, clWhite, clSilver );
			SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
		}
		Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
		if( HasIcon == true )
		{
			ImageList->Draw( Sender->Canvas, Rect.left, Rect.top, 2, true );
			Sender->Canvas->Pen->Color   = clSilver;
			Sender->Canvas->MoveTo( Rect.left + 24 , Rect.top );
			Sender->Canvas->LineTo( Rect.left + 24 , Rect.bottom );
		}
		Rect.Left += 26;
		Sender->Canvas->TextRect( Rect, DrawString, Formats );
	}
	else
	{
		GDIUtility::DrawGradientBar( Sender->Canvas , Rect, clNavy, clBlack );
		Sender->Canvas->Pen->Color   = clSilver;
		Sender->Canvas->MoveTo( Rect.left + 24 , Rect.top );
		Sender->Canvas->LineTo( Rect.left + 24 , Rect.bottom );
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
	}
	if( Sender == OIList )
		DefaultDraw = false;
	else
		DefaultDraw = true;
	for( int i = 1; i <= Item->SubItems->Count; i ++ )
		 FPListCustomDrawSubItem( Sender, Item ,i, CDState, DefaultDraw );
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::FPListCustomDrawSubItem(TCustomListView *Sender,
		  TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)

{
	TRect       ItemRect;
	TTextFormat Formats;
	String      DrawStr = Item->SubItems->Strings[ SubItem -1 ];

	if( ListView_GetSubItemRect( Sender->Handle, Item->Index, SubItem, LVIR_BOUNDS, &ItemRect ) )
	{
		Sender->Canvas->Brush->Style = bsClear;
		Sender->Canvas->Pen->Color   = clSilver;
		if( DefaultDraw == false || Item->Index != Sender->Items->Count - 1 )
		{
			if( Item->Selected == true )
			{
				GDIUtility::DrawGradientBar( Sender->Canvas , ItemRect, clGray, clBlack );
				SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
			}
			else
			{
				GDIUtility::DrawGradientBar( Sender->Canvas , ItemRect, clWhite, clSilver );
				if( Sender != OIList )
				{
					if (SubItem == 4)
						SetTextColor( Sender->Canvas->Handle,(COLORREF)clRed );
					else if (SubItem == 5)
						SetTextColor( Sender->Canvas->Handle,(COLORREF)clGreen );
					else if (SubItem == 11 )
					{
						s888::CPositionStatisticRecord *curPosStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(Item->Data);
						if( curPosStatisticRecPtr != NULL  )
						{
							if( curPosStatisticRecPtr->GetFloatingProfit(s888::rwNotLock) >= 0 )
								SetTextColor( Sender->Canvas->Handle,(COLORREF)clRed );
							 else
								SetTextColor( Sender->Canvas->Handle,(COLORREF)clGreen );
						}
						else
							SetTextColor( Sender->Canvas->Handle,(COLORREF)clBlack );
					}
					else
						SetTextColor( Sender->Canvas->Handle,(COLORREF)clBlack );
				}
				else
				{
					if( SubItem == 3 ) ///< Symbol name
						SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
					else
						SetTextColor( Sender->Canvas->Handle,(COLORREF)clBlack );
				}
			}
		}
		else
		{
			GDIUtility::DrawGradientBar( Sender->Canvas , ItemRect, clNavy, clBlack );
			SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
		}
		Sender->Canvas->MoveTo( ItemRect.left, ItemRect.top );
		Sender->Canvas->LineTo( ItemRect.left, ItemRect.bottom );
		ItemRect.right -= 2;
		Formats <<tfSingleLine<<tfRight<<tfVerticalCenter;
		Sender->Canvas->TextRect( ItemRect, DrawStr, Formats );
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::FormShow(TObject *Sender)
{
	FSubtotalItem->SubItems->Strings[9]  = L"總計:";
	FSubtotalItem->SubItems->Strings[10] = L"浮動損益";
	FSubtotalItem->SubItems->Strings[11] = L"平倉損益";
	FSubtotalItem->SubItems->Strings[12] = L"費用";
	FSubtotalItem->SubItems->Strings[11] = L"交易稅";
	OIMenuItemClick( NULL );
	ProfitButtonClick( ProfitButton );
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::GetDisplayName( const String& Ex, const String& Sym, String& DisplayName )
{
	if( gMarketDataStore != NULL )
	{
		BasicInformation* SymInfo = gMarketDataStore->GetBasicInformation( Ex, Sym, false );
		if( SymInfo != NULL)
		{
			DisplayName = SymInfo->GetDisplayName();
			return;
		}
	}
	DisplayName = Sym;
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::FormCreate(TObject *Sender)
{
	FPList->Columns->Clear();
	TListColumn *CommodityColumnPtr = FPList->Columns->Add(); ///< 0
	CommodityColumnPtr->Caption = L"合約名稱";//Pmfnextstrings_NEXT_OPENINTEREST_COMMODITY_NAME;  //合約名稱
	CommodityColumnPtr->MinWidth = 120;
	CommodityColumnPtr->Width = Canvas->TextWidth( CommodityColumnPtr->Caption ) + 15;
	TListColumn *BuyAvgPxColumnPtr = FPList->Columns->Add();  ///< 1
	BuyAvgPxColumnPtr->Caption = L" 買成均價 ";//Pmfnextstrings_NEXT_OPENINTEREST_BUY_AVG_PX;  //買成均價
	BuyAvgPxColumnPtr->MinWidth = 75;
	BuyAvgPxColumnPtr->Width = Canvas->TextWidth( BuyAvgPxColumnPtr->Caption ) + 15;
	TListColumn *SellAvgPxColumnPtr = FPList->Columns->Add(); ///< 2
	SellAvgPxColumnPtr->Caption = L" 賣成均價 ";//Pmfnextstrings_NEXT_OPENINTEREST_SELL_AVG_PX;  //賣成均價
	SellAvgPxColumnPtr->MinWidth = 75;
	SellAvgPxColumnPtr->Width = Canvas->TextWidth( SellAvgPxColumnPtr->Caption ) + 15;
	TListColumn *NetPositionColumnPtr = FPList->Columns->Add(); ///< 3
	NetPositionColumnPtr->Caption = L"淨部位";//Pmfnextstrings_NEXT_OPENINTEREST_NET_POSITION;  //淨部位
	NetPositionColumnPtr->MinWidth = 60;
	NetPositionColumnPtr->Width =  Canvas->TextWidth( NetPositionColumnPtr->Caption ) + 15;
	TListColumn *buyOpenPositionColumnPtr = FPList->Columns->Add(); ///< 4
	buyOpenPositionColumnPtr->Caption = L"未平倉買";//Pmfnextstrings_NEXT_BUYOPENINTEREST_POSITION;  //未平倉買
	if (FIsOpenInterestColumnDisplayed)
	{
		buyOpenPositionColumnPtr->MinWidth = 80;
		buyOpenPositionColumnPtr->Width = Canvas->TextWidth( buyOpenPositionColumnPtr->Caption ) + 15;
		buyOpenPositionColumnPtr->MaxWidth = 200;
	}
	else
	{
		buyOpenPositionColumnPtr->MinWidth = 0;
		buyOpenPositionColumnPtr->Width = 0;
		buyOpenPositionColumnPtr->MaxWidth = 1;
	}
	TListColumn *sellOpenPositionColumnPtr = FPList->Columns->Add(); ///< 5
	sellOpenPositionColumnPtr->Caption = L"未平倉賣";//Pmfnextstrings_NEXT_SELLOPENINTEREST_POSITION;  //未平倉賣
	if (FIsOpenInterestColumnDisplayed)
	{
		sellOpenPositionColumnPtr->MinWidth = 80;
		sellOpenPositionColumnPtr->Width = Canvas->TextWidth( sellOpenPositionColumnPtr->Caption ) + 15;
		sellOpenPositionColumnPtr->MaxWidth = 200;
	}
	else
	{
		sellOpenPositionColumnPtr->MinWidth = 0;
		sellOpenPositionColumnPtr->Width = 0;
		sellOpenPositionColumnPtr->MaxWidth = 1;
	}
	TListColumn *MarketPxColumnPtr = FPList->Columns->Add(); ///< 6
	MarketPxColumnPtr->Caption = L"市價";//Pmfnextstrings_NEXT_OPENINTEREST_MARKET_PX;  //市價
	MarketPxColumnPtr->MinWidth = 80;
	MarketPxColumnPtr->Width = 80;
	TListColumn *BuyMarketValueColumnPtr = FPList->Columns->Add(); ///< 7
	BuyMarketValueColumnPtr->Caption = L"期權買方市值";//Pmfnextstrings_NEXT_OPENINTEREST_BUY_MARKET_VALUE;  //期權買方市值
	BuyMarketValueColumnPtr->MinWidth = 90;
	BuyMarketValueColumnPtr->Width = Canvas->TextWidth( BuyMarketValueColumnPtr->Caption ) + 15;
	BuyMarketValueColumnPtr->MaxWidth = 180;
	TListColumn *SellMarketValueColumnPtr = FPList->Columns->Add(); ///< 8
	SellMarketValueColumnPtr->Caption = L"期權賣方市值";//Pmfnextstrings_NEXT_OPENINTEREST_SELL_MARKET_VALUE;  //期權賣方市值
	SellMarketValueColumnPtr->MinWidth = 90;
	SellMarketValueColumnPtr->Width = Canvas->TextWidth( SellMarketValueColumnPtr->Caption ) + 15;
	SellMarketValueColumnPtr->MaxWidth = 180;
	TListColumn *BuyPositionColumnPtr = FPList->Columns->Add(); ///< 9
	BuyPositionColumnPtr->Caption = L"買成";//Pmfnextstrings_NEXT_OPENINTEREST_BUY_POSITION;  //買成
	BuyPositionColumnPtr->MinWidth = 45;
	BuyPositionColumnPtr->Width =  Canvas->TextWidth( BuyPositionColumnPtr->Caption ) + 15;
	TListColumn *SellPositionColumnPtr = FPList->Columns->Add();///< 10
	SellPositionColumnPtr->Caption = L"賣成";//Pmfnextstrings_NEXT_OPENINTEREST_SELL_POSITION;  //賣成
	SellPositionColumnPtr->MinWidth = 45;
	SellPositionColumnPtr->Width = Canvas->TextWidth( SellPositionColumnPtr->Caption ) + 15;
	TListColumn *FloatProfitColumnPtr = FPList->Columns->Add(); ///< 11
	FloatProfitColumnPtr->Caption = L"浮動損益";//Pmfnextstrings_NEXT_OPENINTEREST_FLOATPROFIT;  //浮動損益
	FloatProfitColumnPtr->MinWidth = 75;
	FloatProfitColumnPtr->Width = Canvas->TextWidth( FloatProfitColumnPtr->Caption ) + 15;;
	TListColumn *CloseProfitColumnPtr = FPList->Columns->Add(); ///< 12
	CloseProfitColumnPtr->Caption = L"平倉損益";//Pmfnextstrings_NEXT_OPENINTEREST_CLOSEPROFIT;  //平倉損益
	CloseProfitColumnPtr->MinWidth = 75;
	CloseProfitColumnPtr->Width = Canvas->TextWidth( CloseProfitColumnPtr->Caption ) + 15;;
	TListColumn *FeeColumnPtr = FPList->Columns->Add(); ///< 13
	FeeColumnPtr->Caption = L"費用";
	FeeColumnPtr->MinWidth = 60;
	FeeColumnPtr->Width =  Canvas->TextWidth( FeeColumnPtr->Caption ) + 15;;
	TListColumn *TaxColumnPtr = FPList->Columns->Add();
	TaxColumnPtr->Caption = L"交易稅";
	TaxColumnPtr->MinWidth = 60;
	TaxColumnPtr->Width =  Canvas->TextWidth( TaxColumnPtr->Caption ) + 15;;
}
//-------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::FPListColumnRightClick(TObject *Sender, TListColumn *Column,
															  TPoint &Point)
{
	if (FIsOpenInterestColumnDisplayed)
		colMenu->Items->Items[0]->Caption = L"影藏未平倉量";//Pmfnextstrings_NEXT_HIDE_OPENINTEREST;
	else
		colMenu->Items->Items[0]->Caption = L"顯示未平倉量";//Pmfnextstrings_NEXT_DISPLAY_OPENINTEREST;
	POINT mousePos;
	GetCursorPos(&mousePos);
	colMenu->Popup(mousePos.x, mousePos.y);
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::OIMenuItemClick(TObject *Sender)
{
	if( Sender != NULL )
		FIsOpenInterestColumnDisplayed = !FIsOpenInterestColumnDisplayed;
	if (FIsOpenInterestColumnDisplayed)
	{
		FPList->Columns->Items[4]->MaxWidth = 120;
		FPList->Columns->Items[4]->Width = 80;
		FPList->Columns->Items[4]->MinWidth = 80;
		FPList->Columns->Items[5]->MaxWidth = 120;
		FPList->Columns->Items[5]->Width = 80;
		FPList->Columns->Items[5]->MinWidth = 80;
	}
	else
	{
		FPList->Columns->Items[4]->MinWidth = 0;
		FPList->Columns->Items[4]->Width = 0;
		FPList->Columns->Items[4]->MaxWidth = 1;
		FPList->Columns->Items[5]->MinWidth = 0;
		FPList->Columns->Items[5]->Width = 0;
		FPList->Columns->Items[5]->MaxWidth = 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::PositionListCustomDrawSubItem(TCustomListView *Sender,
		  TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)

{
	TRect       ItemRect;
	TTextFormat Formats;
	String      DrawStr = Item->SubItems->Strings[ SubItem -1 ];

	if( ListView_GetSubItemRect( Sender->Handle, Item->Index, SubItem, LVIR_BOUNDS, &ItemRect ) )
	{
		Sender->Canvas->Brush->Style = bsClear;
		Sender->Canvas->Pen->Color   = clSilver;
		GDIUtility::DrawGradientBar( Sender->Canvas , ItemRect, clWhite, clSilver );
		SetTextColor( Sender->Canvas->Handle, (COLORREF)clBlack );
		Sender->Canvas->MoveTo( ItemRect.left, ItemRect.top );
		Sender->Canvas->LineTo( ItemRect.left, ItemRect.bottom );
		ItemRect.right -= 2;
		Formats <<tfSingleLine<<tfRight<<tfVerticalCenter;
		Sender->Canvas->TextRect( ItemRect, DrawStr, Formats );
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::PositionListDrawItem(TCustomListView *Sender,
		  TListItem *Item, TRect &Rect, TOwnerDrawState State)
{
	TCustomDrawState CDState;
	bool             DefaultDraw;
	TTextFormat      Formats;
	String           DrawString = Item->Caption;
	int              ColumnCount = ((TListView*)Sender)->Columns->Count;

	CDState << cdsSelected;
	Sender->Canvas->Brush->Style = bsClear;
	GDIUtility::DrawGradientBar( Sender->Canvas , Rect, clWhite, clSilver );
	SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
	Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
	Sender->Canvas->TextRect( Rect, DrawString, Formats );

	for( int i = 1; i < ColumnCount; i ++ )
		 PositionListCustomDrawSubItem( Sender, Item ,i, CDState, DefaultDraw );
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::FPListDblClick(TObject *Sender)
{
	TListItem *Item = FPList->Selected;

	if( Item != NULL )
	{
		String exchangeCode, symbol;
		s888::CPositionStatisticRecord *curPosStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(Item->Data);
		if (curPosStatisticRecPtr != NULL )
		{
			curPosStatisticRecPtr->LockForRead();
			exchangeCode = curPosStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
			symbol       = curPosStatisticRecPtr->GetSymbol(s888::rwNotLock);
			curPosStatisticRecPtr->UnlockForRead();
			ContractViewerForm->OpenOrderBookForm( exchangeCode, symbol, true );
		}
	}
}
//---------------------------------------------------------------------------
double __fastcall THoldOpenInterestForm::GetCloseOrderPx( const String& exchange, const String& symbol, double Px, int BetterTick )
{
	TTickList* TickList = (TTickList*)gMarketDataStore->GetTickList( exchange, symbol );
	if( TickList != NULL )
	{
		int PxIndex  = TickList->FindTick( Px ) + BetterTick;
		int IndexMax = TickList->ItemCount() -1;

		if( PxIndex < 0 )
			PxIndex = 0;
		if( PxIndex > IndexMax )
			PxIndex = IndexMax;
		return TickList->GetPxFromIndex( PxIndex );
	}
	return 0.0;
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::DeleteAllOrderSync( TOrderStore* OrdStore, const String& exchange, const String& symbol )
{  	///< delete all orders.
	int CurrExecCount = OrdStore->CancelReportCount;
	int CancelCount   = OrdStore->CancelAllOrder( exchange, symbol );
	int StopExecCount = CurrExecCount + CancelCount;

	///< Wait all canceled execution.
	for( int i = 0; i < CancelCount*50; i++ )
	{
		UFC::SleepMS( 10 );
		Application->ProcessMessages();
		if( OrdStore->CancelReportCount >= StopExecCount )
			break;
	}
	if( CancelCount > 0 )
		UFC::SleepMS( 50 );
}
//---------------------------------------------------------------------------
bool __fastcall THoldOpenInterestForm::CloseOrder( TOrderStore* OrdStore, BasicInformation* Info, nsOrderMessageDefine::SideEnum Side, double Price, int Qty, int MaxQty )
{
	if( Qty == 0 )
	{
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"平倉口數不能為0." );
		return false;
	}
	try
	{
		nsOrderMessageDefine::MarketEnum      Market = Info->GetMarket();
		nsOrderMessageDefine::OrderTypeEnum   OrderType = nsOrderMessageDefine::otLimit;
		nsOrderMessageDefine::TimeInForceEnum TIF = nsOrderMessageDefine::tifROD;

		if( Price == 0.0 )
		{
			OrderType = nsOrderMessageDefine::otMarket;
			if( Market == nsOrderMessageDefine::mTSE ||Market == nsOrderMessageDefine::mOTC )
				TIF = nsOrderMessageDefine::tifROD;
			else
				TIF = nsOrderMessageDefine::tifIOC;
		}
		int Times = Qty / MaxQty;
		int LastQty =  Qty % MaxQty;

		for( int i = 0; i< Times; i++ )
		{
			OrdStore->NewOrder( Market, Info->GetExchange().c_str(), Info->GetSymbol().c_str(),
							   Side, Price, MaxQty, OrderType, TIF, nsOrderMessageDefine::peAuto,
							   (nsOrderMessageDefine::EventTypeEnum)Info->GetCallPut(), Info->GetStrikePrice(), 0,0,'0');
		}
		if( LastQty > 0 )
			OrdStore->NewOrder( Market, Info->GetExchange().c_str(), Info->GetSymbol().c_str(),
							   Side, Price, LastQty, OrderType, TIF, nsOrderMessageDefine::peAuto,
							   (nsOrderMessageDefine::EventTypeEnum)Info->GetCallPut(), Info->GetStrikePrice(), 0,0,'0');
		return true;
	}
	catch( UFC::Exception& execp )
	{
		TUnifyDlgs::MessageDialog( L"委託失敗", execp.what() );
		return false;
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::DeleteWorkingCloseAll( TListItem *Item, int BetterTick, bool IsMarket, int MaxQty )
{

	s888::CPositionStatisticRecord *curPosStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(Item->Data);
	if (curPosStatisticRecPtr != NULL )
	{
		String exchange, symbol;
		nsOrderMessageDefine::SideEnum Side;
		BasicInformation* Info;
		double LastPx, OrderPx;
		int    NetPos,dir;

		curPosStatisticRecPtr->LockForRead();
		exchange = curPosStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
		symbol   = curPosStatisticRecPtr->GetSymbol(s888::rwNotLock);
		LastPx   = curPosStatisticRecPtr->GetLastPrice(s888::rwNotLock);
		NetPos   = curPosStatisticRecPtr->GetBuyQuantity( s888::rwNotLock ) - curPosStatisticRecPtr->GetSellQuantity( s888::rwNotLock );
		curPosStatisticRecPtr->UnlockForRead();

		if( NetPos != 0 )
		{
			if( (Info = gMarketDataStore->GetBasicInformation( exchange,symbol,false )) != NULL )
			{
				if( NetPos < 0 )
				{
					dir   = -1;
					Side  = nsOrderMessageDefine::sBuy;
				}
				else
				{
					dir   = 1;
					Side  = nsOrderMessageDefine::sSell;
				}
				if( IsMarket == true || (OrderPx = GetCloseOrderPx( exchange,symbol, LastPx, dir * BetterTick )) != 0.0 ) ///< Get price.
				{
					///< delete all stop orders.
					if( gOrderStore != NULL )
					{   ///< delete all orders.
						DeleteAllOrderSync( gOrderStore, exchange, symbol );
						///< Place new order to close positions.
						if( IsMarket == true )
							CloseOrder( gOrderStore, Info, Side, 0.0, dir * NetPos, MaxQty );
						else
							CloseOrder( gOrderStore, Info, Side, OrderPx, dir * NetPos, MaxQty );
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::CloseAllItemClick(TObject *Sender)
{
	TListItem *Item;

	if( (Item = FPList->Selected) != NULL )
	{
		String StockName;

		StockName.printf( L"平倉[%s]", Item->Caption );
		AskCloseAllForm->Caption = StockName;
		if( AskCloseAllForm->ShowModal() == mrOk )
		{
			bool IsMarket = AskCloseAllForm->IsMarket();
			int BetterTick  =  AskCloseAllForm->BetterTick();
			int QtyPerOrder =  AskCloseAllForm->QtyPerOrder();

			DeleteWorkingCloseAll( Item, BetterTick, IsMarket, QtyPerOrder );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::CloseAllPosItemClick(TObject *Sender)
{
	String CaptionText = L"平倉";

	for( int i = 0; i < FPList->Items->Count -1; i++ )
	{
		TListItem *Item = FPList->Items->Item[ i ];

		if( Item != NULL && Item->Data != NULL && Item->Selected == true )
		{
			String StockName;
			StockName.printf( L"[%s]", Item->Caption );
			CaptionText += StockName;
		}
	}
	AskCloseAllForm->Caption = CaptionText;
	if( AskCloseAllForm->ShowModal() == mrOk )
	{
		bool IsMarket = AskCloseAllForm->IsMarket();
		int  BetterTick  =  AskCloseAllForm->BetterTick();
		int  QtyPerOrder =  AskCloseAllForm->QtyPerOrder();

		for( int i = 0; i < FPList->Items->Count -1; i++ )
		{
			TListItem *Item = FPList->Items->Item[ i ];

			if( Item != NULL && Item->Data != NULL && Item->Selected == true )
				DeleteWorkingCloseAll( Item, BetterTick, IsMarket,  QtyPerOrder );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::FPListMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	TListItem *Item = FPList->GetItemAt( X, Y );

	if( Item != NULL && Item->Data != NULL)
	{
		s888::CPositionStatisticRecord *curPosStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(Item->Data);
        TPoint ScrPoint( X, Y );
		int    Pos = 0;

		ScrPoint = FPList->ClientToScreen( ScrPoint );
		Item->Selected = true;
		curPosStatisticRecPtr->LockForRead();
		Pos = curPosStatisticRecPtr->GetBuyQuantity( s888::rwNotLock ) - curPosStatisticRecPtr->GetSellQuantity( s888::rwNotLock );
		curPosStatisticRecPtr->UnlockForRead();
		if( Shift.Contains( ssRight ) ) ///< Popup menu
		{
			if( Pos == 0 )
				CloseAllItem->Visible = false;
			else
				CloseAllItem->Visible = true;
			PopupMenu->Popup( ScrPoint.x, ScrPoint.y );
		}
		else if( Shift.Contains( ssLeft ) ) ///< Click close all
		{
			if( X < 25 && curPosStatisticRecPtr != NULL )
			{
				curPosStatisticRecPtr->LockForRead();
				String exchangeCode = curPosStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
				String symbol = curPosStatisticRecPtr->GetSymbol(s888::rwNotLock);
				curPosStatisticRecPtr->UnlockForRead();

				ContractViewerForm->OpenOrderBookForm( exchangeCode, symbol, true );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::SaveToClipboard( void )
{
	Graphics::TBitmap* ScreenBmpPtr = new Graphics::TBitmap();
	TClipboard* cb = Clipboard();
	unsigned int DataHandle;
	unsigned short MyFormat;
	HPALETTE APalette;
	Types::TRect SourceRect( 0,0, Width,Height);
	Types::TRect DestRect( 0,0, Width,Height);

	ScreenBmpPtr->Width  = Width;
	ScreenBmpPtr->Height = Height;
	ScreenBmpPtr->Canvas->CopyRect( DestRect, Canvas, SourceRect);
	ScreenBmpPtr->SaveToClipboardFormat( MyFormat, DataHandle, APalette );
	cb->SetAsHandle( MyFormat, DataHandle);
	delete cb;
	delete ScreenBmpPtr;
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::HoldPosition( void )
{
	OIList->Items->Clear();
	String logonID = gOrderStore->ID;
	s888::CTradingAccount *accountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(logonID, s888::rwLockForRead);
	if (accountPtr != 0)
	{
		String orderID, exchangeCode, symbol, tradeDate, itemStr;
		nsOrderMessageDefine::SideEnum side;
		double matchPrice;
		int    holdQuantity;
		s888::CHoldPositionData *holdPosPtr;
		BasicInformation* SymInfo;

		accountPtr->LockForRead();
		for (int oiIndex = 0; oiIndex < accountPtr->GetTaifexHoldPositionCount(s888::rwNotLock); oiIndex++)
		{
			if( (holdPosPtr = accountPtr->GetTaifexHoldPositionFromArray(oiIndex, s888::rwNotLock)) != NULL )
			{
				holdPosPtr->LockForRead();
				orderID      = holdPosPtr->GetOrderID(s888::rwNotLock);
				exchangeCode = holdPosPtr->GetExchangeCode(s888::rwNotLock);
				symbol       = holdPosPtr->GetSymbol(s888::rwNotLock);
				tradeDate    = holdPosPtr->GetTradeDate(s888::rwNotLock);
				side         = holdPosPtr->GetSide(s888::rwNotLock);
				matchPrice   = holdPosPtr->GetMatchPrice(s888::rwNotLock);
				holdQuantity = holdPosPtr->GetHoldQuantity(s888::rwNotLock);
				holdPosPtr->UnlockForRead();

				if( (SymInfo  = gMarketDataStore->GetBasicInformation( exchangeCode, symbol, false )) != NULL )
				{
					TListItem *newItemPtr = OIList->Items->Add();
					newItemPtr->Caption = orderID;
					newItemPtr->SubItems->Add( tradeDate );
					if (side == nsOrderMessageDefine::sBuy)
						itemStr = L"買";
					else if (side == nsOrderMessageDefine::sSell)
						itemStr = L"賣";
					else
						itemStr = L"N/A";
					newItemPtr->SubItems->Add(itemStr);
					newItemPtr->SubItems->Add( SymInfo->GetDisplayName() );
					newItemPtr->SubItems->Add( SymInfo->GetMaturityDate().c_str() );
					itemStr.printf(L"%.*lf", SymInfo->GetDecimalLocator(), SymInfo->GetStrikePrice());
					newItemPtr->SubItems->Add(itemStr);
					switch( SymInfo->GetCallPut() )
					{
						case cpCall:newItemPtr->SubItems->Add(L"C");break;
						case cpPut:	newItemPtr->SubItems->Add(L"P");break;
						default: 	newItemPtr->SubItems->Add(L"-");break;
					}
					itemStr.printf(L"%.*lf", SymInfo->GetDecimalLocator(), matchPrice);
					newItemPtr->SubItems->Add(itemStr);
					itemStr.printf(L"%6d", holdQuantity);
					newItemPtr->SubItems->Add(itemStr);
				}
			}
		}

		for (int oiIndex = 0; oiIndex < accountPtr->GetTWSEHoldPositionCount(s888::rwNotLock); oiIndex++)
		{
			if( (holdPosPtr = accountPtr->GetTWSEHoldPositionFromArray(oiIndex, s888::rwNotLock)) != NULL )
			{
				holdPosPtr->LockForRead();
				orderID      = holdPosPtr->GetOrderID(s888::rwNotLock);
				exchangeCode = holdPosPtr->GetExchangeCode(s888::rwNotLock);
				symbol       = holdPosPtr->GetSymbol(s888::rwNotLock);
				tradeDate    = holdPosPtr->GetTradeDate(s888::rwNotLock);
				side         = holdPosPtr->GetSide(s888::rwNotLock);
				matchPrice   = holdPosPtr->GetMatchPrice(s888::rwNotLock);
				holdQuantity = holdPosPtr->GetHoldQuantity(s888::rwNotLock);
				holdPosPtr->UnlockForRead();

				if( (SymInfo  = gMarketDataStore->GetBasicInformation( exchangeCode, symbol, false )) != NULL )
				{
					TListItem *newItemPtr = OIList->Items->Add();
					newItemPtr->Caption = orderID;
					newItemPtr->SubItems->Add( tradeDate );
					if (side == nsOrderMessageDefine::sBuy)
						itemStr = L"買";
					else if (side == nsOrderMessageDefine::sSell)
						itemStr = L"賣";
					else
						itemStr = L"N/A";
					newItemPtr->SubItems->Add(itemStr);
					newItemPtr->SubItems->Add( SymInfo->GetDisplayName());
					newItemPtr->SubItems->Add( L"---" );
					newItemPtr->SubItems->Add(L"---");
					newItemPtr->SubItems->Add(L"-");
					itemStr.printf(L"%.*lf", SymInfo->GetDecimalLocator(), matchPrice);
					newItemPtr->SubItems->Add(itemStr);
					itemStr.printf(L"%6d", holdQuantity);
					newItemPtr->SubItems->Add(itemStr);
				}
			}
		}
		accountPtr->UnlockForRead();
	}

	for (int i = FPList->Items->Count - 2; i >= 0; i--)
		FPList->Items->Delete(i);
	RefreshSumItem();

	for (int i = 0; i < gOrderStore->GetPosStatisticRecArrayCount(); i++)
	{
		s888::CPositionStatisticRecord *curPosStatisticRecPtr = gOrderStore->GetPosStatisticRecFromArray(i);
		curPosStatisticRecPtr->LockForRead();
		String exchangeCode = curPosStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
		String symbol = curPosStatisticRecPtr->GetSymbol(s888::rwNotLock);
		curPosStatisticRecPtr->UnlockForRead();
		UpdateFloatingProfit( exchangeCode, symbol, curPosStatisticRecPtr );
	}
}
//---------------------------------------------------------------------------
void __fastcall THoldOpenInterestForm::PopupMenuPopup(TObject *Sender)
{
	TListItem *Item = FPList->Selected;

	if( Item != NULL )
		CloseAllItem->Caption = L"[" + Item->Caption + L"] 平倉";
}
//---------------------------------------------------------------------------

void __fastcall THoldOpenInterestForm::ShowDepthMenuItemClick(TObject *Sender)
{
	for( int i = 0; i < FPList->Items->Count -1; i++ )
	{
		TListItem *Item = FPList->Items->Item[ i ];

		if( Item != NULL && Item->Data != NULL && Item->Selected == true )
		{
			String exchangeCode, symbol;
			s888::CPositionStatisticRecord *curPosStatisticRecPtr = (s888::CPositionStatisticRecord*)Item->Data;
			if( curPosStatisticRecPtr != NULL )
			{
				curPosStatisticRecPtr->LockForRead();
				exchangeCode = curPosStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
				symbol = curPosStatisticRecPtr->GetSymbol(s888::rwNotLock);
				curPosStatisticRecPtr->UnlockForRead();

				ContractViewerForm->OpenOrderBookForm( exchangeCode, symbol, true );
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall THoldOpenInterestForm::FormResize(TObject *Sender)
{
	if( Width > 1085 )
		WarningLabel->Visible = true;
	else
		WarningLabel->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall THoldOpenInterestForm::FormConstrainedResize(TObject *Sender, int &MinWidth,
		  int &MinHeight, int &MaxWidth, int &MaxHeight)
{
	MinWidth = 520;
	MinHeight = SettingPanel->Height + RoundFormEx->CaptionHeight + RoundFormEx->BottomHight;
}
//---------------------------------------------------------------------------

