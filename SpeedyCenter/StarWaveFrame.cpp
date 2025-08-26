//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "StarWaveFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PerformanceGauge"
#pragma resource "*.dfm"
TSWFrame *SWFrame;
//---------------------------------------------------------------------------
String GUnits[]={L"kb/sec",L" ",L"bytes",L"bytes",L" ",L" ",L" ",L" ",L" ",L" ",L" "};
//---------------------------------------------------------------------------
__fastcall TSWFrame::TSWFrame(TComponent* Owner)
: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
__fastcall TSWFrame::TSWFrame(TWinControl* Owner, const String& Hostname )
: TFrame(Owner)
{
	String FrameName = L"SWFrame" + String( UFC::GetTickCountMS());

	Parent = Owner;
	Name = FrameName;
	TitleStaticText->Caption = L"StarWave@"+ Hostname;
	PerformanceGauge->MaxValue = 100000;
	PerformanceGauge->Decimal = 0;
	FBuffer = new Graphics::TBitmap();
}
//---------------------------------------------------------------------------
void __fastcall TSWFrame::ValueListEditorDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State)
{
	String Text = ValueListEditor->Cells[ACol][ARow];
	TRect BMPRect( 0,0, Rect.Width(), Rect.Height());

	FBuffer->Width  = Rect.Width();
	FBuffer->Height = Rect.Height();
	FBuffer->Canvas->Brush->Color = ValueListEditor->Color;
	FBuffer->Canvas->Font = ValueListEditor->Font;
	FBuffer->Canvas->FillRect( BMPRect );
	if( ACol == 0 )
	{
		TTextFormat Formats;

		Formats<<tfSingleLine<<tfLeft<<tfVerticalCenter;
		SetTextColor( FBuffer->Canvas->Handle,(COLORREF)clBlack );
		FBuffer->Canvas->TextRect( BMPRect, Text, Formats  );
	}
	else
	{
		TRect LeftRect( 0,0, Rect.Width() - 55 , Rect.Height());
		TRect RightRect( Rect.Width() - 50,0, Rect.Width(), Rect.Height());
		TTextFormat Formats;

		if( ARow == 0 )
			SetTextColor( FBuffer->Canvas->Handle,(COLORREF)clRed );
		else
			SetTextColor( FBuffer->Canvas->Handle,(COLORREF)clNavy );
		Formats<<tfSingleLine<<tfRight<<tfVerticalCenter;
		FBuffer->Canvas->TextRect( LeftRect, Text, Formats  );
		Formats.Clear();
		Formats<<tfSingleLine<<tfLeft<<tfVerticalCenter;
		FBuffer->Canvas->TextRect( RightRect, GUnits[ARow], Formats  );
	}
	ValueListEditor->Canvas->Draw( Rect.Left, Rect.top, FBuffer );
}
//---------------------------------------------------------------------------
void __fastcall TSWFrame::UpdateData( MTree *Tree )
{
	String Value;
	double Bandwidth;
	UFCType::UInt32 UDPRecv,UDPCount,TCPOut;
	UFCType::Int32 MDQueueCount,CMDQueueCount,Connections;

	if( Tree->get( "BandWidth", Bandwidth ) &&
		Tree->get( "UDPCount", UDPCount ) &&
		Tree->get( "UDPRecv", UDPRecv ) &&
		Tree->get( "TCPOUT", TCPOut ) &&
		Tree->get( "MDQueueCount", MDQueueCount ) &&
		Tree->get( "CmdQueueCount", CMDQueueCount ) &&
		Tree->get( "Connections", Connections ) )
	{
		Currency   CUDPRecv( (double)UDPRecv );
		Currency   CUDPCount( (double)UDPCount );
		Currency   CTCPOut( (double)TCPOut );

		PerformanceGauge->Add( Bandwidth*800 );
		Value.printf(L"%0.3f", Bandwidth*8 );
		ValueListEditor->Cells[1][0] = Value;///< Bandwidth
		ValueListEditor->Cells[1][1] = CurrToStrF( CUDPCount, ffNumber, 0 );///< UDP Count
		ValueListEditor->Cells[1][2] = CurrToStrF( CUDPRecv, ffNumber, 0 );///< UDP Recv
		ValueListEditor->Cells[1][3] = CurrToStrF( CTCPOut, ffNumber, 0 );///< TCP Output
		ValueListEditor->Cells[1][4] = String(MDQueueCount);///< MarketData Queue
		ValueListEditor->Cells[1][5] = String(CMDQueueCount);///< Command Queue
		ValueListEditor->Cells[1][6] = String(Connections);///< Connections
	}
}
//---------------------------------------------------------------------------
