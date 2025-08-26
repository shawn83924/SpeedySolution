//---------------------------------------------------------------------------

#ifndef MainSubscriberFormH
#define MainSubscriberFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "MessageObject.h"
#include "Subscriber.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TSubscriberForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *KeyEdit;
	TLabel *KeyLabel;
	TLabel *SubjectLabel;
	TEdit *SubjectEdit;
	TButton *ConnectButton;
	TButton *DisconnectButton;
	TEdit *IPEdit;
	TLabel *HostLabel;
	TMessageObject *MessageObject;
	TStatusBar *StatusBar;
	TPanel *ToolPanel;
	TCheckBox *DetailCheckBox;
	TTreeView *TreeView;
	TSubscriber *Subscriber;
	TTimer *Timer;
	TLabel *PortLabel;
	TEdit *PortEdit;
	TPageControl *PageControl;
	TTabSheet *MsgTabSheet;
	TTabSheet *ProcessTabSheet;
	TPanel *MessagePanel;
	TSpeedButton *ClrButton;
	TImageList *ImageList;
	TTimer *ProcessListTimer;
	TTreeView *MBusTreeView;
	TTimer *MBusTimer;
	void __fastcall ConnectButtonClick(TObject *Sender);
	void __fastcall DisconnectButtonClick(TObject *Sender);
	void __fastcall MessageObjectConnected(TObject *Sender);
	void __fastcall MessageObjectDisconnected(TObject *Sender);
	void __fastcall MessageObjectError(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall SubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree);
	void __fastcall ClrButtonClick(TObject *Sender);
	void __fastcall MessageObjectMBusConnected(const MString &Hostname, bool IsTheTirstOne);
	void __fastcall MessageObjectProcessList( const MString &Hostname,TStringList* Processs);
	void __fastcall ProcessListTimerTimer(TObject *Sender);
	void __fastcall MessageObjectProcessStartup(const MString &Hostname, const MString &ProcessName);
	void __fastcall MessageObjectProcessStopped(const MString &Hostname, const MString &ProcessName);
	void __fastcall MessageObjectMBusList(const MString &Hostname, TStringList *Processs);
	void __fastcall MBusTimerTimer(TObject *Sender);
	void __fastcall MBusTreeViewExpanded(TObject *Sender, TTreeNode *Node);
	void __fastcall MessageObjectMBusStartup(const MString &Hostname);
	void __fastcall MessageObjectMBusStopped(const MString &Hostname);
private:	// User declarations
	int FMsgCount;
	UFC::AnsiString FConnectHost;
	TTreeNode* __fastcall FindMBusNode( const UFC::AnsiString& MBusName );
	TTreeNode* __fastcall FindProcessNode( TTreeNode* MBusNode, const UFC::AnsiString& ProcessName );
	TTreeNode* __fastcall FindProcessNode( const UFC::AnsiString& MBusName, const UFC::AnsiString& ProcessName );
public:		// User declarations
	__fastcall TSubscriberForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSubscriberForm *SubscriberForm;
//---------------------------------------------------------------------------
#endif
