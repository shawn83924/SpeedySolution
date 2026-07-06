//---------------------------------------------------------------------------

#ifndef ContractFormH
#define ContractFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "GraphButton.h"
#include "GraphPanel.h"
#include "RoundFormEx.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include "MarketDataStore.h"
#include "UFC.h"
#include <Menus.hpp>
#include <ImgList.hpp>
#include <Tabs.hpp>
#include <Buttons.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
#define EXCHANGE_MAX 32  ///< Maxinum supported exchange.
#define TYPE_MAX     10  ///< Maxinum supported product type.(Index,Stock,FX...)
//---------------------------------------------------------------------------
class TSymbolItem;
//---------------------------------------------------------------------------
class TContractInfoForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TTabSheet *FutTabSheet;
	TTabSheet *StockTabSheet;
	TDrawGrid *FutDrawGrid;
	TPopupMenu *PopupMenu;
	TMenuItem *ShowDepthMenuItem;
	TMenuItem *AddToMDListMenuItem;
	TMenuItem *CloseDepthMenuItem;
	TMenuItem *N11;
	TMenuItem *N21;
	TMenuItem *N31;
	TMenuItem *N41;
	TMenuItem *N51;
	TMenuItem *ChartMenuItem;
	TTabSet *MarketTabSet;
	TTimer *ExchangeChangeTimer;
	TTreeView *ContractTreeView;
	TListBox *ResultListBox;
	TEdit *SearchEdit;
	TTabSheet *OptTabSheet;
	TDrawGrid *OptDrawGrid;
	TComboBox *SerialComboBox;
	TTabSet *OptMarketTabSet;
	TTimer *OptExchangeChangeTimer;
	TPanel *FutToolPanel;
	TComboBox *TypeComboBox;
	TPanel *OptToolPanel;
	TRoundFormEx *RoundFormEx;
	TGraphButton *FutTabButton;
	TPanel *TopPanel;
	TGraphButton *OptTabButton;
	TGraphButton *StockTabButton;
	TImageList *CheckImageList;
	TGraphButton *SearchGlyphButton;
	TEdit *FutSerachEdit;
	TGraphButton *SearchFutButton;
	void __fastcall FutDrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall SerialComboBoxChange(TObject *Sender);
	void __fastcall OptDrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OptDrawGridDblClick(TObject *Sender);
	void __fastcall FutDrawGridDblClick(TObject *Sender);
	void __fastcall ShowDepthMenuItemClick(TObject *Sender);
	void __fastcall CloseDepthMenuItemClick(TObject *Sender);
	void __fastcall AddToMDListMenuItemClick(TObject *Sender);
	void __fastcall FutDrawGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall OptDrawGridMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall FutDrawGridClick(TObject *Sender);
	void __fastcall OptDrawGridClick(TObject *Sender);
	void __fastcall ChartMenuItemClick(TObject *Sender);
	void __fastcall MarketTabSetChange(TObject *Sender, int NewTab, bool &AllowChange);
	void __fastcall ExchangeChangeTimerTimer(TObject *Sender);
	void __fastcall SearchEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall SearchGlyphButtonClick(TObject *Sender);
	void __fastcall ResultListBoxContextPopup(TObject *Sender, TPoint &MousePos, bool &Handled);
	void __fastcall ResultListBoxDblClick(TObject *Sender);
	void __fastcall ContractTreeViewEditing(TObject *Sender, TTreeNode *Node, bool &AllowEdit);
	void __fastcall ResultListBoxClick(TObject *Sender);
	void __fastcall OptMarketTabSetChange(TObject *Sender, int NewTab, bool &AllowChange);
	void __fastcall OptExchangeChangeTimerTimer(TObject *Sender);
	void __fastcall ContractTreeViewClick(TObject *Sender);
	void __fastcall ResultListBoxData(TWinControl *Control, int Index, UnicodeString &Data);
	void __fastcall ResultListBoxDataObject(TWinControl *Control, int Index, TObject *&DataObject);
	void __fastcall TypeComboBoxChange(TObject *Sender);
	void __fastcall FutTabButtonClick(TObject *Sender);
	void __fastcall OptTabButtonClick(TObject *Sender);
	void __fastcall StockTabButtonClick(TObject *Sender);
	void __fastcall SearchEditMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall SearchFutButtonClick(TObject *Sender);
	void __fastcall FutSerachEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FutSerachEditMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall FutDrawGridContextPopup(TObject *Sender, TPoint &MousePos, bool &Handled);
	void __fastcall OptDrawGridContextPopup(TObject *Sender, TPoint &MousePos, bool &Handled);


private:
	String STR_FUTURES;
	String STR_OPTIONS;
	String STR_STOCK;
	String STR_WARRANT;
	String STR_PUT_WARRANT;
	String STR_CALL;
	String STR_PUT;
private:	// User declarations
	class TOptionsSerial
	{
	public:
		TStringList* StrikePxs;
		TStringList* Maturity;
	public:
		TOptionsSerial( void )
		:StrikePxs( new TStringList() )
		,Maturity( new TStringList() )
		{

		}
		~TOptionsSerial( void )
		{
			delete StrikePxs;
			delete Maturity;
		}
	};
	int          FFutPageWidth;
	int          FOptPageWidth;
	int          FFutPageHeight;
	int          FOptPageHeight;
	bool         FPicker;
	bool         FSupportFutures;
	bool         FSupportOptions;
	bool         FSupportStock;
	bool         FInit;
	bool         FChanging;
	String       FPickExchange;
	String       FPickSymbol;
	int TypeIndex( TFutProdType T )
	{
		switch( T )
		{
			case fptIndex: return 0;
			case fptInterestRate: return 1;
			case fptBond: return 2;
			case fptCommodity: return 3;
			case fptStock: return 4;
			case fptFX: return 5;
			default : return 0;
		}
	} ;
private:
	TStringList* FOptExchangeCode; ///< Options Exchange code.
	TStringList* FOptChineseSerials[EXCHANGE_MAX];
	UFC::PHashedList<UFC::AnsiString,TOptionsSerial*>  FOptionsSerialList[EXCHANGE_MAX];
private: ///< for search
	TStringList* FDisplayList;
	TStringList* FSearchResult;
	TStringList* FSearchKeys;
private:
	int FFutSelPage;
	int FFutSelIndex;
private:
	TStringList* FFutExchangeCode;
	TStringList* FFutSerials[EXCHANGE_MAX][TYPE_MAX];
	TStringList* FChineseSerials[EXCHANGE_MAX][TYPE_MAX];
	TStringList* FMaturityDate[EXCHANGE_MAX][TYPE_MAX];
	int          FFutRowCount[EXCHANGE_MAX][TYPE_MAX];
	int          FFutColCount[EXCHANGE_MAX][TYPE_MAX];

	UFC::PHashedSet<UFC::AnsiString>  FFutSymSet[TYPE_MAX];
	UFC::PHashedSet<UFC::AnsiString>  FFutMaturitySet[TYPE_MAX];

	UFC::PHashedSet<int>                              FOptSelectSet;
	UFC::PHashedSet<int>                              FFutSelectSet;
	UFC::PHashedList<UFC::AnsiString,UFC::AnsiString*> FSymbolList;
	int __fastcall InitFuturesExchangeTab( const UFC::AnsiString& Exchange );
	int __fastcall InitOptionsExchangeTab( const UFC::AnsiString& Exchange );
	void __fastcall OpenOptionsDepth( int Col, int Row, bool Open, bool NewIfExist  );
	void __fastcall OpenFuturesDepth( int Col, int Row, bool Open, bool NewIfExist   );
	void __fastcall AdjuestFutSize( int );
	void __fastcall AdjuestOptSize( int );
	void __fastcall ToFuturesSymbol( int Col, int Row, UFC::AnsiString& Exchange, UFC::AnsiString& Symbol );
	void __fastcall ToOptionsSymbol( int Col, int Row, UFC::AnsiString& Exchange, UFC::AnsiString& Symbol );
	bool __fastcall IsFutSelectedCell( int Col, int Row, TGridDrawState& State );
	bool __fastcall IsOptSelectedCell( int Col, int Row, TGridDrawState& State );
	void __fastcall DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor UpCol, TColor DownCol );
	void __fastcall AddOptionsSymbol( int Index, BasicInformation* Info, const UFC::AnsiString& Symbol );
	void __fastcall AddFuturesSymbol( int Index, BasicInformation* Info );
	void __fastcall AddExchangeSymbols( const UFC::AnsiString& Exchange );
    bool __fastcall IsTWStockOption( BasicInformation* Info );
	void __fastcall AddStockNode( String Exchange );
	void __fastcall SortTAIFEXFutSerials( int Index );
	void __fastcall SortTAIFEXOptSerials( int Index );
	void __fastcall WndProc( TMessage &Msg );
	TSymbolItem* __fastcall AddContractNode( TTreeNode* ParentNode, BasicInformation* Info );
	inline TTreeNode* __fastcall AddNode( TTreeNode* ParentNode, const String& Name, int ImgIndex );
	TTreeNode* __fastcall GetProdNode( UFC::PHashedList<UFC::AnsiString, TTreeNode*>& TickerSymbolTable, TTreeNode *StockNode, BasicInformation* Info );
	void __fastcall SelectrFutSerials( int Page, int Index );
	void __fastcall FutDrawGridRightMouseDown(int Col, int Row, int Key );
    void __fastcall OptDrawGridRightMouseDown(int Col, int Row, int Key );
public:		// User declarations
	__fastcall TContractInfoForm( TComponent* Owner );
	__fastcall TContractInfoForm( TComponent* Owner, bool ForSelect );
	void __fastcall SwitchPage( int index );
	void __fastcall AddSymbol( TLabel* StatusLabel );
	TStringList*  __fastcall OptExchanges( void ){ return FOptExchangeCode; }
	TStringList*  __fastcall OptSerils( int Exchange ){ return FOptChineseSerials[ Exchange ]; }
public:
	void __fastcall SaveProperties( const String& Profile );
	void __fastcall LoadProperties( const String& Profile );
	void __fastcall LoadCustomNames( void );

	bool __fastcall SupportFutures(void){return FSupportFutures; }
	bool __fastcall SupportOptions(void){return FSupportOptions; }
	bool __fastcall SupportStock(void)  {return  FSupportStock;  }
public: ///< For Select contract dialog
	static bool __fastcall SelectSymbol( String& Exchange /* out */, String& Symbol /* out */, int Page = 0 );
	static bool __fastcall SelectSymbol( String OrigExchange, String OrigSymbol, String& Exchange, String& Symbol );
};
//---------------------------------------------------------------------------
extern PACKAGE TContractInfoForm *ContractInfoForm;
extern PACKAGE TContractInfoForm *SelectContractForm;
//---------------------------------------------------------------------------
#endif
