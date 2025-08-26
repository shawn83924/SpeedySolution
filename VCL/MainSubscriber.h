//---------------------------------------------------------------------------

#ifndef MainSubscriberH
#define MainSubscriberH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "MessageObject.h"
#include "Publisher.h"
#include "Subscriber.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TEdit *SubjectEdit;
    TEdit *KeyEdit;
    TLabel *Label1;
    TLabel *Label2;
    TButton *SubscribeButton;
    TListView *ListView;
    TPanel *Panel1;
    TMessageObject *MessageObject;
    void __fastcall MessageObjectConnected(TObject *Sender);
    void __fastcall MessageObjectDisconnected(TObject *Sender);
    void __fastcall MessageObjectError(TObject *Sender);
    void __fastcall SubscribeButtonClick(TObject *Sender);
    void __fastcall Subscriber1Message(const MString &Subject,
          const MString &Key, MTree *Tree);
    void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
