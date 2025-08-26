//---------------------------------------------------------------------------

#ifndef HoldOpenInterestH
#define HoldOpenInterestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "RoundFormEx.h"
#include "GraphButton.h"
#include "GraphPanel.h"
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include "OrderMessageDefine.h"
#include <System.ImageList.hpp>
#include "TradingPosition.h"
//---------------------------------------------------------------------------
class TOrderStore;
class BasicInformation;
//---------------------------------------------------------------------------
class THoldOpenInterestForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TTabSheet *ProfitTabSheet;
	TTabSheet *HoldPositionTabSheet;
	TListView *FPList;
	TListView *OIList;
	TRoundFormEx *RoundFormEx;
	TGraphButton *ProfitButton;
	TGraphButton *HoldPositionButton;
	TStaticText *RTProfitStaticText;
	TStaticText *ProfitStaticText;
	TPopupMenu *colMenu;
	TMenuItem *OIMenuItem;
	TPopupMenu *PopupMenu;
	TMenuItem *ShowDepthMenuItem;
	TMenuItem *CloseAllItem;
	TImageList *ImageList;
	TPanel *SettingPanel;
	TImageList *TabImageList;
	TGraphButton *IMButton;
	TMenuItem *CloseAllPosItem;
	TMenuItem *N1;
	TStaticText *WarningLabel;
	void __fastcall ProfitButtonClick(TObject *Sender);
	void __fastcall FPListDrawItem(TCustomListView *Sender, TListItem *Item, TRect &Rect,
		  TOwnerDrawState State);
	void __fastcall FPListCustomDrawSubItem(TCustomListView *Sender, TListItem *Item,
          int SubItem, TCustomDrawState State, bool &DefaultDraw);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FPListColumnRightClick(TObject *Sender, TListColumn *Column, TPoint &Point);
	void __fastcall OIMenuItemClick(TObject *Sender);
	void __fastcall PositionListCustomDrawSubItem(TCustomListView *Sender, TListItem *Item,
          int SubItem, TCustomDrawState State, bool &DefaultDraw);
	void __fastcall PositionListDrawItem(TCustomListView *Sender, TListItem *Item, TRect &Rect,
          TOwnerDrawState State);
	void __fastcall FPListDblClick(TObject *Sender);
	void __fastcall FPListMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall CloseAllItemClick(TObject *Sender);
	void __fastcall IMButtonClick(TObject *Sender);
	void __fastcall CloseAllPosItemClick(TObject *Sender);
	void __fastcall PopupMenuPopup(TObject *Sender);
	void __fastcall ShowDepthMenuItemClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall FormConstrainedResize(TObject *Sender, int &MinWidth, int &MinHeight,
          int &MaxWidth, int &MaxHeight);

private:	// User declarations
	int        FSubItemCount;
	bool       FIsOpenInterestColumnDisplayed;
	double     FTotalProfit;
	String     FTempExchangeCode;
	TListItem* FSubtotalItem;
	TWndMethod FOrigWindowProc;
	TWndMethod FOrigOIWindowProc;
	TCanvas*   FEarseBKCanvas;
	void __fastcall GetDisplayName( const String& Ex, const String& Sym, String& DisplayName );
	String __fastcall ToCurrencyString( double Number );
	void RefreshItem( const String& ExchangeCode, const String& SymbolCode, TListItem* ItemPtr,
					  void *PositionStatisticRecordPtr);
	void __fastcall ListWndProc( TMessage &Msg );
	void __fastcall OIListWndProc( TMessage &Msg );
	void __fastcall EraseBK( HDC DC , TListView*  );
	void __fastcall SaveToClipboard( void );
private: ///< for close all functions.
	double __fastcall GetCloseOrderPx( const String& exchange, const String& symbol, double Px, int BetterTick );
	void __fastcall DeleteAllOrderSync( TOrderStore* OrdStore, const String& exchange, const String& symbol );
	bool __fastcall CloseOrder( TOrderStore* OrdStore, BasicInformation* Info, nsOrderMessageDefine::SideEnum Side, double Price, int Qty, int MaxQty );
	void CheckItemWidth( int i,  const String& ItemText, TListItem* ItemPtr );
	void __fastcall HoldPosition( void );
	void __fastcall DeleteWorkingCloseAll( TListItem *Item, int BetterTick, bool IsMarket, int MaxQty );
	TListItem* __fastcall FindItem( s888::CPositionStatisticRecord *posStatisticRecPtr, String& sortKey, int& insertPosition );
	void __fastcall InitPositionItem( const String& ExchangeCode, const String& Symbol, TListItem *targetItemPtr, void *posStatisticRecPtr );
public:		// User declarations
	__fastcall THoldOpenInterestForm(TComponent* Owner);
	void RefreshSumItem();
	double GetTotalProfit( void ) { return FTotalProfit; }
	void UpdateFloatingProfit(const String& ExchangeCode, const String& Symbol,
							  void *PositionStatisticRecordPtr);
	void __fastcall SwitchPage( int );
	int  __fastcall CurrentPage( void );
	void __fastcall SaveProperties( void );
	void __fastcall LoadProperties( void );
};
//---------------------------------------------------------------------------
extern PACKAGE THoldOpenInterestForm *HoldOpenInterestForm;
//---------------------------------------------------------------------------
#endif
