//---------------------------------------------------------------------------
#ifndef ChartFormH
#define ChartFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "StockChart.h"
#include "RoundFormEx.h"
#include <ExtCtrls.hpp>
#include "GraphButton.h"
#include "GraphPanel.h"
#include "GraphMarquee.h"
#include <ComCtrls.hpp>
#include <ImgList.hpp>
//#include <GestureMgr.hpp>
#include "CandleStickChart.h"
#include "ChartsStore.h"
#include <Menus.hpp>
#include "Stream.h"
#include <Dialogs.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
using namespace nsOrderMessageDefine;
//---------------------------------------------------------------------------
class TGraphButtonGroup
{
private:
	UFC::PtrList<TGraphButton> FGroup;
	TGraphButton* FSelected;
public:
	__fastcall TGraphButtonGroup();
	void __fastcall Add( TGraphButton* Btn );
	void __fastcall Select( TGraphButton* Btn );
	void __fastcall Select( int index );
	TGraphButton* __fastcall Selected( void );
	void __fastcall SetEnabled( bool enable );
};
//---------------------------------------------------------------------------
class TLineChartForm : public TForm, public IChartListener, public TClientForm, public TFilledListener
{
__published:	// IDE-managed Components
	TGraphPanel *SettingPanel;
	TGraphButton *KBar1Button;
	TGraphButton *KBar5Button;
	TGraphButton *KBar10Button;
	TGraphButton *KBar15Button;
	TGraphButton *KBar20Button;
	TGraphButton *ShowQtyButton;
	TGraphButton *MA1Button;
	TGraphButton *MA2Button;
	TGraphButton *MA3Button;
	TGraphButton *AutoZoomButton;
	TGraphButton *ZoomXButton;
	TGraphButton *ZoomYButton;
	TGraphButton *ZoomXYButton;
	TCandleStickChart *CandleStickChart;
	TRoundFormEx *RoundFormEx;
	TSplitView *ToolSV;
	TLabel *Label3;
	TTimer *RepaintTimer;
	TComboBox *PixelsComboBox;
	TLabel *PixelsLabel;
	TPaintBox *KBarPaintBox;
	TGraphButton *SettingButton;
	TImageList *EmojiImageList;
	TGraphButton *TradePointsButton;
	TGraphButton *CrossGraphButton;
	TGraphButton *HLineButton;
	TGraphButton *VLineButton;
	TGraphButton *LineButton;
	TGraphButton *DelButton;
	TColorBox *LineColorBox;
	TPageControl *PageControl;
	TTabSheet *SettingTabSheet;
	TTabSheet *AOETabSheet;
	TToggleSwitch *ShowBoxSwitch;
	TLabel *BOXLabel;
	TToggleSwitch *SkyLandToggleSwitch;
	TLabel *SkyLandLabel;
	TUpDown *PxGridUpDown;
	TEdit *PxGridEdit;
	TRadioGroup *PxGridRadioGroup;
	TToggleSwitch *PivotSwitch;
	TLabel *PivotLabel;
	TTrackBar *DeviationTrackBar;
	TLabel *PivotDevLabel;
	TComboBox *SectionTypeComboBox;
	TLabel *Label4;
	TToggleSwitch *DayHLSwitch;
	TLabel *Label2;
	TComboBox *SpaceComboBox;
	TLabel *Label5;
	TToggleSwitch *AutoCenterSwitch;
	TLabel *Label1;
	TColorBox *SectionLineColorBox;
	TSpeedButton *DefColorButton;
	TColorBox *MA3ColorBox;
	TLabel *MA3ColorLabel;
	TColorBox *MA2ColorBox;
	TLabel *MA2ColorLabel;
	TColorBox *MA1ColorBox;
	TLabel *MA1ColorLabel;
	TColorBox *BearColorBox;
	TLabel *BearColorLabel;
	TColorBox *BullColorBox;
	TLabel *BullColorLabel;
	TColorBox *CandleLineColorBox;
	TLabel *KLineColorLabel;
	TColorBox *GridColorBox;
	TLabel *GridColodLabel;
	TColorBox *ChartFGColorBox;
	TColorBox *ChartBKColorBox;
	TLabel *TextColorLabel;
	TLabel *BKColorLabel;
	TComboBox *Px3ComboBox;
	TUpDown *MA3KUpDown;
	TEdit *MA3KEdit;
	TLabel *MA3Label;
	TComboBox *Px2ComboBox;
	TUpDown *MA2KUpDown;
	TEdit *MA2KEdit;
	TLabel *MA2Label;
	TUpDown *MA1KUpDown;
	TEdit *MA1KEdit;
	TLabel *MA1Label;
	TComboBox *Px1ComboBox;
	TComboBox *K5ComboBox;
	TComboBox *K4ComboBox;
	TComboBox *K3ComboBox;
	TComboBox *K2ComboBox;
	TComboBox *K1ComboBox;
	TLabel *Label6;
	TImageList *LightImageList;
	TGraphButton *SentRoomiButton;
	TComboBox *TimeGridComboBox;
	TRadioGroup *RadioGroup1;
	TGraphButton *CopyDepthButton;
	TToggleSwitch *AvgPxSwitch;
	TLabel *Label7;
	TLabel *AtAOELabel;
	TCheckBox *OrdConfirmCheckBox;
	TLabel *Label8;
	TSpeedButton *StopSettingBtn;
	TLabel *Label9;
	TToggleSwitch *TradingSwitch;
	TGraphButton *TextButton;
	TGraphButton *SelectButton;
	TCheckBox *HLSectionBox;
	void __fastcall SymbolButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall KBar1ButtonClick(TObject *Sender);
	void __fastcall ShowQtyButtonClick(TObject *Sender);
	void __fastcall MA1ButtonClick(TObject *Sender);
	void __fastcall MA2ButtonClick(TObject *Sender);
	void __fastcall MA3ButtonClick(TObject *Sender);
	void __fastcall ZoomXYButtonClick(TObject *Sender);
	void __fastcall RoundFormExIconClick(TObject *Sender);
	void __fastcall imgMenuClick(TObject *Sender);
	void __fastcall MA1KUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall MA2KUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall MA3KUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall RepaintTimerTimer(TObject *Sender);
	void __fastcall KBarPaintBoxPaint(TObject *Sender);
	void __fastcall PixelsComboBoxChange(TObject *Sender);
	void __fastcall MA1KEditMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall MA1KEditChange(TObject *Sender);
	void __fastcall MA2KEditChange(TObject *Sender);
	void __fastcall MA3KEditChange(TObject *Sender);
	void __fastcall Px1ComboBoxChange(TObject *Sender);
	void __fastcall Px2ComboBoxChange(TObject *Sender);
	void __fastcall Px3ComboBoxChange(TObject *Sender);
	void __fastcall DefColorButtonClick(TObject *Sender);
	void __fastcall ChartBKColorBoxChange(TObject *Sender);
	void __fastcall ChartFGColorBoxChange(TObject *Sender);
	void __fastcall GridColorBoxChange(TObject *Sender);
	void __fastcall CandleLineColorBoxChange(TObject *Sender);
	void __fastcall BullColorBoxChange(TObject *Sender);
	void __fastcall BearColorBoxChange(TObject *Sender);
	void __fastcall MA1ColorBoxChange(TObject *Sender);
	void __fastcall MA2ColorBoxChange(TObject *Sender);
	void __fastcall MA3ColorBoxChange(TObject *Sender);
	void __fastcall ShowBoxSwitchClick(TObject *Sender);
	void __fastcall DayHLSwitchClick(TObject *Sender);
	void __fastcall SectionLineColorBoxChange(TObject *Sender);
	void __fastcall SectionTypeComboBoxChange(TObject *Sender);
	void __fastcall PxGridEditChange(TObject *Sender);
	void __fastcall PxGridUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall PxGridRadioGroupClick(TObject *Sender);
	void __fastcall TradePointsButtonClick(TObject *Sender);
	void __fastcall CrossGraphButtonClick(TObject *Sender);
	void __fastcall EditButtonClick(TObject *Sender);
	void __fastcall LineColorBoxChange(TObject *Sender);
	void __fastcall RoundFormExLockIconClick(TObject *Sender);
	void __fastcall SpaceComboBoxChange(TObject *Sender);
	void __fastcall K1ComboBoxChange(TObject *Sender);
	void __fastcall SkyLandToggleSwitchClick(TObject *Sender);
	void __fastcall PivotSwitchClick(TObject *Sender);
	void __fastcall DeviationTrackBarChange(TObject *Sender);
	void __fastcall SentRoomiButtonClick(TObject *Sender);
	void __fastcall TimeGridComboBoxChange(TObject *Sender);
	void __fastcall CopyDepthButtonClick(TObject *Sender);
	void __fastcall AvgPxSwitchClick(TObject *Sender);
	void __fastcall CandleStickChartDelete(TObject *Sender, SideEnum side, double Price);
	void __fastcall CandleStickChartReplacePx(TObject *Sender, SideEnum side, double Price,
          double NewPrice);
	void __fastcall CandleStickChartNewOrder(TObject *Sender, SideEnum side, double Price);
	void __fastcall CandleStickChartDeleteStopOrder(TObject *Sender, SideEnum side,
          double Price);
	void __fastcall OrdConfirmCheckBoxClick(TObject *Sender);
	void __fastcall AtAOELabelClick(TObject *Sender);
	void __fastcall CandleStickChartStopOrderReplacePx(TObject *Sender, SideEnum side, nsOrderMessageDefine::OrderTypeEnum Type,
		  int Qty, double Price, double NewPrice,double ExecPx);
	void __fastcall TradingSwitchClick(TObject *Sender);
	void __fastcall StopSettingBtnClick(TObject *Sender);
	void __fastcall CandleStickChartNewStopOrder(TObject *Sender, SideEnum side, double Px,
          OrderTypeEnum Type, double ExecPrice);
	void __fastcall CandleStickChartClosePosition(TObject *Sender, SideEnum side, int Pos);
	void __fastcall CandleStickChartNewText(TObject *Sender, TText *NewText, bool &Accept);
	void __fastcall HLSectionBoxClick(TObject *Sender);




private:	// User declarations
	int                   FLeft;
	int                   FTop;
	TIntegerDynArray      FLights;
	UFC::AnsiString       FUniqueKey;
	String                FAliasSymbol;
	String                FSymbol;
	String                FExchange;
	String			      FDisplayName;
	MarketEnum            FMarket;
	EventTypeEnum         FCallPut;
	double                FStrikePX;
	int                   FFontSize;
	int                   FWhichStore;
	bool                  FDefaultDay;
	bool                  FFOKMarket;
	int                   FVLine1X;
	int                   FVLine2X;
	int                   FVLine3X;
	int                   FVLine4X;
	int                   FVLine5X;
	int                   FRefIntPx;
	int                   FSecPerGrid;
	bool                  FFirstResponse;
	bool                  FShowBSFields;
	bool                  FSymbolChanging;
	bool                  FRecoverFinished;
	bool                  FSkyLandMustOff;
	bool                  FSignalInit;
	DWORD                 FLastKeyDownTick;
	DWORD                 FLastUpdate;
	TNotifyEvent          FOnChartFormClosed;
	int					  FStrikePxDigi;
	int                   FDecimalLocator;
	int                   FPixelRate;
	double                FRefPrice;
	bool                  FIsTXFMXF;

	TStringList*          FSymbolComboBoxExchange;
	TStringList*          FSymbolComboBoxSymbol;
	int					  FKMinutes;
	UFC::List<TGraphButton*>  FKMinkeys;
	UFC::List<TComboBox*>  FKMinComboBoxs;
	void __fastcall RandomData( void );
	nsOrderMessageDefine::PositionEffectEnum CheckPositionEffect( void );
	void __fastcall ListResize(  void );
	void __fastcall ShowChart( bool Show );
	void __fastcall GetSymbolProperties( BasicInformation* Info );
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall UpdatePixels5Tick( void );
	void __fastcall SelectSymbol( System::TObject* Sender, TSelectActionType Type, const String& Ex, const String& Sym );
	void __fastcall EnableColorSetting( bool IsEnable );
	void __fastcall FindSkyLand( int HasDataDay, double *H, double *L, double *c );
	double __fastcall GetDeviation( void );
	bool __fastcall PlaceOrder( SideEnum side, double Price, int Qty,OrderTypeEnum OrderType );
public:
	TGraphButtonGroup FEditTools;
	TGraphButtonGroup FZoomTools;
	void __fastcall SetEditMode( bool EditMode );
	void __fastcall LoadKBarSetting( void );
	void __fastcall SaveKBarSetting( void );
public:
	virtual void OnLastKBar(  TMinuteKInfo& KBar );
	virtual void OnRecoverKBars( UFC::List<TMinuteKInfo>& KBars );
	virtual void OnDayKBars(  int Index, double MaxPx, double MinPx, UFC::PStringList& Date, UFC::List<TMinuteKInfoList*>& KBars );
	virtual void OnLastTick( TTickInfo& Tick );
	virtual void OnRecoverTicks( UFC::List<TTickInfo>& Ticks );
public: /// TFilledListener
	virtual void OnRecoverFilled( const String& Ex,const String& Sym, UFC::List<TFilledData>& Filleds );
	virtual void OnFilled( const String& Ex,const String& Sym, TFilledData& Filled );
private:
	void __fastcall RemoveFilledListener( const String& Ex,const String& Sym );
public:		// User declarations
	__fastcall TLineChartForm(TComponent* Owner);
	void __fastcall RequestGraph( String Exchange, String Symbol );
	void __fastcall LoadStopSetting( void );
	void __fastcall SaveStopSetting( void );
	void __fastcall LoadColor( void );
	void __fastcall SaveColor( void );
	void __fastcall LoadSetting( void );
	void __fastcall AOEFunction( const String& Exchange, const String& Symbol );
	void __fastcall LoadProperties( const String& Profile );
	void __fastcall SaveProperties( const String& Profile );
	bool __fastcall SaveAskOrderConfirm( bool NewVal );
	const String& __fastcall GetExchange( void ) { return FExchange; }
	const String& __fastcall GetSymbolAlias( void ) { return FAliasSymbol; }
	__property TNotifyEvent OnChartFormClosed = {read = FOnChartFormClosed, write = FOnChartFormClosed};
public:
	virtual TMDFormType __fastcall Type( void )            { return mdfKBar; }
	virtual void        __fastcall SetActive( bool value ) { RoundFormEx->Active = value; }
	virtual bool        __fastcall IsActive( void )        { return RoundFormEx->Active; }
	virtual TForm*      __fastcall GetTForm( void )        { return this; }
	virtual int         __fastcall GetGroup( void )        { return this->Tag; }
	virtual void        __fastcall Release( void );
   	virtual void        __fastcall SetVisible( bool IsVisible );
};
//---------------------------------------------------------------------------
//extern PACKAGE TLineChartForm *LineChartForm;
//---------------------------------------------------------------------------
#endif
