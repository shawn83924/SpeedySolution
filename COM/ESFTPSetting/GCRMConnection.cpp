#include "GCRMConnection.h"
#include "iniFile.h"
#include <math.h>
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
extern BOOL RecoverMode;
extern UFC::AnsiString AppName;
//------------------------------------------------------------------------------
SecurityParser::SecurityParser(const UFC::AnsiString & iniFilePath)
:FFilePath( "" )
{
	SetIniFile( iniFilePath );
	UFC::FileStream fs( FFilePath, O_RDONLY );
	FParseData.SetDataFormat( this );
	FDataset = FParseData.Parse( &fs );
}
//---------------------------------------------------------------------------
void SecurityParser::LoadConfigSettingEx( UFC::UiniFile * pIniFile )
{
    pIniFile->GetValue( this->GetDefaultConfigSectionName(), "FilePath", FFilePath );
}
//---------------------------------------------------------------------------
UFC::TRecord * SecurityParser::GetRecord( UFCType::Int32 RecordIndex )
{
	return FDataset->GetRecord( RecordIndex );
}
//---------------------------------------------------------------------------
UFCType::Int32 SecurityParser::GetRecordCount()
{
	return FDataset->GetRecordCount();
}
//---------------------------------------------------------------------------
//------------------------------------------------------------------------------
GCRMConnection::GCRMConnection( )
:FIP( "127.0.0.1" )
,FPort( 12345 )
,FDownloaded( FALSE )
,FFTPIP( "192.168.0.45" )
,FFTPPort( 21 )
,FLocal( "AS400Rule.txt" )
,FAlertSubject("SPEEDY.COMMAND")
,FAlertKey(UFC::Hostname)
{    
    LoadSetting();
    UFC::AnsiString LogName;

    LogName.Printf( "../log/%s.%s.log", AppName.c_str(), UFC::GetDateString().c_str() );    
    UFC::BufferedLog::Printf( " LogName:[%s]", LogName.c_str() );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 10240, TRUE ) );
    
        
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   %s module ", AppName.c_str() );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    
    UFC::BufferedLog::Printf( "   " );            
    UFC::BufferedLog::Printf( "   FTP Site [%s:%d]", FFTPIP.c_str(), FFTPPort );
    UFC::BufferedLog::Printf( "   Account  [%s:%s]", FFTPAccount.c_str(), FFTPPassword.c_str() );
    UFC::BufferedLog::Printf( "   Remote file[%s]", FRemote.c_str() );        
    UFC::BufferedLog::Printf( "   Local file [%s]", FLocal.c_str() );        
    UFC::BufferedLog::Printf( "   " );        
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
    
    FMessageObject = new MessageObject( AppName, "1.0.0", "GrandCathat RM connection." );
    UFC::AnsiString FileName;

    FileName.Printf( "../cfg/%s.cfg", AppName.c_str() );
    FConfig = new UFC::UiniFile( FileName );
    ///< Start running thread.
    FMessageObject->Start();
    FMessageObject->WaitForConnected();
}
//------------------------------------------------------------------------------
void GCRMConnection::LoadSetting( void )
{
    UFC::AnsiString FileName( "../cfg/ESFTP.cfg" );
    UFC::AnsiString Name,Value;
    
    try
    {
        UFC::UiniFile   Config( FileName );        

        ///< Load FTP setting
        Config.GetValue( "FTP", "URL", FFTPIP );
        if( Config.GetValue( "FTP", "Port", Value ) )
            FFTPPort = Value.ToInt();
        Config.GetValue( "FTP", "Account",    FFTPAccount );
        Config.GetValue( "FTP", "Password",   FFTPPassword );
        Config.GetValue( "FTP", "RemoteFile", FRemote );
        Config.GetValue( "FTP", "LocalFile",  FLocal );        
        Config.GetValue( "Setting", "MBusIP", FIP );
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" %s not found.", FileName.c_str() );
        exit(1);
    }
}
//------------------------------------------------------------------------------
GCRMConnection::~GCRMConnection( void )
{
    
    
}
//------------------------------------------------------------------------------
void GCRMConnection::SendAlert(const UFC::AnsiString& AlertMessage)
{
    MTree Data;
    
    Data.append( "COMMAND", 9020);
    Data.append( "MSG", AlertMessage );    
    
    if( FMessageObject->Send( FAlertSubject, FAlertKey, Data, FALSE) == FALSE)        
        UFC::BufferedLog::Printf(" GCRMConnection::SendAlert() Send Alert Message[%s] failed.", AlertMessage.c_str());
    
}    
//------------------------------------------------------------------------------
void GCRMConnection::DownloadFailed( const UFC::AnsiString& AlertMessage )
{   
    UFC::BufferedLog::Printf(" %s", AlertMessage.c_str() );    
    SendAlert( AlertMessage );
}
//------------------------------------------------------------------------------
void GCRMConnection::FindReleated( BOOL IsFut, SecurityParser& P09, const UFC::AnsiString& Symbol, UFC::PStringList& Holding, UFC::PStringSet& SymSet )
{        
    UFC::AnsiString  kind_id,ChineseName,StockID,CSVStr;      
    
    for( register Int32 i = 0; i < P09.GetRecordCount() ; i++ )
    {
        UFC::TRecord* rec = P09.GetRecord( i );

        if( rec->GetField( "StockID", StockID ) == TRUE && StockID == Symbol )
        {
            rec->GetField( "kind_id", kind_id );
            rec->GetField( "Name",    ChineseName );
            ChineseName.TrimRight();
            kind_id.TrimRight();
            if( IsFut == TRUE )
            {
                FConfig->SetValue( "Default", kind_id, "0" );
                UFC::BufferedLog::Printf( " Relate Futures ProdID[%s] StockID[%s] Name[%s]", kind_id.c_str(), StockID.c_str(), ChineseName.c_str() );
            }
            else
            {
                FConfig->SetValue( "Default", kind_id, "0" );
                UFC::BufferedLog::Printf( " Relate Options ProdID[%s] StockID[%s] Name[%s]", kind_id.c_str(), StockID.c_str(), ChineseName.c_str() );
            }    
            CSVStr.Printf( "%s,%s,%d\n",  kind_id.c_str(), StockID.c_str(), 0 );
            if( SymSet.Exists( kind_id ) == FALSE )
            {
                SymSet.Add( kind_id );
                Holding.Add( CSVStr );                
            }
        }        
    }    
}
//------------------------------------------------------------------------------
bool GCRMConnection::FindStockID( SecurityParser& P09, const UFC::AnsiString& Symbol, UFC::AnsiString& StockID )
{        
    UFC::AnsiString  kind_id,PID( Symbol );      
    
    PID.TrimRight();
    if( PID.Length() > 1 )
    {
        for( register Int32 i = 0; i < P09.GetRecordCount() ; i++ )
        {
            UFC::TRecord* rec = P09.GetRecord( i );

            rec->GetField( "kind_id", kind_id );    
            kind_id.TrimRight();
            if( PID == kind_id )
            {
                rec->GetField( "StockID", StockID );
                StockID.TrimRight();
                return true;
            }
        }
    }
    return false;
}
//------------------------------------------------------------------------------
void GCRMConnection::ParseHoldingSetting( SecurityParser& P09F, SecurityParser& P09O )
{        
    SecurityParser   sdp(  "../cfg/ESBackend.ini" );
    UFC::PStringList Holding; 
    UFC::AnsiString  HoldingFileName,DateStr;
    UFC::AnsiString  Law,CompanyID,Exchange,Symbol,Name,CSVStr;    
    Int32            RecCount = sdp.GetRecordCount();
    UFC::PStringSet  FSymSet,OSymSet;
    
    UFC::GetYYYYMMDD( DateStr, FALSE );    
    UFC::BufferedLog::Printf( " Convert Holding setting file[../cfg/AS400Rule.txt]" );        
    Holding.Add( "Symbol,StockID,Limit\n" );    
    for( register Int32 i = 0; i < RecCount; i++ )
    {        
        UFC::TRecord*   rec = sdp.GetRecord(i);
        UFC::AnsiString ChkSym; 
        
        rec->GetField( "Law",       Law );
        rec->GetField( "CompanyID", CompanyID );
        rec->GetField( "Exchange",  Exchange );
        rec->GetField( "Symbol",    Symbol );
        rec->GetField( "Name",      Name );
        Name.TrimRight();
        ChkSym = Symbol;
        ChkSym.TrimRight();
        if( ChkSym.Length() > 2 ) ///< Symbol must exists
        {
            UFC::BufferedLog::Printf( " ----------------------------------------------------------------");
            UFC::BufferedLog::Printf( " Law[%s] CompanyID[%s]", Law.c_str(), CompanyID.c_str());
            UFC::BufferedLog::Printf( " Exchange[%s] Symbol[%s] StockName[%s] ",
                                        Exchange == "1" ? "TWSE":"OTC",Symbol.c_str(),Name.c_str() );            
            FindReleated( TRUE, P09F, Symbol, Holding, FSymSet );
            FindReleated( FALSE, P09O, Symbol, Holding, OSymSet );
        }
    }   
    CSVStr.Printf( "Count,%d\n", Holding.ItemCount() -1 );
    Holding.AddFront( CSVStr );
    HoldingFileName.Printf( "../log/Holding_%s.csv", DateStr.c_str() );
    Holding.Save( HoldingFileName );    
    UFC::BufferedLog::Printf( " Generate CSV file[%s]", HoldingFileName.c_str( ) );        
    UFC::BufferedLog::Printf( " ----------------------------------------------------------------");        
}
//------------------------------------------------------------------------------
void GCRMConnection::ParseManualSetting( SecurityParser& P09F, SecurityParser& P09O )
{        
    UFC::PStringList Manual;     
    UFC::AnsiString  ManualFileName,DateStr;  
    UFC::AnsiString  CSVStr,Qty( "0" );        
    
    UFC::GetYYYYMMDD( DateStr, FALSE );    
    UFC::BufferedLog::Printf( " Convert Manual setting file[../cfg/ESManualSetting.ini]" );        
    ManualFileName.Printf( "../log/Manual_%s.csv", DateStr.c_str() );
    UFC::RemoveFile( ManualFileName );
    if( UFC::FileExists( "../cfg/ESManualSetting.ini" ))
    {
        UFC::UiniFile ManualIni( "../cfg/ESManualSetting.ini"  );
        UFC::Section* CurSec;
        
        Manual.Add( "Account,Symbol,StockID,Limit\n" );    
        for( int i = 0; i < ManualIni.SectionCount(); i++ )
        {
            CurSec = ManualIni.GetSection( i );
            
            if( CurSec != NULL )
            {   
                UFC::AnsiString SecName = CurSec->GetSectionName();
                UFC::AnsiString N,V,SID;                
                bool IsDefault = false;
                
                if( SecName == "Default" )
                    IsDefault = true;
                for( int j = 0; j< CurSec->ItemCount(); j++ )
                {
                    if( CurSec->GetNameValue( j, N, V ) )
                    {
                        if( IsDefault == true && N == "Default" )
                        {
                            FConfig->SetValue( "Default", "Default",V );
                            CSVStr.Printf( ",,,%d\n",  V.ToInt() );
                            Manual.Add( CSVStr );                
                            UFC::BufferedLog::Printf( " Manual default Limit[%d]", V.ToInt() );
                        }
                        else if( FindStockID( P09F, N, SID ) == true )
                        {
                            if( FConfig->ValueExists( "Default", N ) == FALSE )
                                FConfig->SetValue( SecName, N,V );
                            if( IsDefault == true )
                                CSVStr.Printf( "%s,%s,%s,%d\n", "", N.c_str(), SID.c_str(), V.ToInt() );
                            else
                                CSVStr.Printf( "%s,%s,%s,%d\n", SecName.c_str(), N.c_str(), SID.c_str(), V.ToInt() );
                            Manual.Add( CSVStr );                
                            UFC::BufferedLog::Printf( " Manual Futures SubAccount[%s] ProdID[%s] StockID[%s] Limit[%d]", SecName.c_str(),N.c_str(), SID.c_str(),V.ToInt() );
                        }
                        else if( FindStockID( P09O, N, SID ) == true )
                        {
                            if( FConfig->ValueExists( "Default", N ) == FALSE )
                                FConfig->SetValue( SecName, N,V );
                            if( IsDefault == true )
                                CSVStr.Printf( "%s,%s,%s,%d\n", "", N.c_str(), SID.c_str(), V.ToInt() );
                            else
                                CSVStr.Printf( "%s,%s,%s,%d\n", SecName.c_str(), N.c_str(), SID.c_str(), V.ToInt() );
                            Manual.Add( CSVStr );                
                            UFC::BufferedLog::Printf( " Manual Options SubAccount[%s] ProdID[%s] StockID[%s] Limit[%d]", SecName.c_str(),N.c_str(), SID.c_str(),V.ToInt() );
                        }
                        else
                        {
                            UFC::BufferedLog::Printf( " Manual SubAccount[%s] ProdID[%s] not exists!", SecName.c_str(),N.c_str() );
                            UFC::BufferedLog::Printf( " ----------------------------------------------------------------");        
                            return;
                        }
                    }    
                }                 
            }    
        }
        CSVStr.Printf( "Count,%d\n", Manual.ItemCount() -1 );
        Manual.AddFront( CSVStr );                        
        UFC::BufferedLog::Printf( " Generate CSV file[%s]", ManualFileName.c_str( ) );        
        Manual.Save( ManualFileName );    
    }
    else
        UFC::BufferedLog::Printf( " Manual setting file[%s] not found!", "../cfg/ESManualSetting.ini" );        
    UFC::BufferedLog::Printf( " ----------------------------------------------------------------");        
}
//------------------------------------------------------------------------------
void GCRMConnection::ParseMargin( void )
{       
            
    SecurityParser   P09F( "../cfg/Futures_P09.ini" );        
    SecurityParser   P09O( "../cfg/Options_P09.ini" );        
    UFC::AnsiString  TimeNow;
    
    UFC::GetTimeString( TimeNow ,TRUE );
    FConfig->Clear();
    FConfig->SetValue( "ConvertTime","Time", TimeNow );
    ParseHoldingSetting( P09F,P09O );
    ParseManualSetting( P09F, P09O );    
    FConfig->Save( );    
    
    if( DumpOnly == TRUE ) 
    {
        UFC::AnsiString  kind_id,ChineseName,StockID;  
        Int32            RecCount;    
        
        RecCount = P09F.GetRecordCount();
        for( register Int32 i = 0; i < RecCount; i++ )
        {
            UFC::TRecord* rec = P09F.GetRecord(i);

            rec->GetField( "kind_id", kind_id );
            rec->GetField( "Name", ChineseName );
            rec->GetField( "StockID", StockID );
            ChineseName.TrimRight();
            UFC::BufferedLog::Printf( " Futures ProdID[%s] StockID[%s] Name[%s]", kind_id.c_str(), StockID.c_str(), ChineseName.c_str() );
        }
        RecCount = P09O.GetRecordCount();
        for( register Int32 i = 0; i < RecCount; i++ )
        {
            UFC::TRecord* rec = P09O.GetRecord(i);

            rec->GetField( "kind_id", kind_id );
            rec->GetField( "Name", ChineseName );
            rec->GetField( "StockID", StockID );
            ChineseName.TrimRight();
            UFC::BufferedLog::Printf( " Options ProdID[%s] StockID[%s] Name[%s]", kind_id.c_str(), StockID.c_str(), ChineseName.c_str() );
        }
    }    
}
//------------------------------------------------------------------------------
void GCRMConnection::Download( void )
{       
    if( FDownloaded == FALSE )
    {
        UFC::PEasyFTPClient FTPClient( FFTPIP, FFTPPort );
        UFC::AnsiString AlertMessage;    

        FTPClient.SetNameAndPassword( FFTPAccount, FFTPPassword );
        try
        {
            if( FTPClient.IsFileExist( FRemote ) )            
            {
                FTPClient.Download( FRemote, FLocal );                            
                ParseMargin();                
                FDownloaded = TRUE;
                return;
            }
            else
                AlertMessage.Printf(" ESFTPSetting Remote file [%s] not exists.", FRemote.c_str() );                    
        }
        catch( UFC::FileException& ex )
        {
            AlertMessage.Printf( " ESFTPSetting FTP FileException:[%s].", ex.what() );                                            
        }
        catch( UFC::SocketException& ex )
        {
            AlertMessage.Printf( " ESFTPSetting FTP SocketException:[%s].", ex.what() );                                                        
        }
        catch( ... )
        {
            AlertMessage.Printf( " ESFTPSetting Unknown FTP Exception." );                                            
        }
        DownloadFailed( AlertMessage );
    }
    else
    {
        if( UFC::FileExists( FLocal ) == FALSE )
            FDownloaded = FALSE;
    }
}
//------------------------------------------------------------------------------
void GCRMConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{

}
//-------------------------------------------------------------------------------

