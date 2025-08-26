//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "FMTConfig.h"
#include "LeaderBoard.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TLeaderBoardForm *LeaderBoardForm;
//------------------------------------------------------------------------------ ///< Upto 18 users
int NAME_POS_X[]={ 374,164,642, 160,160,160,160,160,160,160,160,160,160,160,160,160,160,160 };
int NAME_POS_Y[]={ 346-120,415-120,415-120,   440, 482, 524, 566, 608, 650, 692, 734, 776, 818, 860, 902, 944, 986, 1028 }; // 42 pixel
int NAME_WIDTH[]={ 200,140,140, 300,300,300,300,300,300,300, 300,300,300,300,300,300,300, 300 };

int PROFIT_POS_X[]={ 374,164,642, 480,480,480,480,480,480,480, 480,480,480,480,480,480,480, 480  };
int PROFIT_POS_Y[]={ 380-120,445-120,445-120,  440, 482, 524, 566, 608, 650, 692, 734, 776, 818, 860, 902, 944, 986, 1028 };
//{ 380-120,445-120,445-120, 570-120,   627-120-10,686-120-20,742-120-30,800-120-40,859-120-50,918-120-60 };
int PROFIT_WIDTH[]={ 200,140,140, 160,160,160,160,160,160,160, 160,160,160,160,160,160,160, 160 };

int STATUS_POS_X[]={ 506,254,730, 720,720,720,720,720,720,720, 720,720,720,720,720,720,720, 720 };
//int STATUS_POS_Y[]={ 380-120,445-120,445-120, 570-120,   627-120-15,686-120-30,742-120-45,800-120-60,859-120-75,918-120-90 };
int STATUS_POS_Y[]={ 189-120,284-120,284-120,  440, 482, 524, 566, 608, 650, 692, 734, 776, 818, 860, 902, 944, 986, 1028 };
//{ 189-120,284-120,284-120, 570-120,  627-120-10,686-120-20,742-120-30,800-120-40,859-120-50,918-120-60 };

int ICON_POS_X[]={365,160,637};
int ICON_POS_Y[]={146-120,282-120,282-120};
//---------------------------------------------------------------------------
const int WIN_W = 950;
const int WIN_H = 1084;//977;
//---------------------------------------------------------------------------
__fastcall TLeaderBoardForm::TLeaderBoardForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
	FBuffer = new TBitmap();
	FBuffer->Width = WIN_W;
	FBuffer->Height = WIN_H;
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND: Msg.Result = 1;
							return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
String __fastcall TLeaderBoardForm::StatusToString( int status )
{
	switch( status )
	{
		case 1:  return L"新入榜";
		case 2:  return L"名次上升";
		case 3:  return L"名次下降";
		case 4:  return L"排名持平";
		default: return L"---";
	}
}
//---------------------------------------------------------------------------
TColor __fastcall TLeaderBoardForm::StatusToColor( int status )
{
	switch( status )
	{
		case 1:  return (TColor)RGB( 242, 210, 31 );//新入榜
		case 2:  return (TColor)RGB( 255, 51 , 53 );//名次上升
		case 3:  return (TColor)RGB(  31, 221,113 );//名次下降
		case 4:  return (TColor)RGB( 102, 203,255 );//排名持平
		default: return clWhite;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::SetTopN( int N, const AnsiString& RecordStr )
{
	UFC::PStringList Record;
	int Status;

	Record.SetStrings( RecordStr.c_str(), "," );
	if( Record.ItemCount() >= 3 )
	{
		Status = Record[ Record.ItemCount() -2 ].ToInt();
		FNames[N-1]       = Record[ 0 ].c_str();
		FStatus[N-1]      = StatusToString( Status );
		FStatusCode[N-1]  = Status;
		FProfits[N-1]     = Record[ Record.ItemCount() -1 ].ToDouble();
	}
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::SaveProperties( void )
{
	g_Config.SetDesktopFormPos( "LeaderBoard", "Form", RoundFormEx );
	g_Config.SetDesktopBool( "LeaderBoard", "Visible", Visible );
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::LoadProperties( void )
{
	bool IsShow = g_Config.GetDesktopBool( "LeaderBoard", "Visible", false );

	g_Config.GetDesktopFormPos( "LeaderBoard", "Form", RoundFormEx );
	if( IsShow == true )
		Show();
	else
		Hide();
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::FormConstrainedResize(TObject *Sender, int &MinWidth,
		  int &MinHeight, int &MaxWidth, int &MaxHeight)
{
	MinWidth  = WIN_W + RoundFormEx->LeftFrameWidth + RoundFormEx->RightFrameWidth;
	MaxWidth  = MinWidth;
	MinHeight = WIN_H + RoundFormEx->BottomHight + RoundFormEx->CaptionHeight;
	MaxHeight = MinHeight;
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::DrawTop3( TCanvas* canvas, int i )
{
	TRect NameRect = TRect( NAME_POS_X[i],NAME_POS_Y[i],NAME_POS_X[i]+NAME_WIDTH[i],NAME_POS_Y[i]+40);
	TRect ProfitRect = TRect( PROFIT_POS_X[i] + 42 ,PROFIT_POS_Y[i]+4,PROFIT_POS_X[i] + 42 + PROFIT_WIDTH[i],PROFIT_POS_Y[i]+40);
	Currency    Balance( FProfits[i] );
	TTextFormat CenterFormats;
	TTextFormat LeftFormats;
	String      BalanceStr = CurrToStrF( Balance, ffNumber, 0 );

	CenterFormats<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
	LeftFormats<<tfLeft<<tfVerticalCenter<<tfEndEllipsis;
	///< Draw Name
	if( i == 0 )
		canvas->Font->Size  = 15;
	else
		canvas->Font->Size  = 12;
	canvas->Font->Color = clWhite;
	canvas->TextRect( NameRect, FNames[i], CenterFormats ); ///< Name text
	///< Draw profit
	canvas->Font->Size  = 12;
	canvas->Font->Color = (TColor)RGB( 254, 218, 124 );
	canvas->TextRect( ProfitRect, BalanceStr, LeftFormats ); ///< Profit text
	CoinImageList->Draw( canvas, PROFIT_POS_X[i],PROFIT_POS_Y[i], 0 ); ///< Draw $ icon
	///< Draw Status
	if( i == 0 )
	{
		canvas->Draw( ICON_POS_X[0],ICON_POS_Y[0], Top1Image->Picture->Graphic );
		Top3StatusImageList->Draw( canvas, STATUS_POS_X[0],STATUS_POS_Y[0], FStatusCode[0]-1 ); ///< Draw status icon
	}
	else if( i == 1 )
	{
		canvas->Draw( ICON_POS_X[1],ICON_POS_Y[1], Top2Image->Picture->Graphic );
		Top3StatusImageList->Draw( canvas, STATUS_POS_X[1],STATUS_POS_Y[1], FStatusCode[1]-1 ); ///< Draw status icon
	}
	else
	{
		canvas->Draw( ICON_POS_X[2],ICON_POS_Y[2], Top3Image->Picture->Graphic );
		Top3StatusImageList->Draw( canvas, STATUS_POS_X[2],STATUS_POS_Y[2], FStatusCode[2]-1 ); ///< Draw status icon
	}
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::DrawTop4To10( TCanvas* canvas, int i )
{
	TRect       ProfitRect = TRect( PROFIT_POS_X[i] ,PROFIT_POS_Y[i], PROFIT_POS_X[i] + PROFIT_WIDTH[i], PROFIT_POS_Y[i] + 40 );
	Currency    Balance( FProfits[i] );
	String      BalanceStr = CurrToStrF( Balance, ffNumber, 0 );
	String      No;
	int         IconIndex;
	TTextFormat RightFormats;

	RightFormats<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
    ///< Draw No.
	canvas->Font->Color = clWhite;
	No.printf( L"%d.", i + 1 );
	canvas->TextOutW( NAME_POS_X[i] - 92 , NAME_POS_Y[i], No );
	///< Draw name
	IconIndex = i - 3;
	if( IconIndex > 6 )
		IconIndex = 6;
	MedalImageList->Draw( canvas, NAME_POS_X[i] - 32 ,NAME_POS_Y[i], IconIndex ); ///< Draw Medal icon
	canvas->TextOutW( NAME_POS_X[i], NAME_POS_Y[i], FNames[i] );
	///< Draw profit
	canvas->Font->Color = (TColor)RGB( 254, 218, 124 );
	canvas->TextRect( ProfitRect, BalanceStr, RightFormats ); ///< Profit text
	///< Draw Status
	canvas->Font->Color = StatusToColor( FStatusCode[i] );
	StatusImageList->Draw( canvas, STATUS_POS_X[i],STATUS_POS_Y[i], FStatusCode[i]-1 ); ///< Draw status icon
	canvas->TextOutW( STATUS_POS_X[i] + 32 ,STATUS_POS_Y[i], FStatus[i] );
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::Paint( TCanvas* canvas )
{
	canvas->Draw(0,0, BKImage->Picture->Graphic );
	canvas->Brush->Style = bsClear;
	for( int i = 0; i < 18; i++ )
	{
		if( FStatusCode[i] != 0 )
		{
			if( i >= 3 )
				DrawTop4To10( canvas, i );
			else
				DrawTop3( canvas, i );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::FormPaint(TObject *Sender)
{
	Paint( FBuffer->Canvas );
	Canvas->Draw(0,0, FBuffer );
}
//---------------------------------------------------------------------------
void __fastcall TLeaderBoardForm::FormShow(TObject *Sender)
{
	for( int i = 0; i < 32; i++ )
	{
		FNames[i]   = L"";
		FProfits[i] = 0.0;
		FStatus[i]  = L"";
		FStatusCode[i]= 0;
	}
}
//---------------------------------------------------------------------------

