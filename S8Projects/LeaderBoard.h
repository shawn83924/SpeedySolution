//---------------------------------------------------------------------------

#ifndef LeaderBoardH
#define LeaderBoardH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include "RoundFormEx.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.jpeg.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Imaging.pngimage.hpp>
//---------------------------------------------------------------------------
class TLeaderBoardForm : public TForm
{
__published:	// IDE-managed Components
	TRoundFormEx *RoundFormEx;
	TImage *BKImage;
	TImageList *CoinImageList;
	TImage *Top1Image;
	TImage *Top2Image;
	TImage *Top3Image;
	TImageList *Top3StatusImageList;
	TImageList *StatusImageList;
	TImageList *MedalImageList;
	void __fastcall FormConstrainedResize(TObject *Sender, int &MinWidth, int &MinHeight,
          int &MaxWidth, int &MaxHeight);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	TBitmap* FBuffer;
	String FNames[32];
	String FStatus[32];
	double FProfits[32];
    int    FStatusCode[32];
	String __fastcall StatusToString( int status );
	TColor __fastcall StatusToColor( int status );
	void __fastcall Paint( TCanvas* canvas );
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall DrawTop3( TCanvas* canvas, int i );
	void __fastcall DrawTop4To10( TCanvas* canvas, int i );
public:		// User declarations

	__fastcall TLeaderBoardForm(TComponent* Owner);
	void __fastcall SetTopN( int N, const AnsiString& RecordStr );
	void __fastcall LoadProperties( void );
	void __fastcall SaveProperties( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TLeaderBoardForm *LeaderBoardForm;
//---------------------------------------------------------------------------
#endif
