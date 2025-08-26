//---------------------------------------------------------------------------

#pragma hdrstop

#include <System.NetEncoding.hpp>
#include "UnifyUtility.h"
#include "UFC.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
class Base64Obj
{
private:
	 TBase64Encoding* FBase64;
public:
	 Base64Obj( void )
	 {
		FBase64 = new TBase64Encoding();
	 }
	 ~Base64Obj( void )
	 {
		delete FBase64;
	 }
	 String __fastcall Encode( const String EncodeStr )
	 {
		AnsiString Str( EncodeStr );
		return FBase64->EncodeBytesToString( Str.c_str(), Str.Length());
	 }
	 String __fastcall Decode( const String DecodeStr )
	 {
		TByteDynArray Array = FBase64->DecodeStringToBytes( DecodeStr );
		String Result;
		try
		{
			Result = TEncoding::UTF8->GetString( Array );
		}
		catch( ... )
		{
			Result = DecodeStr;
		}
		return Result;
     }
};
//---------------------------------------------------------------------------
Base64Obj GBase64Obj;
//---------------------------------------------------------------------------
void __fastcall DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor UpCol, TColor DownCol )
{
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;

	vert [0] .x      = dRect.Left;
	vert [0] .y      = dRect.Top;
	vert [0] .Red    = GetRValue( (DWORD)UpCol)<<8;
	vert [0] .Green  = GetGValue( (DWORD)UpCol)<<8;
	vert [0] .Blue   = GetBValue( (DWORD)UpCol)<<8;
	vert [0] .Alpha  = 0x0000;

	vert [1] .x      = dRect.Right;
	vert [1] .y      = dRect.Bottom;
	vert [1] .Red    = GetRValue( (DWORD)DownCol)<<8;;
	vert [1] .Green  = GetGValue( (DWORD)DownCol)<<8;;
	vert [1] .Blue   = GetBValue( (DWORD)DownCol)<<8;;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill( canvas->Handle, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
}
//---------------------------------------------------------------------------
String __fastcall Base64Encode( const String& EncodeStr )
{
	return GBase64Obj.Encode( EncodeStr );
}
//---------------------------------------------------------------------------
String __fastcall Base64Decode( const String& DeCodeStr )
{
	return GBase64Obj.Decode( DeCodeStr );
}
//---------------------------------------------------------------------------
void __fastcall  WinSleep( int ms )
{
	int Times = ms/10;

	if(Times == 0 )
		Times = 1;
	for( int i = 0; i < Times; i ++ )
	{
		Application->ProcessMessages();
		UFC::SleepMS(10);
	}
}
//---------------------------------------------------------------------------
void __fastcall  WinSleep( int Times, int ms )
{
	for( int i = 0; i < Times; i ++ )
	{
		Application->ProcessMessages();
		UFC::SleepMS( ms );
	}
}
//---------------------------------------------------------------------------

