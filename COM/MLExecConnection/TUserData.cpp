#include "TUserData.h"
#include "MLExecConnection.h"
//------------------------------------------------------------------------------
TUserData::TUserData( const char* UserData )
:FSubAccount( "0000000" )
,FAE( "0000" )
,FApKind( "s" )
,FSourceIP( UFC::PSocket::GetLocalIP() )
,FClOrdID( "" )
,FOrigClOrdID( "" )
{
    if( UserData != NULL )
        Parse( UserData );
    FSourceIP.PadThis( 16, ' ' );    
}
//------------------------------------------------------------------------------
void TUserData::Parse( const UFC::AnsiString& Data )
{
    UFC::AnsiString Str;
    UFC::PStringList StringList;
    StringList.SetStrings( Data, ",^" );

    if( StringList.ItemCount() == 0  )
    {
        if( Data.Length() >= 7 )
            FSubAccount = Data.SubString( 0, 7 );
        else
            FSubAccount.PadThis( 7, ' ' );
    }
    else
    {
        for( register int i = 0; i < StringList.ItemCount(); i++ )
        {
            Str = StringList.GetItem( i );
            UFC::BufferedLog::DebugPrintf( " [%s]", Str.c_str() );
            Int32 Pos = Str.AnsiPos( "=" );

            if( Pos > 0 && Str.LastChar() != '=' )
            {
                UFC::AnsiString Name  = Str.SubString( 0, Pos );
                UFC::AnsiString Value = Str.SubString( Pos + 1, Str.Length() - Name.Length() - 1 );
                UFC::BufferedLog::DebugPrintf( " %s=%s", Name.c_str(), Value.c_str() );

                if( Name.AnsiCompare( "subacno" ) == 0 )
                    FSubAccount.Printf( "%07d", Value.ToInt() );
                else if( Name.AnsiCompare( "aeid" ) == 0 )
                    FAE.Printf( "%04d", Value.ToInt() );
                else if( Name.AnsiCompare( "apkind" ) == 0 )
                    FApKind = Value;
                else if( Name.AnsiCompare( "ip" ) == 0 )
                    FSourceIP = Value;
                else if( Name.AnsiCompare( "11" ) == 0 ) ///< ClOrdID
                    FClOrdID = Value;
                else if( Name.AnsiCompare( "41" ) == 0 ) ///< OrigClOrdID
                    FOrigClOrdID = Value;            
            }
        }
    }    
}
//------------------------------------------------------------------------------


