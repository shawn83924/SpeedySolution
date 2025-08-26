//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "MessageObject.h"
#include "Publisher.h"
#include <ExtCtrls.hpp>
#include <ScktComp.hpp>
#include "ThdTimer.h"
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TPublisherForm : public TForm
{
__published:	// IDE-managed Components
    TPublisher *Publisher1;
    TMessageObject *MessageObject1;
    TRadioGroup *RadioGroup1;
    TTimer *BenchmarkTimer;
    TLabel *Label1;
    TThreadedTimer *Timer;
    TBitBtn *BitBtn1;
    void __fastcall RadioGroup1Click(TObject *Sender);
    void __fastcall MessageObject1Connected(TObject *Sender);
    void __fastcall MessageObject1Error(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall BenchmarkTimerTimer(TObject *Sender);
    void __fastcall MessageObject1Disconnected(TObject *Sender);
    void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
    bool loop;
    bool stop;
    unsigned int count;
    unsigned int Orgcount;
    AnsiString FHost;
    void __fastcall Stop();    
public:		// User declarations
    __fastcall TPublisherForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPublisherForm *PublisherForm;
//---------------------------------------------------------------------------
#endif
