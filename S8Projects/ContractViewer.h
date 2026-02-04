//---------------------------------------------------------------------------
#ifndef ContractViewerH
#define ContractViewerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
#include "OrderBookList.h"
#include "MarketDataStore.h"
#include "MarketDataConnection.h"
#include "DepthTrade.h"
#include "GraphButton.h"
//---------------------------------------------------------------------------
class TLineChartForm;
class TContractListForm;
class TExecutionForm;
//---------------------------------------------------------------------------
class TSymbolItem : public TObject
{
public:
	TTreeNode*        ProdNode;
	BasicInformation* Info;
	String            DispName;
	String  	      ChineseName;
	String            Symbol;
public:
	__fastcall TSymbolItem( TTreeNode* pNode, BasicInformation* sym )
	:ProdNode( pNode )
	,Info( sym )
	,Symbol( Info->GetSymbol().c_str() )
	{
		if( Info->GetMarket() == ::mTSE || Info->GetMarket() == ::mOTC)
		{
			UFC::AnsiString UTF8ChineseName( Info->GetChineseName() );
			ChineseName = UTF8ToString( UTF8ChineseName.c_str() );
		}
		else
			ChineseName = Info->GetDisplayName();

		DispName.printf(L"[%s] %s", String( Info->GetSymbol().c_str()) ,ChineseName );
	}

	__fastcall TSymbolItem( const String& Name )
	:ProdNode( NULL )
	,Info( NULL )
	,Symbol( Name )
	,ChineseName( Name)
	,DispName( Name)
	{
	}

	__fastcall TSymbolItem( BasicInformation* sym )
	:ProdNode( NULL )
	,Info( sym )
	,Symbol( Info->GetSymbol().c_str() )
	{
		if( Info->GetMarket() == ::mTSE || Info->GetMarket() == ::mOTC)
		{
			UFC::AnsiString UTF8ChineseName( Info->GetChineseName() );
			ChineseName = UTF8ToString( UTF8ChineseName.c_str() );
		}
		else
			ChineseName = Info->GetDisplayName();

		DispName.printf(L"[%s] %s", String( Info->GetSymbol().c_str()) ,ChineseName );
	}
	bool __fastcall IsContract( void ) { return (Info!=NULL); }
};
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TOnSelectSymbolEvent)(System::TObject* Sender, TSelectActionType Type, const String& Ex, const String& Sym );
//---------------------------------------------------------------------------
class TContractViewerForm : public TForm
{
__published:	// IDE-managed Components
	TImageList *ImageList;
	TPopupMenu *PopupMenu;
	TMenuItem *ShowDepthMenuItem;
	TMenuItem *CloseDepthMenuItem;
	TMenuItem *ChartMenuItem;
	TMenuItem *AddToMDListMenuItem;
	TMenuItem *Custom1MenuItem;
	TMenuItem *Custom2MenuItem;
	TMenuItem *Custom3MenuItem;
	TMenuItem *Custom4MenuItem;
	TMenuItem *Custom5MenuItem;
	TImageList *IconImageList;
	TEdit *SearchEdit;
	TListBox *ResultListBox;
	void __fastcall ContractTreeViewEditing(TObject *Sender, TTreeNode *Node, bool &AllowEdit);
	void __fastcall ShowDepthMenuItemClick(TObject *Sender);
	void __fastcall CloseDepthMenuItemClick(TObject *Sender);
	void __fastcall ChartMenuItemClick(TObject *Sender);
	void __fastcall AddToMDListMenuItemClick(TObject *Sender);

	void __fastcall AddTAIFEX( String Exchange );
	void __fastcall AddStock( String Exchange );
	void __fastcall SearchGlyphButtonClick(TObject *Sender);
	void __fastcall SearchEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ResultListBoxDblClick(TObject *Sender);
	void __fastcall ResultListBoxContextPopup(TObject *Sender, TPoint &MousePos, bool &Handled);
	void __fastcall ResultListBoxData(TWinControl *Control, int Index, UnicodeString &Data);
	void __fastcall ResultListBoxDataObject(TWinControl *Control, int Index, TObject *&DataObject);
	void __fastcall ResultListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
		  TOwnerDrawState State);
	void __fastcall ResultListBoxMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall ResultListBoxMouseLeave(TObject *Sender);
	void __fastcall ResultListBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall ResultListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);

private:
	String STR_FUTURES;
	String STR_OPTIONS;
	String STR_STOCK;
	String STR_WARRANT;
	String STR_CALL;
	String STR_PUT;
private:	// User declarations
	TCMarketDataStore*                                         CMarketDataStore;
private:	// User declarations
	TStringList*                                               FTWSESearchKeys;
	TStringList*                                               FOTCSearchKeys;
	TStringList*                                               FTWSEWarrantSearchKeys;
	TStringList*                                               FOTCWarrantSearchKeys;
	TStringList*                                               FTAIFEXFuturesSearchKeys;
	TStringList*                                               FTAIFEXOptionsSearchKeys;
	TStringList*                                               FResult;
	Graphics::TBitmap*                                         FBufferBmp;
	TOnSelectSymbolEvent                                       FOnSelectSymbol;
    bool                                                       FTBarOpened;
	void __fastcall Search( const String& SearchStr, TStringList *SearchKeys );
private:	// User declarations
	UFC::List<TClientForm*>                                    FFormList;
	UFC::List<TDepthForm*>                                     FDepthForms;
	UFC::PHashedList<UFC::AnsiString, UFC::List<TDepthForm*>*> FSymbolFormMap;
	bool __fastcall CheckSymbol( const String& Ex, String& Sym );
	void __fastcall OnClientClose( TObject *Sender, TCloseAction &Action );
	void __fastcall OnDepthFormClose(  TDepthForm* DelForm );
	void __fastcall ActiveNextClient( int Group );
	void __fastcall ClientPos( TMDFormType Type, int Group, int& L, int& T );
	int  __fastcall ClientCount( TMDFormType Type, int Group );
	int  __fastcall ClientCount( TMDFormType Type );
	TContractListForm*  __fastcall FirstContractListForm( int Group );
	TExecutionForm*  __fastcall FirstExecutionForm( int Group );
	TExecutionForm*  __fastcall FirstExecutionForm( void );
	void __fastcall AddCatalog( void );
	void __fastcall ClearList( TStringList* ClearKeys );
public:		// User declarations
	__fastcall TContractViewerForm(TComponent* Owner);
public:		// User declarations
	void __fastcall AddSymbols( TLabel* StatusLabel  );
	void __fastcall ClearSymbols( void );
	void __fastcall CloseDepthForms( void );
	void __fastcall EnableDepthForms( bool IsEnable );
	void __fastcall SaveProperties( void );
	void __fastcall LoadProperties( void );
	void __fastcall LoadHotkey( TDepthForm* Who );
	void __fastcall UpdateContract( const String& Exchange );
	void __fastcall EnableStepOrder( bool IsEnable );
	void __fastcall EnableDelBeforeNew( bool IsEnable );
	void __fastcall EnableAutoLots( bool IsEnable );
	void __fastcall UpdateBalance( const String& Ex, const String& Sym, double Balance );
private:
	void __fastcall AddToMap( String Exchange, String Symbol, UFC::List<TDepthForm*>* AddList );
	void __fastcall AddToMap( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UFC::List<TDepthForm*>* AddList );
	void __fastcall RemoveFromMap( String Exchange, String Symbol, UFC::List<TDepthForm*>* DelList );
	void __fastcall RemoveFromMap( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UFC::List<TDepthForm*>* DelList );
	TDepthForm* __fastcall DepthFormInPage( UFC::List<TDepthForm*>* ListPtr,  int Page );
	bool __fastcall IsListExists( String Exchange, String Symbol, int Page = -1 );
	bool __fastcall IsListExists( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, int Page = -1 );
	UFC::List<TDepthForm*>* __fastcall SearchListFromMap( String Exchange, String Symbol );
	UFC::List<TDepthForm*>* __fastcall SearchListFromMap( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
	TDepthForm* __fastcall NewDepthForm( TWinControl* Owner, int Page, UFC::List<TDepthForm*>* DepthFormList, AnsiString& Exchange, AnsiString& Symbol );
public:		// User declarations
	///< Close all Depth from for given Exchange and Symbol.
	void __fastcall CloseOrderBookForm( String Exchange, String Symbol );
	///< Open a Depth from for given Exchange and Symbol.
	TDepthForm* __fastcall OpenOrderBookForm( String Exchange, String Symbol,  bool NewIfExist = false, int Page = -1 );
	///< Center filled price
	void __fastcall CenterPx( TObject *Sender );
	///< Change depth from symbool.
	void __fastcall  ChangeSymbol( const String& OrigEx, const String& OrigSymbol, const String& NexEx, const String& NewSymbol, TDepthForm* );
	bool __fastcall  IsDepthViewOpen( const String& Exchange, const String& Symbol );
	int  __fastcall  OpenTBarOrderBookForm( String Exchange, String Symbol, int Index );
	void  __fastcall OpenTBarForm( void );
	void  __fastcall OpenExecutionForm( void );
	void  __fastcall CloseExecutionForm( void );
	TContractListForm*  __fastcall OpenMarketDataListForm( int Page = -1, bool Load = false );
	TLineChartForm*     __fastcall OpenKBarForm(  const String& Ex, const String& Sym, int Page = -1 );
	void __fastcall AddCustomSymbol( int CustomListIndex, const String& Exchange, const String& Symbol );
	void __fastcall CloseAllForm( void );
	void __fastcall CenterPx( TDepthForm* Who );
	void __fastcall ActiveClient( TClientForm* ActForm );
	void __fastcall LoadDepthColor( TDepthForm* Who );
	void __fastcall LoadDepthOCO( TDepthForm* Who );
	void __fastcall LoadStopSetting( TClientForm* Who );
	void __fastcall OrderStoreReady( void );
	void __fastcall LoadChartColor( TLineChartForm* Who );
	void __fastcall LoadChartKSetting( TLineChartForm* Who );
	void __fastcall LoadContractListColor( TContractListForm* Who );
	void __fastcall LoadCustomList( TContractListForm* Who, int Index );
	void __fastcall LoadCustomName( TContractListForm* Who );
	void __fastcall LoadCustomNames( void );
	void __fastcall ShowPage( int Page );
	void __fastcall RegisterOrderStore( void );
	void __fastcall UnregisterOrderStore( void );
public:
	__property TOnSelectSymbolEvent  OnSelectSymbol = { read = FOnSelectSymbol, write = FOnSelectSymbol };
};
//---------------------------------------------------------------------------
extern PACKAGE TContractViewerForm *ContractViewerForm;
//---------------------------------------------------------------------------
#endif
