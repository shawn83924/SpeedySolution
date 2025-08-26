//---------------------------------------------------------------------------
#ifndef OrderListViewH
#define OrderListViewH
//---------------------------------------------------------------------------
#include <queue>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include "TExecutionUnit.h"
#include "OrderStore.h"
//---------------------------------------------------------------------------
typedef enum
{
	ftAll       = 0,
	ftFill      = 1,
	ftCanceled  = 2,
	ftActive    = 3,
	ftStopOrder = 4,
    ftNone      = 5

} OrderFilterEnum;

//--------------------------------------------------------------------------
typedef enum
{
	fltmFutures     = 0, ///< TAIFEX Taiwan Futures
	fltmOptions     = 1, ///< TAIFEX Taiwan Options
	fltmTSE         = 2, ///< TSEC   Taiwan equity
	fltmOTC         = 3, ///< OTC    Taiwan
	fltmPATSFutures = 4, ///< PATS   Futures
	fltmPATSOptions = 5, ///< PATS   Options
	fltmCNFutures   = 6, ///< China  Futures
	fltmCNOptions   = 7, ///< China  Options
	fltmAll         = 10

} MarketFilterEnum;

//--------------------------------------------------------------------------
typedef enum
{
	fltsAll = 0,
	fltsBuy  = 1,
	fltsSell = 2

} SideFilterEnum;

//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TOnProgress)(System::TObject* Sender, int CurrentItemNo, int TotalItemCount);
typedef void __fastcall (__closure *TOnOrderRightClick)(System::TObject* Sender, int OrderNID, UnicodeString OrderID,
														nsOrderMessageDefine::MarketEnum Market,
														UnicodeString ExchangeCode, UnicodeString SymbolCode,
														nsOrderMessageDefine::SideEnum Side, double Price, int LeavesQty,
														nsOrderMessageDefine::OrderStatusEnum OrderStatus);
typedef void __fastcall (__closure *TOnOrderKeyDown)(System::TObject* Sender, WORD &Key, TShiftState Shift,
													 int OrderNID, UnicodeString OrderID,
													 nsOrderMessageDefine::MarketEnum Market, UnicodeString SymbolCode,
													 nsOrderMessageDefine::SideEnum Side, double Price, int LeavesQty,
													 nsOrderMessageDefine::OrderStatusEnum OrderStatus);
typedef int (__stdcall* TSortProc )(long,long,long );
//---------------------------------------------------------------------------
class PACKAGE TOrderListView : public TCustomListView, public TOrderMessageListener
{
private:
	OrderFilterEnum    FFilter;
	TOrderStore*       FOrderStore;
	TColor             FPendingBKColor;
	TColor             FPendingColor;
	TColor             FBuyNewBKColor;
	TColor             FBuyNewColor;
	TColor             FSellNewBKColor;
	TColor             FSellNewColor;
	TColor             FBuyPartialFillBKColor;
	TColor             FBuyPartialFillColor;
	TColor             FSellPartialFillBKColor;
	TColor             FSellPartialFillColor;
	TColor             FBuyFillBKColor;
	TColor             FBuyFillColor;
	TColor             FSellFillBKColor;
	TColor             FSellFillColor;
	TColor             FCanceledBKColor;
	TColor             FCanceledColor;
	TColor             FRejectBKColor;
	TColor             FRejectColor;
	Graphics::TBitmap* FCancelBMP;
	Graphics::TBitmap* FLightCancelBMP;
	Graphics::TBitmap* FColumn0BMP;
	Graphics::TBitmap* FExpandBMP;
	Graphics::TBitmap* FCollapseBMP;
	Graphics::TBitmap* FMessageLinkBMP;
	Graphics::TBitmap* FMessageLinkEndBMP;
	Graphics::TBitmap* FTextSizeBMP;
	int                FSortColumnTag;
	int                FOrderingCriteria;  //0 : ascending;  1 : descending
	DRAWTEXTPARAMS     FDrawTextParam;
	//Filter
	MarketFilterEnum   FFilterMarket;
	SideFilterEnum     FFilterSide;
	UnicodeString      FFilterSymbol;
	double             FFilterPrice;

	DWORD   FLastProcessTick;
	TTimer* FMessageTimer;
	void __fastcall OnMessageTimer(TObject *Sender);
	std::queue<TExecution*> FMessageQueue;

	void __fastcall MenuClicked(TObject* Sender);

	void __fastcall SetOrderStore(TOrderStore* FOrderStore);
	void __fastcall SetFilter(OrderFilterEnum Filter);
	void __fastcall SetNewOnTop(bool NewOnTop);
	void __fastcall SetCancelBMP(Graphics::TBitmap* CancelBMP);
	void __fastcall SetExpandBMP(Graphics::TBitmap* ExpandBMP);
	void __fastcall SetCollapseBMP(Graphics::TBitmap* CollapseBMP);
	void __fastcall SetMessageLinkBMP(Graphics::TBitmap* MessageLinkBMP);
	void __fastcall SetMessageLinkEndBMP(Graphics::TBitmap* MessageLinkEndBMP);

	void __fastcall SetFilterMarket(MarketFilterEnum FilterMarket);
	void __fastcall SetFilterSide(SideFilterEnum FilterSide);
	void __fastcall SetFilterSymbol(UnicodeString FilterSymbol);
	void __fastcall SetFilterPrice(double FilterPrice);

	void __fastcall RefreshSubItem(TListItem* ItemPtr, TExecution* OrderRootPtr);
	void __fastcall RefreshItem(TListItem* ItemPtr, TExecution* OrderRootPtr);
	void __fastcall AddNewItem(TExecution* OrderRootPtr);

	void __fastcall MDBSAdvancedCustomDraw(TCustomListView *Sender, const TRect &ARect,
										   TCustomDrawStage Stage, bool &DefaultDraw);
	void __fastcall MDBSAdvancedCustomDrawItem(TCustomListView* Sender, TListItem* Item,
											   TCustomDrawState State, TCustomDrawStage Stage, bool &DefaultDraw);
	void __fastcall MDBSMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
								  int X, int Y);
	void __fastcall ExpandListItem(TListItem *ItemPtr);
	void __fastcall CollapseListItem(TListItem *ItemPtr);
	void __fastcall MDBSColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall MDBSKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ProcessNewMessage(TExecution* OrderRootPtr);
	void __fastcall SetCanvasColor( TCanvas* canvas, bool IsSelected, TExecution* Execution );
	void __fastcall DrawCancelExpandIcon( TExecution* Execution, TRect& ItemRect );
	void __fastcall GetTextFormat( int SubItemIndex, TTextFormat& Formats );
	TOnProgress FOnProgress;
	TOnOrderRightClick FOnOrderRightClick;
	TOnOrderKeyDown FOnOrderKeyDown;
protected:
	DYNAMIC void __fastcall Click(void);
	virtual void __fastcall WndProc(Messages::TMessage &Message);
private:
	void __fastcall InitString( void );
	void __fastcall InitialColumn( void );
	void __fastcall InitPopupMenu( void );
	void __fastcall DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor DownCol );
	void __fastcall Loaded( void );
	bool __fastcall IsFilterMarketPassed(nsOrderMessageDefine::MarketEnum TestMarket);
	bool __fastcall IsFilterSidePassed(nsOrderMessageDefine::SideEnum TestSide);
	bool __fastcall IsFilterSymbolPassed( const String& TestSymbol);
	bool __fastcall IsFilterPricePassed(double TestPrice);
	bool __fastcall PassFilter( const String& TestSymbol,TExecution* OrderRootPtr );
	bool __fastcall ExecutionStatusPassFilter(  nsOrderMessageDefine::OrderStatusEnum OrderStatus );
	void __fastcall ResetSortColumnCaption( void );
	void __fastcall SortByColumn( TListColumn *Column, TSortProc SortProc);
	CStringTExecutionMap* __fastcall GetExecutionMap( void );
	int  __fastcall GetColumnCaptionWidth( int ColumnIndex );
//	int  __fastcall CalculateColumnTextMaxWidth( int ColumnIndex );
	int  __fastcall GetColumnTextMaxWidth( int ColumnIndex );
	void __fastcall FitColumnWidth(int ColumnIndex);
public: ///< Implement interface TOrderMessageListener
	virtual void OrderMessageArrived(TExecution* OrderRootPtr);
	virtual void DeleteRejectedOrderRoot(TExecution* OrderRootPtr);
	virtual void RefreshDataNotice( void );
	virtual void StopOrderChanged(TExecution* OrderRootPtr, StopOrderStatusEnum StopOrderStatus);
public:
	__fastcall TOrderListView(TComponent* Owner);
	__fastcall ~TOrderListView();
	void __fastcall RefreshOrderData();
	void __fastcall GetColumns( String& Str );
	void __fastcall SetColumns( const String& ColStr);
	void __fastcall ExpandAllListItem();
	void __fastcall CollapseAllListItem();
	int  __fastcall CancelFilteredOrder();
	void __fastcall FitAllColumnWidth();
	void __fastcall SetFilters( MarketFilterEnum FilterMarket, SideFilterEnum FilterSide, const String& FilterSymbol, double FilterPrice);
	bool __fastcall WriteContentToCSVFile(const String& FullFileName);
    int __fastcall GetJSON( String& CSVStr );
__published:
	__property Font;
	__property OrderFilterEnum Filter = {read = FFilter, write = SetFilter, default = ftAll};
	__property TOrderStore* OrderStore = {read = FOrderStore, write = SetOrderStore, default = 0};
	__property TColor PendingBKColor = {read = FPendingBKColor, write = FPendingBKColor};
	__property TColor PendingColor = {read = FPendingColor, write = FPendingColor};
	__property TColor BuyNewBKColor = {read = FBuyNewBKColor, write = FBuyNewBKColor};
	__property TColor BuyNewColor = {read = FBuyNewColor, write = FBuyNewColor};
	__property TColor SellNewBKColor = {read = FSellNewBKColor, write = FSellNewBKColor};
	__property TColor SellNewColor = {read = FSellNewColor, write = FSellNewColor};
	__property TColor BuyPartialFillBKColor = {read = FBuyPartialFillBKColor, write = FBuyPartialFillBKColor};
	__property TColor BuyPartialFillColor = {read = FBuyPartialFillColor, write = FBuyPartialFillColor};
	__property TColor SellPartialFillBKColor = {read = FSellPartialFillBKColor, write = FSellPartialFillBKColor};
	__property TColor SellPartialFillColor = {read = FSellPartialFillColor, write = FSellPartialFillColor};
	__property TColor BuyFillBKColor = {read = FBuyFillBKColor, write = FBuyFillBKColor};
	__property TColor BuyFillColor = {read = FBuyFillColor, write = FBuyFillColor};
	__property TColor SellFillBKColor = {read = FSellFillBKColor, write = FSellFillBKColor};
	__property TColor SellFillColor = {read = FSellFillColor, write = FSellFillColor};
	__property TColor CanceledBKColor = {read = FCanceledBKColor, write = FCanceledBKColor};
	__property TColor CanceledColor = {read = FCanceledColor, write = FCanceledColor};
	__property TColor RejectBKColor = {read = FRejectBKColor, write = FRejectBKColor};
	__property TColor RejectColor = {read = FRejectColor, write = FRejectColor};
	__property Graphics::TBitmap* CancelBMP = {read = FCancelBMP, write = SetCancelBMP};
	__property Graphics::TBitmap* ExpandBMP = {read = FExpandBMP, write = SetExpandBMP};
	__property Graphics::TBitmap* CollapseBMP = {read = FCollapseBMP, write = SetCollapseBMP};
	__property Graphics::TBitmap* MessageLinkBMP = {read = FMessageLinkBMP, write = SetMessageLinkBMP};
	__property Graphics::TBitmap* MessageLinkEndBMP = {read = FMessageLinkEndBMP, write = SetMessageLinkEndBMP};
	__property OnClick;
	__property Align;
	__property ShowHint;
	__property Hint;
	__property Color;
	__property TOnProgress OnProgress = {read = FOnProgress, write = FOnProgress};
	__property TOnOrderRightClick OnOrderRightClick = {read = FOnOrderRightClick, write = FOnOrderRightClick};
	__property TOnOrderKeyDown OnOrderKeyDown = {read = FOnOrderKeyDown, write = FOnOrderKeyDown};
	//Filter
	__property MarketFilterEnum FilterMarket = {read = FFilterMarket, write = SetFilterMarket, default = fltmAll};
	__property SideFilterEnum FilterSide = {read = FFilterSide, write = SetFilterSide, default = fltsAll};
	__property UnicodeString FilterSymbol = {read = FFilterSymbol, write = SetFilterSymbol};
	__property double FilterPrice = {read = FFilterPrice, write = SetFilterPrice};
};
//---------------------------------------------------------------------------
#endif
