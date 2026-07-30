//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <string>
#include <TypInfo.hpp>
#include "SysUtils.hpp"
#include "OrderMessageDefine.h"
#include "OrderListView.h"
#include "TradingObjects.h"
#include "TradingCommodities.h"
#include "MDComponentStrings.hpp"
#pragma package(smart_init)
//---------------------------------------------------------------------------
const int NumberOfOrderListViewColumn = 36;                ///        0       1       2       3           4       5       6      7         8        9        10      11         12       13       14      15         16    17     18     19      20      21        22         23         24      25      26      27      28      29      30          31        32        33     34     35
const int OrderListViewColumnTag[NumberOfOrderListViewColumn] =     { 0,      1,      6,      16,         17,     18,     19,    20,       21,      23,      25,     27,        22,      24,      26,     28,        29,   30,    31,    15,     35,     32,       33,        34,        11,     12,     13,     14,     8,      9,      10,         3,        37,       4,     38,    39 };
String OrderListViewColumnString[NumberOfOrderListViewColumn];/// = { "全刪", "時間", "單號", "商品名稱", "代碼", "年月", "C/P", "履約價", "代碼1", "年月1", "C/P1", "履約價1", "代碼2", "年月2", "C/P2", "履約價2", "BS", "BS1", "BS2", "價格", "均價", "成交價", "成交價1", "成交價2", "狀態", "委託", "成交", "剩餘", "倉別", "單別", "委託條件", "錯誤碼", "策略名", "訊息","觸價","種類" };
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
int __stdcall OrderListViewSortLocalMsgTime(long lParam1, long lParam2, long ParamSort)
{
	if ((ParamSort != 0) && (ParamSort != 1)) return 0;

	TListItem* Item1 = reinterpret_cast<TListItem *>(lParam1);
	TListItem* Item2 = reinterpret_cast<TListItem *>(lParam2);
	if ((Item1 == NULL) || (Item2 == NULL)) return 0;
	if ((Item1->Data == 0) || (Item2->Data == 0)) return 0;

	TExecution* OrderRootPtr1 = (TExecution*)Item1->Data;
	TExecution* OrderRootPtr2 = (TExecution*)Item2->Data;
	UnicodeString Key1, Key2;
	Key1.printf(L"%s%s", OrderRootPtr1->GetLocalMessageTime().c_str(), OrderRootPtr1->GetOrderID().c_str());
	Key2.printf(L"%s%s", OrderRootPtr2->GetLocalMessageTime().c_str(), OrderRootPtr2->GetOrderID().c_str());
	if (Key1 < Key2)
	{
		if (ParamSort == 0)
			return -1;
		else
			return 1;
	}
	else if (Key1 > Key2)
	{
		if (ParamSort == 0)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}  //OrderListViewSortLocalMsgTime()
//---------------------------------------------------------------------------
int __stdcall OrderListViewSortOrderID(long lParam1, long lParam2, long ParamSort)
{
	if ((ParamSort != 0) && (ParamSort != 1)) return 0;

	TListItem* Item1 = reinterpret_cast<TListItem *>(lParam1);
	TListItem* Item2 = reinterpret_cast<TListItem *>(lParam2);
	if ((Item1 == NULL) || (Item2 == NULL)) return 0;
	if ((Item1->Data == 0) || (Item2->Data == 0)) return 0;

	TExecution* OrderRootPtr1 = (TExecution*)Item1->Data;
	TExecution* OrderRootPtr2 = (TExecution*)Item2->Data;
	UnicodeString Key1, Key2;
	Key1.printf(L"%s%s", OrderRootPtr1->GetOrderID().c_str(), OrderRootPtr1->GetLocalMessageTime().c_str());
	Key2.printf(L"%s%s", OrderRootPtr2->GetOrderID().c_str(), OrderRootPtr2->GetLocalMessageTime().c_str());
	if (Key1 < Key2)
	{
		if (ParamSort == 0)
			return -1;
		else
			return 1;
	}
	else if (Key1 > Key2)
	{
		if (ParamSort == 0)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}  //OrderListViewSortOrderID()
//---------------------------------------------------------------------------
int __stdcall OrderListViewSortProduct(long lParam1, long lParam2, long ParamSort)
{
	if ((ParamSort != 0) && (ParamSort != 1)) return 0;

	TListItem* Item1 = reinterpret_cast<TListItem *>(lParam1);
	TListItem* Item2 = reinterpret_cast<TListItem *>(lParam2);
	if ((Item1 == NULL) || (Item2 == NULL)) return 0;
	if ((Item1->Data == 0) || (Item2->Data == 0)) return 0;

	TExecution* OrderRootPtr1 = (TExecution*)Item1->Data;
	TExecution* OrderRootPtr2 = (TExecution*)Item2->Data;
	UnicodeString Key1, Key2;
	Key1.printf(L"%s%s", OrderRootPtr1->GetSymbol()->GetSymbol(0).c_str(), OrderRootPtr1->GetOrderID().c_str());
	Key2.printf(L"%s%s", OrderRootPtr2->GetSymbol()->GetSymbol(0).c_str(), OrderRootPtr2->GetOrderID().c_str());
	if (Key1 < Key2)
	{
		if (ParamSort == 0)
			return -1;
		else
			return 1;
	}
	else if (Key1 > Key2)
	{
		if (ParamSort == 0)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}  //OrderListViewSortLocalMsgTime()
//---------------------------------------------------------------------------
int __stdcall OrderListViewSortSide(long lParam1, long lParam2, long ParamSort)
{
	if ((ParamSort != 0) && (ParamSort != 1)) return 0;

	TListItem* Item1 = reinterpret_cast<TListItem *>(lParam1);
	TListItem* Item2 = reinterpret_cast<TListItem *>(lParam2);
	if ((Item1 == NULL) || (Item2 == NULL)) return 0;
	if ((Item1->Data == 0) || (Item2->Data == 0)) return 0;

	TExecution* OrderRootPtr1 = (TExecution*)Item1->Data;
	TExecution* OrderRootPtr2 = (TExecution*)Item2->Data;
	UnicodeString Key1, Key2;
	Key1.printf(L"%02d%s", OrderRootPtr1->GetSide(), OrderRootPtr1->GetOrderID().c_str());
	Key2.printf(L"%02d%s", OrderRootPtr2->GetSide(), OrderRootPtr2->GetOrderID().c_str());
	if (Key1 < Key2)
	{
		if (ParamSort == 0)
			return -1;
		else
			return 1;
	}
	else if (Key1 > Key2)
	{
		if (ParamSort == 0)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}  //OrderListViewSortSide()
//---------------------------------------------------------------------------
int __stdcall OrderListViewSortPrice(long lParam1, long lParam2, long ParamSort)
{
	if ((ParamSort != 0) && (ParamSort != 1)) return 0;

	TListItem* Item1 = reinterpret_cast<TListItem *>(lParam1);
	TListItem* Item2 = reinterpret_cast<TListItem *>(lParam2);
	if ((Item1 == NULL) || (Item2 == NULL)) return 0;
	if ((Item1->Data == 0) || (Item2->Data == 0)) return 0;

	TExecution* OrderRootPtr1 = (TExecution*)Item1->Data;
	TExecution* OrderRootPtr2 = (TExecution*)Item2->Data;
	UnicodeString Key1, Key2;
	Key1.printf(L"%015.5lf%s", OrderRootPtr1->GetPrice(), OrderRootPtr1->GetOrderID().c_str());
	Key2.printf(L"%015.5lf%s", OrderRootPtr2->GetPrice(), OrderRootPtr2->GetOrderID().c_str());
	if (Key1 < Key2)
	{
		if (ParamSort == 0)
			return -1;
		else
			return 1;
	}
	else if (Key1 > Key2)
	{
		if (ParamSort == 0)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}  //OrderListViewSortPrice()
//---------------------------------------------------------------------------
int __stdcall OrderListViewSortLastPrice(long lParam1, long lParam2, long ParamSort)
{
	if ((ParamSort != 0) && (ParamSort != 1)) return 0;

	TListItem* Item1 = reinterpret_cast<TListItem *>(lParam1);
	TListItem* Item2 = reinterpret_cast<TListItem *>(lParam2);
	if ((Item1 == NULL) || (Item2 == NULL)) return 0;
	if ((Item1->Data == 0) || (Item2->Data == 0)) return 0;

	TExecution* OrderRootPtr1 = (TExecution*)Item1->Data;
	TExecution* OrderRootPtr2 = (TExecution*)Item2->Data;
	UnicodeString Key1, Key2;
	Key1.printf(L"%015.5lf%s", OrderRootPtr1->GetLastPx(), OrderRootPtr1->GetOrderID().c_str());
	Key2.printf(L"%015.5lf%s", OrderRootPtr2->GetLastPx(), OrderRootPtr2->GetOrderID().c_str());
	if (Key1 < Key2)
	{
		if (ParamSort == 0)
			return -1;
		else
			return 1;
	}
	else if (Key1 > Key2)
	{
		if (ParamSort == 0)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}  //OrderListViewSortLastPrice()
//---------------------------------------------------------------------------
int __stdcall OrderListViewSortOrderStatus(long lParam1, long lParam2, long ParamSort)
{
	if ((ParamSort != 0) && (ParamSort != 1)) return 0;

	TListItem* Item1 = reinterpret_cast<TListItem *>(lParam1);
	TListItem* Item2 = reinterpret_cast<TListItem *>(lParam2);
	if ((Item1 == NULL) || (Item2 == NULL)) return 0;
	if ((Item1->Data == 0) || (Item2->Data == 0)) return 0;

	TExecution* OrderRootPtr1 = (TExecution*)Item1->Data;
	TExecution* OrderRootPtr2 = (TExecution*)Item2->Data;
	UnicodeString Key1, Key2;
	Key1.printf(L"%04d%s", OrderRootPtr1->GetOrderStatus(), OrderRootPtr1->GetOrderID().c_str());
	Key2.printf(L"%04d%s", OrderRootPtr2->GetOrderStatus(), OrderRootPtr2->GetOrderID().c_str());
	if (Key1 < Key2)
	{
		if (ParamSort == 0)
			return -1;
		else
			return 1;
	}
	else if (Key1 > Key2)
	{
		if (ParamSort == 0)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}  //OrderListViewSortOrderStatus()
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TOrderListView *)
{
	new TOrderListView(NULL);
}
//----------------------------- TOrderListView ------------------------------
//---------------------------------------------------------------------------
__fastcall TOrderListView::TOrderListView(TComponent* Owner)
:TCustomListView(Owner)
,FPendingBKColor( clWhite )
,FPendingColor( clBlack )
,FBuyNewBKColor( clWhite )
,FBuyNewColor( clBlack )
,FSellNewBKColor( clWhite )
,FSellNewColor( clBlack )
,FBuyPartialFillBKColor( clWhite )
,FBuyPartialFillColor( clBlack )
,FSellPartialFillBKColor( clWhite )
,FSellPartialFillColor( clBlack )
,FBuyFillBKColor( clWhite )
,FBuyFillColor( clBlack )
,FSellFillBKColor( clWhite )
,FSellFillColor( clBlack )
,FCanceledBKColor( clWhite )
,FCanceledColor( clBlack )
,FRejectBKColor( clWhite )
,FRejectColor( clBlack )
{
	InitString( );
	Checkboxes              = false;
	ViewStyle               = vsReport;
	DoubleBuffered          = true;
	RowSelect               = true;
	FullDrag                = true;
	OwnerDraw               = true;
	MultiSelect             = false;//true;
	ReadOnly                = true;
	HoverTime               = -1;
	HotTrack                = false;
	AllocBy                 = 4096;
	FSortColumnTag          = 1;
	FOrderingCriteria       = 1;
	///< Bind event handler.
	OnAdvancedCustomDraw     = MDBSAdvancedCustomDraw;
	OnAdvancedCustomDrawItem = MDBSAdvancedCustomDrawItem;
	OnMouseDown              = MDBSMouseDown;
	OnColumnClick            = MDBSColumnClick;
	OnKeyDown                = MDBSKeyDown;
	///< Create bitmaps
	FCancelBMP         = new Graphics::TBitmap();
	FExpandBMP         = new Graphics::TBitmap();
	FCollapseBMP       = new Graphics::TBitmap();
	FLightCancelBMP    = new Graphics::TBitmap();
	FColumn0BMP        = new Graphics::TBitmap();
	FMessageLinkBMP    = new Graphics::TBitmap();
	FMessageLinkEndBMP = new Graphics::TBitmap();
	FTextSizeBMP       = new Graphics::TBitmap();
	///< Create image list for Cancel/Collapse/Expand icons.
	SmallImages        = new TImageList(this);
	///< Initialize Column Popup Menu.
	InitPopupMenu( );
	///< Initialize Columns.
	InitialColumn();
	///< Initialize repaint timer.
	FMessageTimer = new TTimer(this);
	FMessageTimer->Enabled  = false;
	FMessageTimer->Interval = 33; ///< Upto 30 FPS.
	FMessageTimer->OnTimer  = OnMessageTimer;
}
//---------------------------------------------------------------------------
__fastcall  TOrderListView::~TOrderListView()
{
	delete FCancelBMP;
	delete FExpandBMP;
	delete FCollapseBMP;
	delete FLightCancelBMP;
	delete FColumn0BMP;
	delete FMessageLinkBMP;
	delete FMessageLinkEndBMP;
	delete FTextSizeBMP;
	delete SmallImages;
	delete PopupMenu;

	if (FMessageTimer != 0)
	{
		FMessageTimer->Enabled = false;
		delete FMessageTimer;
	}
}  //TOrderListView::~TOrderListView()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::InitPopupMenu( void )
{
	PopupMenu = new TPopupMenu(this);
	PopupMenu->AutoPopup   = false;
	PopupMenu->AutoHotkeys = maManual;
	for( int i = 1; i < NumberOfOrderListViewColumn; i++)
	{
		TMenuItem* Item = new TMenuItem( PopupMenu );
		PopupMenu->Items->Add( Item );
		Item->Caption = OrderListViewColumnString[ i ];
		Item->Tag     = i;
		Item->Checked = true;
		Item->OnClick = MenuClicked;
	}
}
/// productNo = 4, productName= 3,bs=16,price=21,qty= 26,txDatetime=1,
//---------------------------------------------------------------------------
void __fastcall TOrderListView::InitString( void )
{
  OrderListViewColumnString[ 0] = Mdcomponentstrings_MD_ORDERLIST_CANCELALL;///'全刪';
  OrderListViewColumnString[ 1] = Mdcomponentstrings_MD_ORDERLIST_TIME;///'時間';
  OrderListViewColumnString[ 2] = Mdcomponentstrings_MD_ORDERLIST_ORDERID;///'單號';
  OrderListViewColumnString[ 3] = Mdcomponentstrings_MD_ORDERLIST_NAME;///'商品名稱';
  OrderListViewColumnString[ 4] = Mdcomponentstrings_MD_ORDERLIST_SYMBOL;///'代碼';
  OrderListViewColumnString[ 5] = Mdcomponentstrings_MD_ORDERLIST_DATE;///'年月';
  OrderListViewColumnString[ 6] = Mdcomponentstrings_MD_ORDERLIST_CALL_PUT;///'C/P';
  OrderListViewColumnString[ 7] = Mdcomponentstrings_MD_ORDERLIST_STRIKE_PX;///'履約價';
  OrderListViewColumnString[ 8] = Mdcomponentstrings_MD_ORDERLIST_SYM_LEG1;///'代碼1';
  OrderListViewColumnString[ 9] = Mdcomponentstrings_MD_ORDERLIST_DATE_LEG1;///'年月1';
  OrderListViewColumnString[10] = Mdcomponentstrings_MD_ORDERLIST_CALL_PUT1;///'C/P1';
  OrderListViewColumnString[11] = Mdcomponentstrings_MD_ORDERLIST_STRIKE_PX1;///'履約價1';
  OrderListViewColumnString[12] = Mdcomponentstrings_MD_ORDERLIST_SYM_LEG2;///'代碼2';
  OrderListViewColumnString[13] = Mdcomponentstrings_MD_ORDERLIST_DATE_LEG2;///'年月2';
  OrderListViewColumnString[14] = Mdcomponentstrings_MD_ORDERLIST_CALL_PUT2;///'C/P2';
  OrderListViewColumnString[15] = Mdcomponentstrings_MD_ORDERLIST_STRIKE_PX2;///'履約價2';
  OrderListViewColumnString[16] = Mdcomponentstrings_MD_ORDERLIST_SIDE;///'BS';
  OrderListViewColumnString[17] = Mdcomponentstrings_MD_ORDERLIST_SIDE_LEG1;///'BS1';
  OrderListViewColumnString[18] = Mdcomponentstrings_MD_ORDERLIST_SIDE_LEG2;///'BS2';
  OrderListViewColumnString[19] = Mdcomponentstrings_MD_ORDERLIST_PRICE;///'價格';
  OrderListViewColumnString[20] = Mdcomponentstrings_MD_ORDERLIST_AVG_PX;///'均價';
  OrderListViewColumnString[21] = Mdcomponentstrings_MD_ORDERLIST_FILL_PX;///'成交價';
  OrderListViewColumnString[22] = Mdcomponentstrings_MD_ORDERLIST_FILL_PX1;///'成交價1';
  OrderListViewColumnString[23] = Mdcomponentstrings_MD_ORDERLIST_FILL_PX2;///'成交價2';
  OrderListViewColumnString[24] = Mdcomponentstrings_MD_ORDERLIST_STATUS;///'狀態';
  OrderListViewColumnString[25] = Mdcomponentstrings_MD_ORDERLIST_ST_NEW;///'委託';
  OrderListViewColumnString[26] = Mdcomponentstrings_MD_ORDERLIST_ST_FILLED;///'成交';
  OrderListViewColumnString[27] = Mdcomponentstrings_MD_ORDERLIST_LEAVES_QTY;///'剩餘';
  OrderListViewColumnString[28] = Mdcomponentstrings_MD_ORDERLIST_OPEN_CLOSE;///'倉別';
  OrderListViewColumnString[29] = Mdcomponentstrings_MD_ORDERLIST_ORDER_TYPE;///'單別';
  OrderListViewColumnString[30] = Mdcomponentstrings_MD_ORDERLIST_ORDER_COND;///'委託條件';
  OrderListViewColumnString[31] = Mdcomponentstrings_MD_ORDERLIST_ERROR_CODE;///'錯誤碼';
  OrderListViewColumnString[32] = Mdcomponentstrings_MD_ORDERLIST_STRATEGY_NAME;///'策略名';
  OrderListViewColumnString[33] = Mdcomponentstrings_MD_ORDERLIST_MESSGAE;///'訊息';
  OrderListViewColumnString[34] = Mdcomponentstrings_MD_ORDERLIST_STOP_PX;///'觸價';
  OrderListViewColumnString[35] = L"種類";
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::InitialColumn( void )
{
	TListColumn* NewColumn;
	String       CaptionText;
	int          CaptionWidth;

	Columns->BeginUpdate();
	Columns->Clear();
	FTextSizeBMP->Canvas->Font->Assign( Font );
	SmallImages->Height = FTextSizeBMP->Canvas->TextHeight( L"A" ) + 4;
	for( int i = 0; i < NumberOfOrderListViewColumn; i++ )
	{
		NewColumn = Columns->Add();
		NewColumn->Tag = i;
		if (i == FSortColumnTag)
		{
			if( FOrderingCriteria == 0 )
				CaptionText = OrderListViewColumnString[i] + L"▲";
			else
				CaptionText = OrderListViewColumnString[i] + L"▼";
		}
		else
			CaptionText = OrderListViewColumnString[i];
		NewColumn->Caption = CaptionText;
		if (i == 0)
		{
			NewColumn->Width    = 55;
			NewColumn->MinWidth = 55;
			//NewColumn->MaxWidth = 55;
		}
		else
		{
			CaptionWidth = FTextSizeBMP->Canvas->TextWidth(CaptionText) + 20;
			NewColumn->Width    = CaptionWidth;
			NewColumn->MinWidth = CaptionWidth;
			//NewColumn->MaxWidth = CaptionWidth * 2;
		}
	}
	Columns->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::Loaded( void )
{
	FCancelBMP->TransparentMode = tmAuto;
	FCancelBMP->Transparent = true;
	FExpandBMP->TransparentMode = tmAuto;
	FExpandBMP->Transparent = true;
	FCollapseBMP->TransparentMode = tmAuto;
	FCollapseBMP->Transparent = true;
	FMessageLinkBMP->TransparentMode = tmAuto;
	FMessageLinkBMP->Transparent = true;
	FMessageLinkEndBMP->TransparentMode = tmAuto;
	FMessageLinkEndBMP->Transparent = true;
}  //TOrderListView::Loaded()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::MenuClicked(TObject* Sender)
{
	TMenuItem* ClickedItem = (TMenuItem*)Sender;
	if (ClickedItem->Checked == true)
	{
		ClickedItem->Checked = false;
		for (register int i = 0; i < Columns->Count; i++)
		{
			if (ClickedItem->Tag == Columns->Items[i]->Tag)
			{
				Columns->Delete(i);
				break;
			}
		}  //for (register int i = 0; i < Columns->Count; i++)
		Scroll(0, 0);
	}
	else
	{
		ClickedItem->Checked = true;
		TListColumn* NewCol;

		if (ClickedItem->Tag > Columns->Count)
			NewCol = (TListColumn*)Columns->Add();
		else
			NewCol = (TListColumn*)Columns->Insert(ClickedItem->Tag);
		if (ClickedItem->Tag == FSortColumnTag)
		{
			if (FOrderingCriteria == 0)
				NewCol->Caption = OrderListViewColumnString[ClickedItem->Tag] + L"▲";
			else
				NewCol->Caption = OrderListViewColumnString[ClickedItem->Tag] + L"▼";
		}
		else
			NewCol->Caption = OrderListViewColumnString[ClickedItem->Tag];
		NewCol->Tag = ClickedItem->Tag;
		FitColumnWidth(NewCol->Index);
	}
	TPoint P = PopupMenu->PopupPoint;
	PopupMenu->Popup( P.X, P.Y );
}  //TOrderListView::MenuClicked()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetOrderStore(TOrderStore* OrderStore)
{
	if( FOrderStore != NULL )
		FOrderStore->RemoveOrderMessageListener(this);
	FOrderStore = OrderStore;
	if( FOrderStore != NULL )
		FOrderStore->AddOrderMessageListener(this);
}  //TOrderListView::SetOrderStore()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetFilter(OrderFilterEnum Filter)
{
	if (FFilter != Filter)
	{
		FFilter = Filter;
		RefreshOrderData();
	}
}  //TOrderListView::SetFilter()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetFilterMarket(MarketFilterEnum FilterMarket)
{
	if (FFilterMarket != FilterMarket)
	{
		FFilterMarket = FilterMarket;
		RefreshOrderData();
	}
}  //TOrderListView::SetFilterMarket()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetFilterSide(SideFilterEnum FilterSide)
{
	if (FFilterSide != FilterSide)
	{
		FFilterSide = FilterSide;
		RefreshOrderData();
	}
}  //TOrderListView::SetFilterSide()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetFilterSymbol(UnicodeString FilterSymbol)
{
	if (FFilterSymbol != FilterSymbol)
	{
		FFilterSymbol = FilterSymbol;
		RefreshOrderData();
	}
}  //TOrderListView::SetFilterSymbol()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetFilterPrice(double FilterPrice)
{
	if (FFilterPrice != FilterPrice)
	{
		FFilterPrice = FilterPrice;
		RefreshOrderData();
	}
}  //TOrderListView::SetFilterPrice()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetFilters(MarketFilterEnum FilterMarket, SideFilterEnum FilterSide,
										   const String& FilterSymbol, double FilterPrice)
{
	bool IsFilterChanged = false;

	if (FFilterMarket != FilterMarket)
	{
		FFilterMarket = FilterMarket;
		IsFilterChanged = true;
	}
	if (FFilterSide != FilterSide)
	{
		FFilterSide = FilterSide;
		IsFilterChanged = true;
	}
	if (FFilterSymbol != FilterSymbol)
	{
		FFilterSymbol = FilterSymbol;
		IsFilterChanged = true;
	}
	if (FFilterPrice != FilterPrice)
	{
		FFilterPrice = FilterPrice;
		IsFilterChanged = true;
	}
	if (IsFilterChanged) RefreshOrderData();
}  //TOrderListView::SetFilters()
//---------------------------------------------------------------------------
int __fastcall TOrderListView::CancelFilteredOrder()
{
	int CanceledCount = 0;
	if (FOrderStore == 0) return CanceledCount;
	if (this->Items->Count <= 0) return CanceledCount;

	TExecution* OrderRootPtr;

	for( int i = 0; i < this->Items->Count; i++)
	{
		OrderRootPtr = (TExecution*)Items->Item[i]->Data;
		if( OrderRootPtr != NULL )
		{
			nsOrderMessageDefine::OrderStatusEnum OrderStatus = OrderRootPtr->GetOrderStatus();
			if (OrderRootPtr->IsRoot() &&
				((OrderStatus == nsOrderMessageDefine::osNew) ||
				 (OrderStatus == nsOrderMessageDefine::osReplaced) ||
				 (OrderStatus == nsOrderMessageDefine::osPartiallyFilled)))
			{
				if (this->FFilter != ftStopOrder)
				{
					try
					{
						UFC::AnsiString Strategy( OrderRootPtr->GetStrategyName().c_str() );

						FOrderStore->CancelOrder( OrderRootPtr->GetMarket(),
												  OrderRootPtr->GetExchangeCode(),
												  OrderRootPtr->GetOrderID(),
												  Strategy );
						CanceledCount++;
					}
					catch (UFC::Exception x)
					{
						ShowMessage( x.what() );
					}  //try
				}
				else
				{
					FOrderStore->CancelStopOrder(OrderRootPtr->GetExchangeCode(),OrderRootPtr->GetStopSymbol(), OrderRootPtr->GetStopPrice(),
												 OrderRootPtr->GetExchangeCode(),OrderRootPtr->GetSymbol()->GetSymbol(0), OrderRootPtr->GetPrice(), OrderRootPtr->GetSide());
					CanceledCount++;
				}
			}
		}
	}
	return CanceledCount;
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetCancelBMP(Graphics::TBitmap* CancelBMP)
{
	FCancelBMP->Assign(CancelBMP);
	FCancelBMP->Dormant();
	FCancelBMP->FreeImage();
	FCancelBMP->TransparentMode = tmAuto;
	FCancelBMP->Transparent = true;
	FLightCancelBMP->SetSize(20, 20);
	FLightCancelBMP->Canvas->Draw(0, -20, FCancelBMP);
	FLightCancelBMP->Dormant();
}  //TOrderListView::SetCancelBMP()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetExpandBMP(Graphics::TBitmap* ExpandBMP)
{
	FExpandBMP->Assign(ExpandBMP);
	FExpandBMP->Dormant();
	FExpandBMP->FreeImage();
	FExpandBMP->TransparentMode = tmAuto;
	FExpandBMP->Transparent = true;
}  //TOrderListView::SetExpandBMP()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetCollapseBMP(Graphics::TBitmap* CollapseBMP)
{
	FCollapseBMP->Assign(CollapseBMP);
	FCollapseBMP->Dormant();
	FCollapseBMP->FreeImage();
	FCollapseBMP->TransparentMode = tmAuto;
	FCollapseBMP->Transparent = true;
}  //TOrderListView::SetCollapseBMP()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetMessageLinkBMP(Graphics::TBitmap* MessageLinkBMP)
{
	FMessageLinkBMP->Assign(MessageLinkBMP);
	FMessageLinkBMP->Dormant();
	FMessageLinkBMP->FreeImage();
	FMessageLinkBMP->TransparentMode = tmAuto;
	FMessageLinkBMP->Transparent = true;
}  //TOrderListView::SetMessageLinkBMP()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetMessageLinkEndBMP(Graphics::TBitmap* MessageLinkEndBMP)
{
	FMessageLinkEndBMP->Assign(MessageLinkEndBMP);
	FMessageLinkEndBMP->Dormant();
	FMessageLinkEndBMP->FreeImage();
	FMessageLinkEndBMP->TransparentMode = tmAuto;
	FMessageLinkEndBMP->Transparent = true;
}  //TOrderListView::SetMessageLinkEndBMP()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::RefreshSubItem(TListItem* ItemPtr, TExecution* OrderRootPtr)
{
	if( ItemPtr->SubItems->Count > 0 )
		ItemPtr->SubItems->Clear();
	for( register int i = 1; i < NumberOfOrderListViewColumn; i++)
		ItemPtr->SubItems->Add( OrderRootPtr->GetStringField( OrderListViewColumnTag[i] ));
}  //TOrderListView::RefreshSubItem()
//---------------------------------------------------------------------------
//  Add a new execution
//---------------------------------------------------------------------------
void __fastcall TOrderListView::AddNewItem(TExecution* OrderRootPtr)
{
	TListItem* ItemPtr;

	Items->BeginUpdate();
	if( /*!FIsProgressing &&*/ (FOrderingCriteria == 1))
		ItemPtr = Items->Insert( 0 );
	else
		ItemPtr = Items->Add();
	ItemPtr->Data    = OrderRootPtr;
	ItemPtr->Caption = String( OrderRootPtr->GetNID() );
	RefreshSubItem( ItemPtr, OrderRootPtr);
	Items->EndUpdate();
//	if( (FIsProgressing && FIsProgressingLastItem) || !FIsProgressing)
	ItemPtr->MakeVisible( false ); ///< MakeVisible ensures that the entire list item is visible in the list view
}  //TOrderListView::AddNewItem()
//---------------------------------------------------------------------------
// Update an execution record.
//---------------------------------------------------------------------------
void __fastcall TOrderListView::RefreshItem(TListItem* ItemPtr, TExecution* OrderRootPtr)
{
	Items->BeginUpdate();
	ItemPtr->Caption = String( OrderRootPtr->GetNID() );
	ItemPtr->Data    = OrderRootPtr;
	RefreshSubItem( ItemPtr, OrderRootPtr );
	Items->EndUpdate();
} //TOrderListView::RefreshItem()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor BarColor )
{
	int R = GetRValue( (DWORD)BarColor );
	int G = GetGValue( (DWORD)BarColor );
	int B = GetBValue( (DWORD)BarColor );
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;

	vert [1] .x      = dRect.Right;
	vert [1] .y      = dRect.Bottom;
	vert [1] .Red    = R<<8;
	vert [1] .Green  = G<<8;
	vert [1] .Blue   = B<<8;
	vert [1] .Alpha  = 0x0000;

/*	R+=64;
	if( R > 255 ) R = 255;
	G+=64;
	if( G > 255 ) G = 255;
	B+=64;
	if( B > 255 ) B = 255;*/

	vert [0] .x      = dRect.Left;
	vert [0] .y      = dRect.Top;
	vert [0] .Red    = R<<8;
	vert [0] .Green  = G<<8;
	vert [0] .Blue   = B<<8;
	vert [0] .Alpha  = 0x0000;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill( canvas->Handle, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetCanvasColor( TCanvas* canvas, bool IsSelected, TExecution* Execution )
{
	nsOrderMessageDefine::OrderStatusEnum ExecutionStatus = Execution->GetOrderStatus();
	nsOrderMessageDefine::SideEnum        ExecutionSide   = Execution->GetSide();

	if( IsSelected == false )
	{
		switch( ExecutionStatus )
		{
			case nsOrderMessageDefine::osPendingNew:
				canvas->Brush->Color = FPendingBKColor;
				canvas->Font->Color  = FPendingColor;
				break;
			case nsOrderMessageDefine::osPendingReplace:
				canvas->Brush->Color = FPendingBKColor;
				canvas->Font->Color  = FPendingColor;
				break;
			case nsOrderMessageDefine::osPendingCancel:
				canvas->Brush->Color = FPendingBKColor;
				canvas->Font->Color  = FPendingColor;
				break;
			case nsOrderMessageDefine::osNew:
				if( ExecutionSide == nsOrderMessageDefine::sBuy)
				{
					canvas->Brush->Color = FBuyNewBKColor;
					canvas->Font->Color  = FBuyNewColor;
				}
				else
				{
					canvas->Brush->Color = FSellNewBKColor;
					canvas->Font->Color  = FSellNewColor;
				}
				break;
			case nsOrderMessageDefine::osPartiallyFilled:
				if( ExecutionSide == nsOrderMessageDefine::sBuy)
				{
					canvas->Brush->Color = FBuyPartialFillBKColor;
					canvas->Font->Color  = FBuyPartialFillColor;
				}
				else
				{
					canvas->Brush->Color = FSellPartialFillBKColor;
					canvas->Font->Color  = FSellPartialFillColor;
				}
				break;
			case nsOrderMessageDefine::osFilled:
				if (ExecutionSide == nsOrderMessageDefine::sBuy)
				{
					canvas->Brush->Color = FBuyFillBKColor;
					canvas->Font->Color  = FBuyFillColor;
				}
				else
				{
					canvas->Brush->Color = FSellFillBKColor;
					canvas->Font->Color  = FSellFillColor;
				}
				break;
			case nsOrderMessageDefine::osCanceled:
				if(  Execution->IsRoot() &&
					(Execution->GetCumQty() > 0) &&
					(Execution->GetOrderQty() > Execution->GetCumQty()))
				{
					if( ExecutionSide == nsOrderMessageDefine::sBuy)
					{
						canvas->Brush->Color = FBuyPartialFillBKColor;
						canvas->Font->Color  = FBuyPartialFillColor;
					}
					else
					{
						canvas->Brush->Color = FSellPartialFillBKColor;
						canvas->Font->Color  = FSellPartialFillColor;
					}
				}
				else
				{
					canvas->Brush->Color = FCanceledBKColor;
					canvas->Font->Color = FCanceledColor;
				}
				break;
			case nsOrderMessageDefine::osRejected:
				canvas->Brush->Color = FRejectBKColor;
				canvas->Font->Color  = FRejectColor;
				break;
			default:
				canvas->Brush->Color = clWhite;
				canvas->Font->Color  = clBlack;
				break;
		}
	}
	else
	{
		canvas->Brush->Color = clHighlight;
		canvas->Font->Color  = clHighlightText;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::DrawCancelExpandIcon( TExecution* Execution, TRect& ItemRect )
{
	nsOrderMessageDefine::OrderStatusEnum ExecutionStatus = Execution->GetOrderStatus();
	TRect DrawRect;

	if( Execution->IsRoot() )
	{
		///< Draw Cancel Icon.
		if ((ExecutionStatus == nsOrderMessageDefine::osNew) ||
			(ExecutionStatus == nsOrderMessageDefine::osReplaced) ||
			(ExecutionStatus == nsOrderMessageDefine::osPartiallyFilled))
		{
			DrawRect.Left   = 0;
			DrawRect.Top    = 0;
			DrawRect.Right  = ItemRect.Height();
			DrawRect.Bottom = ItemRect.Height();
			FColumn0BMP->Canvas->StretchDraw( DrawRect, FCancelBMP);
		}
		///< Draw expand icon
		if( Execution->GetMessageCount() > 0 )
		{
			DrawRect.Left   = ItemRect.Height();
			DrawRect.Top    = 0;
			DrawRect.Right  = (2 * ItemRect.Height());
			DrawRect.Bottom = ItemRect.Height();
			if( Execution->IsExpanded())
				FColumn0BMP->Canvas->StretchDraw( DrawRect, FCollapseBMP);
			else
				FColumn0BMP->Canvas->StretchDraw( DrawRect, FExpandBMP);
		}
	}
	else
	{
		DrawRect.Left   = ItemRect.Height();
		DrawRect.Top    = 0;
		DrawRect.Right  = (2 * ItemRect.Height());
		DrawRect.Bottom = ItemRect.Height();
		if( Execution->IsLastMessage())
			FColumn0BMP->Canvas->StretchDraw( DrawRect, FMessageLinkEndBMP);
		else
			FColumn0BMP->Canvas->StretchDraw( DrawRect, FMessageLinkBMP);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::GetTextFormat( int SubItemIndex, TTextFormat& Formats )
{
	switch( SubItemIndex )
	{
		case 5:
		case 9:
		case 13:
		case 15:
		case 16:
		case 17:
		case 23:
		case 27:
		case 28:
		case 29:
		case 30: Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis; break;
		case 6:
		case 10:
		case 14:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 24:
		case 25:
		case 26:Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;break;
		default:Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter<<tfEndEllipsis;break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::MDBSAdvancedCustomDrawItem(TCustomListView* Sender, TListItem* Item,
														   TCustomDrawState State, TCustomDrawStage Stage,
														   bool &DefaultDraw)
{
	DefaultDraw = false;
	TExecution *ExecutionPtr = (TExecution*)Item->Data;
	if( ExecutionPtr != NULL )
	{
		TRect        ItemRect = Item->DisplayRect(drBounds);
		TRect        DrawRect;
		String       SubItemText;
		TListColumn* PreColumn;
		TListColumn* CurColumn;
		int          SubItemIndex;

		FColumn0BMP->Canvas->Font->Assign( this->Font );
		if ((FColumn0BMP->Width != ItemRect.Width()) || (FColumn0BMP->Height != ItemRect.Height()))
			FColumn0BMP->SetSize( ItemRect.Width(), ItemRect.Height() );
		SetCanvasColor( FColumn0BMP->Canvas, Item->Selected, ExecutionPtr );
		DrawRect = TRect(0, 0, FColumn0BMP->Width, FColumn0BMP->Height);
		//Fill Gradient Background
		DrawGradientBar( FColumn0BMP->Canvas, DrawRect, FColumn0BMP->Canvas->Brush->Color );
		//Draw Cancel Icon and Expand Icon
		DrawCancelExpandIcon(  ExecutionPtr, ItemRect );
		//Draw SubItems
		DrawRect.Left   = 0;
		DrawRect.Right  = ItemRect.Width() - 1;
		DrawRect.Top    = 0;
		DrawRect.Bottom = ItemRect.Height() - 1;
		FColumn0BMP->Canvas->Brush->Style = bsClear;
		for( register int ColumnIndex = 1; ColumnIndex < Columns->Count; ColumnIndex++)
		{
			TTextFormat Formats;

			PreColumn = Columns->Items[ColumnIndex - 1];
			CurColumn = Columns->Items[ColumnIndex];
			SubItemIndex = CurColumn->Tag - 1;
			if ((SubItemIndex < 0) || ( SubItemIndex >= Item->SubItems->Count)) continue;
			SubItemText    = Item->SubItems->Strings[ SubItemIndex ];
			DrawRect.left  = DrawRect.left + PreColumn->Width;
			DrawRect.Right = DrawRect.Left + CurColumn->Width;
			GetTextFormat( SubItemIndex, Formats );
			FColumn0BMP->Canvas->TextRect( DrawRect, SubItemText, Formats );
		}
		///< Past Bitmap
		Canvas->Draw(ItemRect.Left, ItemRect.Top, FColumn0BMP);
	}
	else
		DefaultDraw = true;
}  //TOrderListView::MDBSAdvancedCustomDrawItemEvent()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::MDBSAdvancedCustomDraw(TCustomListView *Sender,
													   const TRect &ARect,
													   TCustomDrawStage Stage,
													   bool &DefaultDraw)
{   //保留第一行作為刪單、展開使用，預防使用者將資料欄位移到第一行之前
	TListColumn *firstColumn = Columns->Items[0];
	int firstColumnTag = firstColumn->Tag;
	if (firstColumnTag != 0)
	{   //尋找原第一行被移到哪裡
		for (int columnIndex = 1; columnIndex < Columns->Count; columnIndex++)
		{
			TListColumn *curColumn = Columns->Items[columnIndex];
			int curColumnTag = curColumn->Tag;
			if (curColumnTag == 0)  //第一行的tag為0
			{   //將兩行交換
				String curCaption = curColumn->Caption;
				int curColumnMinWidth = curColumn->MinWidth;
				//int curColumnMaxWidth = curColumn->MaxWidth;
				int curColumnWidth = curColumn->Width;
				curColumn->Caption = firstColumn->Caption;
				curColumn->Tag = firstColumnTag;
				curColumn->MinWidth = firstColumn->MinWidth;
				//curColumn->MaxWidth = firstColumn->MaxWidth;
				curColumn->Width = firstColumn->Width;
				firstColumn->Caption = curCaption;
				firstColumn->Tag = curColumnTag;
				firstColumn->MinWidth = curColumnMinWidth;
				//firstColumn->MaxWidth = curColumnMaxWidth;
				firstColumn->Width = curColumnWidth;
				break;
			}  //if (curColumnTag == 0)
		}  //for (int columnIndex = 1; columnIndex < Columns->Count; columnIndex++)
	}  //if (firstColumnTag != 0)

	if( Stage == cdPreErase )
	{
		DefaultDraw = false;
		return;
	}
	if( Stage == cdPrePaint )
	{
		TRect PaintRect = ARect;

		if( Items->Count > 0 )
		{
			TListItem* LastItemPtr = (*(this->Items))[this->Items->Count - 1];
			TRect      ItemRect    = LastItemPtr->DisplayRect(drBounds);
			TRect      RightRect   = ARect;

			if (RightRect.Right > ItemRect.Right)
			{
				RightRect.Left = ItemRect.Right;
				Canvas->Brush->Color = Color;//clWindow;
				Canvas->FillRect( RightRect );
			}
			if (ItemRect.Bottom < PaintRect.Bottom)
			{
				PaintRect.Top = ItemRect.Bottom;
				Canvas->Brush->Color = Color;//clWindow;
				Canvas->FillRect( PaintRect );
			}
		}
		else
		{
			Canvas->Brush->Color = Color;//clWindow;
			Canvas->FillRect( PaintRect );
		}
		DefaultDraw = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::ResetSortColumnCaption( void )
{
	for( register int i = 0; i < Columns->Count; i++)
	{
		if ((Columns->Items[i]->Tag == 1) || (Columns->Items[i]->Tag == 2) ||
			(Columns->Items[i]->Tag == 3) || (Columns->Items[i]->Tag == 16) ||
			(Columns->Items[i]->Tag == 19) || (Columns->Items[i]->Tag == 21) ||
			(Columns->Items[i]->Tag == 24))
			Columns->Items[i]->Caption = OrderListViewColumnString[Columns->Items[i]->Tag];
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SortByColumn( TListColumn *Column, TSortProc SortProc )
{
	ResetSortColumnCaption();
	if (FOrderingCriteria == 0)
		FOrderingCriteria = 1;
	else
		FOrderingCriteria = 0;
	if (FOrderingCriteria == 0)
		Column->Caption = OrderListViewColumnString[Column->Tag] + Mdcomponentstrings_MD_ORDERLIST_CONFIRM_UP_ARROW;
	else
		Column->Caption = OrderListViewColumnString[Column->Tag] + Mdcomponentstrings_MD_ORDERLIST_CONFIRM_DOWN_ARROW;
	FSortColumnTag = Column->Tag ;
	CollapseAllListItem();
	CustomSort( SortProc, FOrderingCriteria);
	FitColumnWidth( Column->Index );
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::MDBSColumnClick(TObject *Sender, TListColumn *Column)
{
	if (FOrderStore != NULL )  //△▽ ▲▼
	{
		String CaptionText;
		String ConfirmText = Mdcomponentstrings_MD_ORDERLIST_CONFIRM_DELETE;

		switch( Column->Tag )
		{
			case  0: ///< Cancel all
				if( FFilter == ftStopOrder )
				{
					CaptionText = Mdcomponentstrings_MD_ORDERLIST_CONFIRM_DELETE_ALL_STOP_ORDER;
					if( Application->MessageBox( ConfirmText.c_str(), CaptionText.c_str(), MB_YESNO) == IDYES)
						FOrderStore->CancelAllStopOrder();
				}
				else
				{
					CaptionText = Mdcomponentstrings_MD_ORDERLIST_CONFIRM_DELETE_ALL_ACTIVE_ORDER;
					if( Application->MessageBox( ConfirmText.c_str(), CaptionText.c_str(), MB_YESNO) == IDYES)
					{
						try
						{
							FOrderStore->CancelAllOrder();
						}
						catch (UFC::Exception x)
						{
							ShowMessage(x.what());
						}  //try
					}
				}
				break;
			case  1: SortByColumn( Column, OrderListViewSortLocalMsgTime );break;///< Sort by 時間
			case  2: SortByColumn( Column, OrderListViewSortOrderID );break;///< Sort by 單號
			case  3: SortByColumn( Column, OrderListViewSortProduct );break;///< Sort by 合約
			case 16: SortByColumn( Column, OrderListViewSortSide );break;///< Sort by 買賣
			case 19: SortByColumn( Column, OrderListViewSortPrice);break;///< Sort by 價格
			case 21: SortByColumn( Column, OrderListViewSortLastPrice );break;///< Sort by 成交價
			case 24: SortByColumn( Column, OrderListViewSortOrderStatus );break;///< Sort by 狀態
			default: break;
		}
	}
}  //TOrderListView::MDBSColumnClick()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::MDBSKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	TListItem* ItemPtr;
	TExecution* OrderMessagePtr;
	for (int i = 0; i < this->Items->Count; i++)
	{
		ItemPtr = (*(this->Items))[i];
		if (ItemPtr->Selected)
		{
			OrderMessagePtr = (TExecution*)ItemPtr->Data;
			if (OrderMessagePtr->IsRoot() && (FOnOrderKeyDown != 0))
			{
				FOnOrderKeyDown(Sender, Key, Shift,
								OrderMessagePtr->GetNID(), OrderMessagePtr->GetOrderID(), OrderMessagePtr->GetMarket(),
								OrderMessagePtr->GetSymbol()->GetSymbol(0), OrderMessagePtr->GetSide(),
								OrderMessagePtr->GetPrice(), OrderMessagePtr->GetLeavesQty(), OrderMessagePtr->GetOrderStatus());
				break;
			}
		}  //if (ItemPtr->Selected)
	}  //for (int i = (this->Items->Count - 1); i >= 0; i--)
}  //TOrderListView::MDBSKeyDown()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::WndProc(Messages::TMessage &Message)
{
	NMHDR* pnmh;
	POINT Point;
	switch(Message.Msg)
	{
		case WM_NOTIFY:
			pnmh = ((NMHDR*)Message.LParam);
			if (pnmh->code == NM_RCLICK)
			{
				GetCursorPos( &Point );
				PopupMenu->Popup( Point.x, Point.y );
				Message.Result = 1;
			}
			else
				inherited::WndProc( Message );
			break;
		case WM_ERASEBKGND:
			Message.Result = 1;
			return;
		default:
			inherited::WndProc( Message );
			break;
	}  //switch(Message.Msg)
}  //TOrderListView::WndProc()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::ExpandListItem(TListItem *ItemPtr)
{
	TExecution* OrderRootPtr = (TExecution*)ItemPtr->Data;
	if( (OrderRootPtr == 0) || !OrderRootPtr->IsRoot() || OrderRootPtr->IsExpanded() )
		return;
	OrderRootPtr->SetExpanded(true);
	RefreshItem(ItemPtr, OrderRootPtr);

	CStringTExecutionMultiMap* MsgListPtr = OrderRootPtr->GetExecutions();
	if( MsgListPtr->size() > 0 )
	{
		typedef CStringTExecutionMultiMap::iterator MI;
		TExecution* CurMessagePtr;
		TListItem*  NewItemPtr;
		int         InsertPos = ItemPtr->Index + 1;

		Items->BeginUpdate();
		for (MI MsgIt = MsgListPtr->begin(); MsgIt != MsgListPtr->end(); MsgIt++)
		{
			CurMessagePtr       = MsgIt->second;
			NewItemPtr          = Items->Insert( InsertPos );
			InsertPos           = NewItemPtr->Index + 1;
			NewItemPtr->Data    = CurMessagePtr;
			NewItemPtr->Caption = String(CurMessagePtr->GetNID());
			RefreshSubItem( NewItemPtr, CurMessagePtr );
			NewItemPtr->MakeVisible( false );
		}
		Items->EndUpdate();
	}
}  //TOrderListView::ExpandListItem()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::ExpandAllListItem()
{
	if( Items->Count > 0 )
	{
		for( register int i = Items->Count - 1; i >= 0; i--)
			 ExpandListItem( Items->Item[i] );
	}
}  //TOrderListView::ExpandAllListItem()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::CollapseListItem(TListItem *ItemPtr)
{
	TExecution* OrderRootPtr = (TExecution*)ItemPtr->Data;
	if( (OrderRootPtr == 0) || !OrderRootPtr->IsRoot() || !OrderRootPtr->IsExpanded() )
		return;
	OrderRootPtr->SetExpanded(false);
	RefreshItem( ItemPtr, OrderRootPtr);

	TExecution* CurMessagePtr;
	TListItem*  CurItemPtr;
	int CurIndex;
	int BeginIndex = ItemPtr->Index + 1;
	int EndIndex = -1;

	for (CurIndex = BeginIndex; CurIndex < Items->Count; CurIndex++)
	{
		CurItemPtr = (*Items)[CurIndex];
		CurMessagePtr = (TExecution*)CurItemPtr->Data;
		if( !CurMessagePtr->IsRoot())
			EndIndex = CurIndex;
		else
			break;
	}
	Items->BeginUpdate();
	for( int CurIndex = EndIndex; CurIndex >= BeginIndex; CurIndex--)
	{
		CurItemPtr = (*Items)[CurIndex];
		CurItemPtr->Delete();
	}
	Items->EndUpdate();
}  //TOrderListView::Collapse()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::CollapseAllListItem()
{
	if (this->Items->Count <= 0) return;

	TListItem* ItemPtr;
	TExecution* OrderRootPtr;

	Items->BeginUpdate();
	for (int i = (this->Items->Count - 1); i >= 0; i--)
	{
		ItemPtr = (*(this->Items))[i];
		OrderRootPtr = (TExecution*)ItemPtr->Data;

		if (OrderRootPtr->IsRoot())
		{
			if (OrderRootPtr->IsExpanded())
			{
				OrderRootPtr->SetExpanded(false);
				RefreshItem(ItemPtr, OrderRootPtr);
			}
		}
		else
			ItemPtr->Delete();
	}  //for (int i = (this->Items->Count - 1); i >= 0; i--)
	Items->EndUpdate();
}  //TOrderListView::CollapseAllListItem()
//---------------------------------------------------------------------------
int __fastcall TOrderListView::GetColumnCaptionWidth( int ColumnIndex )
{
	int CaptionWidth = 0;

	if( ColumnIndex < 0 || ColumnIndex >= Columns->Count )
		return CaptionWidth;
	CaptionWidth = FTextSizeBMP->Canvas->TextWidth( Columns->Items[ ColumnIndex ]->Caption );
	return CaptionWidth;
}
//---------------------------------------------------------------------------
int __fastcall TOrderListView::GetColumnTextMaxWidth(int ColumnIndex)
{
	if( ColumnIndex < 1 || ColumnIndex >= Columns->Count )
		return GetColumnCaptionWidth( 0 );

	TListItem*  CurItemPtr;
	String      ColumnText;
	int         SubItemIndex = Columns->Items[ ColumnIndex ]->Tag - 1;
	int         ColumnTextWidth;
	int         TextMaxWidth = 0;

	for( register int ItemIndex = 0; ItemIndex < Items->Count; ItemIndex++ )
	{
		CurItemPtr = Items->Item[ ItemIndex ];
		ColumnText = CurItemPtr->SubItems->Strings[ SubItemIndex ];
		ColumnTextWidth = FTextSizeBMP->Canvas->TextWidth( ColumnText );
		if( ColumnTextWidth > TextMaxWidth )
			TextMaxWidth = ColumnTextWidth;
	}
	return TextMaxWidth;
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::FitColumnWidth(int ColumnIndex)
{
	if( ColumnIndex < 1 || ColumnIndex >= Columns->Count )
		return;
	FTextSizeBMP->Canvas->Font->Assign(this->Font);
	TListColumn* CurColumn    = Columns->Items[ ColumnIndex ];
	int          CaptionWidth = GetColumnCaptionWidth(ColumnIndex);
	int          MaxTextWidth = GetColumnTextMaxWidth(ColumnIndex);

	if( CaptionWidth > MaxTextWidth )
		MaxTextWidth = CaptionWidth;
	MaxTextWidth += 20;
	CurColumn->MinWidth = CaptionWidth + 20;
	//CurColumn->MaxWidth = MaxTextWidth * 1.5;
	CurColumn->Width    = MaxTextWidth;
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::FitAllColumnWidth()
{
	TListColumn* CurColumn;
	TListItem*   CurItemPtr;
	String       tempStr;
	int          CaptionWidth, MaxTextWidth, MaxWidth;

	Items->BeginUpdate();
	FTextSizeBMP->Canvas->Font->Assign(this->Font);
	SmallImages->Height = FTextSizeBMP->Canvas->TextHeight( L"A" ) + 4;
	for( int ColumnIndex = 0; ColumnIndex < Columns->Count; ColumnIndex++)
	{
		CaptionWidth = GetColumnCaptionWidth(ColumnIndex);
		MaxTextWidth = GetColumnTextMaxWidth(ColumnIndex);
		if (CaptionWidth > MaxTextWidth)
			MaxWidth = CaptionWidth;
		else
			MaxWidth = MaxTextWidth;
		CurColumn = Columns->Items[ ColumnIndex ];
		CurColumn->MinWidth = CaptionWidth + 20;
		MaxWidth += 20;
		//CurColumn->MaxWidth = MaxWidth * 1.5;
		CurColumn->Width = MaxWidth;
	}  //for (int ColumnIndex = 1; ColumnIndex < Columns->Count; ColumnIndex++)
	Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::Click(void)
{
	TPoint CurPosition = CalcCursorPos();
	TListItem *CurItemPtr = this->GetItemAt(CurPosition.x, CurPosition.y);
	TExecution* OrderMessagePtr = 0;
	bool IsClickCancel = false;
	bool IsClickExpand = false;
	if (CurItemPtr != 0)
	{
		OrderMessagePtr = (TExecution*)CurItemPtr->Data;
		if ((OrderMessagePtr != 0) && (OrderMessagePtr->IsRoot()))
		{
			TRect ItemRect = CurItemPtr->DisplayRect(drBounds);
			nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = OrderMessagePtr->GetOrderStatus();
			if ((CurPosition.x >= ItemRect.left) && (CurPosition.x < ItemRect.Height()))
			{
				if ((OrderRootStatus == nsOrderMessageDefine::osNew) ||
                    (OrderRootStatus == nsOrderMessageDefine::osReplaced) ||
					(OrderRootStatus == nsOrderMessageDefine::osPartiallyFilled))
					IsClickCancel = true;
			}
			else if ((CurPosition.x >= ItemRect.Height()) && (CurPosition.x < (2 * ItemRect.Height())))
			{
				IsClickExpand = true;
			}
		}  //if (OrderRootPtr != 0)
	}  //if (CurItemPtr != 0)

	if ((FOrderStore != 0) && IsClickCancel)
	{
		UnicodeString ExchangeCode = OrderMessagePtr->GetExchangeCode();
		if (this->FFilter != ftStopOrder)
		{
			try
			{
				UFC::AnsiString ansiStrategy = "";
				if (OrderMessagePtr->GetStrategyName().Length() > 0)
					ansiStrategy = s888::ConvertUnicodeToUFCAnsiStr(OrderMessagePtr->GetStrategyName());
				FOrderStore->CancelOrder( OrderMessagePtr->GetMarket(),
										  ExchangeCode,
										  OrderMessagePtr->GetOrderID(),
										  ansiStrategy );
			}
			catch (UFC::Exception x)
			{
				ShowMessage(x.what());
			}  //try
		}
		else
		{
			UnicodeString StopExchangeCode = OrderMessagePtr->GetStopExchangeCode();
			UnicodeString StopSymbolCode = OrderMessagePtr->GetStopSymbol();
			UnicodeString SymbolCode = OrderMessagePtr->GetSymbol()->GetSymbol(0);
			FOrderStore->CancelStopOrder(StopExchangeCode, StopSymbolCode, OrderMessagePtr->GetStopPrice(),
										 ExchangeCode, SymbolCode, OrderMessagePtr->GetPrice(), OrderMessagePtr->GetSide());
		}
	}
	else if ((FOrderStore != 0) && IsClickExpand)
		if (OrderMessagePtr->IsExpanded())
			CollapseListItem(CurItemPtr);
		else
			ExpandListItem(CurItemPtr);
	else
		TControl::Click();
}  //TOrderListView::Click(void)

//---------------------------------------------------------------------------
void __fastcall TOrderListView::MDBSMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
											  int X, int Y)
{
	if ((Button == mbRight) && (FOnOrderRightClick != 0) && (FFilter != ftStopOrder))
	{
		TListItem *CurItemPtr = this->GetItemAt(X, Y);
		TExecution* OrderMessagePtr = 0;
		if (CurItemPtr != 0)
		{
			OrderMessagePtr = (TExecution*)CurItemPtr->Data;
			if ((OrderMessagePtr != 0) && (OrderMessagePtr->IsRoot()))
			{
				FOnOrderRightClick(Sender, OrderMessagePtr->GetNID(), OrderMessagePtr->GetOrderID(), OrderMessagePtr->GetMarket(),
								   OrderMessagePtr->GetExchangeCode(), OrderMessagePtr->GetSymbol()->GetSymbol(0), OrderMessagePtr->GetSide(),
								   OrderMessagePtr->GetPrice(), OrderMessagePtr->GetLeavesQty(), OrderMessagePtr->GetOrderStatus());
			}
		}  //if (CurItemPtr != 0)
	}  //if ((Button == mbRight) && (FOnRightClick != 0))
}  //TOrderListView::MDBSMouseDown()
//---------------------------------------------------------------------------
bool __fastcall TOrderListView::IsFilterMarketPassed(nsOrderMessageDefine::MarketEnum TestMarket)
{
	switch (FFilterMarket)
	{
		case fltmFutures: ///< TAIFEX Taiwan Futures
			if( TestMarket == nsOrderMessageDefine::mTWFutures)
				return true;
			break;
		case fltmOptions: ///< TAIFEX Taiwan Options
			if( TestMarket == nsOrderMessageDefine::mTWOptions)
				return true;
			break;
		case fltmTSE: ///< TSEC   Taiwan equity
			if (TestMarket == nsOrderMessageDefine::mTSE)
				return true;
			break;
		case fltmOTC: ///< OTC    Taiwan
			if (TestMarket == nsOrderMessageDefine::mOTC)
				return true;
			break;
		case fltmPATSFutures: ///< PATS          Futures
			if (TestMarket == nsOrderMessageDefine::mForeignFutures)
				return true;
			break;
		case fltmPATSOptions: ///< PATS          Options
			if (TestMarket == nsOrderMessageDefine::mForeignOptions)
				return true;
			break;
		case fltmCNFutures: ///< GLQH   China  Futures
			if (TestMarket == nsOrderMessageDefine::mCNFutures)
				return true;
			break;
		case fltmCNOptions: ///< GLQH   China  Options
			if (TestMarket == nsOrderMessageDefine::mCNOptions)
				return true;
			break;
		case fltmAll:
		default: return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderListView::IsFilterSidePassed(nsOrderMessageDefine::SideEnum TestSide)
{
	switch (FFilterSide)
	{
		case fltsBuy:	if( TestSide == nsOrderMessageDefine::sBuy)
							return true;
						break;
		case fltsSell:  if( TestSide == nsOrderMessageDefine::sSell)
							return true;
						break;
		case fltsAll:
		default:		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderListView::IsFilterSymbolPassed( const String& TestSymbol )
{
	if( FFilterSymbol.Length() == 0 || TestSymbol.Pos( FFilterSymbol ) > 0 )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderListView::IsFilterPricePassed(double TestPrice)
{
	if (FFilterPrice > 0.000001)
	{
		double diffPx = FFilterPrice - TestPrice;
		if ((diffPx >= -0.000001) && (diffPx <= 0.000001))
			return true;
		else
			return false;
	}
	else
		return true;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderListView::PassFilter( const String& TestSymbol,TExecution* OrderRootPtr)
{
	if( IsFilterSidePassed( OrderRootPtr->GetSide()) &&
		IsFilterMarketPassed( OrderRootPtr->GetMarket()) &&
		IsFilterPricePassed( OrderRootPtr->GetPrice()) &&
		IsFilterSymbolPassed( TestSymbol ) )
		return true;
	return false;
}
//---------------------------------------------------------------------------
CStringTExecutionMap* __fastcall TOrderListView::GetExecutionMap( void )
{
	CStringTExecutionMap* OrderMapPtr;
	switch( FFilter )
	{
		case ftAll:	     OrderMapPtr = FOrderStore->GetOrderRootTimeOrderIDIndex();break;
		case ftFill:     OrderMapPtr = FOrderStore->GetFilledOrderRootTimeOrderIDIndex();break;
		case ftCanceled: OrderMapPtr = FOrderStore->GetCanceledOrderRootTimeOrderIDIndex();break;
		case ftActive:   OrderMapPtr = FOrderStore->GetActiveOrderRootTimeOrderIDIndex();break;
		case ftStopOrder:OrderMapPtr = FOrderStore->GetStopOrderTimeNIDIndex();break;
		default:         OrderMapPtr = FOrderStore->GetOrderRootTimeOrderIDIndex();break;
	}  //switch (FFilter)
	return 	OrderMapPtr;
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::RefreshOrderData()
{
	if( FOrderStore == NULL ) return;

	CStringTExecutionMap* OrderMapPtr    = GetExecutionMap();
	int                   TotalItemCount = OrderMapPtr->size();
	int                   CurItemNo      = 0;
	String                TestSymbol;
	TExecution*           OrderRootPtr;
	TListItem*            ItemPtr;

//	Application->ProcessMessages();
	Items->BeginUpdate();
	Items->Clear();
	if( OnProgress != NULL )
		OnProgress( this, CurItemNo, TotalItemCount);
	if( FOrderingCriteria == 1 )
	{
		CStringTExecutionMap::reverse_iterator it;
		for( it = OrderMapPtr->rbegin(); it != OrderMapPtr->rend(); it++)
		{
			OrderRootPtr = it->second;
			OrderRootPtr->SetExpanded( false );
			if( OrderRootPtr->GetSymbol() != NULL )
				TestSymbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
			else
				TestSymbol = L"";
			CurItemNo++;
			if( PassFilter( TestSymbol, OrderRootPtr ) == true )
				AddNewItem(OrderRootPtr);
			if( OnProgress != NULL )
				OnProgress(this, CurItemNo, TotalItemCount);
		}  //for (it = OrderMapPtr->begin(); it != OrderMapPtr->end(); it++)
	}
	else
	{
		CStringTExecutionMap::iterator it;
		for( it = OrderMapPtr->begin(); it != OrderMapPtr->end(); it++ )
		{
			OrderRootPtr = it->second;
			OrderRootPtr->SetExpanded( false );
			if( OrderRootPtr->GetSymbol() != NULL )
				TestSymbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
			else
				TestSymbol = L"";
			CurItemNo++;
			if( PassFilter( TestSymbol, OrderRootPtr ) == true )
				AddNewItem(OrderRootPtr);
			if( OnProgress != NULL )
				OnProgress( this, CurItemNo, TotalItemCount);
		}  //for (it = OrderMapPtr->begin(); it != OrderMapPtr->end(); it++)
	}  //if (FOrderingCriteria == 1)

	switch( FSortColumnTag )
	{
		case  1: CustomSort(OrderListViewSortLocalMsgTime, FOrderingCriteria); break; ///< Sort by 時間
		case  2: CustomSort(OrderListViewSortOrderID,      FOrderingCriteria); break; ///< Sort by 單號
		case  3: CustomSort(OrderListViewSortProduct,      FOrderingCriteria); break; ///< Sort by 合約
		case 16: CustomSort(OrderListViewSortSide,         FOrderingCriteria); break; ///< Sort by 買賣
		case 19: CustomSort(OrderListViewSortPrice,        FOrderingCriteria); break; ///< Sort by 價格
		case 21: CustomSort(OrderListViewSortLastPrice,    FOrderingCriteria); break; ///< Sort by 成交價
	}  //switch(FSortColumnTag)
	FitAllColumnWidth();
	Items->EndUpdate();
}  //TOrderListView::RefreshOrderData()
//---------------------------------------------------------------------------
bool __fastcall TOrderListView::ExecutionStatusPassFilter(  nsOrderMessageDefine::OrderStatusEnum OrderStatus )
{
	 switch( FFilter )
	 {
		case ftAll: 	 return true;
		case ftFill:	 if( OrderStatus == nsOrderMessageDefine::osPartiallyFilled ||
							 OrderStatus == nsOrderMessageDefine::osFilled )
							 return true;
						 return false;
		case ftCanceled: if( OrderStatus == nsOrderMessageDefine::osCanceled )
							 return true;
						 return false;
		case ftActive:	 if( OrderStatus == nsOrderMessageDefine::osNew ||
							 OrderStatus == nsOrderMessageDefine::osReplaced ||
							 OrderStatus == nsOrderMessageDefine::osPartiallyFilled ||
							 OrderStatus == nsOrderMessageDefine::osRejected ||
							 OrderStatus == nsOrderMessageDefine::osPendingNew )
							 return true;
						 return false;
		default:         return false;
	 }
}
//---------------------------------------------------------------------------
void __fastcall TOrderListView::ProcessNewMessage( TExecution* OrderRootPtr )
{
	String TestSymbol( L"" );

	if( OrderRootPtr->GetSymbol() != NULL )
		TestSymbol = OrderRootPtr->GetSymbol()->GetSymbol( 0 );
	if( PassFilter( TestSymbol, OrderRootPtr ) == true )
	{
		nsOrderMessageDefine::OrderStatusEnum OrderStatus = OrderRootPtr->GetOrderStatus();
		TListItem* ItemPtr = FindCaption( 0, String( OrderRootPtr->GetNID()), false, true, false);

		if( OrderStatus == nsOrderMessageDefine::osPartiallyFilled &&
			( OrderRootPtr->GetLeavesQty() <= 0 || OrderRootPtr->GetCumQty() >= OrderRootPtr->GetOrderQty()))
			OrderStatus == nsOrderMessageDefine::osFilled;
		if( ItemPtr == NULL )  //New Order Root
		{
			if( ExecutionStatusPassFilter( OrderStatus ) == true  )
			{
				OrderRootPtr->SetExpanded( false );
				AddNewItem( OrderRootPtr );
			}
		}
		else  //Order Status Changed
		{
			CollapseListItem( ItemPtr );
			if( ExecutionStatusPassFilter( OrderStatus ) == false  )
			{
				Items->BeginUpdate();
				ItemPtr->Delete();
				Items->EndUpdate();
			}
			else
				RefreshItem( ItemPtr, OrderRootPtr );
		}
	}
	FLastProcessTick = UFC::GetTickCountMS();
}  //TOrderListView::ProcessNewMessage()
//---------------------------------------------------------------------------
void TOrderListView::OrderMessageArrived(TExecution* OrderRootPtr)
{
	if (OrderRootPtr !=  NULL )
	{
		int DiffMillisecond =  UFC::GetTickCountMS() - FLastProcessTick;

		if( DiffMillisecond >= (int)FMessageTimer->Interval ) ///< Timeout
		{
			FMessageQueue.push(OrderRootPtr);
			OnMessageTimer( NULL );
		}
		else
		{
			FMessageQueue.push(OrderRootPtr);
			if( !FMessageTimer->Enabled )
				FMessageTimer->Enabled = true;
		}
	}
}  //TOrderListView::OrderMessageArrived()
//---------------------------------------------------------------------------
void TOrderListView::DeleteRejectedOrderRoot(TExecution* OrderRootPtr)
{
	if ((FFilter != ftAll) && (FFilter != ftActive)) return;
	if( OrderRootPtr == NULL ) return;

	UnicodeString NIDString;
	NIDString.printf(L"%lld", OrderRootPtr->GetNID());
	TListItem* ItemPtr = this->FindCaption(0, NIDString, false, true, false);
	if (ItemPtr == 0) return;

	TExecution* ItemOrderRootPtr = (TExecution*)ItemPtr->Data;
	if (ItemOrderRootPtr->IsRoot() && ItemOrderRootPtr->IsExpanded())
		CollapseListItem(ItemPtr);
	Items->BeginUpdate();
	ItemPtr->Delete();
	Items->EndUpdate();
}  //TOrderListView::DeleteRejectedOrderRoot()
//---------------------------------------------------------------------------
void TOrderListView::RefreshDataNotice( )
{
	RefreshOrderData();
}
//---------------------------------------------------------------------------
void TOrderListView::StopOrderChanged(TExecution* OrderRootPtr, StopOrderStatusEnum StopOrderStatus)
{
	if (FFilter != ftStopOrder) return;
	if (OrderRootPtr == 0) return;

	String     NIDString(OrderRootPtr->GetNID());
	TListItem* ItemPtr = this->FindCaption( 0, NIDString, false, true, false);

	switch( StopOrderStatus )
	{
		case soNew:
			if( ItemPtr == NULL )
				AddNewItem(OrderRootPtr);
			break;
		case soRemove:
		case soSend:
			if (ItemPtr != NULL)
			{
				Items->BeginUpdate();
				ItemPtr->Delete();
				Items->EndUpdate();
			}
			break;
	}  //switch(StopOrderStatus)
}  //TOrderListView::StopOrderChanged()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::GetColumns(UnicodeString& Str)
{
	UnicodeString RtStr;
	UFC::NameValueMessage Msg;
	for (int i = 1; i < Columns->Count ; i++)
	{
		TListColumn* Col = Columns->Items[i];
		Msg.Append(UFC::AnsiString( Col->Tag ), (long)Col->Width );
	}
	Str = Msg.ToString().c_str();
}  //TOrderListView::GetColumns()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::SetColumns( const String& ColStr)
{
	UFC::NameValueMessage Msg;
	UFC::AnsiString Name, Value;
	UFC::AnsiString AnsiColStr( ColStr.c_str() );
	TListColumn *NewCol;

	if( AnsiColStr.Length() > 10 )
		Msg.FromString( AnsiColStr );
	else
		Msg.FromString( "1=56,2=56,3=92,28=56,16=44,25=56," );
	Columns->BeginUpdate();
	Columns->Clear();
	///< Add first column
	NewCol = (TListColumn*)Columns->Add();
	NewCol->Caption = OrderListViewColumnString[ 0 ];
	NewCol->Tag = 0;
	NewCol->Width    = 55;
	NewCol->MinWidth = 55;
	//NewCol->MaxWidth = 55;
	for (register int i = 0; i < PopupMenu->Items->Count; i++)
		PopupMenu->Items->operator [](i)->Checked = false;
	for(int i = 0; i < Msg.ItemCount(); i++)
	{
		if (Msg.Get(i, Name, Value))
		{
			TMenuItem* Item = PopupMenu->Items->operator []( Name.ToInt() - 1);
			Item->Checked = true;

			TListColumn *NewCol = (TListColumn*)Columns->Add();
			NewCol->Caption = OrderListViewColumnString[Item->Tag];
			NewCol->Tag = Item->Tag;
			NewCol->Width = Value.ToInt();
		}
	}
	Columns->EndUpdate();
}  //TOrderListView::SetColumns()
//---------------------------------------------------------------------------
void __fastcall TOrderListView::OnMessageTimer(TObject *Sender)
{
	FMessageTimer->Enabled = false;
	if( FMessageQueue.empty() == false  )
	{
		TExecution* OrderRootPtr;

		while (!FMessageQueue.empty())
		{
			OrderRootPtr = FMessageQueue.front();
			FMessageQueue.pop();
			ProcessNewMessage(OrderRootPtr);
		}
	}
}  //TOrderListView::OnMessageTimer()
//---------------------------------------------------------------------------
bool __fastcall TOrderListView::WriteContentToCSVFile(const String& FullFileName)
{
	if (Sysutils::FileExists(FullFileName) && !Sysutils::DeleteFile(FullFileName)) return false;
	int FileHandle = Sysutils::FileCreate(FullFileName);
	if (FileHandle < 0) return false;
	Sysutils::FileSeek(FileHandle, 0, 0);

	//Output Column Name
	char CarriageReturnBuffer[2];
	char LineFeedBuffer[2];
	UnicodeString CSVStr = L"";

	CarriageReturnBuffer[0] = '\r';   CarriageReturnBuffer[1] = 0;
	LineFeedBuffer[0] = '\n';         LineFeedBuffer[1] = 0;
	for (int ColumnIndex = 1; ColumnIndex < Columns->Count; ColumnIndex++)
	{
		TListColumn *CurColumn = (*(this->Columns))[ColumnIndex];
		UnicodeString CaptionText = CurColumn->Caption;
		if (CSVStr.Length() > 0)
			CSVStr = CSVStr + L",";
		CSVStr = CSVStr + CaptionText;
	}  //for (int ColumnIndex = 1; ColumnIndex < Columns->Count; ColumnIndex++)
	AnsiString HeadLine( CSVStr.c_str() );

	Sysutils::FileWrite(FileHandle, HeadLine.c_str(), HeadLine.Length() );
	Sysutils::FileWrite(FileHandle, CarriageReturnBuffer, 1);
	Sysutils::FileWrite(FileHandle, LineFeedBuffer, 1);

	//Output Order Data
	TListItem* CurItemPtr;
	TExecution* CurOrderRootPtr;
	for (int RowIndex = 0; RowIndex < this->Items->Count; RowIndex++)
	{
		CurItemPtr = (*(this->Items))[RowIndex];
		CurOrderRootPtr = (TExecution*)CurItemPtr->Data;
		if (!CurOrderRootPtr->IsRoot()) continue;
		CSVStr = L"";
		for (int ColumnIndex = 1; ColumnIndex < Columns->Count; ColumnIndex++)
		{
			TListColumn *CurColumn = (*(this->Columns))[ColumnIndex];
			UnicodeString FieldText = CurOrderRootPtr->GetStringField(OrderListViewColumnTag[CurColumn->Tag]);
			if (CSVStr.Length() > 0)
				CSVStr = CSVStr + L",";
			CSVStr = CSVStr + FieldText;
		}  //for (int ColumnIndex = 1; ColumnIndex < Columns->Count; ColumnIndex++)
		AnsiString AnsiLine( CSVStr.c_str() );

		Sysutils::FileWrite(FileHandle, AnsiLine.c_str(), AnsiLine.Length() );
		Sysutils::FileWrite(FileHandle, CarriageReturnBuffer, 1);
		Sysutils::FileWrite(FileHandle, LineFeedBuffer, 1);
	}  //for (int RowIndex = 0; RowIndex < this->Items->Count; RowIndex++)
	Sysutils::FileClose(FileHandle);
	return true;
}  //TOrderListView::WriteContentToCSVFile()
//---------------------------------------------------------------------------
int __fastcall TOrderListView::GetJSON( String& jsonstr )
{
	TListItem*   CurItemPtr;
	TExecution*  CurOrderRootPtr;
	String       CSVStr,FieldText,TimeStr,Today, JSONItem;;
	TStringList* StringL = new TStringList();
	UFC::AnsiString ThisDay;
	int FilledCount = 0;

	UFC::GetTradeYYYYMMDD( ThisDay );
	Today = ThisDay.c_str();
	UFC::BufferedLog::Printf( "------------------ Begin Upload ----------------------------");
	UFC::BufferedLog::Printf( " %d Executions",Items->Count );
	for( int i = 0; i < Items->Count; i++)
	{
		CurItemPtr = (*(this->Items))[i];
		CurOrderRootPtr = (TExecution*)CurItemPtr->Data;
		if( CurOrderRootPtr->IsRoot() )
		{
			FieldText = L"\"productNo\":\"" + CurOrderRootPtr->GetStringField(OrderListViewColumnTag[4])+ L"\",";
			String ProdID = CurOrderRootPtr->GetStringField(OrderListViewColumnTag[4]).SubString( 1,3 );
			if( ProdID == L"TXF" )
			{
				///< Product Name
				CSVStr = FieldText;
				FieldText = L"\"productName\":\"" + CurOrderRootPtr->GetStringField(OrderListViewColumnTag[3])+ L"\",";
				CSVStr = CSVStr + FieldText;
				///< Side
				if( CurOrderRootPtr->GetStringField(OrderListViewColumnTag[16]) == L"買" )
					FieldText = L"\"bs\":\"B\",";
				else
					FieldText = L"\"bs\":\"S\",";
				CSVStr = CSVStr + FieldText;
				///< Price
				FieldText = L"\"price\":\"" + CurOrderRootPtr->GetStringField(OrderListViewColumnTag[21])+ L"\",";
				CSVStr = CSVStr + FieldText;
				///< Qty
				FieldText = L"\"qty\"=\"" + CurOrderRootPtr->GetStringField(OrderListViewColumnTag[26])+ L"\",";
				CSVStr = CSVStr + FieldText;
				///< Contract Multiplyer
				FieldText = L"\"qtyPrice\":\"200\",";
				CSVStr = CSVStr + FieldText;
				///< Time
				TimeStr = CurOrderRootPtr->GetStringField(OrderListViewColumnTag[1]);
				TimeStr.Delete( 9, 1);
				TimeStr.Delete( 6, 1);
				TimeStr.Delete( 3, 1);
				FieldText = L"\"txDatetime\":\"" + Today + TimeStr+ L"\"";
				CSVStr = CSVStr + FieldText;
				///< To JSON string
				JSONItem = L"{"+CSVStr + L"}";
				StringL->Add( JSONItem );

				AnsiString JSONItemAnsi( JSONItem );
				UFC::BufferedLog::Printf( " %s", JSONItemAnsi.c_str() );

			}
		}
	}
	UFC::BufferedLog::Printf( " %d TXF Filled executions.", StringL->Count );
	UFC::BufferedLog::Printf( "------------------ Finished Upload -------------------------");

	jsonstr = L"[";
	for( int i =0; i< StringL->Count; i ++ )
	{
		jsonstr += StringL->Strings[i];
		if( i != StringL->Count-1 )
			jsonstr += L",";
	}
	jsonstr +=  L"]";
	FilledCount = StringL->Count;
	delete StringL;
	return FilledCount;
}
/// productNo = 4, productName= 3,bs=16,price=21,qty= 26,txDatetime=1,
//---------------------------------------------------------------------------
namespace Orderlistview
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TOrderListView)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
