//---------------------------------------------------------------------------

#ifndef NewSessionH
#define NewSessionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "LinkFrame.h"
#include "cspin.h"
//---------------------------------------------------------------------------
class TSessionForm : public TForm
{
__published:	// IDE-managed Components
    TPageControl *PageControl;
    TTabSheet *AcceptSheet;
    TTabSheet *InitiatorSheet;
    TLabel *Label4;
    TEdit *HeartbeatEdit;
    TLabel *Label5;
    TEdit *PortEdit;
    TEdit *HostEdit;
    TLabel *Label6;
    TEdit *Port1Edit;
    TLabel *Label7;
    TEdit *Host1Edit;
    TLabel *Label9;
    TEdit *Port2Edit;
    TLabel *Label10;
    TEdit *Host2Edit;
    TLabel *Label11;
    TLabel *Label12;
    TEdit *APortEdit;
    TLabel *Label13;
    TComboBox *FIXVerComboBox;
    TLabel *Label14;
    TEdit *SenderIDEdit;
    TLabel *Label15;
    TEdit *TargetIDEdit;
	TBitBtn *OKBtn;
	TBitBtn *CancelBtn;
    TLabel *Label8;
    TRadioGroup *TypeRadioGroup;
    TLabel *Label1;
    TComboBox *FIXDictComboBox;
	TLabel *StartLabel;
	TCSpinEdit *StartHourSpinEdit;
	TCSpinEdit *StartMinutesSpinEdit;
	TCSpinEdit *StartSecSpinEdit;
	TLabel *Label2;
	TCSpinEdit *EndHourSpinEdit;
	TCSpinEdit *EndMinutesSpinEdit;
	TCSpinEdit *EndSecSpinEdit;
	TLabel *Label3;
	TEdit *ReconnectEdit;
	TLabel *Label16;
	TLabel *Label17;
	TCheckBox *PersistCheckBox;
	TBevel *Bevel1;
	TCheckBox *ResetOnLogonCheckBox;
	TCheckBox *VerifySeqCheckBox;
	TComboBox *AppVComboBox;
	TLabel *AppVLabel;
	TComboBox *StartDayComboBox;
	TComboBox *EndDayComboBox;
    void __fastcall OKBtnClick(TObject *Sender);
    void __fastcall TypeRadioGroupClick(TObject *Sender);
    void __fastcall FIXVerComboBoxChange(TObject *Sender);
	void __fastcall AppVComboBoxChange(TObject *Sender);
	void __fastcall StartDayComboBoxChange(TObject *Sender);
	void __fastcall EndDayComboBoxChange(TObject *Sender);
private:	// User declarations
    bool __fastcall CheckInitiator( );
    bool __fastcall CheckAcceptor( );
    TInitiator* FModifyInitiator;
    TAcceptor*  FModifyAcceptor;
    AnsiString FIXVersion;
    AnsiString SenderCompID;
    AnsiString TargetCompID;
    int        AcceptorPort;
	int        HeartbeatIntival;
	int        ReconnectIntival;
public:		// User declarations
    __fastcall TSessionForm( TComponent* Owner );
    __fastcall TSessionForm( TStringList* FIXDicts );
    __fastcall TSessionForm( TFIXSession* Session, TStringList* FIXDicts );
    TFIXSession* __fastcall CreateSession( bool& IsAcceptor );
    void __fastcall UpdateSession();
};
//---------------------------------------------------------------------------
extern PACKAGE TSessionForm *SessionForm;
//---------------------------------------------------------------------------
#endif
