//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "FileStream.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MessageObject"
#pragma link "Publisher"
#pragma link "ThdTimer"
#pragma resource "*.dfm"
TPublisherForm *PublisherForm;
bool Sending = false;
//---------------------------------------------------------------------------
__fastcall TPublisherForm::TPublisherForm(TComponent* Owner)
    : TForm(Owner),loop( true ),count(0),Orgcount(0)
{
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::RadioGroup1Click(TObject *Sender)
{
       switch( RadioGroup1->ItemIndex )
       {
            case 0: Timer->Interval = 1;    Timer->Enabled = true; loop = false; break;
            case 1: Timer->Interval = 2;    Timer->Enabled = true; loop = false; break;
            case 2: Timer->Interval = 5;    Timer->Enabled = true; loop = false; break;
            case 3: Timer->Interval = 10;   Timer->Enabled = true; loop = false; break;
            case 4: Timer->Interval = 20;   Timer->Enabled = true; loop = false; break;
            case 5: Timer->Interval = 50;   Timer->Enabled = true; loop = false; break;
            case 6: Timer->Interval = 100;  Timer->Enabled = true; loop = false; break;
            case 7: Timer->Interval = 200;  Timer->Enabled = true; loop = false; break;
            case 8: Timer->Interval = 500;  Timer->Enabled = true; loop = false; break;
            case 9: Timer->Interval = 1000; Timer->Enabled = true; loop = false; break;
            case 10: Button1Click( NULL ); break;
            case 11: Stop(); break;
       }
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::Stop()
{
    Timer->Enabled = false;
    loop = false;
    RadioGroup1->ItemIndex = 8;
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::MessageObject1Connected(TObject *Sender)
{
    char Name[64];

    Sending = true;
    gethostname( Name, 64 );
    FHost = Name;
    RadioGroup1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::MessageObject1Error(TObject *Sender)
{
    Sending = false;
    RadioGroup1->Enabled = false;
    Stop();
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::MessageObject1Disconnected(TObject *Sender)
{
    Sending = false;
    RadioGroup1->Enabled = false;    
    Stop();
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::FormShow(TObject *Sender)
{
    MessageObject1->Connect();
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::TimerTimer(TObject *Sender)
{
    if( MessageObject1->IsConnected())
    {
  	    int Value = GetTickCount();
    	MTree Data;
        count++;
	    Data.append("Tick", Value );
        Data.append("From", FHost.c_str() );
        Publisher1->SendData( &Data );
    }
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::Button1Click(TObject *Sender)
{
    Timer->Enabled = false;

    loop = true;
    while( loop && Sending )
    {
    	MTree Data;
        int Value = GetTickCount();
        Data.append( "Tick", Value );
        Data.append("From", FHost.c_str() );        
        count++;
        Publisher1->SendData( &Data );
        Application->ProcessMessages();
    }
    Timer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TPublisherForm::BenchmarkTimerTimer(TObject *Sender)
{
    Label1->Caption = AnsiString( (count - Orgcount) )+" msg/sec";
    Orgcount = count;
}
//---------------------------------------------------------------------------

void __fastcall TPublisherForm::BitBtn1Click(TObject *Sender)
{
	UFC::FileStream* FS = new UFC::FileStream( "c:\\test.txt", O_CREAT|O_TRUNC|O_WRONLY );
    AnsiString Buffer;
    long OrgValue = GetTickCount();
    long UseMS;

    for( int i =0 ;i< 1000;i++ )
    {
        Buffer.printf("Line:%04d OrderTEXT12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n", i );
        FS->Write( Buffer.c_str(), Buffer.Length());
        if( i%100 == 0 )
            FS->Flush();
    }
//    FS->Flush();
    UseMS = GetTickCount() - OrgValue;
    delete FS;
    AnsiString Result = AnsiString( 1000*1000/UseMS )+" sync IO/sec";
    ShowMessage( Result );
}
//---------------------------------------------------------------------------

