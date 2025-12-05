//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "UFC.h"
#include "iniFile.h"
#include "AgentForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
#define MAIN_EXE       "SpeedyUnify.exe"
#define PREV_EXE       "SpeedyUnifyPrev.exe"
#define NEW_EXE        "SpeedyUnify.ex_"
#define UPDATE_FILES   "FileList.ini"
//---------------------------------------------------------------------------
TAgentMain *AgentMain;
//------------------------------------------------------------------------------
void FileVersion( const char* FileName, String& VerStr )
{   // Reading version number....
	DWORD Version,n;
	AnsiString CheckFile( FileName );

	if( (n = GetFileVersionInfoSizeA( CheckFile.c_str(), NULL)) > 0 ) ///< File contains version information.
	{
		char *pBuf = new char[ n ];
		VS_FIXEDFILEINFO *vsinfo;
		UINT Len;

		GetFileVersionInfoA( CheckFile.c_str(), 0, n, pBuf);
		if( VerQueryValueA( pBuf, "\\", (void **)&vsinfo, &Len))
		{
			int MajorVersion = HIWORD(vsinfo->dwFileVersionMS)%100;
			int MinorVersion = LOWORD(vsinfo->dwFileVersionMS)%100;
			int Release      = HIWORD(vsinfo->dwFileVersionLS)%100;
			int Build        = LOWORD(vsinfo->dwFileVersionLS)%100;

			VerStr.printf( L"%02d.%02d.%02d", MajorVersion, MinorVersion, Release );
		}
		else
			VerStr = L"00.00.00";
		delete [] pBuf;
	}
	else
		VerStr = L"00.00.00";
}
//------------------------------------------------------------------------------
void RegisterDlls( void )
{
	UFC::UiniFile ini( UPDATE_FILES, true );
	UFC::Section* iniSection;
	UFC::AnsiString SectionName,Value;

	for( int i = 0; i < ini.SectionCount(); i++ )
	{
		if( (iniSection = ini.GetSection( i )) != NULL )
		{
			SectionName = iniSection->GetSectionName();
			if( SectionName.AnsiCompare("Host") != 0 &&
				iniSection->GetValue( "Register", Value )== true &&  Value.ToInt() == 1 )
			{
				UFC::AnsiString CMD;
				String dllName( SectionName.c_str() );
				String Msg,VerStr;

				Msg.printf( L"註冊動態連結庫:%s", dllName );
				AgentMain->Memo->Lines->Add( Msg );
				FileVersion( SectionName.c_str(), VerStr );
				Msg.printf( L"版本[%s]",VerStr );
				AgentMain->Memo->Lines->Add( Msg );
				Application->ProcessMessages();
				CMD.Printf( "regsvr32 /s %s", SectionName.c_str() );
				system( CMD.c_str());
			}
		}
	}
}
//---------------------------------------------------------------------------
__fastcall TAgentMain::TAgentMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAgentMain::OKButtonClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TAgentMain::FormShow(TObject *Sender)
{
	Memo->Lines->Clear();
	Memo->Lines->Add( L"等待" + Mdcomponentstrings_MD_SpeedyUnify_AppName + L"程式停止...");
	OKButton->Enabled = false;
	CheckProcessTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TAgentMain::CheckProcessTimerTimer(TObject *Sender)
{
	HANDLE hMutex;
	char* pMUTEX_NAME ="SpeedyUnify";

	CheckProcessTimer->Enabled = false;
	if(( hMutex = OpenMutex( MUTEX_ALL_ACCESS, FALSE, pMUTEX_NAME ))!=NULL)
	{
		 WaitForSingleObject(hMutex, INFINITE); /// Wait for application stopped.
		 CloseHandle( hMutex );
	}
	Sleep( 500 );
	Memo->Lines->Add( Mdcomponentstrings_MD_SpeedyUnify_AppName + L"程式已停止.");
	UpdateTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TAgentMain::UpdateTimerTimer(TObject *Sender)
{
	UFC::PStringList Files;

	UpdateTimer->Enabled = false;
	RegisterDlls();

	if( UFC::FileExists( NEW_EXE ) == TRUE ) /// File SpeedyUnify.ex_ exists
	{
		String VerStr;
		String Msg;
		///< Backup the old version. SpeedyUnify.exe -> SpeedyUnifyPrev.exe
		FileVersion( MAIN_EXE, VerStr );
		AgentMain->Memo->Lines->Add( L"更新 " + Mdcomponentstrings_MD_SpeedyUnify_AppName);
		Msg.printf( L"移除舊版[%s]",VerStr );
		AgentMain->Memo->Lines->Add( Msg );
		if( MoveFileEx( MAIN_EXE, PREV_EXE, MOVEFILE_REPLACE_EXISTING ) )///< Rename old EXE file
		{	// SpeedyUnify.ex_ > SpeedyUnify.exe
			MoveFile( NEW_EXE, MAIN_EXE );  ///< rename to new
			FileVersion( MAIN_EXE, VerStr );
			Msg.printf( L"更新成新版[%s]",VerStr );
			AgentMain->Memo->Lines->Add( Msg );
		}
	}
	else
		AgentMain->Memo->Lines->Add( Mdcomponentstrings_MD_SpeedyUnify_AppName + L"主程式並無更新." );
	///< Rename all *.*.download to *.*
	UFC::FileList( ".", "*.download", Files );
	for( int i=0;i< Files.ItemCount(); i ++)
	{
		UFC::AnsiString OrgFile = Files[i];
		UFC::AnsiString NewFile = UFC::AnsiString( OrgFile.c_str(), OrgFile.Length() - 9 );

		MoveFileEx( OrgFile.c_str(), NewFile.c_str(), MOVEFILE_REPLACE_EXISTING );
	}
	///< Rename all *.*.upd to *.*
	UFC::FileList( ".", "*.upd", Files );
	for( int i=0;i< Files.ItemCount(); i ++)
	{
		UFC::AnsiString OrgFile = Files[i];
		UFC::AnsiString NewFile = UFC::AnsiString( OrgFile.c_str(), OrgFile.Length() - 4 );

		MoveFileEx( OrgFile.c_str(), NewFile.c_str(), MOVEFILE_REPLACE_EXISTING );
	}
	///< Remove all old *.log file in log directory.
	UFC::FileList( "log", "*.log", Files );
	for( int i=0;i< Files.ItemCount(); i ++)
	{
		UFC::AnsiString DelFile = "log\\" + Files[i];
		UFC::RemoveFile( DelFile );
	}
	AgentMain->Memo->Lines->Add( L"程式更新完畢!" );
	AgentMain->Memo->Lines->Add( L"請重新啟動 " + Mdcomponentstrings_MD_SpeedyUnify_AppName + L"." );
	OKButton->Enabled = true;
}
//---------------------------------------------------------------------------

