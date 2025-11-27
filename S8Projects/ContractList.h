//---------------------------------------------------------------------------
#ifndef ContractListH
#define ContractListH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "MarketDataList.h"
#include "MarketDataStore.h"
#include "RoundFormEx.h"
#include <ComCtrls.hpp>
#include "GraphPanel.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "OrderMessageDefine.h"
#include "GraphButton.h"
#include <ImgList.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
using namespace nsOrderMessageDefine;
//------------------------------------------------------------------------------
const int CUSTOM_LIST_COUNT = 5;
const int LIST_FONT_SIZE_MIN = 9;
const int LIST_FONT_SIZE_DEF = 11;
const int LIST_FONT_SIZE_MAX = 28;
//---------------------------------------------------------------------------
class TContractListForm : public TForm, public TClientForm
{
__published:	// IDE-managed Components
	TMarketDataList *MarketDataList;
	TGraphPanel     *GraphPanel;
	TComboBox       *ExchangeComboBox;
	TComboBox       *SerialComboBox;
	TPopupMenu      *PopupMenu;
	TMenuItem       *DelSymbolMenuItem;
	TMenuItem       *OpenDepthMenuItem;
	TGraphButton    *SortButton;
	TGraphButton    *SmallButton;
	TGraphButton    *LargeButton;
	TRoundFormEx *RoundFormEx;
	TSplitView *ToolSV;
	TTimer *FitTimer;
	TColorBox *FallBKColorBox;
	TColorBox *RiseBKColorBox;
	TColorBox *EvenBKColorBox;
	TColorBox *OddBKColorBox;
	TColorBox *TextColorBox;
	TLabel *Label2;
	TLabel *Label1;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TSpeedButton *DefColorButton;
	TMenuItem *OpenKMenuItem;
	TPopupMenu *CustomPopupMenu;
	TMenuItem *CustomNameMenuItem;
	TLabel *Label6;
	TColorBox *SymbolColorBox;
	void __fastcall SerialComboBoxChange(TObject *Sender);
	void __fastcall ExchangeComboBoxChange(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall MarketDataListMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y);
	void __fastcall OpenDepthMenuItemClick(TObject *Sender);
	void __fastcall DelSymbolMenuItemClick(TObject *Sender);
	void __fastcall LargeButtonClick(TObject *Sender);
	void __fastcall SmallButtonClick(TObject *Sender);
	void __fastcall MarketDataListItemPosChanged(TObject *Sender);
	void __fastcall SortButtonClick(TObject *Sender);
	void __fastcall FitTimerTimer(TObject *Sender);
	void __fastcall ColorBoxChange(TObject *Sender);
	void __fastcall DefColorButtonClick(TObject *Sender);
	void __fastcall OpenKMenuItemClick(TObject *Sender);
	void __fastcall CustomNameMenuItemClick(TObject *Sender);
	void __fastcall CustomPopupMenuPopup(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
	int FID;
	int FFontSize;
	int FLeft;
	int FTop;
	String FColumns;
	TStringList* FFutExchanges;
	static UFC::List<BasicInformation*> FCustomMDList[];
	void __fastcall ForceSortSymbol( const String& Exchange, const String& SerieName, int Place );
	void __fastcall ForceSortSymbol( const String& Exchange );
	void __fastcall ClearSerialComboBox( void );
	void __fastcall AddSymbols( String Exchange, UFC::PStringList& Symbols, bool Sort  );
	void __fastcall ChangeToPage( int ExchangeIndex, int SerialIndex );
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall SaveColor( void );
	void __fastcall DefaultColor( void );
    void __fastcall OnColumnClick(TObject* Sender, TListColumn* Column);
public:		// User declarations
	__fastcall TContractListForm(TComponent* Owner, int ID );
	void __fastcall SetContractListFormID( int ID );
	void __fastcall SaveCustomList( const String& Profile );
	void __fastcall LoadCustomList( const String& Profile );
	void __fastcall SaveProperties( const String& Group );
	void __fastcall LoadProperties( const String& Group );
	void __fastcall AddSymbol( int index, String Exchange, String Symbol );
	void __fastcall RemoveSymbol( int index, String Exchange, String Symbol );
	void __fastcall RemoveAllSymbols( void );
	void __fastcall ToCustomList( int index );
	void __fastcall LoadColor( void );
	void __fastcall LoadCustomList( int Index );
	void __fastcall LoadCustomNames( void );
	void __fastcall UpdateContract( const String& SDExchange );
	void __fastcall CustomListMode( bool Custom );
	void __fastcall Init( const String& Exchange, int ProdID );
	int  __fastcall GetID( void ) { return FID; }
public:
	virtual TMDFormType __fastcall Type( void )            { return mdfList; }
	virtual void        __fastcall SetActive( bool value ) { RoundFormEx->Active = value; }
	virtual bool        __fastcall IsActive( void )        { return RoundFormEx->Active; }
	virtual TForm*      __fastcall GetTForm( void )         { return this; }
	virtual int         __fastcall GetGroup( void )        { return this->Tag; }
	virtual void        __fastcall Release( void );
   	virtual void        __fastcall SetVisible( bool Visible );
};
//---------------------------------------------------------------------------
extern PACKAGE TContractListForm *ContractListForm;
//---------------------------------------------------------------------------
#endif
