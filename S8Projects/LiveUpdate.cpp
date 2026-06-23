//---------------------------------------------------------------------------
#include <WideStrUtils.hpp>
#include "FMTConfig.h"
#include <IdHTTP.hpp>
//------------------------------------------------------------------------------
//
//  Functions about live update.
//
//---------------------------------------------------------------------------
const UFC::AnsiString MAIN_PROGRAM_NAME = "SpeedyUnify.exe";
const UFC::AnsiString MAIN_PROGRAM_TEMP_NAME = "SpeedyUnify.ex_";
//---------------------------------------------------------------------------
namespace UFC
{
	extern int UncompressFile(char* SourceFileName, char* UncompressFileName);
}
//------------------------------------------------------------------------------
void Config::LoadFromInIFile()
{
	UFC::AnsiString Value;

	///< Loade Trade Date info.
	LoadOpenCloseTime( "FMTMD.ini" );
	///< Get the update base url and the update files list.
	LoadUpdateSetting( "FMTMD.ini" );
	///< Get CA URL
//	LoadCASetting( SettingsFile );
}
//------------------------------------------------------------------------------
void Config::LoadUpdateSetting( const char* FileName )
{
	UFC::UiniFile   MDSettingsFile( FileName, true );
	UFC::AnsiString Value, ValueName;

	m_BaseURLs.Clear();
	m_BaseURLIndex = 0;
	///< Get the update base url and the update files list.
	if( MDSettingsFile.GetValue( "UpdateHost", "URL", Value ) == TRUE )
	{
		if( Value.LastChar() != '/' )
			Value += "/";
		m_BaseURLs.Add( Value );
		for( int i = 1;;i++)
		{
			ValueName.Printf( "URL%d", i );
			if( MDSettingsFile.GetValue( "UpdateHost", ValueName, Value ) == FALSE )
				break;
			if( Value.LastChar() != '/' )
				Value += "/";
			m_BaseURLs.Add( Value );
		}
	}
	else
	{
		m_BaseURLs.Add( "https://mdbs-sharespace.mdevelop.com/SpeedyRelease/emegafuture-pro/" );
		m_BaseURLs.Add( "https://unify-files.mpublic.net/");
	}
}
//------------------------------------------------------------------------------
String Config::GetUpdateBaseURL( void )
{
	if( m_BaseURLIndex < 0 || m_BaseURLIndex >= m_BaseURLs.ItemCount() )
		m_BaseURLIndex = 0;
	m_BaseURLIndex++;
	return m_BaseURLs[ m_BaseURLIndex - 1].c_str();
}
//------------------------------------------------------------------------------
int Config::GetVersion( void  )
{
	AnsiString ThisFile = Application->ExeName;

	return FileVersion( ThisFile.c_str());
}
//------------------------------------------------------------------------------
DWORD Config::FileVersion( const char* FileName, const char* FileName2 )
{   // Reading version number....
	DWORD Version,n;
	AnsiString CheckFile( FileName );

	if( FileExists( CheckFile ) == false )
	{
		CheckFile = FileName2;
		if( FileExists( CheckFile ) == true )
			Version =  1000000;///< 1.00.00.00
		else
			Version =  0000000;///< 0.00.00.00
	}
	else
		Version =  1000000;///< 1.00.00.00
	if( (n = GetFileVersionInfoSizeA( CheckFile.c_str(), NULL)) > 0 ) ///< File contains version information.
	{
		char *pBuf = new char[ n ];
		VS_FIXEDFILEINFO *vsinfo;
		UINT Len;

		GetFileVersionInfoA( CheckFile.c_str(), 0, n, pBuf);
		if( VerQueryValueA( pBuf, "\\", (void **)&vsinfo, &Len))
		{
			AnsiString VersionString;
			int MajorVersion = HIWORD(vsinfo->dwFileVersionMS)%100;
			int MinorVersion = LOWORD(vsinfo->dwFileVersionMS)%100;
			int Release      = HIWORD(vsinfo->dwFileVersionLS)%100;
			int Build        = LOWORD(vsinfo->dwFileVersionLS)%100;
			Version = MajorVersion*1000000 + MinorVersion*10000 + Release*100 + Build;
		}
		delete [] pBuf;
	}
	return Version;
}
//------------------------------------------------------------------------------
DWORD Config::FileCreateDate( const char* FileName )
{
	TDateTime FileDateTime;

	if(  FileAge( FileName, FileDateTime ) == true )
	{
		unsigned short yyyy,mm,dd;

		FileDateTime.DecodeDate( &yyyy, &mm, &dd );
		return yyyy*10000 + mm*100 + dd;
	}
	else
		return 19711008;
}
//------------------------------------------------------------------------------
DWORD  Config::FileCreateToday( void )
{
	TDateTime FileDateTime = TDateTime::CurrentDate();
	unsigned short yyyy,mm,dd;

	FileDateTime.DecodeDate( &yyyy, &mm, &dd );
	return yyyy*10000 + mm*100 + dd;
}
//------------------------------------------------------------------------------
// Retrive update list before updating.
// Return number of items need to update.
//------------------------------------------------------------------------------
int Config::RetriveUpdateList()
{
	// initialize update list
	m_UpdateItems.clear();
	m_TotalBytes  = 0;
	m_nBytesRecvd = 0;
	m_Downloaded  = 0;
	m_CancelUpdate = false;
	m_UpdateMainProgram = false;

	if( m_AutoUpdate == true  )
	{
		for( int i =0; i < m_BaseURLs.ItemCount(); i++ )
		{
			AnsiString BaseURL   = GetUpdateBaseURL();
			String ConfigFileURL = BaseURL + m_ConfigFile.c_str(); ///< File URL
			TFileStream* SaveFile;
			try
			{
				UFC::AnsiString ConfigFileOld = m_ConfigFile + ".old";
				UFC::AnsiString ConfigFileDownload = m_ConfigFile + ".download";

				SaveFile = new TFileStream( ConfigFileDownload.c_str(), fmCreate );
				m_pHTTP->Get( ConfigFileURL, SaveFile );
				delete SaveFile;
				MoveFileExA( m_ConfigFile.c_str(), ConfigFileOld.c_str(), MOVEFILE_REPLACE_EXISTING );
				MoveFileExA( ConfigFileDownload.c_str(), m_ConfigFile.c_str(), MOVEFILE_REPLACE_EXISTING );
				LoadFromInIFile( );
				return GetUpdateList( BaseURL.c_str() );
			}
			catch(...)
			{
				if( SaveFile != NULL )
					delete SaveFile;
			}
		}
	}
	else
		return 0;
	return -1; ///< connect to all update server failed.
}
//------------------------------------------------------------------------------
//
// [File name]
// Version=everytime  ///< Update every login.
//        =everyday   ///< Update every day. ( check file date)
//        =1000000    ///< Update if file not exists.
//        =19711231   ///< Version > 19711231 means compare by file date < Version
//        =2020210    ///< Version 2.02.02.10, compare by file Version.
// Size=390           ///< file size.( must have)
// Compress=1         ///< Compressed file. (extention with .gz)
// Restart=1          ///< Needs restart after update.
// SaveAs=XXX.xx_     ///< After downloaded save to XXX.xx_
// Register=1         ///< call regsvr32.exe /s after updated.
// Depenency=DDD      ///< Update this item when DDD needs update.
//------------------------------------------------------------------------------
int Config::GetUpdateList( UFC::AnsiString BaseURL )
{
	UFC::UiniFile ini( m_ConfigFile );
	UFC::Section* iniSection;
	UFC::AnsiString SectionName,Version,FileName,FileSize,Depenency;

	for( int i = 0; i < ini.SectionCount(); i++ )
	{
		if( (iniSection = ini.GetSection( i )) != NULL )
		{
			SectionName = iniSection->GetSectionName();
			if( iniSection->GetValue( "Size", FileSize )== TRUE )
			{
				UpdateItem      item;
				UFC::AnsiString Value;
				bool            UpdateEveryDay = false;
				bool            UpdateEveryTime = false;

				item.filesize =  FileSize.ToInt();
				if( iniSection->GetValue( "Version", Version )== TRUE )
				{
					item.version   = 1000000;
					if( Version == "everyday" )
						UpdateEveryDay = true;
					else if( Version == "everytime" )
						UpdateEveryTime = true;
					else
						item.version = Version.ToInt();
				}
				else
					item.version = 1000000;
				if( iniSection->GetValue( "SaveAs", item.filename )== FALSE )
					item.filename = SectionName;
				if( iniSection->GetValue( "Register", Value )== true )
					item.autoreg = (bool)Value.ToInt();
				else
					item.autoreg = false;
				if( iniSection->GetValue( "Restart", Value )== true )
					item.needrestart = (bool)Value.ToInt();
				else
					item.needrestart = false;
				if( iniSection->GetValue( "Compress", Value )== true )
					item.compress = (bool)Value.ToInt();
				else
					item.compress = false;
				if( item.compress == false )
					item.url.Printf("%s%s", BaseURL.c_str(), SectionName.c_str() );
				else
					item.url.Printf("%s%s.gz", BaseURL.c_str(), SectionName.c_str() );
				if( iniSection->GetValue( "Depenency", Depenency )== true ) ///< Depenency file update.
				{
					if( UpdateItemExists( Depenency ) == true )
					{
						AddUpdateItem( item );///< This item need update.
						continue;
					}
				}
				if( UpdateEveryDay == true )///< Update every day.
				{
					if( FileCreateDate( SectionName.c_str()) < FileCreateToday() )///< File created before today
						AddUpdateItem( item );///< This item need update.
					continue;
				}
				if( UpdateEveryTime == true )///< Update every time.
				{
					AddUpdateItem( item );///< This item always needs update.
					continue;
				}
				if( item.version > 19711231 )///< Compare by date
				{
					DWORD CurrentFileAge = FileCreateDate( SectionName.c_str() );
					if( CurrentFileAge < item.version )
						AddUpdateItem( item );///< This item need update.
				}
				else  ///< Compare by file version.
				{
					DWORD CurrentVer = FileVersion( SectionName.c_str(), item.filename );
					if( CurrentVer < item.version )
						AddUpdateItem( item );///< This item need update.
				}
			}
		}
	}
	return m_UpdateItems.size();
}
//------------------------------------------------------------------------------
// Start update
//------------------------------------------------------------------------------
bool Config::StartUpdate( bool& NeedRestart )
{
	m_CurrentItemIter = m_UpdateItems.begin();
	m_nBytesRecvd = 0;
	NeedRestart = false;

	if( m_UpdateMainProgram == true && NeedsReInstall() == true )
	{
		TFileStream* SaveFile = NULL;
		try
		{
			m_ThisFileBytes = 27058383;
			SaveFile = new TFileStream( L"SpeedyUnifySetup.exe", fmCreate );
			m_pHTTP->Get( SetupURL(), SaveFile );
			delete SaveFile;
		}
		catch(...)
		{
			if( SaveFile )
				delete SaveFile;
			return false;
		}
	}
	else
	{
		while( m_CurrentItemIter != m_UpdateItems.end())
		{
			TFileStream* SaveFile = NULL;
			try
			{
				UpdateItem&  Item = *m_CurrentItemIter;
				AnsiString   FileName = Item.filename.c_str();
				AnsiString   URL      = Item.url.c_str();
				bool         AutoReg  = Item.autoreg;
				AnsiString   DownloadFileName;

				if( Item.needrestart == true )
					NeedRestart = true;
				if( Item.compress == true )
					DownloadFileName = FileName+".download.gz"; ///< gzip format
				else
					DownloadFileName = FileName+".download";
				m_ThisFileBytes = Item.filesize;
				SaveFile = new TFileStream( DownloadFileName, fmCreate );
				m_pHTTP->Get( URL, SaveFile );
				delete SaveFile;
				if( Item.compress == true )
				{
					UFC::UncompressFile( DownloadFileName.c_str(), FileName.c_str() );
					UFC::RemoveFile( DownloadFileName.c_str() );
				}
				else
					MoveFileExA( DownloadFileName.c_str(), FileName.c_str(), MOVEFILE_REPLACE_EXISTING );
				if( AutoReg == true )
				{
					UFC::AnsiString CMD;
					CMD.Printf("regsvr32 /s %s", FileName.c_str() );
					system( CMD.c_str() );
				}
			}
			catch(...)
			{
				if( SaveFile )
					delete SaveFile;
				return false;
			}
			if( m_CancelUpdate == true )
				return false;
			m_CurrentItemIter++;
		}
	}
	return true;
}
//------------------------------------------------------------------------------
// Cancel updating progress
//------------------------------------------------------------------------------
void Config::CancelUpdate()
{
	m_CancelUpdate = true;
	m_pHTTP->Disconnect();
}
//------------------------------------------------------------------------------
// Get current progress... range from 0 to 100
//------------------------------------------------------------------------------
int Config::GetTotalUpdateProgress()
{
	return m_TotalBytes > 0 ? ((m_nBytesRecvd + m_Downloaded )*100 )/ m_TotalBytes : 0;
}
//------------------------------------------------------------------------------
int Config::GetCurrentUpdateProgress()
{
	return m_ThisFileBytes > 0 ? ( m_nBytesRecvd *100 )/ m_ThisFileBytes : 0;
}
//------------------------------------------------------------------------------
// Get current updating filename
//------------------------------------------------------------------------------
UFC::AnsiString Config::GetCurrentUpdateFile()
{
	if( NeedsReInstall() == true )
		return "SpeedyUnifySetup.exe";
	return m_CurrentItemIter != m_UpdateItems.end() ?
    	 (*m_CurrentItemIter).filename : UFC::AnsiString();
}
//------------------------------------------------------------------------------
// Add a update item if its version is newest
//------------------------------------------------------------------------------
void Config::AddUpdateItem(const UpdateItem &item)
{
	std::list<UpdateItem>::iterator p = m_UpdateItems.begin();

	while(p!=m_UpdateItems.end())
	{
		if((*p).filename.UpperCase() == item.filename.UpperCase())
		{
			if((*p).version < item.version)
			{
				*p = item; 	//replace by the new one
				m_TotalBytes += item.filesize - (*p).filesize;
			}
			return;
		}
		p++;
	}
	// it's the newest one!
	if( item.filename == MAIN_PROGRAM_TEMP_NAME )
	{
		m_UpdateMainProgram = true;
		m_UpdateVersion = item.version;
	}
	m_UpdateItems.push_back(item);
	m_TotalBytes += item.filesize;
}
//------------------------------------------------------------------------------
const String& Config::SetupURL( void )
{
	UFC::UiniFile   SettingsFile( m_ConfigFile, true );
	UFC::AnsiString Value;

	if( SettingsFile.ValueExists( MAIN_PROGRAM_NAME, "Setup" ) == true )
	{
		SettingsFile.GetValue( MAIN_PROGRAM_NAME, "Setup", Value );
		m_SetupURL = Value.c_str();
	}
	else
		m_SetupURL = L"https://mdbs-sharespace.mdevelop.com/SpeedyRelease/Unify/¥ü´ÁPro_Setup.exe";
	return m_SetupURL;
}
//------------------------------------------------------------------------------
bool Config::NeedsReInstall( void )
{
	UFC::UiniFile   SettingsFile( m_ConfigFile, true );
	UFC::AnsiString Value;

	if( SettingsFile.ValueExists( MAIN_PROGRAM_NAME, "ReInstall" ) == true )
	{
		SettingsFile.GetValue( MAIN_PROGRAM_NAME, "ReInstall", Value );
		return (bool)Value.ToInt();
	}
	else
		return false;
}
//------------------------------------------------------------------------------
bool Config::NeedsForceUpdate( void )
{
	UFC::UiniFile   SettingsFile( m_ConfigFile, true );
	UFC::AnsiString Value;

	if( SettingsFile.ValueExists( MAIN_PROGRAM_NAME, "ForceUpdate" ) == true )
	{
		SettingsFile.GetValue( MAIN_PROGRAM_NAME, "ForceUpdate", Value );
		return (bool)Value.ToInt();
	}
	else
		return false;
}
//------------------------------------------------------------------------------
void Config::LoadUpdateDescription( TStrings* Lines )
{
	UFC::UiniFile   SettingsFile( m_ConfigFile, true );
	UFC::AnsiString Name,Value;

	Lines->Clear();
	for( int i = 1;;i++ )
	{
		Name.Printf( "Line%d", i );
		if( SettingsFile.ValueExists( MAIN_PROGRAM_NAME, Name ) == true )
		{
			SettingsFile.GetValue( MAIN_PROGRAM_NAME, Name, Value );
			if( IsUTF8String( Value.c_str() ) == true )
				Lines->Add( UTF8ToString( Value.c_str() ) );
			else
				Lines->Add( Value.c_str() );
		}
		else
			return;
	}
}
//------------------------------------------------------------------------------
bool Config::UpdateItemExists(  const UFC::AnsiString& FileName )
{
	UFC::AnsiString                 FindFileName = FileName.UpperCase();
	std::list<UpdateItem>::iterator p = m_UpdateItems.begin();

	while( p != m_UpdateItems.end( ) )
	{
		if( (*p).filename.UpperCase() == FindFileName )
			return true;
		p++;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall Config::WorkEnd(TObject *Sender,TWorkMode AWorkMode)
{
	m_Downloaded += m_nBytesRecvd;
}
//---------------------------------------------------------------------------
void __fastcall Config::Work(TObject *Sender, TWorkMode AWorkMode, __int64 AWorkCount)
{
	m_nBytesRecvd = AWorkCount;
	Application->ProcessMessages();
}
//---------------------------------------------------------------------------
