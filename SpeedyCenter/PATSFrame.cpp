//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "PATSFrame.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PerformanceGauge"
#pragma resource "*.dfm"
TConnectionFrame *ConnectionFrame;
//---------------------------------------------------------------------------
__fastcall TConnectionFrame::TConnectionFrame(TComponent* Owner)
:TFrame(Owner),
FPATSExecCount( 0 ),
FPATSOrderCount( 0 ),
FPATSTPMax( 0.0 ),
FPATSLastUpdateTick( 0 ),
FTimeCounter( 0 )
{
	TimeoutTimer->Enabled = true;
}
//---------------------------------------------------------------------------
bool __fastcall TConnectionFrame::UpdateLineStat( int Stat )
{
	bool IsBroken = false;

	if( Stat == 1 ) ///< Connected.
	{
		PATSStateText->Color = clGreen;
		PATSStateText->Caption = Scstrings_MAIN_SESSION_STAT_CONNECTED;
	}
	else
	{
		if( PATSStateText->Color == clGreen )
			IsBroken = true;
		PATSStateText->Color = clRed;
		PATSStateText->Caption = Scstrings_MAIN_SESSION_STAT_BROKEN;
	}
	FTimeCounter = 0;
	return IsBroken;
}
//---------------------------------------------------------------------------
bool __fastcall TConnectionFrame::UpdateInfo( int ECount ,int OCount, int Stat, DWORD Tick )
{
	bool IsBroken = UpdateLineStat( Stat );
	if( FPATSLastUpdateTick != 0 )
	{
		int TimeDiff  = Tick   - FPATSLastUpdateTick;
		int OrderDiff = OCount - FPATSOrderCount;
		int ExecDiff  = ECount - FPATSExecCount;
		if( TimeDiff <= 0 )
			TimeDiff = 1000;
		float OThroughput = (float)OrderDiff*1000/(float)TimeDiff;
		float EThroughput = (float)ExecDiff*1000/(float)TimeDiff;
		String Msg;

		if( OThroughput > FPATSTPMax )
		{
			FPATSTPMax = OThroughput;
			Msg.printf( L"%0.2f µ§/¬í", FPATSTPMax );
			PATSMaxTPLabel->Caption = Msg;
		}
		Msg.printf( L"%0.2f µ§/¬í", OThroughput );
		PATSOrderTPLabel->Caption = Msg;
		Msg.printf( L"%0.2f µ§/¬í", EThroughput );
		PATSExecTPLabel->Caption = Msg;
		PatsPerformanceGauge->Add2( EThroughput*100, OThroughput*100   );
	}
	FPATSLastUpdateTick = Tick;
	if( FPATSOrderCount != OCount )
	{
		FPATSOrderCount = OCount;
		PATSOrderTotalLabel->Caption = String( OCount ) + L"µ§";
	}
	if( FPATSExecCount != ECount )
	{
		FPATSExecCount  = ECount;
		PATSExecTotalLabel->Caption = String( ECount ) + L"µ§";
	}
	FTimeCounter = 0;
	return IsBroken;
}
//---------------------------------------------------------------------------
void __fastcall TConnectionFrame::TimeoutTimerTimer(TObject *Sender)
{
	FTimeCounter ++;
	if( FTimeCounter > 5 )
		UpdateLineStat( 0 );
}
//---------------------------------------------------------------------------
