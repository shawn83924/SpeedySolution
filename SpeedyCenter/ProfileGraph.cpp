//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "ProfileGraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PerformanceGauge"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TProfileFrame *ProfileFrame;
//---------------------------------------------------------------------------
__fastcall TProfileFrame::TProfileFrame(TComponent* Owner)
: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
__fastcall TProfileFrame::TProfileFrame(TWinControl* Owner, int ID, const AnsiString& Title  )
:TFrame(Owner)
,FPrevCount(0)
,FPrevTick(0)
,FMaxThroughput(0.0)
{
	AnsiString FrameName = "Frame" + AnsiString(ID);

	Parent = Owner;
	Name = FrameName;
	TitleStaticText->Caption = Title;
	Left = ID%2 * Width;
	Top  = ID/2 * Height;
	PerformanceGauge->MaxValue = 1000;
	PerformanceGauge->Decimal = 1;
	FBuffer = new Graphics::TBitmap();
}
//---------------------------------------------------------------------------
void __fastcall TProfileFrame::SetBackgroundColor( TColor Color )
{
	this->PerformanceGauge->BackgroundColor = Color;
}
//---------------------------------------------------------------------------
void __fastcall TProfileFrame::SetGridColor( TColor Color )
{
	this->PerformanceGauge->GridColor = Color;
}
//---------------------------------------------------------------------------
void __fastcall TProfileFrame::SetLineColor( TColor Color )
{
	this->PerformanceGauge->LineColor = Color;
}
//---------------------------------------------------------------------------
void __fastcall TProfileFrame::SetData( int TickNow, const char* DataStr )
{
	TStringList* Strs = new TStringList();

	Strs->Delimiter = ',';
	Strs->DelimitedText = DataStr;
	if( Strs->Count == 5 )
	{
		int        Count = Strs->Strings[0].ToInt();
		__int64    Total = StrToInt64( Strs->Strings[1] );
		Currency   MaxVal( Strs->Strings[2] );
		Currency   MinVal( Strs->Strings[3] );
		Currency   AvgVal( (int)(Total/(__int64)Count) );
		Currency   CountVal( Count );
		float      ThroughputNow = 0.0;
		AnsiString Value;

		if( !(FPrevCount == 0 && FPrevTick ==0) )///<  Not First time call this func.
		{
			ThroughputNow = (float)((Count-FPrevCount)*1000)/ (float)(TickNow-FPrevTick);
			if( ThroughputNow > FMaxThroughput )
				FMaxThroughput = ThroughputNow;
		}
		FPrevTick  = TickNow;
		FPrevCount = Count;
		ValueListEditor->Cells[1][0] = CurrToStrF( MaxVal, ffNumber, 0)+" us";///< Max
		ValueListEditor->Cells[1][1] = CurrToStrF( MinVal, ffNumber, 0)+" us";///< Min
		ValueListEditor->Cells[1][2] = CurrToStrF( AvgVal, ffNumber, 0)+" us";///< Avg
		ValueListEditor->Cells[1][3] = Strs->Strings[4];                      ///< Key of Max value
		Value.printf("%0.2f msg/sec", ThroughputNow );
		PerformanceGauge->Add( ThroughputNow*100 );
		ValueListEditor->Cells[1][4] = Value;                                 ///< Throughput
		Value.printf("%0.2f msg/sec", FMaxThroughput );
		ValueListEditor->Cells[1][5] = Value;                                 ///< Throughput Max
		ValueListEditor->Cells[1][6] = CurrToStrF( CountVal, ffNumber, 0);    ///< Total Count
	}
	delete Strs;
}
//---------------------------------------------------------------------------
void __fastcall TProfileFrame::ValueListEditorDrawCell(TObject *Sender,
	  int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
	AnsiString Text = ValueListEditor->Cells[ACol][ARow];
	TRect BMPRect( 0,0, Rect.Width(), Rect.Height());

	FBuffer->Width = Rect.Width();
	FBuffer->Height = Rect.Height();
	FBuffer->Canvas->Brush->Color = ValueListEditor->Color;
	FBuffer->Canvas->Font = ValueListEditor->Font;
	FBuffer->Canvas->FillRect( BMPRect );
	if( ACol == 0 )
	{
		SetTextColor( FBuffer->Canvas->Handle,(COLORREF)clBlack );
		DrawTextExA( FBuffer->Canvas->Handle,Text.c_str(),Text.Length(),&BMPRect,DT_SINGLELINE|DT_VCENTER|DT_LEFT,NULL);
	}
	else
	{
		if( ARow == 0 )
			SetTextColor( FBuffer->Canvas->Handle,(COLORREF)clRed );
		else
			SetTextColor( FBuffer->Canvas->Handle,(COLORREF)clNavy );
		DrawTextExA( FBuffer->Canvas->Handle,Text.c_str(),Text.Length(),&BMPRect,DT_SINGLELINE|DT_VCENTER|DT_RIGHT,NULL);
	}
	ValueListEditor->Canvas->Draw( Rect.Left, Rect.top, FBuffer );
}
//---------------------------------------------------------------------------

