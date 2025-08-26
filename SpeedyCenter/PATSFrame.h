//---------------------------------------------------------------------------

#ifndef PATSFrameH
#define PATSFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "PerformanceGauge.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TConnectionFrame : public TFrame
{
__published:	// IDE-managed Components
	TPanel *Panel5;
	TLabel *Label50;
	TLabel *Label51;
	TLabel *Label52;
	TLabel *Label53;
	TLabel *PATSMaxTPLabel;
	TLabel *PATSOrderTPLabel;
	TLabel *PATSExecTPLabel;
	TLabel *Label54;
	TLabel *PATSOrderTotalLabel;
	TLabel *Label56;
	TLabel *PATSExecTotalLabel;
	TStaticText *PATSStateText;
	TPerformanceGauge *PatsPerformanceGauge;
	TStaticText *TitleStaticText;
	TTimer *TimeoutTimer;
	void __fastcall TimeoutTimerTimer(TObject *Sender);
private:	// User declarations
	int   FPATSExecCount;
	int   FPATSOrderCount;
	float FPATSTPMax;
	DWORD FPATSLastUpdateTick;
	int   FTimeCounter;
public:		// User declarations
	__fastcall TConnectionFrame(TComponent* Owner);
	bool __fastcall UpdateInfo( int ECount ,int OCount, int Stat, DWORD Tick );
	bool __fastcall UpdateLineStat( int Stat );
};
//---------------------------------------------------------------------------
extern PACKAGE TConnectionFrame *ConnectionFrame;
//---------------------------------------------------------------------------
#endif
