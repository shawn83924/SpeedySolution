//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "UFC.h"
#include "iniFile.h"
#include "AgentForm.h"
#include "MDComponentStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
#define MAIN_EXE       "兆期Atrade+.exe"
#define PREV_EXE       "兆期Atrade+Prev.exe"
#define NEW_EXE        "兆期Atrade+.ex_"
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
//------------------------------------------------------------------------------
/// 搬移檔案, 若檔案仍被佔用則重試 (最多約 10 秒).
/// 失敗時回傳 false, 並由 Err 傳回 GetLastError() 的錯誤碼.
static bool MoveFileRetry( const char* From, const char* To, DWORD& Err )
{
	Err = 0;
	for( int i = 0; i < 20; i++ )
	{
		if( MoveFileExA( From, To, MOVEFILE_REPLACE_EXISTING ) )
			return true;
		Err = GetLastError();
		if( Err != ERROR_SHARING_VIOLATION && Err != ERROR_ACCESS_DENIED && Err != ERROR_LOCK_VIOLATION )
			break;  ///< 不是被佔用造成的錯誤, 重試也沒用.
		Sleep( 500 );
		Application->ProcessMessages();
	}
	return false;
}
//------------------------------------------------------------------------------
/// 將 Files 內所有檔名去掉結尾 ExtLen 個字元後改名, 失敗的寫到 Memo 並回傳 false.
static bool RenameDownloadedFiles( UFC::PStringList& Files, int ExtLen )
{
	bool Success = true;

	for( int i=0;i< Files.ItemCount(); i ++)
	{
		UFC::AnsiString OrgFile = Files[i];
		UFC::AnsiString NewFile = UFC::AnsiString( OrgFile.c_str(), OrgFile.Length() - ExtLen );
		DWORD Err;

		if( MoveFileRetry( OrgFile.c_str(), NewFile.c_str(), Err ) == false )
		{
			String Msg;
			Msg.printf( L"無法更新檔案 %s: %s", String( NewFile.c_str() ).c_str(), SysErrorMessage( Err ).c_str() );
			AgentMain->Memo->Lines->Add( Msg );
			Success = false;
		}
	}
	return Success;
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
	const char* pMUTEX_NAME = "Atradeplus"; ///< 與 兆期Atrade+.cpp 主程式建立的 mutex 名稱一致, 勿更改

	CheckProcessTimer->Enabled = false;
	if(( hMutex = OpenMutexA( MUTEX_ALL_ACCESS, FALSE, pMUTEX_NAME ))!=NULL)
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
	bool Success = true;

	UpdateTimer->Enabled = false;
	RegisterDlls();

	if( UFC::FileExists( NEW_EXE ) == TRUE ) /// File 兆期Atrade+.ex_ exists
	{
		String VerStr;
		String Msg;
		DWORD  Err;
		///< Backup the old version. 兆期Atrade+.exe -> 兆期Atrade+Prev.exe
		FileVersion( MAIN_EXE, VerStr );
		AgentMain->Memo->Lines->Add( L"更新 " + Mdcomponentstrings_MD_SpeedyUnify_AppName);
		Msg.printf( L"移除舊版[%s]", VerStr.c_str() );
		AgentMain->Memo->Lines->Add( Msg );
		if( UFC::FileExists( MAIN_EXE ) == TRUE &&
			MoveFileRetry( MAIN_EXE, PREV_EXE, Err ) == false ) ///< Rename old EXE file
		{
			Msg.printf( L"無法備份舊版主程式 %s: %s", String( MAIN_EXE ).c_str(), SysErrorMessage( Err ).c_str() );
			AgentMain->Memo->Lines->Add( Msg );
			Success = false;
		}
		else if( MoveFileRetry( NEW_EXE, MAIN_EXE, Err ) == false ) // 兆期Atrade+.ex_ > 兆期Atrade+.exe
		{
			Msg.printf( L"無法更新主程式 %s: %s", String( MAIN_EXE ).c_str(), SysErrorMessage( Err ).c_str() );
			AgentMain->Memo->Lines->Add( Msg );
			///< 還原舊版, 避免主程式消失.
			if( UFC::FileExists( PREV_EXE ) == TRUE && UFC::FileExists( MAIN_EXE ) == FALSE )
				MoveFileExA( PREV_EXE, MAIN_EXE, MOVEFILE_REPLACE_EXISTING );
			Success = false;
		}
		else
		{
			FileVersion( MAIN_EXE, VerStr );
			Msg.printf( L"更新成新版[%s]", VerStr.c_str() );
			AgentMain->Memo->Lines->Add( Msg );
		}
	}
	else
		AgentMain->Memo->Lines->Add( Mdcomponentstrings_MD_SpeedyUnify_AppName + L"主程式並無更新." );
	///< Rename all *.*.download to *.*
	UFC::FileList( ".", "*.download", Files );
	if( RenameDownloadedFiles( Files, 9 ) == false )
		Success = false;
	///< Rename all *.*.upd to *.*
	UFC::FileList( ".", "*.upd", Files );
	if( RenameDownloadedFiles( Files, 4 ) == false )
		Success = false;
	///< Remove all old *.log file in log directory.
	UFC::FileList( "log", "*.log", Files );
	for( int i=0;i< Files.ItemCount(); i ++)
	{
		UFC::AnsiString DelFile = "log\\" + Files[i];
		UFC::RemoveFile( DelFile );
	}
	if( Success == true )
	{
		AgentMain->Memo->Lines->Add( L"程式更新完畢!" );
		AgentMain->Memo->Lines->Add( L"請重新啟動 " + Mdcomponentstrings_MD_SpeedyUnify_AppName + L"." );
		OKButton->Enabled = true;
	}
	else
	{
		String Msg = L"程式更新失敗!\n請確認 " + Mdcomponentstrings_MD_SpeedyUnify_AppName +
					 L" 已完全關閉後重新執行更新, 若仍失敗請重新安裝.";
		AgentMain->Memo->Lines->Add( L"程式更新失敗!" );
		OKButton->Enabled = true;
		MessageDlg( Msg, mtError, TMsgDlgButtons() << mbOK, 0 );
	}
}
//---------------------------------------------------------------------------

