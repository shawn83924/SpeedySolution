//---------------------------------------------------------------------------

#ifndef TBarFormH
#define TBarFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "GraphPanel.h"
#include "OptionsStrikePriceView.h"
#include "RoundFormEx.h"
#include "FMFHeader.h"
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "GraphButton.h"
#include <Menus.hpp>
#include "OrderMessageDefine.h"
#include <Tabs.hpp>
#include "ContractForm.h"
#include <Vcl.Buttons.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
//---------------------------------------------------------------------------
using namespace nsOrderMessageDefine;
//---------------------------------------------------------------------------
class TOptionsTBarForm : public TForm, public TClientForm
{
__published:	// IDE-managed Components
	TGraphPanel *GraphPanel;
	TComboBox *OptionsSerialComboBox;
	TComboBox *MaturityComboBox;
	TOptionsStrikePriceView *OptionsStrikePriceView;
	TCheckBox *InvertCheckBox;
	TGraphButton *LargeButton;
	TGraphButton *SmallButton;
	TTabSet *OptMarketTabSet;
	TLabel *BKLabel;
	TComboBox *BKComboBox;
	TGraphButton *SettingButton;
	TRoundFormEx *RoundFormEx;
	TSplitView *ToolSV;
	TSpeedButton *DefColorButton;
	TPageControl *SettingPageControl;
	TTabSheet *ColorTabSheet;
	TColorBox *TextColorBox;
	TLabel *TextLabel;
	TLabel *Label1;
	TColorBox *BKColorBox;
	TLabel *Label7;
	TLabel *Label6;
	TLabel *Label5;
	TLabel *Label4;
	TLabel *Label3;
	TColorBox *EqualPxColorBox;
	TColorBox *FallColorBox;
	TColorBox *RisePxColorBox;
	TColorBox *PutBKColorBox;
	TColorBox *CallBKColorBox;
	TLabel *Label9;
	TLabel *Label8;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TColorBox *StkPxBKColorBox;
	TColorBox *StkPxColorBox;
	TColorBox *BKBarColorBox;
	TColorBox *VGridColorBox;
	TColorBox *HGridColorBox;
	TTabSheet *PinsTabSheet;
	TListView *FPList;
	TPanel *StrikePxTopPanel;
	TComboBox *PercentComboBox;
	TLabel *StrikePxLabel;
	TLabel *StrikePxPercentLabel;
	TTimer *ReloadPinsTimer;
	TButton *SelectPinsButton;
	TButton *ApplyButton;
	TButton *CancelButton;
	TImageList *ImageList;
	TColorBox *HeaderTextColorBox;
	TColorBox *HeaderBKColorBox;
	TLabel *Label2;
	TLabel *Label13;
	TLabel *InvertLabel;
	void __fastcall OptionsSerialComboBoxChange(TObject *Sender);
	void __fastcall MaturityComboBoxChange(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall InvertCheckBoxClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall LargeButtonClick(TObject *Sender);
	void __fastcall SmallButtonClick(TObject *Sender);
	void __fastcall OptionsStrikePriceViewTBarStrikePxMouseClick(TObject *Sender, UnicodeString Symbol,
		  CallPutCode CallPut);
	void __fastcall OptMarketTabSetChange(TObject *Sender, int NewTab, bool &AllowChange);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall BKComboBoxChange(TObject *Sender);
	void __fastcall SettingButtonClick(TObject *Sender);
	void __fastcall SelectPinsButtonClick(TObject *Sender);
	void __fastcall ApplyButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FPListDrawItem(TCustomListView *Sender, TListItem *Item, TRect &Rect,
          TOwnerDrawState State);
	void __fastcall FPListCustomDrawSubItem(TCustomListView *Sender, TListItem *Item,
          int SubItem, TCustomDrawState State, bool &DefaultDraw);
	void __fastcall FPListMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall TextColorBoxChange(TObject *Sender);
	void __fastcall BKColorBoxChange(TObject *Sender);
	void __fastcall CallBKColorBoxChange(TObject *Sender);
	void __fastcall PutBKColorBoxChange(TObject *Sender);
	void __fastcall RisePxColorBoxChange(TObject *Sender);
	void __fastcall FallColorBoxChange(TObject *Sender);
	void __fastcall EqualPxColorBoxChange(TObject *Sender);
	void __fastcall HGridColorBoxChange(TObject *Sender);
	void __fastcall VGridColorBoxChange(TObject *Sender);
	void __fastcall BKBarColorBoxChange(TObject *Sender);
	void __fastcall StkPxColorBoxChange(TObject *Sender);
	void __fastcall StkPxBKColorBoxChange(TObject *Sender);
	void __fastcall DefColorButtonClick(TObject *Sender);
	void __fastcall HeaderTextColorBoxChange(TObject *Sender);
	void __fastcall HeaderBKColorBoxChange(TObject *Sender);
	void __fastcall OptionsStrikePriceViewTBarFieldsChanged(TObject *Sender);
	void __fastcall InvertLabelClick(TObject *Sender);

private:	// User declarations
	int  FID;
	int  FFontSize;
	int  FDepthFormIndex;
	bool FIsInit;
	TStringList*      FOptExchanges;
	TStringList*      FOptSerial;
	void __fastcall   AdjuestFont( void );
	void __fastcall   AdjuestWidth( void );
	void __fastcall   InitTBar( void );
	String __fastcall GetMaturityString( void );
	void __fastcall   SetMaturityComboBox( const String& Exchange, const String& SerialName );
	void __fastcall   GenExcludeSet( UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxSet );
	void __fastcall   ClearExcludeSet( UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxList );
	void __fastcall   GenStrikePxSet( UFC::PHashedSet<double>* StrikePxSet, const String& StrikePxString );
	void __fastcall   WndProc( TMessage &Msg );
	void __fastcall   InitExchange( int ExIndex );
	void __fastcall   InitSerial( int ExIndex, int SerialIndex );
	void __fastcall   ListWndProc( TMessage &Msg );
	void __fastcall   EraseBK( HDC DC );
private:	// User declarations
	TWndMethod FOrigWindowProc;
	TCanvas*   FEarseBKCanvas;
	bool FIsAll;
	int  FStrikePXDigi;
	UFC::PStringList FYearMonth;
	UFC::PDoubleList FStrikePx;
	UFC::PDoubleList FSpotPx;
public:
	void __fastcall Init( bool IsAll, int Digi, UFC::PStringList& YM, UFC::PDoubleList& StrikePx, UFC::PDoubleList& SpotPx,	UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& Exclude  );
	void __fastcall SavePinsSetting( void );
	void __fastcall LoadPinsSetting( void );
	void __fastcall UpdateOptionsStrikePriceViewPins( void );
	void __fastcall ApplyToColorBox( void );
public:		// User declarations
	__fastcall TOptionsTBarForm(TComponent* Owner, int );
	void __fastcall SetTBarFormID( int ID );
	void __fastcall LoadProperties( const String& Profile );
	void __fastcall SaveProperties( const String& Profile );
public:
	virtual TMDFormType __fastcall Type( void )            { return mdfList; }
	virtual void        __fastcall SetActive( bool value ) { RoundFormEx->Active = value; }
	virtual bool        __fastcall IsActive( void )        { return RoundFormEx->Active; }
	virtual TForm*      __fastcall GetTForm( void )         { return this; }
	virtual int         __fastcall GetGroup( void )        { return this->Tag; }
	virtual void        __fastcall Release( void );
    virtual void        __fastcall SetVisible( bool IsVisible );
};
//---------------------------------------------------------------------------
extern PACKAGE TOptionsTBarForm *OptionsTBarForm;
//---------------------------------------------------------------------------
#endif
