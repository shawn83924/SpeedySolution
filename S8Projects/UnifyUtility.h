//---------------------------------------------------------------------------

#ifndef UnifyUtilityH
#define UnifyUtilityH
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
extern void __fastcall DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor UpCol, TColor DownCol );
extern String __fastcall Base64Encode( const String& EncodeStr );
extern String __fastcall Base64Decode( const String& DecodeStr );
extern void __fastcall  WinSleep( int ms );
extern void __fastcall  WinSleep( int Times, int ms  );
//---------------------------------------------------------------------------
