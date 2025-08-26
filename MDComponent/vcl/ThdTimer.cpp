////////////////////////////////////////////////////////////////////////////////
// TThreadedTimer component
//
// Component which allows to have an accurate timer (based on Delphi
// Developper's Journal, issue of May 1996)
//
// by Alan GARNY
// Modify by Yuan
////////////////////////////////////////////////////////////////////////////////
#include <vcl.h>
#pragma hdrstop
#include "ThdTimer.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TThreadedTimer *)
{
    new TThreadedTimer(NULL);
}
//---------------------------------------------------------------------------
// TThreadedTimer: constructor

__fastcall TThreadedTimer::TThreadedTimer(TComponent *aOwner)
   : TComponent(aOwner),
     FEnabled(true),
     FInterval(1000),
     FOnTimer(NULL),
     FThreadPriority(tpNormal)
{
   FTimerThread = new TTimerThread(false);
   FTimerThread->OwnerTimer = this;
   FTimerThread->FreeOnTerminate = true;
}
//---------------------------------------------------------------------------
// TThreadedTimer: destructor

__fastcall TThreadedTimer::~TThreadedTimer()
{
   FEnabled = false;

   FTimerThread->Terminate( );
}
//---------------------------------------------------------------------------
void __fastcall TThreadedTimer::WaitFor()
{
   FTimerThread->WaitFor();
}
//---------------------------------------------------------------------------
void __fastcall TThreadedTimer::Timer()
{
   if (FOnTimer)
      FOnTimer(this);
}

//---------------------------------------------------------------------------
void __fastcall TThreadedTimer::UpdateTimer()
{
   if (!FTimerThread->Suspended)
      FTimerThread->Suspend();

   if (FInterval && FEnabled && FOnTimer)
      if (FTimerThread->Suspended)
         FTimerThread->Resume();
}

//---------------------------------------------------------------------------
void __fastcall TThreadedTimer::SetEnabled(const bool Value)
{
   if (Value != FEnabled)
   {
      FEnabled = Value;

      UpdateTimer();
   }
}

//---------------------------------------------------------------------------
void __fastcall TThreadedTimer::SetInterval(const WORD Value)
{
   if (Value != FInterval)
   {
      FInterval = Value;

      UpdateTimer();
   }
}

//---------------------------------------------------------------------------
void __fastcall TThreadedTimer::SetOnTimer(const TNotifyEvent Value)
{
   FOnTimer = Value;

   UpdateTimer();
}

//---------------------------------------------------------------------------
void __fastcall TThreadedTimer::SetThreadPriority(const TThreadPriority Value)
{
   if (Value != FThreadPriority)
   {
      FThreadPriority = Value;

      UpdateTimer();
   }
}

//---------------------------------------------------------------------------
// TTimerThread  body

void __fastcall TTimerThread::Execute()
{
   Priority = OwnerTimer->ThreadPriority;

   do
   {
      // when Thread wake up trigger the OnTimer event
      SleepEx(OwnerTimer->Interval, false) ;
      Synchronize(&OwnerTimer->Timer);

   } while (!Terminated);
}

//---------------------------------------------------------------------------
// Registration

namespace Thdtimer
{
   void __fastcall PACKAGE Register()
   {
      TComponentClass classes[1] = {__classid(TThreadedTimer)};
      RegisterComponents("MD", classes, 0);
   }
}

//---------------------------------------------------------------------------
// End of file
//---------------------------------------------------------------------------

