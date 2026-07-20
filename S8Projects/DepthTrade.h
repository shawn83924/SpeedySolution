//---------------------------------------------------------------------------

#ifndef DepthTradeH
#define DepthTradeH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "OrderBookList.h"
#include <Vcl.Grids.hpp>
#include "main.h"
#include <Vcl.ExtCtrls.hpp>
#include "ChartsStore.h"
#include "MarketDataStore.h"
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.WinXCtrls.hpp>
#include "RoundFormEx.h"
#include <Vcl.ComCtrls.hpp>
#include "GraphButton.h"
#include <Vcl.Buttons.hpp>
#include <Vcl.Menus.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "SettingOCODlg.h"
#include "OCODetail.h"
#include "GraphButtonV2.h"
//---------------------------------------------------------------------------
class TDepthForm : public TForm, public TClientForm, public TOrderQtyListener, public IOrderFilled
{
__published:	// IDE-managed Components
	TOrderBookList *OrderBookList;
	TPanel *pnlToolbar;
	TSplitView *ToolSV;
	TToggleSwitch *FillToggleSwitch;
	TLabel *FillLabel;
	TToggleSwitch *DrivedToggleSwitch;
	TLabel *DrivedPxLabel;
	TToggleSwitch *FillQtyToggleSwitch;
	TLabel *FillQtyLabel;
	TRoundFormEx *RoundFormEx;
	TPanel *ProfitPanel;
	TLabel *AvgPxTextLabel;
	TLabel *AvgPxLabel;
	TLabel *ProfitTextLabel;
	TLabel *ProfitLabel;
	TUpDown *LotsUpDown;
	TEdit *LotsPerOrderEdit;
	TLabel *CxlBeforeNewLabel;
	TToggleSwitch *CxlBeforeNewToggleSwitch;
	TGraphButton *SmallButton;
	TGraphButton *LargeButton;
	TGraphButton *SettingButton;
	TUpDown *StepUpDown;
	TEdit *StepEdit;
	TLabel *TickLabel;
	TEdit *StepCountEdit;
	TUpDown *StepCountUpDown;
	TLabel *CountLabel;
	TLabel *SliceOrderLabel;
	TToggleSwitch *SliceOrderSwitch;
	TStaticText *NetPosText;
	TStaticText *PosLabel;
	TGraphButton *ShowKBarButton;
	TLabel *MarketLabel;
	TToggleSwitch *MarketToggleSwitch;
	TPageControl *PageControl;
	TTabSheet *SettingTabSheet;
	TTabSheet *ColorTabSheet;
	TColorBox *FixRowColorBox;
	TColorBox *BuyTextColorBox;
	TColorBox *SellTextColorBox;
	TComboBox *BuyComboBox;
	TComboBox *FixRowComboBox;
	TComboBox *SellComboBox;
	TStaticText *StaticText1;
	TBevel *Bevel1;
	TStaticText *StaticText2;
	TStaticText *StaticText3;
	TBevel *Bevel2;
	TColorBox *TickColorBox;
	TComboBox *TickComboBox;
	TStaticText *StaticText4;
	TBevel *Bevel3;
	TSpeedButton *DefColorButton;
	TLabel *Label1;
	TToggleSwitch *AutoCenterSwitch;
	TSpeedButton *AsDefColorButton;
	TGraphButton *CopyDepthButton;
	TLabel *Label2;
	TComboBox *LotsComboBox;
	TTabSheet *StopSettingTabSheet;
	TSpeedButton *StopSettingBtn;
	TToggleSwitch *FilledStopToggleSwitch;
	TLabel *Label3;
	TToggleSwitch *StopToggleSwitch;
	TLabel *StopLabel;
	TBevel *Bevel5;
	TLabel *Label5;
	TLabel *Label6;
	TToggleSwitch *DelOnCloseToggleSwitch;
	TGroupBox *NuclearGroupBox;
	TUpDown *NuclearLotsUpDown;
	TEdit *Edit1;
	TLabel *Label7;
	TLabel *Label9;
	TEdit *Edit3;
	TUpDown *UpStepUpDown;
	TUpDown *UpCountUpDown;
	TEdit *Edit2;
	TLabel *Label8;
	TComboBox *RatioComboBox;
	TUpDown *DownCountUpDown;
	TEdit *Edit4;
	TLabel *Label10;
	TUpDown *DownStepUpDown;
	TEdit *Edit5;
	TLabel *Label11;
	TUpDown *ProfitTickUpDown;
	TEdit *Edit6;
	TLabel *Label12;
	TBevel *Bevel6;
	TBevel *Bevel7;
	TUpDown *ProfitStepUpDown;
	TEdit *Edit7;
	TLabel *Label13;
	TUpDown *ProfitCountUpDown;
	TEdit *Edit8;
	TLabel *Label14;
	TGraphButton *NormalButton;
	TTabSheet *TFTTabSheet;
	TGraphButton *LoanButton;
	TGraphButton *LoanShareButton;
	TGraphButton *DayTradeButton;
	TComboBox *TIFComboBox;
	TPanel *ButtonPanel;
	TLabel *CenterLabel;
	TLabel *MarketBuyLabel;
	TLabel *MarketSellLabel;
	TPanel *SplitPanel;
	TUpDown *SplitUpDown;
	TToggleSwitch *SplitToggleSwitch;
	TLabel *Label17;
	TLabel *Label16;
	TEdit *Edit9;
	TPanel *BullOutPanel;
	TToggleSwitch *BullOutToggleSwitch;
	TUpDown *BullOutOnceUpDown;
	TUpDown *BullOutAccumulateUpDown;
	TEdit *Edit15;
	TComboBox *BullOutAccumulateTimeComboBox;
	TEdit *Edit16;
	TLabel *Label26;
	TCheckBox *BullOutAccumulateCheckBox;
	TCheckBox *BullOutOnceCheckBox;
	TLabel *Label25;
	TPanel *FataOrdPanel;
	TLabel *FateTestResultLabel;
	TLabel *Label18;
	TButton *FateTestButton;
	TButton *FateTestCancelButton;
	TPanel *DepthPanel;
	TLabel *Label20;
	TPanel *BullInPanel;
	TLabel *Label27;
	TLabel *Label28;
	TToggleSwitch *BullInToggleSwitch;
	TUpDown *BullInOnceUpDown;
	TUpDown *BullInAccumulateUpDown;
	TEdit *Edit12;
	TComboBox *BullInAccumulateTimeComboBox;
	TEdit *Edit13;
	TCheckBox *BullInAccumulateCheckBox;
	TCheckBox *BullInOnceCheckBox;
	TImageList *BtnImageList;
	TButton *SplitExpBtn;
	TButton *BullInExpBtn;
	TButton *BullOutExpBtn;
	TButton *DepthExpBtn;
	TToggleSwitch *DepthCancelToggleSwitch;
	TListBox *SymbolListBox;
	TBevel *Bevel8;
	TLabel *Label19;
	TEdit *Edit10;
	TUpDown *BullOutTickUpDown;
	TLabel *Label21;
	TRadioGroup *AutoStopRadioGroup;
	TUpDown *StopTickUpDown;
	TEdit *StopTickEdit;
	TComboBox *StopPercentComboBox;
	TRadioGroup *AutoStopProfitRadioGroup;
	TEdit *StopProfitTickEdit;
	TUpDown *StopProfitTickUpDown;
	TComboBox *StopProfitPercentComboBox;
	TToggleSwitch *FilledStopProfitToggleSwitch;
	TTimer *StopLossTimer;
	TTimer *TakeProfitTimer;
	TComboBox *ExchangeComboBox;
	TScrollBox *SettingScrollBox;
	TScrollBox *StopSettingScrollBox;
	TScrollBox *ColorScrollBox;
	TPanel *SmartOrderTab;
	TPanel *ExtraPanel;
	TLabel *OrderByOneClickLabel;
	TToggleSwitch *OrderByOneClickSwitch;
	TLabel *CancelByRightClickLabel;
	TToggleSwitch *CancelByRightClickSwitch;
	TGraphButtonV2 *EscapeButton;
	TGraphButtonV2 *OCODetailBtn;
	TGraphButtonV2 *SettingOCOBtn;
	TShape *PnlOutlineShape;
	TShape *SmartOutlineShape;
	TGraphButtonV2 *OCOTabBtn;
	TGraphButtonV2 *QuickTabBtn;
	TShape *ExtraShape;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FillToggleSwitchClick(TObject *Sender);
	void __fastcall StopToggleSwitchClick(TObject *Sender);
	void __fastcall DrivedToggleSwitchClick(TObject *Sender);
	void __fastcall FillQtyToggleSwitchClick(TObject *Sender);
	void __fastcall imgMenuClick(TObject *Sender);
	void __fastcall RoundFormExIconClick(TObject *Sender);
	void __fastcall OrderBookListNewOrder(TObject *Sender, SideEnum side, double Price,
          int Qty);
	void __fastcall OrderBookListReduceQty(TObject *Sender, SideEnum side, double Price,
          int Qty);
	void __fastcall OrderBookListDeleteAllOrders(TObject *Sender, SideEnum side, const TDoubleDynArray &PriceArray,
          int &DelCount);
	void __fastcall OrderBookListReplacePx(TObject *Sender, SideEnum side, double Price,
          double NewPrice);
	void __fastcall OrderBookListNetPositionUpdate(TObject *Sender, int NetPosition,
          double Profitint, int BuyOpenQty, int SellOpenQty);
	void __fastcall OrderBookListProfitAndAvgPxUpdate(TObject *Sender, double NetPositionProfit,
          double AvgPx);
	void __fastcall EditKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall OrderBookListKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall EditMouseActivate(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y, int HitTest, TMouseActivate &MouseActivate);
	void __fastcall EditMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall CxlBeforeNewToggleSwitchClick(TObject *Sender);
	void __fastcall SmallButtonClick(TObject *Sender);
	void __fastcall LargeButtonClick(TObject *Sender);
	void __fastcall OrderBookListNewConditionOrder(TObject *Sender, SideEnum side, double Px,
		  OrderTypeEnum Type, double OrderPrice);
	void __fastcall OrderBookListDeleteStopOrder(TObject *Sender, SideEnum side, double StopPx);
	void __fastcall OrderBookListFocus(TObject *Sender);
	void __fastcall SliceOrderSwitchClick(TObject *Sender);
	void __fastcall ShowKBarButtonClick(TObject *Sender);
	void __fastcall OrderBookListNewMarketOrder(TObject *Sender, SideEnum side, int Qty);
	void __fastcall OrderBookListNewOrders(TObject *Sender, SideEnum side, TDoubleDynArray &PriceArray,
          int Qty);
	void __fastcall StepCountUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall StepUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall FixRowComboBoxChange(TObject *Sender);
	void __fastcall FixRowColorBoxChange(TObject *Sender);
	void __fastcall TickComboBoxChange(TObject *Sender);
	void __fastcall TickColorBoxChange(TObject *Sender);
	void __fastcall BuyComboBoxChange(TObject *Sender);
	void __fastcall BuyTextColorBoxChange(TObject *Sender);
	void __fastcall SellComboBoxChange(TObject *Sender);
	void __fastcall SellTextColorBoxChange(TObject *Sender);
	void __fastcall DefColorButtonClick(TObject *Sender);
	void __fastcall AutoCenterSwitchClick(TObject *Sender);
	void __fastcall AsDefColorButtonClick(TObject *Sender);
	void __fastcall CopyDepthButtonClick(TObject *Sender);
	void __fastcall StopSettingBtnClick(TObject *Sender);
	void __fastcall LotsComboBoxChange(TObject *Sender);
	void __fastcall OrderBookListCenterPx(TObject *Sender);
	void __fastcall StopTickUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall FilledStopToggleSwitchClick(TObject *Sender);
	void __fastcall NuclearLotsUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction);
	void __fastcall RatioComboBoxChange(TObject *Sender);
	void __fastcall RoundFormExLockIconClick(TObject *Sender);
	void __fastcall NormalButtonClick(TObject *Sender);
	void __fastcall DayTradeButtonClick(TObject *Sender);
	void __fastcall CenterLabelMouseEnter(TObject *Sender);
	void __fastcall CenterLabelMouseLeave(TObject *Sender);
	void __fastcall CenterLabelClick(TObject *Sender);
	void __fastcall ToolSVOpened(TObject *Sender);
	void __fastcall ToolSVClosed(TObject *Sender);
	void __fastcall MarketBuyLabelClick(TObject *Sender);
	void __fastcall MarketSellLabelClick(TObject *Sender);
	void __fastcall ExpBtnClick(TObject *Sender);
	void __fastcall FateTestButtonClick(TObject *Sender);
	void __fastcall FateTestCancelButtonClick(TObject *Sender);
	void __fastcall PageControlChanging(TObject *Sender, bool &AllowChange);
	void __fastcall OrderBookListTick(TObject *Sender, const AnsiString &time, double MatchPx,
          int MatchQty);
	void __fastcall DepthCancelToggleSwitchClick(TObject *Sender);
	void __fastcall SymbolListBoxData(TWinControl *Control, int Index, UnicodeString &Data);
	void __fastcall BullInToggleSwitchClick(TObject *Sender);
	void __fastcall BullOutToggleSwitchClick(TObject *Sender);
	void __fastcall SymbolListBoxClick(TObject *Sender);
	void __fastcall StopPercentComboBoxChange(TObject *Sender);
	void __fastcall AutoStopRadioGroupClick(TObject *Sender);
	void __fastcall OrderBookListPxAlarm(TObject *Sender);
	void __fastcall OrderBookListLowerPxAlarm(TObject *Sender);
	void __fastcall StopProfitTickUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction);
	void __fastcall OrderBookListAutoStopLoss(TObject *Sender, SideEnum side, double Price,
          int Qty);
	void __fastcall OrderBookListAutoTakeProfit(TObject *Sender, SideEnum side, double Price,
          int Qty);
	void __fastcall StopLossTimerTimer(TObject *Sender);
	void __fastcall TakeProfitTimerTimer(TObject *Sender);
	void __fastcall ExchangeComboBoxChange(TObject *Sender);
	void __fastcall EscapeButtonClick(TObject *Sender);
	void __fastcall ScrollBoxMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta,
		  TPoint &MousePos, bool &Handled);
	void __fastcall SettingOCOBtnClick(TObject *Sender);
	void __fastcall OrderBookListNewOCOOrder(TObject *Sender, SideEnum side, double Price);
	void __fastcall OrderBookListNewOCOFail(TObject *Sender, const AnsiString &ErrorMessage);
	void __fastcall OrderByOneClickSwitchClick(TObject *Sender);
	void __fastcall CancelByRightClickSwitchClick(TObject *Sender);
	void __fastcall OCODetailBtnClick(TObject *Sender);
	void __fastcall TabBtnClick(TObject *Sender);

private:	// Interface TOrderQtyListener
	virtual void OrderQtyChanged( MarketEnum Market, const String& Symbol, SideEnum Side, double Px, int Qty);
	virtual void StopOrderQtyChanged( nsOrderMessageDefine::MarketEnum Market,
									 const String& Symbol, double StopPx,
									 nsOrderMessageDefine::SideEnum Side,
									 double Px, int Qty,
									 nsOrderMessageDefine::OrderTypeEnum OrderType );
private:	// User declarations
	TStringList* FExList;
	TStringList* FSymList;
	BasicInformation* FSymInfo;
	TWinControl*      FParent;
	int               FDigit;
	nsOrderMessageDefine::MarketEnum FMarket;
	String FEx;
	String FSym;
	String FOldEx;
	String FOldSym;
	EventTypeEnum FCallPut;
	double FStrikePX;
	double FBullPx;
	DWORD FLastKeyDownTick;
	DWORD FKeyCxlStop;
	DWORD FKeyCxlAndClose;
	DWORD FKeyNuclearBuy;
	DWORD FKeyNuclearSell;
	int                   FLeft;
	int                   FTop;
	int FGroup;
	int FNetPos;
	int FFontSize;
	int FPostSizeOffset;
	int FNuclearUpperLots;
	int FNuclearLowerLots;
    int FRangeMarketOrderTick;
	UFC::PHashMap<TButton*,TPanel*>  FExpBtn2Panel;
	static UFC::List<BasicInformation*> FCustomMDList[];
private:	// User declarations
	bool FRegOrderStore;
	void __fastcall AdjustByExchange( const String& Ex );
	void __fastcall ChangeSymbol( void );
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall SetWidth( int Add );
	void __fastcall CancelAllAndClose( bool UsingStep );
	void __fastcall AdjuestFont( void );
	void __fastcall SetEditTag( void );
	void __fastcall OnPositionClosed( void );
	void __fastcall EnableStopAfterFilled( bool Enable );
	void __fastcall NuclearBuy( void );
	void __fastcall NuclearSell( void );
	void __fastcall UpdateNuclearLimit( void );
	void __fastcall OrderTypeStr( nsOrderMessageDefine::OrderTypeEnum ot, AnsiString& OTStr );
	void __fastcall NewStopOrder( SideEnum side, double Px, int Qty );
	void __fastcall AlignLabels( void );
	bool __fastcall CheckOrder( int Qty );
	void __fastcall UpdateCaption( void );
	void __fastcall ApplyStopTick( int New, int NewProfit );
	void __fastcall AutoStop( SideEnum side, double Price, int Qty, const String& Msg );
	void __fastcall InitExchangeComboBox( int Idx );
	void __fastcall InitSmartOrderTabs( TObject *Sender );
	void __fastcall EnableAllTabBtns( void );
	void __fastcall EnableQuickMode( void );
	void __fastcall DisableExtraPanelControls( void );
	void __fastcall EnableOCOMode( void );
public:		// User declarations
	bool __fastcall PlaceOrder( SideEnum side,double Price, int Qty,OrderTypeEnum OrderType, bool EnableStop );
	void __fastcall PlaceMarketOrder( SideEnum side, int Qty,TimeInForceEnum TIF );
private: ///< TFT Functions
	bool    FFateTesting;
	int     FBid1Qty;
	int     FAsk1Qty;
	double  FBid1Px;
	double  FAsk1Px;
	bool    FAutoCancelDepth;
	int     RegCount;
	unsigned int FBullInBeginTick;
	int          FBullInAccuQty;
	unsigned int FBullOutBeginTick;
	int          FBullOutAccuQty;
	void __fastcall SubscribeFull( void );
	void __fastcall UnsubscribeFull( void );
	void __fastcall LoadTFT( void );
	void __fastcall SaveTFT( void );
	void __fastcall SetExpBtnPanel( TButton* ExpBtn);
	void __fastcall LockUI( bool Locked );
	void __fastcall LockOtherPanel( bool Locked, TPanel* thisPanel );
	bool __fastcall GetExSymbol( String &Ex, String &Sym );
	void __fastcall CheckBullIn(  double MatchPx, int MatchQty );
	void __fastcall CheckBullOut(  double MatchPx, int MatchQty );
	void __fastcall UpdateSymbolListCount(void);
	void OnOrderFilled( const String &Exchange,const String &Symbol, const String &OrderID,SideEnum Side, double Px,int Qty, OrderStatusEnum OrderStatus);
public:		// User declarations
	String FProfile;
	__fastcall TDepthForm(TComponent* Owner);
	__fastcall ~TDepthForm( void );
	__fastcall TDepthForm(TWinControl* Owner, int Page, String , String );
	void __fastcall PrintStopSetting( void );
	void __fastcall LoadStopSetting( void );
	void __fastcall LoadHotkey( void );
	void __fastcall LoadColor( void );
	void __fastcall SaveStopSetting( void );
	void __fastcall SaveDefColor( void );
	void __fastcall SaveColorToConfig( const String& Name, bool DorL );
	void __fastcall LoadDefColor( bool DorL );
	void __fastcall ShowDepth( int L,int T );
	void __fastcall ShowDepth( int L,int T, const String& Profile  );
	const String& GetExchange( void ) { return FEx; }
	const String& GetSymbol( void ) { return FSym; }
	void __fastcall   ChangeSymbol( String Ex, String Sym );
	String __fastcall GetSymbolAlias( void ) { return FSymInfo->GetAliasName().c_str(); }
	void __fastcall SaveProperty( const String& Profile );
	bool __fastcall LoadProperty( const String& Profile );
	void __fastcall UpdateBalance( double Net );
	void __fastcall RegisterOrderStore( void );
	void __fastcall UnregisterOrderStore( void );
	void __fastcall CopyThis( TDepthForm* NewDepth );
	void __fastcall SelectSymbol( System::TObject* Sender, TSelectActionType Type, const String& Ex, const String& Sym );
	void __fastcall OnSymbolChanged( void );
	char __fastcall GetTWSEOrderType( SideEnum side );
	PositionEffectEnum   __fastcall GetPositionEffect( void );
	TimeInForceEnum      __fastcall GetTimeInForce( bool IsHotKey );
	bool __fastcall CanClose( void );
	void __fastcall OnOrderStoreReady( void );
    void __fastcall LoadCustomList(void);
private:
	SideEnum FStopLossSide;
	double   FStopLossPrice;
	int      FStopLossQty;
	SideEnum FTakeProfitSide;
	double   FTakeProfitPrice;
	int      FTakeProfitQty;
private:
	TColor __fastcall GetFixColor( int Index );
	TColor __fastcall GetTickColor( int Index );
	TColor __fastcall GetBuyColor( int Index );
	TColor __fastcall GetSellColor( int Index );
	void __fastcall   SetFixColor( int Index, TColor Color );
	void __fastcall   SetTickColor( int Index, TColor Color );
	void __fastcall   SetBuyColor( int Index, TColor Color );
	void __fastcall   SetSellColor( int Index, TColor Color );
	__property TColor FixRowColors[int Index ] = { read = GetFixColor,  write = SetFixColor };
	__property TColor TickColors[int Index ]   = { read = GetTickColor, write = SetTickColor };
	__property TColor BuyColors[int Index ]    = { read = GetBuyColor,  write = SetBuyColor };
	__property TColor SellColors[int Index ]   = { read = GetSellColor, write = SetSellColor };
public:
	virtual TMDFormType __fastcall Type( void )            { return mdfDepth; }
	virtual void        __fastcall SetActive( bool value ) { RoundFormEx->Active = value; }
	virtual bool        __fastcall IsActive( void )        { return RoundFormEx->Active; }
	virtual TForm*      __fastcall GetTForm( void )        { return this; }
	virtual int         __fastcall GetGroup( void )        { return FGroup; }
	virtual void        __fastcall Release( void );
	virtual void        __fastcall SetVisible( bool IsVisible );
};
//---------------------------------------------------------------------------
extern PACKAGE TDepthForm *DepthForm;
//---------------------------------------------------------------------------
#endif
