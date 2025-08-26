//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CMDform.h"
#include "SimTFXMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Publisher"
#pragma link "Subscriber"
#pragma resource "*.dfm"
TTelnetForm *TelnetForm;
extern AnsiString SpeedyHost;
//---------------------------------------------------------------------------
__fastcall TTelnetForm::TTelnetForm(TComponent* Owner)
    : TForm(Owner)
{
    FLines = new TStringList();
}
//---------------------------------------------------------------------------
void __fastcall TTelnetForm::FormShow(TObject *Sender)
{
    CMDPublisher->Subject = SpeedyHost;
    CMDPublisher->Key = UFC::Hostname;
    STDIOSubscriber->Subject = SpeedyHost;
    STDIOSubscriber->Key = UFC::Hostname;
    MTree Data;
    Data.append( "ACT", 0 );
    Data.append( "CMD", "Start" );
    CMDPublisher->SendData( & Data );
}
//---------------------------------------------------------------------------
void __fastcall TTelnetForm::FormHide(TObject *Sender)
{
    MTree Data;
    Data.append( "ACT", 2 );
    Data.append( "CMD", "Start" );
    CMDPublisher->SendData( & Data );
}
//---------------------------------------------------------------------------
void __fastcall TTelnetForm::CMDEditKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if( Key == VK_RETURN )
    {
        MTree Data;
        Data.append( "ACT", 1 );
        Data.append( "CMD", CMDEdit->Text.c_str() );
        CMDPublisher->SendData( & Data );
        CMDEdit->Text = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TTelnetForm::STDIOSubscriberMessage(const MString &Subject,
      const MString &Key, MTree *Tree)
{
    UFC::AnsiString Line;
    if( Tree->get("LINE",Line))
    {
        FLines->Add( Line.c_str() );
        if( FLines->Count > 500 )
            FLines->Delete( 0 );
        int LineCount = FLines->Count;
        int Srclines = StdoutListBox->Height /StdoutListBox->ItemHeight;

        StdoutListBox->Count = LineCount;
        if( LineCount > Srclines )
            StdoutListBox->TopIndex = LineCount - Srclines;
        else
            StdoutListBox->TopIndex = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TTelnetForm::StdoutListBoxData(TWinControl *Control,
      int Index, AnsiString &Data)
{
    Data = FLines->Strings[ Index ];
}
//---------------------------------------------------------------------------
