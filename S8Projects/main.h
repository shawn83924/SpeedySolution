//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "MarketDataStore.h"
#include "MarketDataList.h"
#include <Vcl.ComCtrls.hpp>
#include "ChartsStore.h"
#include "CandleStickChart.h"
#include "OptionsStrikePriceView.h"
#include <Vcl.Grids.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "OrderStore.h"
#include "OrderBookList.h"
#include <Vcl.WinXCtrls.hpp>
#include <System.Actions.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.CategoryButtons.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.WinXPanels.hpp>
#include <Vcl.Buttons.hpp>
#include "RoundFormEx.h"
#include "GraphButton.h"
#include <Vcl.Graphics.hpp>
#include "RoundFormExRes.h"
#include "GraphPanel.h"
#include <Vcl.AppEvnts.hpp>
#include "FMFHeader.h"
#include <sapi.h>
#include <Vcl.FileCtrl.hpp>
#include <Vcl.Tabs.hpp>
#include <Vcl.Menus.hpp>
#include "SHDocVw_OCX.h"
#include <Vcl.OleCtrls.hpp>
#include <SHDocVw.hpp>
#include <Xml.XMLDoc.hpp>
#include <System.JSON.hpp>
#include "CAChecker.h"
//#include "ContractViewer.h"
//---------------------------------------------------------------------------
class	TBrokerConfig;
class	TSpeedyConfig;
//---------------------------------------------------------------------------
const int RTT_WIDTH = 100;
//---------------------------------------------------------------------------
class IOrderFilled
{
public:
	virtual void OnOrderFilled( const String &Exchange,const String &Symbol, const String &OrderID,SideEnum Side, double Px,int Qty, OrderStatusEnum OrderStatus) = 0;
};
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TCMarketDataStore *CMarketDataStore;
	TImageList *imlIcons;
	TActionList *MainActionList;
	TAction *actProducts;
	TAction *actDepth;
	TAction *actTBar;
	TSplitView *SV;
	TCategoryButtons *catMenuItems;
	TAction *actSetting;
	TAction *actSettingHotkey;
	TAction *actList;
	TPageControl *PageControl;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TAction *actKBar;
	TImageList *ToolImageList;
	TGraphButton *SubscribeButton;
	TGraphButton *EditButton;
	TPanel *BackgroundPanel;
	TSplitView *SettingSV;
	TPageControl *SettingPageControl;
	TTabSheet *TabNormal;
	TLabel *Label3;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *NormalSettingLabel;
	TComboBox *FillSoundComboBox;
	TComboBox *RejectSoundComboBox;
	TComboBox *NewOrdSoundComboBox;
	TRadioButton *DarkRadioButton;
	TRadioButton *LightRadioButton;
	TTabSheet *TabHotKey;
	TLabel *BuyLabel;
	TLabel *SellLabel;
	TLabel *BuyMktLabel;
	TLabel *SellMktLabel;
	TLabel *IncLabel;
	TLabel *DecLabel;
	TLabel *CxAllLabel;
	TLabel *CxAllStopLabel;
	TLabel *CxCloseAllLabel;
	TLabel *HotkeySettingLabel;
	TComboBox *KeyCodeComboBox;
	TRoundFormExRes *RoundFormExRes;
	TAction *actLogout;
	TAction *actExec;
	TAction *actReport;
	TAction *actLogin;
	TImageList *TabImageList;
	TTabSheet *BrokerTabSheet;
	TLabel *IDLabel;
	TEdit *IDEdit;
	TLabel *PasswordLabel;
	TEdit *PasswordEdit;
	TImageList *BrokerImageList;
	TComboBoxEx *BrokerComboBoxEx;
	TLabel *StatusLabel;
	TPanel *TabsPanel;
	TTabSheet *ProdTabSheet;
	TApplicationEvents *ApplicationEvents;
	TLabel *CenterPxLabel;
	TComboBox *CloseAllStepComboBox;
	TLabel *Label34;
	TRoundFormExRes *RoundFormExResX;
	TGraphButton *Tab1Button;
	TGraphButton *Tab2Button;
	TGraphButton *Tab3Button;
	TGraphButton *CloseButton;
	TGraphButton *MaxButton;
	TGraphButton *MinButton;
	TGraphButton *ExButton;
	TImageList *LogoImageList;
	TImageList *UserImageList;
	TEdit *SearchEditBox;
	TGraphButton *SearchButton;
	TPopupMenu *TabPopupMenu;
	TMenuItem *RenameItem;
	TGraphButton *Tab4Button;
	TGraphButton *Tab5Button;
	TTabSheet *TabSheet4;
	TTabSheet *TabSheet5;
	TTimer *RTTTimer;
	TTabSheet *WebTabSheet;
	TTimer *PreventIdleTimer;
	TWebBrowser *WebBrowser;
	TImageList *GrayTabImageList;
	TGraphButton *QuiteButton;
	TPanel *NormalSettingPanel;
	TBevel *Bevel1;
	TBevel *Bevel2;
	TBevel *Bevel3;
	TBevel *Bevel4;
	TBevel *Bevel5;
	TBevel *Bevel6;
	TGraphButton *NewOrdSoundButton;
	TGraphButton *RejectSoundButton;
	TLabel *Label6;
	TGraphButton *FillSoundButton;
	TPanel *HotkeySettingPanel;
	TBevel *Bevel7;
	TBevel *Bevel8;
	TBevel *Bevel9;
	TBevel *Bevel10;
	TBevel *Bevel11;
	TBevel *Bevel12;
	TGraphButton *GraphButton1;
	TBevel *Bevel13;
	TGraphButton *NewConfirmSwitch;
	TGraphButton *CancelConfirmSwitch;
	TGraphButton *NewSoundSwitch;
	TGraphButton *RejectSoundSwitch;
	TGraphButton *FillSoundSwitch;
	TGraphButton *CAButton;
	TGraphButton *LoginButton;
	TGraphButton *CancelButton;
	TLabel *LoginLabel;
	TPanel *LoginPanel;
	TBevel *Bevel15;
	TBevel *Bevel16;
	TComboBoxEx *LinkComboBoxEx;
	TImageList *HKBtnImageList;
	TGraphButton *BuyText;
	TGraphButton *BuyMarketText;
	TGraphButton *SellText;
	TGraphButton *SellMarketText;
	TGraphButton *IncBetterText;
	TGraphButton *DecBetterText;
	TGraphButton *CancelAllText;
	TGraphButton *CancelAllStopText;
	TGraphButton *CancelAllandCloseText;
	TGraphButton *CenterPxStaticText;
	TTrayIcon *TrayIcon;
	TPopupMenu *TrayPopupMenu;
	TMenuItem *RestoreItem;
	TMenuItem *CloseItem;
	TGraphButton *BuyHistroyButton;
	TTimer *PurchaseTimer;
	TLabel *NuclearBuyLabel;
	TGraphButton *NuclearBuyText;
	TGraphButton *NuclearSellText;
	TLabel *NuclearSellLabel;
	TAction *actInfo;
	TTabSheet *TabSysInfo;
	TLabel *Label7;
	TPanel *Panel1;
	TLabel *FreeMemLabel;
	TLabel *CPUClockLabel;
	TLabel *OSLabel;
	TLabel *SUVerLabel;
	TLabel *MemLabel;
	TBevel *Bevel17;
	TBevel *Bevel18;
	TBevel *Bevel19;
	TBevel *Bevel20;
	TBevel *Bevel21;
	TBevel *Bevel22;
	TLabel *MarketDataLabel;
	TGraphButton *InfoOKButton;
	TLabel *TradeServerLabel;
	TLabel *NetworkLabel;
	TBevel *Bevel23;
	TLabel *GWIPLabel;
	TLabel *ChartServerLabel;
	TGraphButton *IMButton;
	TLabel *Label8;
	TComboBox *StopSoundComboBox;
	TGraphButton *StopSoundButton;
	TGraphButton *StopSoundSwitch;
	TBevel *Bevel14;
	TTimer *CancelAllTimer;
	TGraphButton *CxlWorkingButton;
	TTimer *LogonReadyTimer;
	TLabel *Label9;
	TGraphButton *PxAlarmSoundButton;
	TComboBox *PxAlarmSoundComboBox;
	TGraphButton *PxAlarmSoundSwitch;
	TTimer *FormShowTimer;
	TTabSheet *TabSheetLoading;
	TProgressBar *LoadingProgressBar;
	TLabel *LoadingStaticText;
	TPanel *ProgressPanel;
	TLabel *MDSourceLabel;
	TLabel *Label10;
	TBevel *Bevel24;
	TGraphButton *LowPxAlarmSoundButton;
	TComboBox *LowPxAlarmSoundComboBox;
	TGraphButton *LowPxAlarmSoundSwitch;
	void __fastcall CMarketDataStoreAppConnected(TObject *Sender);
	void __fastcall CMarketDataStoreContractDownloadCompleted(int Count, int UseMS);
	void __fastcall ChartsStoreXAppConnected(TObject *Sender);
	void __fastcall OrderStoreConnect(TObject *Sender);
	void __fastcall OrderStoreDisconnect(TObject *Sender);
	void __fastcall OrderStoreLogonFailed(TObject *Sender, const UnicodeString &ReplyMessage,
          int CID);
	void __fastcall OrderStoreLogonOK(TObject *Sender, const UnicodeString &ReplyMessage,
          int CID);
	void __fastcall imgMenuClick(TObject *Sender);
	void __fastcall actProductsExecute(TObject *Sender);
	void __fastcall actSettingExecute(TObject *Sender);
	void __fastcall actSettingHotkeyExecute(TObject *Sender);
	void __fastcall actDepthExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall actTBarExecute(TObject *Sender);
	void __fastcall actListExecute(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall CMarketDataStoreAppDisconnected(TObject *Sender);
	void __fastcall KeyCodeComboBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall KeyCodeComboBoxMouseEnter(TObject *Sender);
	void __fastcall KeyCodeComboBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall KeyCodeComboBoxChange(TObject *Sender);
	void __fastcall BuyTextClick(TObject *Sender);
	void __fastcall TabHotKeyMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall actKBarExecute(TObject *Sender);
	void __fastcall ChartsStoreXAppDisconnected(TObject *Sender);
	void __fastcall ExitButtonClick(TObject *Sender);
	void __fastcall OnNCHitTest( TMessage &Message );
	void __fastcall actLoginExecute(TObject *Sender);
	void __fastcall actExecExecute(TObject *Sender);
	void __fastcall LoginButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall Tab1ButtonClick(TObject *Sender);
	void __fastcall actReportExecute(TObject *Sender);
	void __fastcall SearchBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall SearchBoxInvokeSearch(TObject *Sender);
	void __fastcall ApplicationEventsDeactivate(TObject *Sender);
	void __fastcall BuyTextMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall actLogoutExecute(TObject *Sender);
	void __fastcall BrokerComboBoxExChange(TObject *Sender);
	void __fastcall NewOrdSoundButtonClick(TObject *Sender);
	void __fastcall RejectSoundButtonClick(TObject *Sender);
	void __fastcall FillSoundButtonClick(TObject *Sender);
	void __fastcall OrderStoreNewOrder(TObject *Sender, const UnicodeString &Exchange,
		  const UnicodeString &Symbol, SideEnum Side, double Px,
		  int Qty, OrderTypeEnum OrderType, TimeInForceEnum TimeInForce, PositionEffectEnum PositionEffect,
		  bool &IsAccept);
	void __fastcall OrderStoreRejectedReply(TObject *Sender);
	void __fastcall OrderStoreNewOrderReply(TObject *Sender, const UnicodeString &OrderID,
		  const UnicodeString &Symbol, SideEnum Side, double Px,
		  int Qty, OrderTypeEnum OrderType, TimeInForceEnum TimeInForce, PositionEffectEnum PositionEffect);
	void __fastcall OrderStoreCancelByOrderID(TObject *Sender, const UnicodeString &OrderID,
		  bool &IsAccept);
	void __fastcall OrderStoreFilledReply(TObject *Sender,
		  const UnicodeString &Exchange,const UnicodeString &Symbol,const UnicodeString &OrderID, SideEnum Side,double Px,int Qty, OrderStatusEnum OrderStatus);
	void __fastcall OrderStoreFloatingProfit( TObject *Sender,
											  const String& ExchangeCode, const String& Symbol,
											  void *PositionStatisticRecordPtr);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall NewOrdSoundComboBoxChange(TObject *Sender);
	void __fastcall OrderStoreBeginRecover(TObject *Sender);
	void __fastcall OrderStoreRecoverFinished(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall CloseButtonClick(TObject *Sender);
	void __fastcall MaxButtonClick(TObject *Sender);
	void __fastcall MinButtonClick(TObject *Sender);
	void __fastcall SearchEditBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Tab2ButtonMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall CMarketDataStoreMarketDataServerAck(int RTT);
	void __fastcall RTTTimerTimer(TObject *Sender);
	void __fastcall RenameItemClick(TObject *Sender);
	void __fastcall EditButtonClick(TObject *Sender);
	void __fastcall SubscribeButtonClick(TObject *Sender);
	void __fastcall PreventIdleTimerTimer(TObject *Sender);
	void __fastcall WebBrowserBeforeNavigate2(TObject *ASender, IDispatch * const pDisp,
          const OleVariant &URL, const OleVariant &Flags, const OleVariant &TargetFrameName,
          const OleVariant &PostData, const OleVariant &Headers,
          WordBool &Cancel);
	void __fastcall QuiteButtonClick(TObject *Sender);
	void __fastcall NewConfirmSwitchClick(TObject *Sender);
	void __fastcall LightRadioButtonClick(TObject *Sender);
	void __fastcall CAButtonClick(TObject *Sender);
	void __fastcall RestoreItemClick(TObject *Sender);
	void __fastcall CloseItemClick(TObject *Sender);
	void __fastcall WebBrowserNavigateComplete2(TObject *ASender, IDispatch * const pDisp,
          const OleVariant &URL);
	void __fastcall BuyHistroyButtonClick(TObject *Sender);
	void __fastcall PurchaseTimerTimer(TObject *Sender);
	void __fastcall CMarketDataStoreLeaderBoard(TStringList *Board);
	void __fastcall FormConstrainedResize(TObject *Sender, int &MinWidth, int &MinHeight,
          int &MaxWidth, int &MaxHeight);
	void __fastcall actInfoExecute(TObject *Sender);
	void __fastcall InfoOKButtonClick(TObject *Sender);
	void __fastcall IMButtonClick(TObject *Sender);
	void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall StopOrderChange( TObject* Sender, const String& Exchange, const String& StopSymbol, double StopPrice,
									const String& Symbol, double Price, int Qty,
									nsOrderMessageDefine::TimeInForceEnum TimeInForce,
									nsOrderMessageDefine::SideEnum Side,
									StopOrderStatusEnum StopStatus, bool& IsAccept);
	void __fastcall StopSoundButtonClick(TObject *Sender);
	void __fastcall CxlWorkingButtonClick(TObject *Sender);
	void __fastcall CancelAllTimerTimer(TObject *Sender);
	void __fastcall LogonReadyTimerTimer(TObject *Sender);
	void __fastcall PxAlarmSoundButtonClick(TObject *Sender);
	void __fastcall FormShowTimerTimer(TObject *Sender);
	void __fastcall TabSheetLoadingResize(TObject *Sender);
	void __fastcall LowPxAlarmSoundButtonClick(TObject *Sender);
public:
	TChartsStore *ChartsStore;
	TOrderStore  *OrderStore;
	ISpVoice*     FVoice;
	TCAChecker*   FCAChecker;
private:
	UFC::List<TGraphButton*>  FHotkeys;
	UFC::List<TGraphButton*>  FTabs;
	TGraphButton*             FSelectedFuncText;
	String                   FSoundDir;
	Graphics::TBitmap*       FBKBuffer;
	Graphics::TBitmap*       FStatusBuffer;
	Graphics::TPicture* 	 FUserImg;
	TPoint*                  FMDPolygonPoints;
	TList*                   FMDRTTDataList;
	int                      FMDRTT;
	TRect                    RTTGraphRect;
	String                   FLoginUserStr;
	TBrokerConfig*           FLoginBroker;
	TSpeedyConfig*  		 FSpeedyCfg;
	AnsiString               FID;
	AnsiString               FPassword;
	bool FDownloadOK;
    bool FForceClose;
	bool FProxyLogon;
	bool FRecovering;
	bool FUILoaded;
	bool FConnectFailed;
	bool FRequestWeb;
	int  FWebFunc;
	int  FFutAccIndex;
	int  FTseAccIndex;
    double FNetBalance;
	void __fastcall PaintStatusBar( void );
	void __fastcall PaintMDRTT( TCanvas* canvas, TRect& RTTGraph );
	HRGN __fastcall ExcludeRect( HRGN CaptionRgn, TRect EraseRect );
	HRGN __fastcall CreateCaptionDrawRgn( void );
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall DrawNCArea( HRGN Rgn  );
	void __fastcall OnNCCalcSize( TMessage &Message );
	void __fastcall OnNCPaint( TMessage &Message );
	void __fastcall OnNCActive( TMessage &Message );
	void __fastcall PaintBk( TMessage &Msg );
	void __fastcall CloseAll( void );
	bool __fastcall IsHotkeyDup( const String& NewHotkey );
	void __fastcall DefaultSymbol( String& Ex, String& Sym );
	int  __fastcall KeyStringToIndex( const String& KeyStr );
	void __fastcall LoadWavesSetting( void );
	void __fastcall SaveWavesSetting( void );
	void __fastcall LoadWaves( void );
	void __fastcall LoadTabName( void );
	void __fastcall SaveTabName( void );
	void __fastcall ShowTabs( bool Show );
	void __fastcall LiveUpdate( void );
	void __fastcall CACheckFail( TCAChecker* Sender, UnicodeString& Reason );
	void __fastcall PrintLogHeader( void );
	void __fastcall LoadIDPassword( int BrokerN );
	void __fastcall SaveIDPassword( int BrokerN );
	void __fastcall OSNotSupport( const AnsiString& OSStr );
	void __fastcall LoginSimBroker( void );
	void __fastcall CheckAgreement( void );
	void __fastcall SetIE11( void );
	void __fastcall ControlPosition( void );
	void __fastcall EnableNuclear( void );
private:	// User declarations
	String  FCopyRights;
	bool __fastcall Support( const UnifyProductKey& key, int& YYYYMMDD );
	void __fastcall EnableUnifyLicense( bool Enable );
	float __fastcall ProcSpeedCalc();
public:
	UFC::PHashMap<String,UnifyProductInfo*>  FPurchaseList;
	UFC::PHashMap<String,UnifyProductInfo*>  FProductList;
	void __fastcall UnifyLicense( void );
	void __fastcall ClearProductInfo( void );
	void __fastcall UseLastLicense( UFC::PHashMap<String,UnifyProductInfo*>& InfoList, const String& ID, const String& ExpDate );
private:
	void __fastcall SelectSymbol( TObject* Sender, TSelectActionType Type, const String& Ex, const String& Sym );
	void __fastcall SystemInfo( void );
	void __fastcall UpdateAutoCancel( void );
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
	__fastcall ~TMainForm( void );
	bool __fastcall SettingPanel( bool Open );
	void __fastcall Connect( void );
	bool __fastcall Ready( void );
	bool __fastcall ConnectionFailed( void ) { return FConnectFailed; }
	void __fastcall LoadBrokers( void );
	void __fastcall LoadMainFormPosition( void );
	void __fastcall LoadProperties( void );
	void __fastcall SaveProperties( void );
	void __fastcall LoadHotkey( void );
	void __fastcall SaveHotkey( void );
	void __fastcall Speak( const String& Str );
	void __fastcall SetPage( int Page );
	int __fastcall  CloseBetterTick( void );
	void __fastcall InitOrderStore( void );
	void __fastcall LoadImage( const String& ImgFile );
	void __fastcall HandleTokenExpired( void );
	bool __fastcall UserRightsInfo( void );
	void __fastcall LoadUserInfo( TJSONObject *lpRoot );
	void __fastcall UpdateAccount( bool IsFut );
	__property AnsiString LoginID = { read = FID };
	__property AnsiString LoginPassword = { read = FPassword };
	void __fastcall LoadIDPassword(String id, String password);
private:
    bool FLoadPositionOK;
	UFC::PCriticalSection FFillSubLock;
	UFC::PHashMap<String,UFC::List<IOrderFilled*>*>  FFulledMap;
	void __fastcall  TriggerFill( const String &Exchange, const String &Symbol, const String &OrderID, SideEnum Side, double Px,int Qty, OrderStatusEnum OrderStatus);
	void __fastcall SetChartBKColor(bool checkLightButton);
public:
	void __fastcall SubscribeFilled( const String &Exchange,const String &Symbol, IOrderFilled* Listener );
	bool __fastcall UnsubscribeFilled( const String &Exchange,const String &Symbol, IOrderFilled* Listener );
public:
	int __fastcall GetPositionCount( void );
	void __fastcall GetPositionSymbol( int i , String& Ex,String& Sym,String& Disp );
	bool __fastcall HasPosition( const String& Ex, const String& Sym );
	bool __fastcall EnablePxAlarm( void ) { return PxAlarmSoundSwitch->Selected; }
	bool __fastcall EnableLowPxAlarm( void ) { return LowPxAlarmSoundSwitch->Selected; }
	void __fastcall LoadingProgress( int Percent,  const String& Msg );
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
