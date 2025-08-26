//---------------------------------------------------------------------------

#ifndef RoundFormExResH
#define RoundFormExResH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
class PACKAGE TRoundFormExRes : public TComponent
{
private:
	bool               FTransparent;
	int                FCaptionHeight;
	int                FBottomHight;
	int                FLeftFrameWidth;
	int                FRightFrameWidth;
	Graphics::TBitmap *FTopBmp;
	Graphics::TBitmap *FLeftBmp;
	Graphics::TBitmap *FRightBmp;
	Graphics::TBitmap *FBottomBmp;
	Graphics::TBitmap *FMinBtnBmp;
	Graphics::TBitmap *FMaxBtnBmp;
	Graphics::TBitmap *FRestoreBtnBmp;
	Graphics::TBitmap *FCloseBtnBmp;
	Graphics::TBitmap *FLockBtnBmp;
	Graphics::TBitmap *FLockedBtnBmp;
	Graphics::TBitmap *FIconBmp;
private:
	void __fastcall Loaded( void );
	void __fastcall SetTopBmp(Graphics::TBitmap *Value);
	void __fastcall SetLeftBmp(Graphics::TBitmap *Value);
	void __fastcall SetRightBmp(Graphics::TBitmap *Value);
	void __fastcall SetBottomBmp(Graphics::TBitmap *Value);
	void __fastcall SetCloseBoxBmp(Graphics::TBitmap *Value);
	void __fastcall SetMinimizeBmp(Graphics::TBitmap *Value);
	void __fastcall SetMaximizeBmp(Graphics::TBitmap *Value);
	void __fastcall SetRestoreBmp(Graphics::TBitmap *Value);
	void __fastcall SetLockBmp(Graphics::TBitmap *Value);
	void __fastcall SetLockedBmp(Graphics::TBitmap *Value);
	void __fastcall SetIconBmp(Graphics::TBitmap *Value);
	void __fastcall SetTransparent( bool Locked );
public:
	__fastcall TRoundFormExRes(TComponent* Owner);
__published:
	__property int                CaptionHeight     ={read=FCaptionHeight};
	__property int                BottomHight       ={read=FBottomHight};
	__property int                LeftFrameWidth    ={read=FLeftFrameWidth};
	__property int                RightFrameWidth   ={read=FRightFrameWidth};
	__property bool               ButtonTransparent ={read=FTransparent, write=SetTransparent };
	__property Graphics::TBitmap* CaptionGraph      ={read=FTopBmp, write=SetTopBmp};
	__property Graphics::TBitmap* LeftGraph         ={read=FLeftBmp, write=SetLeftBmp};
	__property Graphics::TBitmap* RightGraph        ={read=FRightBmp, write=SetRightBmp};
	__property Graphics::TBitmap* BottomGraph       ={read=FBottomBmp, write=SetBottomBmp};

	__property Graphics::TBitmap* CloseGraph        ={read=FCloseBtnBmp, write=SetCloseBoxBmp};
	__property Graphics::TBitmap* MinimizeGraph     ={read=FMinBtnBmp, write=SetMinimizeBmp};
	__property Graphics::TBitmap* MaximizeGraph     ={read=FMaxBtnBmp, write=SetMaximizeBmp};
	__property Graphics::TBitmap* RestoreGraph      ={read=FRestoreBtnBmp, write=SetRestoreBmp};
	__property Graphics::TBitmap* LockGraph         ={read=FLockBtnBmp, write=SetLockBmp};
	__property Graphics::TBitmap* LockedGraph       ={read=FLockedBtnBmp, write=SetLockedBmp};
	__property Graphics::TBitmap* IconGraph         ={read=FIconBmp, write=SetIconBmp};
};
//---------------------------------------------------------------------------
#endif
