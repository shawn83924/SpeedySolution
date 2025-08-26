//---------------------------------------------------------------------------

#ifndef CMDformH
#define CMDformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "Publisher.h"
#include "Subscriber.h"
//---------------------------------------------------------------------------
class TTelnetForm : public TForm
{
__published:	// IDE-managed Components
    TListBox *StdoutListBox;
    TEdit *CMDEdit;
    TPanel *Panel1;
    TPublisher *CMDPublisher;
    TSubscriber *STDIOSubscriber;
    TLabel *Label1;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall CMDEditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall STDIOSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall StdoutListBoxData(TWinControl *Control, int Index,
          AnsiString &Data);
private:	// User declarations
    TStringList* FLines;
public:		// User declarations
    __fastcall TTelnetForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTelnetForm *TelnetForm;
//---------------------------------------------------------------------------
#endif
