//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainSubscriber.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MessageObject"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MessageObjectConnected(TObject *Sender)
{
    SubscribeButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MessageObjectDisconnected(TObject *Sender)
{
    SubscribeButton->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MessageObjectError(TObject *Sender)
{
    SubscribeButton->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SubscribeButtonClick(TObject *Sender)
{
    TSubscriber* Subscriber = new TSubscriber( this );

    Subscriber->MessageObject = MessageObject;
//    Subscriber->Key = KeyEdit->Text;
//    Subscriber->Subject = SubjectEdit->Text;
//    Subscriber->OnMessage = Subscriber1Message;

    Subscriber->Tag = 0;

//    TListItem  *ListItem = ListView->Items->Add();
//    ListItem->Caption = SubjectEdit->Text;
//    ListItem->SubItems->Add( KeyEdit->Text );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Subscriber1Message(const MString &Subject,
      const MString &Key, MTree *Tree)
{
 ///
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
    MessageObject->Connect();
}
//---------------------------------------------------------------------------
