#include "TUserData.h"
#include "CaptialExecConnection.h"
//---------------------------------------------------------------------------
const UFC::AnsiString PROXY_PREFIX( "<Proxy:" );
const int             PROXY_PREFIX_LEN = PROXY_PREFIX.Length();
const int             PROXY_NID_LEN    = 10;
//------------------------------------------------------------------------------
TUserData::TUserData( const char* UserData )
:FOriginalSubAccount( "" )
,FSubAccount( "0000000" )
,FAE( "0000" )
,FApKind( CaptialExecConnection::ApKind )
,FSourceIP( UFC::PSocket::GetLocalIP() )
,FPE("")
,FOriginalUserData("")
{
    if( UserData != NULL )
    {
        FOriginalUserData = UserData;
        UFC::AnsiString ProxyData;
        
        if( IsProxyUserData( FOriginalUserData, ProxyData ) == TRUE )
            Parse( ProxyData );
        else
            Parse( FOriginalUserData );
    }
    FSourceIP.PadThis( 16, ' ' );
}
//---------------------------------------------------------------------------
bool TUserData::IsProxyUserData( const UFC::AnsiString& UserData, UFC::AnsiString& Data )
{
    if( UserData.Length() >= 25 ) ///< Need 25 bytes TAIFEX "<Proxy:"(7) + account(7) + NID(10) + ">"(1)
    {
        Int32 RightQuote = UserData.AnsiPos( ">" );

        if( UserData.AnsiNCompare( PROXY_PREFIX, PROXY_PREFIX_LEN ) == 0 &&  RightQuote > PROXY_PREFIX_LEN )
        {
            if( UserData.Length() > RightQuote + 1 )
                Data = UFC::AnsiString( UserData.c_str() + RightQuote + 1 );
            else
                Data = "";
            return true;
        }        
    }
    Data = UserData;
    return false;
}
//------------------------------------------------------------------------------
void TUserData::Parse( const UFC::AnsiString& Data )
{
    UFC::AnsiString Str;
    UFC::PStringList StringList;
    StringList.SetStrings( Data, "," );

    for( register int i = 0; i < StringList.ItemCount(); i++ )
    {
        Str = StringList.GetItem( i );
        UFC::BufferedLog::DebugPrintf( " [%s]", Str.c_str() );
        Int32 Pos = Str.AnsiPos( "=" );

        if( Pos > 0 && Str.LastChar() != '=' )
        {
            UFC::AnsiString Name = Str.SubString( 0, Pos );
            UFC::AnsiString Value = Str.SubString( Pos + 1, Str.Length() - Name.Length() - 1 );
            UFC::BufferedLog::DebugPrintf( " %s=%s", Name.c_str(), Value.c_str() );

            if( Name.AnsiCompare( "subacno" ) == 0 || Name.AnsiCompare( "SUBACNO" ) == 0 )
            {
                FOriginalSubAccount = Value;
                FSubAccount.Printf( "%07d", Value.ToInt() );
            }
            else if( Name.AnsiCompare( "aeid" ) == 0 || Name.AnsiCompare( "AEID" ) == 0 )
            {
                Int32 AENumber = Value.ToInt();
                if (AENumber > 9999)
                {    
                    UFC::AnsiString totalAE = "";
                    totalAE.Printf("%d", AENumber);
                    FAE = totalAE.SubString(0, 4);
                }
                else                    
                    FAE.Printf("%04d", AENumber);
            }
            else if( Name.AnsiCompare( "apkind" ) == 0 || Name.AnsiCompare( "APKIND" ) == 0 )
                FApKind = Value;
            else if( Name.AnsiCompare( "ip" ) == 0 || Name.AnsiCompare( "IP" ) == 0 )
                FSourceIP = Value;
            else if( Name.AnsiCompare( "pe" ) == 0 || Name.AnsiCompare( "PE" ) == 0 )
                FPE = Value;
        }
    }
    if( FAE.Length() > 4 )
        FAE.SetLength( 4 );    
}
//------------------------------------------------------------------------------
void TUserData::DumpData()
{
    UFC::BufferedLog::Printf(" Dump User Data[%s] Fields.", FOriginalUserData.c_str());
    UFC::BufferedLog::Printf(" SubAccount=\"%s\", \"%s\"", FOriginalSubAccount.c_str(), FSubAccount.c_str());
    UFC::BufferedLog::Printf(" AE=\"%s\"", FAE.c_str());
    UFC::BufferedLog::Printf(" ApKind=\"%s\"", FApKind.c_str());
    UFC::BufferedLog::Printf(" SourceIP=\"%s\"", FSourceIP.c_str());
    UFC::BufferedLog::Printf(" PE=\"%s\"", FPE.c_str());
    UFC::BufferedLog::FlushToFile();
}  //TUserData::DumpData()
//------------------------------------------------------------------------------


