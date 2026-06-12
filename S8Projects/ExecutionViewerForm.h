//---------------------------------------------------------------------------

#ifndef ExecutionViewerFormH
#define ExecutionViewerFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "GraphButton.h"
#include "GraphPanel.h"
#include "OrderListView.h"
#include "RoundFormEx.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include "cgauges.h"
#include <Dialogs.hpp>
#include <Vcl.WinXCtrls.hpp>
#include "FMFHeader.h"
#include <SHDocVw.hpp>
#include <Vcl.OleCtrls.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
using namespace nsOrderMessageDefine;
const int TABS = 5;

//---------------------------------------------------------------------------
class TExecutionForm : public TForm, public TClientForm
{
__published:	// IDE-managed Components
	TGraphButton *AllButton;
	TGraphButton *CancelButton;
	TGraphButton *FillButton;
	TGraphButton *ActiveButton;
	TOrderListView *OrderListView;
	TPanel *ProgressPanel;
	TTimer *InitTimer;
	TGraphButton *StopPxButton;
	TPopupMenu *PopupMenu;
	TMenuItem *DeleteMenuItem;
	TMenuItem *QueryMenuItem;
	TCGauge *ProgressBar;
	TGraphButton *WidthFitButton;
	TMenuItem *ReplaceMenuItem;
	TButton *QueryButton;
	TEdit *edtPrice;
	TEdit *edtSymbol;
	TRadioGroup *MarketRadioGroup;
	TRadioGroup *SideRadioGroup;
	TGroupBox *FilterGroupBox;
	TLabel *SymbolLabel;
	TLabel *PxLabel;
	TButton *ClearButton;
	TSaveDialog *SaveCSVDialog;
	TGraphButton *LargeButton;
	TGraphButton *SmallButton;
	TPanel *FilterPanel;
	TRoundFormEx *RoundFormEx;
	TPanel *SettingPanel;
	TPageControl *PageControl;
	TTabSheet *ExecTabSheet;
	TTabSheet *BalanceTabSheet;
	TWebBrowser *WebBrowser;
	TImageList *TabImageList;
	void __fastcall FilterButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OrderListViewProgress(TObject *Sender, int CurrentItemNo, int TotalItemCount);
	void __fastcall InitTimerTimer(TObject *Sender);
	void __fastcall OrderListViewOrderRightClick(TObject *Sender, int OrdetNID, UnicodeString OrderID,
												 MarketEnum Market,
												 UnicodeString ExchangeCode, UnicodeString SymbolCode,
												 SideEnum Side,
												 double Price, int LeavesQty,
												 OrderStatusEnum OrderStatus);
	void __fastcall DeleteMenuItemClick(TObject *Sender);
	void __fastcall QueryMenuItemClick(TObject *Sender);
	void __fastcall WidthFitButtonClick(TObject *Sender);
	void __fastcall OrderListViewOrderKeyDown(TObject *Sender, WORD &Key, TShiftState Shift,
		  int OrderNID, UnicodeString OrderID, MarketEnum Market, UnicodeString SymbolCode,
		  SideEnum Side, double Price, int LeavesQty, OrderStatusEnum OrderStatus);
	void __fastcall QueryButtonClick(TObject *Sender);
	void __fastcall MarketRadioGroupClick(TObject *Sender);
	void __fastcall SideRadioGroupClick(TObject *Sender);
	void __fastcall ClearButtonClick(TObject *Sender);
	void __fastcall edtSymbolKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtPriceKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall LargeButtonClick(TObject *Sender);
	void __fastcall SmallButtonClick(TObject *Sender);
	void __fastcall RoundFormExLockIconClick(TObject *Sender);

private:	// User declarations
	TGraphButton*     				  FBtns[TABS];
	OrderFilterEnum   				  FFilters[TABS];
	int               				  FID;
	int               				  FFontSize;
	OrderFilterEnum   				  FFilter;
	String            				  FColumns;
	int                               FSelectedNID;
	nsOrderMessageDefine::MarketEnum  FSelectedMarket;
	UnicodeString                     FSelectedOrderID;
	UnicodeString                     FSelectedExchange;
	UnicodeString                     FSelectedSymbol;
	nsOrderMessageDefine::SideEnum    FSelectedSide;
	double                            FSelectedPx;
	int                               FSelectedQty;
	bool                              FPartialFill;
	String                            FProfile;
	void __fastcall  WndProc( TMessage &Msg );
	void __fastcall  ShowBalanceWeb( int i );
	void __fastcall  TAIFEXBalanceURL( String& URL );
	void __fastcall  TWSEBalanceURL( String& URL );
public:		// User declarations
	__fastcall TExecutionForm(TComponent* Owner );
	OrderFilterEnum	__fastcall GetFilterType( void );
	void __fastcall SetFilterType( OrderFilterEnum );
	OrderFilterEnum __fastcall SelectBtn(TObject *Sender);
public:		// User declarations
    void __fastcall LoadColor( void );
	void __fastcall LoadProperties( void );
	void __fastcall SaveProperties( void );
public:
	virtual TMDFormType __fastcall Type( void )            { return mdfExec; }
	virtual void        __fastcall SetActive( bool value ) { RoundFormEx->Active = value; }
	virtual bool        __fastcall IsActive( void )        { return RoundFormEx->Active; }
	virtual TForm*      __fastcall GetTForm( void )        { return this; }
	virtual int         __fastcall GetGroup( void )        { return this->Tag; }
	virtual void        __fastcall Release( void );
	virtual void        __fastcall SetVisible( bool IsVisible );
};
//---------------------------------------------------------------------------
#endif
