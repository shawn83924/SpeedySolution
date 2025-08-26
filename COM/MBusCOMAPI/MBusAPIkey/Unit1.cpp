//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "TLicenseKey.h"
#include "NetUtility.h"
#pragma link "UFC.lib"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
AnsiString __fastcall TForm1::CreateKey( AnsiString& Vender, AnsiString& Subject )
{

	AnsiString Str,Result;
	Str.printf( "[%s]-[%s]-[MBusAPI]", Vender.UpperCase().c_str(), Subject.UpperCase().c_str() );

	UInt8 Buffer[16];
	UFC::MD5 KeyMD5( (UInt8*)(Str.c_str()), Str.Length() );
	memcpy( Buffer, KeyMD5.GetCode(), 16 );

	Int32 n1 = abs( UFC::NInt32::ToInt32( Buffer )      % 10000 );
	Int32 n2 = abs( UFC::NInt32::ToInt32( Buffer + 4 )  % 10000 );
	Int32 n3 = abs( UFC::NInt32::ToInt32( Buffer + 8 )  % 10000 );
	Int32 n4 = abs( UFC::NInt32::ToInt32( Buffer + 12 ) % 10000 );

	Result.printf( "%04d-%04d-%04d-%04d", n1, n2, n3, n4 );
	return Result;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FCreateNewKeyButtonClick(TObject *Sender)
{
	AnsiString Vender = FVendorEdit->Text;

	if( Vender.Length() == 0 )
	{
		ShowMessage(" Please input the vendor name." );
		return;
	}
	if( FMemo->Lines->Count == 0 )
	{
		ShowMessage(" Please input the subjects." );
		return;
	}
	SaveDialog->FileName = FVendorEdit->Text + ".reg";
	if( SaveDialog->Execute() == true )
	{
		AnsiString VenderStr;
		AnsiString KeyStr;
		AnsiString SubjectStr;
		TStringList* RegStrings = new TStringList();

		VenderStr.printf("[HKEY_LOCAL_MACHINE\\SOFTWARE\\MDBS\\MBusAPI\\%s]", FVendorEdit->Text.c_str() );
		RegStrings->Add( "Windows Registry Editor Version 5.00\n");
		RegStrings->Add( VenderStr );
		for( int i = 0; i < FMemo->Lines->Count; i ++ )
		{
			SubjectStr = FMemo->Lines->Strings[i];
			if( SubjectStr.Length() >= 3 )
			{
			   KeyStr.printf("\"%s\"=\"%s\"", SubjectStr.c_str(), CreateKey( Vender, SubjectStr).c_str() );
			   RegStrings->Add( KeyStr );
			}
        }

		RegStrings->SaveToFile( SaveDialog->FileName );
		delete RegStrings;
    }
}
//---------------------------------------------------------------------------

