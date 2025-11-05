//---------------------------------------------------------------------------

#ifndef SimTFXMainH
#define SimTFXMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <IniFiles.hpp>
#include <SyncObjs.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
#include "MessageObject.h"
#include "Publisher.h"
#include "Subscriber.h"
//---------------------------------------------------------------------------
#include "LinkFrame.h"
#include "PerformanceGauge.h"
#include "GraphMarquee.h"
#include "Speedy.h"
#include <Dialogs.hpp>
#include "SysTrayIcon.h"
#include <AppEvnts.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
#include "GraphButton.h"
#include <XPMan.hpp>
#include <Graphics.hpp>
#include "GraphTrackBar.h"
#include "PATSFrame.h"
#include <Tabs.hpp>
#include "GraphPanel.h"
#include <System.ImageList.hpp>
#include <set>
#include "LineEditForm.h"
//---------------------------------------------------------------------------
const int FIX_MSG_WIDTH = 1000;
//---------------------------------------------------------------------------
typedef enum
{
	mtNone,
	mtNewUser,
	mtModifyUser,
	mtChangePassword

} TModifyType;
//---------------------------------------------------------------------------
class TSimTFXForm : public TForm
{
__published:	// IDE-managed Components
	TMessageObject *MessageObject;
	TStatusBar *StatusBar;
	TImageList *ImageList;
	TPublisher *AgentPublisher;
	TSubscriber *AgentSubscriber;
	TPanel *PerformencePanel;
	TGroupBox *FUTGroupBox;
	TLabel *Label2;
	TLabel *Label3;
    TLabel *Label4;
    TLabel *FTotalLabel;
    TLabel *FSpeedLabel;
    TLabel *FMaxTimeLabel;
    TGroupBox *OPTGroupBox;
    TLabel *Label14;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *TotalLabel;
    TLabel *SpeedLabel;
    TLabel *MaxTimeLabel;
    TPerformanceGauge *PerformanceGaugeOPT;
    TPerformanceGauge *PerformanceGaugeFUT;
    TPageControl *PageControl;
    TTabSheet *LineSheet;
    TTabSheet *LogSheet;
    TTabSheet *ProcessSheet;
    TPanel *PVCLabelPanel;
    TStaticText *StaticText2;
    TStaticText *StaticText4;
    TStaticText *StaticText5;
    TStaticText *StaticText6;
    TStaticText *StaticText9;
    TStaticText *StaticText10;
    TStaticText *StaticText1;
    TPanel *PVCToolPanel;
    TListBox *LineListBox;
    TStaticText *StaticText3;
	TPanel *LogToolPanel;
    TListBox *LogListBox;
    TComboBox *LineComboBox;
    TPublisher *AdminPublisher;
    TSubscriber *AdminSubscriber;
    TPageControl *AdminPageControl;
    TTabSheet *UserTabSheet;
    TTabSheet *LogTabSheet;
    TPanel *Panel3;
    TSpeedButton *NewUserButton;
    TSpeedButton *AttribButton;
    TSpeedButton *DelUserButton;
    TListView *UserListView;
    TTimer *ResponseTimer;
    TPublisher *SpeedyAgentPublisher;
	TSubscriber *SpeedyAgentSubscriber;
    TPanel *Panel4;
    TListView *UserLogListView;
    TLabel *AccountLabel;
    TButton *QueryButton;
    TButton *ClearButton;
    TImageList *StateImageList;
    TTabSheet *FTTabSheet;
    TGroupBox *GroupBox4;
    TLabel *Label8;
    TComboBox *SysComboBox;
    TButton *FTRequestButton;
    TLabel *Label10;
    TDateTimePicker *EndDateTimePicker;
    TLabel *BeginDateLabel;
    TLabel *EndDateLabel;
    TDateTimePicker *BeginDateTimePicker;
    TListView *FileListView;
    TComboBox *FCodeComboBox;
    TSpeedButton *PasswdButton;
    TPopupMenu *UserPopupMenu;
    TMenuItem *N1;
    TMenuItem *N2;
    TMenuItem *N3;
    TMenuItem *N4;
	TMenuItem *N5;
    TMenuItem *N6;
    TPanel *OPPanel;
    TTabSheet *FIXSheet;
    TListView *FIXSessionsListView;
    TSubscriber *imFIXSubscriber;
    TPublisher *imFIXPublisher;
    TPanel *FIXToolPanel;
    TSpeedButton *NewSessionButton;
    TSpeedButton *SessionSettingButton;
    TSpeedButton *DeleteSessionButton;
    TPopupMenu *FIXPopupMenu;
    TMenuItem *FIXBrokenSessionMenu;
    TMenuItem *FIXModifyMenu;
    TMenuItem *FIXSetSessionSeqMenu;
	TMenuItem *FIXSessionEndOfDayMenu;
    TMenuItem *FIXDlelteMenu;
    TMenuItem *N11;
    TSpeedButton *ModifyPVCButton;
    TSpeedButton *NewPVCButton;
    TSpeedButton *DeletePVCButton;
    TSpeedButton *StateButton;
    TSaveDialog *SaveDialog;
    TSpeedButton *SaveLogButton;
    TSpeedButton *MonitorButton;
    TPublisher *SpeedyFutFtpPublisher;
    TSubscriber *SpeedyFutFtpSubscriber;
    TSubscriber *SpeedyOptFtpSubscriber;
    TPublisher *SpeedyOptFtpPublisher;
    TPopupMenu *PVCPopupMenu;
    TMenuItem *MonitorMenuItem;
    TSpeedButton *FIXStateButton;
    TPanel *PVCViewPanel;
    TPanel *FIXViewPanel;
    TLabel *Label9;
    TLabel *Label13;
    TLabel *FIXInSpeedLabel;
    TLabel *FIXInVolLabel;
    TGroupBox *FIXInGroupBox;
    TLabel *Label21;
    TLabel *Label22;
    TLabel *FIXOutSpeedLabel;
    TLabel *FIXOutVolLabel;
    TPerformanceGauge *FIXInPerformanceGauge;
    TPerformanceGauge *FIXOutPerformanceGauge;
    TGroupBox *FIXOutGroupBox;
    TGroupBox *InfoGroupBox;
    TComboBox *AccountComboBox;
    TImageList *StatImageList;
	TSpeedButton *SaveUserLogButton;
    TSpeedButton *PVCMonitorButton;
	TMenuItem *N7;
	TMenuItem *N8;
    TPageControl *FIXPageControl;
    TTabSheet *PerformenceTabSheet;
    TTabSheet *MessageTabSheet;
    TListBox *FIXMsgListBox;
    TSpeedButton *MonitorFIXButton;
    TSubscriber *FIXOutSubscriber;
    TSubscriber *FIXInSubscriber;
    TTimer *FIXLogTimer;
	TPopupMenu *FIXLogPopupMenu;
	TMenuItem *Clear1;
    TSplitter *Splitter;
    TTimer *HeartbeatResponseTimer;
    TCheckBox *ShowMsgCheckBox;
    TStaticText *StaticText7;
    TGroupBox *StockGroupBox;
    TLabel *Label1;
    TLabel *Label6;
    TLabel *Label11;
    TLabel *StockTotalLabel;
    TLabel *StockSpeedLabel;
    TLabel *StockMaxTimeLabel;
    TPerformanceGauge *PerformanceGaugeSTOCK;
    TStaticText *StaticText8;
    TMenuItem *N9;
    TMenuItem *MenuItemModifyPVC;
    TMenuItem *MenuItemDeletePVC;
    TMenuItem *N10;
    TMenuItem *DisablePVCMenuItem;
    TMenuItem *EnablePVCMenuItem;
	TTabSheet *AccountsTabSheet;
    TPanel *Panel2;
    TSpeedButton *NewAccountButton;
    TSpeedButton *ModifyAccountButton;
    TSpeedButton *DelAccountButton;
    TSpeedButton *ChangeAccountPwdButton;
    TListView *AccountsListView;
	TLabel *FIXSessionInfoLabel;
	TMenuItem *CopySettingMenuItem;
	TGroupBox *OTCGroupBox;
	TLabel *Label12;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *OTCTotalLabel;
	TLabel *OTCSpeedLabel;
	TLabel *OTCMaxTimeLabel;
	TPerformanceGauge *PerformanceGaugeOTC;
	TTimer *ProcessStateRequestTimer;
	TMenuItem *N13;
	TMenuItem *APCodeMenuItem;
	TMenuItem *N15;
	TMenuItem *TenderMenuItem;
	TMenuItem *AuctionMenuItem;
	TLabel *Label24;
	TPublisher *SystemInfoPublisher;
	TSubscriber *SystemInfoSubscriber;
	TGraphButton *OPTAlarmButton;
	TGraphButton *FUTAlarmButton;
	TGraphButton *TSEAlarmButton;
	TGraphButton *OTCAlarmButton;
	TTimer *StateBtnEnableTimer;
	TPanel *BKPanel;
	TPanel *ServerResourcePanel;
	TListView *ProcessListView;
	TListView *OtherProcessListView;
	TSplitter *ProcessSplitter;
	TSubscriber *TOPSubscriber;
	TGroupBox *GroupBox1;
	TLabel *Label28;
	TLabel *MEMLabel;
	TPerformanceGauge *MEMPerformanceGauge;
	TGroupBox *GroupBox2;
	TLabel *Label26;
	TLabel *CPULabel;
	TPerformanceGauge *CPUPerformanceGauge;
	TLabel *Label29;
	TLabel *MEMFreeLabel;
	TGroupBox *GroupBox5;
	TLabel *Label27;
	TLabel *Label31;
	TComboBox *BrokerComboBox;
	TComboBox *AppComboBox;
	TComboBox *SystemComboBox;
	TLabel *Label30;
	TLabel *Label32;
	TLabel *Label33;
	TGroupBox *GroupBox6;
	TLabel *Label34;
	TLabel *ProcessCountLabel;
	TLabel *Label36;
	TLabel *ActiveProcessCountLabel;
	TGraphTrackBar *MemTrackBar;
	TLabel *CPUWarningLabel;
	TLabel *MemWarningLabel;
	TGraphTrackBar *CPUTrackBar;
	TGraphButton *CPUAlarmButton;
	TGraphButton *MemAlarmButton;
	TLabel *Label35;
	TComboBox *LineStatusComboBox;
	TTimer *FilterTimer;
	TMenuItem *TenderExMenuItem;
	TGraphButton *OPTShrinkButton;
	TGraphButton *FUTShrinkButton;
	TGraphButton *TSEShrinkButton;
	TGraphButton *OTCShrinkButton;
	TMenuItem *N14;
	TMenuItem *N16;
	TMenuItem *N17;
	TMenuItem *N18;
	TPopupMenu *PVCMonitorPopupMenu;
	TMenuItem *LogClearMenuItem;
	TMenuItem *Save1;
	TTimer *PVCLogTimer;
	TPageControl *ResendPageControl;
	TTabSheet *TCPTabSheet;
	TTabSheet *X25TabSheet;
	TLabel *Label5;
	TLabel *FLEXStatusLabel;
	TComboBox *FLEXSysComboBox;
	TEdit *RootSymbolEdit;
	TButton *NewFLEXButton;
	TLabel *Label37;
	TLabel *Label38;
	TComboBox *B30SysComboBox;
	TEdit *B30BeginEdit;
	TEdit *B30EndEdit;
	TButton *ResendB30Button;
	TLabel *B30StatusLabel;
	TLabel *Label40;
	TLabel *Label41;
	TEdit *B30FCMEdit;
	TEdit *B30SessionEdit;
	TLabel *Label42;
	TTabSheet *ProfileTabSheet;
	TSubscriber *ProfileSubscriber;
	TScrollBox *ScrollBox;
	TSpeedButton *AESettingButton;
	TGraphButton *TimeAlarmButton;
	TLabel *Label25;
	TLabel *TimeDiffLabel;
	TGraphTrackBar *TimeDiffTrackBar;
	TGroupBox *TimeDiffGroupBox;
	TLabel *Label39;
	TLabel *MaxDiffLabel;
	TPerformanceGauge *TimeDiffPerformanceGauge;
	TLabel *ConnectionLabel;
	TToolBar *ServersToolBar;
	TPopupMenu *ProcessPopupMenu;
	TLabel *TSEStatusLabel;
	TLabel *OTCStatusLabel;
	TLabel *Label43;
	TGraphButton *DiskAlarmButton;
	TGraphTrackBar *DiskTrackBar;
	TLabel *DiskWarningLabel;
	TGroupBox *DiskGroupBox;
	TLabel *Label44;
	TLabel *DiskLabel;
	TLabel *Label46;
	TLabel *DiskFreeLabel;
	TPerformanceGauge *DiskPerformanceGauge;
	TTabSheet *ProxySheet;
	TListView *ProxyUsersListView;
	TPanel *TopPanel;
	TSubscriber *ProxySubscriber;
	TPopupMenu *ProxyPopupMenu;
	TMenuItem *LockUserMenuItem;
	TMenuItem *UnlockUserMenuItem;
	TPanel *StatusPanel;
	TLabel *Label19;
	TStaticText *CIDStaticText;
	TLabel *Label20;
	TStaticText *CIDMaxStaticText;
	TLabel *Label23;
	TStaticText *FillOrdStaticText;
	TLabel *Label45;
	TStaticText *ConnectionStaticText;
	TLabel *Label47;
	TStaticText *ConnectionMaxStaticText;
	TLabel *Label48;
	TStaticText *NewOrdCountStaticText;
	TLabel *Label49;
	TStaticText *CancelOrdCountStaticText;
	TTabSheet *ConnectionSheet;
	TSubscriber *StarWaveSubscriber;
	TTabSheet *StarWaveTabSheet;
	TScrollBox *SWScrollBox;
	TScrollBox *ConnectionScrollBox;
	TTabSet *ProxyTabSet;
	TTabSheet *EmomeSheet;
	TPerformanceGauge *EmomePerformanceGauge;
	TSubscriber *EmomeSubscriber;
	TLabel *EmomeCountLabel;
	TLabel *EmomeSpeedLabel;
	TLabel *EmomeMaxLabel;
	TShape *Shape1;
	TMenuItem *N12;
	TMenuItem *EODItem;
	TMenuItem *FIXSeqItem;
	TSpeedButton *TSERejectButton;
	TSpeedButton *OTCRecjectButton;
	TStaticText *TSENormalText;
	TStaticText *TSEOddText;
	TStaticText *TSEPostText;
	TStaticText *OTCNormalText;
	TStaticText *OTCOddText;
	TStaticText *OTCPostText;
	TMenuItem *TFXBackItem;
	TStaticText *StaticText11;
	TLabel *Label50;
	TLabel *Label51;
	TLabel *FOffHourTotalLabel;
	TLabel *OffHourTotalLabel;
	TComboBox *RuleComboBox;
	TButton *ExAccButton;
	TTabSet *RuleTabSet;
	TListView *RuleListView;
	TListView *RuleExListView;
	TStaticText *StaticText12;
	TLabel *Label53;
	TLabel *Label54;
	TButton *FExButton;
	TButton *CounterPartyButton;
	TPageControl *RoutePageControl;
	TTabSheet *RuleTabSheet;
	TSpeedButton *NewExRuleButton;
	TSpeedButton *DelExRuleButton;
	TSpeedButton *SpeedButton1;
	TImageList *RuleImageList;
	TSpeedButton *MessageButton;
	TSpeedButton *ModExRuleButton;
	TSpeedButton *ThroughputButton;
	TMenuItem *BrokenUserMenuItem;
	TMenuItem *NextConnectionMenuItem;
	TMenuItem *CODPVCMenuItem;
	TLabel *Label55;
	TComboBox *PartIDComboBox;
	TPanel *PanelTop;
	TComboBox *SessionComboBox;
	TLabel *Label52;
	TComboBox *FLEXTypeComboBox;
	TLabel *Label7;
	TEdit *BIDEdit;
	TComboBox *AppTypeComboBox;
	TLabel *Label56;
	TEdit *AccEdit;
	TLabel *Label57;
	TEdit *ContractDateEdit;
	TPanel *OPTPanel;
	TLabel *Label58;
	TEdit *StrikePxEdit;
	TComboBox *CallPutComboBox;
	TSubscriber *SpeedyFLEXFUTSubscriber;
	TSubscriber *SpeedyFLEXOPTSubscriber;
	TTabSheet *APITabSheet;
	TListView *APILockedListView;
	TSpeedButton *APIStateButton;
	TPanel *Panel1;
	TPanel *Panel5;
	TLabel *Label60;
	TLabel *Label61;
	TLabel *ThroughputCtrlLabel;
	TLabel *Label64;
	TStaticText *DayLimitText;
	TStaticText *ModifyTimeText;
	TStaticText *EditorText;
	TSpeedButton *APISettingButton;
	TPublisher *APIStatePublisher;
	TSubscriber *APIStateSubscriber;
	TSubscriber *APISettingSubscriber;
	TPopupMenu *APIPopupMenu;
	TMenuItem *MenuItem1;
    void __fastcall MessageObjectConnected(TObject *Sender);
    void __fastcall MessageObjectError(TObject *Sender);
    void __fastcall LogonButtonClick(TObject *Sender);
    void __fastcall LogoutButtonClick(TObject *Sender);
    void __fastcall AgentSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall MessageObjectDisconnected(TObject *Sender);
    void __fastcall LineListBoxDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
    void __fastcall StateButtonClick(TObject *Sender);
    void __fastcall AdminSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall ResponseTimerTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall NewUserButtonClick(TObject *Sender);
    void __fastcall DelUserButtonClick(TObject *Sender);
	void __fastcall UserListViewCustomDrawSubItem(TCustomListView *Sender,
          TListItem *Item, int SubItem, TCustomDrawState State,
          bool &DefaultDraw);
    void __fastcall AttribButtonClick(TObject *Sender);
    void __fastcall SpeedyAgentSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall ProcessListViewCustomDrawSubItem(
          TCustomListView *Sender, TListItem *Item, int SubItem,
          TCustomDrawState State, bool &DefaultDraw);
    void __fastcall PageControlChange(TObject *Sender);
    void __fastcall SysComboBoxChange(TObject *Sender);
    void __fastcall NewPVCButtonClick(TObject *Sender);
    void __fastcall ProcessStateRequestTimerTimer(TObject *Sender);
    void __fastcall PasswdButtonClick(TObject *Sender);
    void __fastcall ProcessListViewMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
    void __fastcall ProcessListViewMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall imFIXSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall FIXSessionsListViewCustomDrawSubItem(
          TCustomListView *Sender, TListItem *Item, int SubItem,
          TCustomDrawState State, bool &DefaultDraw);
    void __fastcall NewSessionButtonClick(TObject *Sender);
    void __fastcall FIXSetSessionSeqMenuClick(TObject *Sender);
    void __fastcall FIXSessionEndOfDayMenuClick(TObject *Sender);
    void __fastcall FIXBrokenSessionMenuClick(TObject *Sender);
    void __fastcall SessionSettingButtonClick(TObject *Sender);
    void __fastcall DeleteSessionButtonClick(TObject *Sender);
    void __fastcall ModifyPVCButtonClick(TObject *Sender);
    void __fastcall DeletePVCButtonClick(TObject *Sender);
    void __fastcall MonitorButtonClick(TObject *Sender);
    void __fastcall LogListBoxDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
    void __fastcall SaveLogButtonClick(TObject *Sender);
    void __fastcall FTRequestButtonClick(TObject *Sender);
    void __fastcall SpeedyFutFtpSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall SpeedyOptFtpSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall MonitorMenuItemClick(TObject *Sender);
    void __fastcall FIXStateButtonClick(TObject *Sender);
    void __fastcall FIXSessionsListViewSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall QueryButtonClick(TObject *Sender);
	void __fastcall ClearButtonClick(TObject *Sender);
    void __fastcall SaveUserLogButtonClick(TObject *Sender);
	void __fastcall HeartbeatResponseTimerTimer(TObject *Sender);
        void __fastcall N8Click(TObject *Sender);
    void __fastcall MonitorFIXButtonClick(TObject *Sender);
    void __fastcall FIXOutSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall FIXInSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall FIXMsgListBoxDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
    void __fastcall FIXMsgListBoxMeasureItem(TWinControl *Control,
          int Index, int &Height);
    void __fastcall FIXMsgListBoxClick(TObject *Sender);
    void __fastcall FIXLogTimerTimer(TObject *Sender);
    void __fastcall FileListViewSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall Clear1Click(TObject *Sender);
    void __fastcall ShowMsgCheckBoxClick(TObject *Sender);
    void __fastcall OtherProcessListViewCustomDrawSubItem(
          TCustomListView *Sender, TListItem *Item, int SubItem,
          TCustomDrawState State, bool &DefaultDraw);
    void __fastcall DisablePVCMenuItemClick(TObject *Sender);
    void __fastcall EnablePVCMenuItemClick(TObject *Sender);
    void __fastcall NewAccountButtonClick(TObject *Sender);
    void __fastcall ModifyAccountButtonClick(TObject *Sender);
    void __fastcall ChangeAccountPwdButtonClick(TObject *Sender);
    void __fastcall DelAccountButtonClick(TObject *Sender);
	void __fastcall CopySettingMenuItemClick(TObject *Sender);
	void __fastcall AccountsListViewDblClick(TObject *Sender);
	void __fastcall N15Click(TObject *Sender);
	void __fastcall TenderMenuItemClick(TObject *Sender);
	void __fastcall AuctionMenuItemClick(TObject *Sender);
	void __fastcall LineListBoxContextPopup(TObject *Sender, TPoint &MousePos,
          bool &Handled);
	void __fastcall SystemInfoSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
	void __fastcall AlarmButtonClick(TObject *Sender);
	void __fastcall FIXSessionsListViewDblClick(TObject *Sender);
	void __fastcall LineListBoxDataObject(TWinControl *Control, int Index,
          TObject *&DataObject);
	void __fastcall StateBtnEnableTimerTimer(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall AccountsListViewAdvancedCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, TCustomDrawStage Stage,
          bool &DefaultDraw);
	void __fastcall StaticText5DblClick(TObject *Sender);
	void __fastcall TOPSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
	void __fastcall SystemComboBoxChange(TObject *Sender);
	void __fastcall CPUTrackBarPositionChange(TObject *Sender);
	void __fastcall MemTrackBarPositionChange(TObject *Sender);
	void __fastcall CPUAlarmButtonClick(TObject *Sender);
	void __fastcall LineListBoxKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall FilterTimerTimer(TObject *Sender);
	void __fastcall TenderExMenuItemClick(TObject *Sender);
	void __fastcall ChangePasswdButtonClick(TObject *Sender);
	void __fastcall OPTShrinkButtonClick(TObject *Sender);
	void __fastcall ProcessListViewDrawItem(TCustomListView *Sender,
          TListItem *Item, TRect &Rect, TOwnerDrawState State);
	void __fastcall OtherProcessListViewMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall OtherProcessListViewMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
	void __fastcall OtherProcessListViewDrawItem(TCustomListView *Sender,
          TListItem *Item, TRect &Rect, TOwnerDrawState State);
	void __fastcall StatusBarDrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel,
          const TRect &Rect);
	void __fastcall FIXSessionsListViewDrawItem(TCustomListView *Sender,
          TListItem *Item, TRect &Rect, TOwnerDrawState State);
	void __fastcall AccountsListViewCustomDrawSubItem(TCustomListView *Sender,
          TListItem *Item, int SubItem, TCustomDrawState State,
          bool &DefaultDraw);
	void __fastcall AccountsListViewDrawItem(TCustomListView *Sender,
          TListItem *Item, TRect &Rect, TOwnerDrawState State);
	void __fastcall LogListBoxMeasureItem(TWinControl *Control, int Index,
          int &Height);
	void __fastcall LogListBoxClick(TObject *Sender);
	void __fastcall N16Click(TObject *Sender);
	void __fastcall N17Click(TObject *Sender);
	void __fastcall N18Click(TObject *Sender);
	void __fastcall LogClearMenuItemClick(TObject *Sender);
	void __fastcall PVCLogTimerTimer(TObject *Sender);
	void __fastcall ResendPageControlChange(TObject *Sender);
	void __fastcall ResendB30ButtonClick(TObject *Sender);
	void __fastcall UserListViewDrawItem(TCustomListView *Sender, TListItem *Item,
          TRect &Rect, TOwnerDrawState State);
	void __fastcall FIXSessionsListViewContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
	void __fastcall ProfileSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall AESettingButtonClick(TObject *Sender);
	void __fastcall TimeDiffTrackBarPositionChange(TObject *Sender);
	void __fastcall MessageObjectMBusStartup(const MString &Hostname);
	void __fastcall MessageObjectMBusStopped(const MString &Hostname);
	void __fastcall MessageObjectMBusList(const MString &Hostname, TStringList *List);
	void __fastcall ToolButtonClick(TObject *Sender);
	void __fastcall DiskTrackBarPositionChange(TObject *Sender);
	void __fastcall ProxySubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree);
	void __fastcall ProxyUsersListViewAdvancedCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, TCustomDrawStage Stage,
          bool &DefaultDraw);
	void __fastcall ProxyUsersListViewCustomDrawSubItem(TCustomListView *Sender, TListItem *Item,
          int SubItem, TCustomDrawState State, bool &DefaultDraw);
	void __fastcall ProxyUsersListViewDrawItem(TCustomListView *Sender, TListItem *Item,
          TRect &Rect, TOwnerDrawState State);
	void __fastcall ProxyUsersListViewMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall LockUserMenuItemClick(TObject *Sender);
	void __fastcall UnlockUserMenuItemClick(TObject *Sender);
	void __fastcall StarWaveSubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree);
	void __fastcall ProxyUsersListViewData(TObject *Sender, TListItem *Item);
	void __fastcall ProxyTabSetChange(TObject *Sender, int NewTab, bool &AllowChange);
	void __fastcall EmomeSubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree);
	void __fastcall EODItemClick(TObject *Sender);
	void __fastcall FIXSeqItemClick(TObject *Sender);
	void __fastcall OTCRecjectButtonClick(TObject *Sender);
	void __fastcall TSERejectButtonClick(TObject *Sender);
	void __fastcall TSENormalTextClick(TObject *Sender);
	void __fastcall ProcessListViewColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall AccountsListViewCompare(TObject *Sender, TListItem *Item1, TListItem *Item2,
          int Data, int &Compare);
	void __fastcall AccountsListViewColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall BackupLineClick(TObject *Sender);
	void __fastcall RuleTabSetChange(TObject *Sender, int NewTab, bool &AllowChange);
	void __fastcall RuleListViewClick(TObject *Sender);
	void __fastcall FExButtonClick(TObject *Sender);
	void __fastcall CounterPartyButtonClick(TObject *Sender);
	void __fastcall ExAccButtonClick(TObject *Sender);
	void __fastcall RuleComboBoxChange(TObject *Sender);
	void __fastcall NewExRuleButtonClick(TObject *Sender);
	void __fastcall DelExRuleButtonClick(TObject *Sender);
	void __fastcall MessageButtonClick(TObject *Sender);
	void __fastcall ModExRuleButtonClick(TObject *Sender);
	void __fastcall ThroughputButtonClick(TObject *Sender);
	void __fastcall BrokenUserMenuItemClick(TObject *Sender);
	void __fastcall NextConnectionMenuItemClick(TObject *Sender);
	void __fastcall B30SysComboBoxChange(TObject *Sender);
	void __fastcall FLEXSysComboBoxChange(TObject *Sender);
	void __fastcall NewFLEXButtonClick(TObject *Sender);
	void __fastcall SpeedyFLEXOPTSubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree);
	void __fastcall APIStateButtonClick(TObject *Sender);
	void __fastcall APIStateSubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree);
	void __fastcall APISettingSubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree);
	void __fastcall APISettingButtonClick(TObject *Sender);
	void __fastcall MenuItem1Click(TObject *Sender);



private:	// User declarations
	THashedStringList* FFIXSessions;
	THashedStringList* FLinkPVCToPanel;
	THashedStringList* FDispToAll;
	THashedStringList* FSWPanels;
	THashedStringList* FProxys;
	TCriticalSection*  FUIUpdateCS;
    bool FSpeedyExists;
	bool FFirstTimeDetect;
	int  FOptMaxTP;
	int  FFutMaxTP;
	int  FStockMaxTP;
	int  FOTCMaxTP;
	long FSendHBTime;
	long FReceiveHBTime;
	int  FLastAlarmCID;
	AnsiString FUser;
	AnsiString FPassword;
	String FStatusText;
	AnsiString FConfigFileName;
	bool FIsLogon;
	bool FLogonResponse;
	bool FHeartbeatResponse;
	bool FMonitorFIX;
	TFIXSession* FMonitorSession;
	int        FProcessState[ MAX_PROCESS ];
	bool       FProcessAutoStart[ MAX_PROCESS ];
	bool       FIsStandby[ MAX_PROCESS ];
	bool       FIsChecker[ MAX_PROCESS ];
	bool       FIsRemote[ MAX_PROCESS ];
	bool       FIsExchange[ MAX_PROCESS ];
	bool       FEnableWatchDog[ MAX_PROCESS ];
	int        FWatchDogSeconds[ MAX_PROCESS ];
	int        FWatchDogTimes[ MAX_PROCESS ];
	AnsiString FProcessStartTime[ MAX_PROCESS ];
	AnsiString FProcessStopTime[ MAX_PROCESS ];
	int        FProcessStartSec[ MAX_PROCESS ];
	int        FProcessStopSec[ MAX_PROCESS ];
	AnsiString FProcessConfig[ MAX_PROCESS ];
	int        FOtherProcessState[ MAX_PROCESS ];
	bool       FOtherProcessAutoStart[ MAX_PROCESS ];
	AnsiString FOtherProcessStartTime[ MAX_PROCESS ];
	AnsiString FOtherProcessStopTime[ MAX_PROCESS ];
	int  FSpeedyRunningState;
	int  FimFIXRunningState;
	int  FMonitorProcess;
	int  FMonitorOtherProcess;
	DWORD FFIXRecvTick;
	int  FInSeq;
	int  FOutSeq;
	int  FMaxInVol;
	int  FMaxOutVol;
	int    FOPTTick;
	int    FFUTTick;
	int    FTSETick;
	int    FOTCTick;
	int    FOPTQueueCount;
	int    FFUTQueueCount;
	int    FTSEQueueCount;
	int    FOTCQueueCount;
	bool   FPasswordExp;
	int    FLoginIDSortDir;
	///< For Emome
	int FEmomeOrdCount;
	UFCType::UInt32 FEmomeOrdTick;
	double FEmomeThroughputMax;
	///< For Top
	int FCPUCount;
	int FMemCount;
	int FDiskCount;
	int FDiffCount;
	int FDiffMax;
	///< For Monotoring session header
	int BROKERID_POS;
	int PASSWORD_POS;
	int LINE_STAT_POS;
	int LINE_TYPE_POS;
	int LINE_PORT_POS;
	int LINE_PVC_POS;
	int LINE_APC_POS;
	int LINE_MSG_POS;
	int LINE_IP_POS;
	int LINE_GROUP_POS;
	int TEXT_Y_OFFSET;
	///< For Monotoring proxy
	bool FProxyStatEmpty;
	int  FCIDMax;
	int  FCurrentCID;
	int  FProxyConnectionMax;
	int  FProxyConnectionCount;
	///< for PATS
	int   FPATSExecCount;
	int   FPATSOrderCount;
	float FPATSTPMax;
	UFCType::UInt32 FPATSLastUpdateTick;
private:
	AnsiString    FAccSettingFile;
	AnsiString    FChSettingFile;
	AnsiString    FExSettingFile;
	AnsiString    FRuleFile[ 5 ];
	AnsiString    FRemoteRuleFile[ 5 ];
	TStringList*  FRuleExchangeList;
	TStringList*  FRuleChannelList;
	UFC::UiniFile FRulesIni[5];
	UFC::UiniFile FRuleExchangeIni;
	UFC::UiniFile FRuleChannelIni;
	UFC::UiniFile FRuleAccountIni;
	void __fastcall LoadRule( int index );
	void __fastcall SaveAndLoadRule( int index, char* Buffer, int Size  );
	bool __fastcall CheckExceptionProdID( void );
	void __fastcall UpdateExceptionRule( void );
private:
	bool __fastcall IsErrorProcess( int ProcessIndex );
	bool __fastcall IsRunningTime( int ProcessIndex );
	void __fastcall ReplaceChar( UFC::AnsiString& FIXMsg );
	void __fastcall SendFile( const AnsiString& File,const AnsiString& Name,int CMD, const char* FileName = NULL );
	void __fastcall AddEventMessage( const String& Msg );
	void __fastcall AddFIXMessage( bool, const AnsiString& );
	void __fastcall AddProcess( void );
	void __fastcall LogonDialog( void );
	void __fastcall AdminLogon( int Days, bool Force = false );
	void __fastcall GroupAttrib(  MTree *Tree );
	void __fastcall GroupNames(  MTree *Tree );
	void __fastcall OPLogon( int Days , int Group, MTree *Tree );
	void __fastcall UpdateLineState( int Index, int NewState, int InSeq, int OutSeq, int backindex );
	void __fastcall UpdateOPTPerformance( int OrderSec, int Total, int qcount, int OHTotal, int OHqcount, UFCType::UInt32 Status );
	void __fastcall UpdateFUTPerformance( int OrderSec, int Total, int qcount, int OHTotal, int OHqcount,UFCType::UInt32 Status );
	void __fastcall UpdateStockPerformance( int OrderSec, int Total, int qcount, UFCType::UInt32 Status );
	void __fastcall UpdateOTCPerformance( int OrderSec, int Total, int qcount, UFCType::UInt32 Status );
	void __fastcall UpdatePVCIOLog( TMemoryStream* Stream );
	void __fastcall UpdateUserConfigFile( void );
	void __fastcall LoadUserLog( void );
    void __fastcall BuildUserlist( void );
	void __fastcall FetchUserlist( MTree* Tree );
	void __fastcall UpdateAEStatus( MTree* Tree );
    void __fastcall ClearAllLines();
	void __fastcall AddSettings( MTree *Tree );
    void __fastcall WndProc( TMessage& Msg );
	void __fastcall RequestUpdatePVCList( TMarket Market );
    void __fastcall RequestPVCSettingList( void );
	void __fastcall RequestFIXSessionList( void );
	void __fastcall RequestFIXSessionsState( void );
	void __fastcall RequestUserConfigFile( void );
	void __fastcall RequestProfileConfigFile( void );
	void __fastcall RequestClusterConfigFile( void );
	void __fastcall RequestRouteFiles( void );
	void __fastcall RequestConfigFile( const char* FileName );
	void __fastcall ClearFIXSessions();
	void __fastcall ClearProxyPage( void );
	void __fastcall SendStatusRequest( const char* BrokerID, int Link, int PVC, const char* PVCID, TMarket Market, BOOL IsOffHour );
	void __fastcall UpdateItem( int Index );
    void __fastcall AddSettingToListView();
    void __fastcall AddToListView( TStringList* SettingList, TMarket Market );
	void __fastcall UpdateProcess( TListItem* ModifyItem );
	void __fastcall UpdateOtherProcess( TListItem* ModifyItem );
	void __fastcall UpdateProcessState( int State, int multi32 = 0 );
    void __fastcall UpdateOtherProcessState( int State, int multi32 );
	void __fastcall SendShellCMD( int CMD, int ProcessIndex, bool IsMBusApp );
//	bool __fastcall LineExists( TMarket Market, int Link, int PVC );
	void __fastcall MakeLineKey( TPVCSettingForm* Setting, AnsiString& Key );
	bool __fastcall LineExists( TPVCSettingForm* Setting );
    void __fastcall ChangeProcessState( int Index, int State );
    void __fastcall ChangeOtherProcessState( int Index, int State );

    int  __fastcall GetInsertPos( TMarket Market );
	void __fastcall SaveCFGFile( TMarket Market, TStream* Stream );
	void __fastcall SetFIXDefaultIniSection( UFC::UiniFile* ini );
	void __fastcall GetSessionsFromSettingFile();
    void __fastcall SaveSessionsToSettingFile();
    void __fastcall ClearFIXState( bool StopMonitoring = false );
    void __fastcall OnProcessCrashed( int Index );
    void __fastcall OnOtherProcessCrashed( int Index );    
    void __fastcall OnFIXSessionBroken( TFIXSession* session );
    void __fastcall OnPVCBroken( TLineInfo*  Line  );
    void __fastcall OnProcessStateChanged( int Index, int State );
    void __fastcall MBusDisconnect( const String& State );
	void __fastcall StopMonitorFIXSession( void );
	void __fastcall MonitorFIXSession( void );
	void __fastcall EnablePVC(  TLineInfo* Line, bool Enabled, bool COD  );
	void __fastcall ReadUsers( void );
	void __fastcall CreateProfilePanels( void );
	void __fastcall CreateCluster( const AnsiString& SettingFile );
	bool __fastcall ClusterButtonExists( const AnsiString& Name );
	void __fastcall SaveToFile( const AnsiString& FileName, char *Buffer, int Size );
	void __fastcall CheckMarketExist( void );
	void __fastcall ChangeAPCode( TLineInfo* Line, char APCode  );
	void __fastcall ChangeBackup( TLineInfo* Line, int Index );
	void __fastcall LockUserUI( bool Lock );
	void __fastcall MonitorServer( AnsiString Host, bool Force = false );
	void __fastcall ClearPerformanceGauges( void );
	void __fastcall ConfigProcess( void );
	void __fastcall ReloadMBusProcess( void );
	void __fastcall DisconnectMBus( void );
	//File Transfer
	TPublisher *FFtpPublisher;
	void __fastcall CreateFtpMTreeData( MTree* pData );
    void __fastcall ChangeFileListViewStatus( const AnsiString& FileCode, int StatusCode, bool IsOpt );
	void __fastcall LockDownloadControls( bool Value );
    String __fastcall GetStatusStringByStatusCode( int StatusCode );
	void __fastcall EraseBKGnd( TListView* ListView );
	void __fastcall MonitorOtherProcess( bool Monitor );
	int  __fastcall DisplayIndexToAllIndex( int DispIndex );
	int  __fastcall AllIndexToDisplayIndex( int AllIndex );
	TConnectionFrame* __fastcall GetConnectionPanel( const UFC::AnsiString& Name );
public:
	bool __fastcall IsProcessRunning( const AnsiString& ProcessName );
private:	// Proxy
	void __fastcall UpdatProxyInfo( const MString& ProxyName, MTree *Tree  );
	void __fastcall ModifyThroughput( const MString& ProxyName, MTree *Tree  );
private:	// User declarations
	bool         FShowPassword;
	MarketSet    FMarketSet;
	TStringList* FOptList;
	TStringList* FFutList;
	TStringList* FStockList;
	TStringList* FOTCList;
	TStringList* FProcessList;
	TStringList* FOtherProcessList;
	TStringList* FUserList;
	TStringList* FUserLogList;
	TStringList* FFIXLogList;
	TStringList* FPVCLogList;
	TStringList* FFIXDicts;
	TStringList* FCheckers;
	TStringList* FGroupNames;
	TObjectList* FGraphPanels;
	TModifyType  FModifyType;
	TCriticalSection*  FFIXLogCS;
	TCriticalSection*  FPVCLogCS;
	Graphics::TBitmap* FListBuffer;
	AnsiString         FSpeedyHost;
	AnsiString         FSpeedyHostIP;
	AnsiString         FTimeStr;
	bool FTPPageVisible;
	bool AEPageVisible;
	bool ProcessControl;
	bool ProcessSetting;
	bool ConnectionControl;
	bool ConnectionSetting;
	bool FIXControl;
	bool FIXSetting;
	bool FAutoLogon;
	int  FServerVersion;
	TNotifyEvent       FOnLogon;
	TNotifyEvent       FOnLogonFalied;
	TNotifyEvent       FOnAdminLogon;
	TNotifyEvent       FOnLogoff;
	TNotifyEvent       FOnMaxinum;
	TNotifyEvent       FOnMDIClosed;
	TNotifyEvent       FOnMDIActive;
	TNotifyEvent       FOnMDITime;
	UFC::PHashMap<UFC::AnsiString,TConnectionFrame*> FConnectionPanelMap;
	void __fastcall OnChangeToFIXPage( void );
	void __fastcall OnChangeToPVCPage( void );
	void __fastcall OnChangeToFileTransferPage( void );
	void __fastcall UpdateFIXPerformance( int In,int Out );
	void __fastcall ParseFIXStatus( TStringList* Status, UFCType::UInt32 TickNow );
	void __fastcall UpdateFIXStat( BOOL broken, int Index );
	void __fastcall WriteUserLog( const AnsiString& Type, const AnsiString& Msg );
	TColor __fastcall StringToColor( const char* ColStr );
	void __fastcall SetLogon( bool IsLogon );
	String __fastcall GetStatusString( UFCType::UInt32 Status );
	void __fastcall SetSessionStatusString( bool IsTSE, UFCType::UInt32 Status );
	void __fastcall QueryAPISetting( void );
public:		// User declarations
	__fastcall TSimTFXForm(TComponent* Owner);
	void __fastcall LogonOK( const AnsiString& HostName, int Version );
	AnsiString __fastcall GetUserConfigFileName( bool FullPath = true );
	AnsiString __fastcall GetProfileConfigFileName( bool FullPath = true );
	void __fastcall RestoreLogonState( void );
	void __fastcall UpdateProxyUserStat(const UFC::AnsiString& Status );
public:
	bool            IsAdmin;
	int             FOSIndex;
	void __fastcall Logon( const AnsiString& IP );
	void __fastcall Logon( const AnsiString& HostName, const AnsiString& IP );
	void __fastcall Logoff( void )                { LogoutButtonClick( NULL ); }
	void __fastcall ChangePassword( void )        { ChangePasswdButtonClick( NULL ); }
	bool __fastcall IsLogOn( void )               { return FIsLogon; }
	int  __fastcall GetOSIndex( void )            { return FOSIndex; }
	void __fastcall GetSystemInfo( void );
	const AnsiString& __fastcall GetIP( void )    { return FSpeedyHostIP; }
	const AnsiString& __fastcall GetTime( void )  { return FTimeStr; }
	bool __fastcall RequestCancelAll( bool IsOptions, bool IsOffHour, String Password, String Account );
	void __fastcall RequestKillSwitchList( bool IsOptions, bool IsOffHour );
	bool __fastcall RequestKillSwitch( bool IsOptions, bool IsOffHour, bool Enable, String Password, String FCM  );
	bool __fastcall RequestKillSwitch( bool IsOptions, bool IsOffHour, bool Enable, String Password, String FCM, String SFCM, String SID  );
private:
	static AnsiString   FLastUser;
	static AnsiString   FLastPasswordMD5;
public:
	__property TNotifyEvent OnLogon     = {read = FOnLogon, write = FOnLogon};
	__property TNotifyEvent OnLogonFailed = {read = FOnLogonFalied, write = FOnLogonFalied};
	__property TNotifyEvent OnAdminLogon= {read = FOnAdminLogon, write = FOnAdminLogon};
	__property TNotifyEvent OnLogoff    = {read = FOnLogoff, write = FOnLogoff};
	__property TNotifyEvent OnMaxinum   = {read = FOnMaxinum, write = FOnMaxinum};
	__property TNotifyEvent OnMDIClosed = {read = FOnMDIClosed, write = FOnMDIClosed};
	__property TNotifyEvent OnMDIActive = {read = FOnMDIActive, write = FOnMDIActive};
	__property TNotifyEvent OnMDITime   = {read = FOnMDITime, write = FOnMDITime};
};
//---------------------------------------------------------------------------
class TProxyInfo : public TObject
{
private:
	UFC::AnsiString FName;
	int FCID;
	int FCIDMAX;
	int FLogon;
	int FLogonMax;
	int FNewOrder;
	int FCancelOrder;
	int FFillOrder;
	int FTabIndex;
	TSimTFXForm* FOwner;
	UFC::PHashMap< UFC::AnsiString,TStringList* > FHashedUsers;
	UFC::List<TStringList*> FUsers;
private:
	void __fastcall UpdateProxyUserStat( const UFC::AnsiString& Status, bool IsActiveTab );
public:
	__fastcall TProxyInfo( TSimTFXForm* own, const UFC::AnsiString& Name );
	__fastcall ~TProxyInfo();
	void __fastcall Update( MTree* Data );
	void __fastcall Active( void );
	void __fastcall UpdateTabIndex(  );
	UFC::List<TStringList*>* GetUsers(){ return &FUsers; }
};
//---------------------------------------------------------------------------
extern PACKAGE TSimTFXForm *SimTFXForm;
//---------------------------------------------------------------------------
#endif
