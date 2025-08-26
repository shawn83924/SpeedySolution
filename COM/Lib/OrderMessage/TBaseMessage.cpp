#include "TBaseMessage.h"
#include "TNetworkID.h"
//--------------------------------------------------------------------------
//
//  Class TBaseMessage
//
//--------------------------------------------------------------------------
TBaseMessage::TBaseMessage(void )
:TMdMessage()
,FAccount( "       " )
,FAccountFlag( " " )
,FAE( "     " )
,FClOrdID( "   " )
,FSymbol("     ")
,FOrderID( "00000" )
,FNID( 0 )
,FGroup( 0 )
,FMarket( nsOrderMessageDefine::mTWFutures )
,FMessageType( nsOrderMessageDefine::mtNew )
,FLocalIP( "" )
,FPeerIP( "" )
,FPriceBase( nsOrderMessageDefine::pbNone )
,FAENO("")
,FStrikePrice( 0.0 )
,FEventType( nsOrderMessageDefine::evtNone )
,FOrderChannel( "" )
,FCASessionID( "" )
,FCASignature( "" )
,FCAPlainText( "" )
,FIsRequestSend(false)
{
}
//--------------------------------------------------------------------------
TBaseMessage::TBaseMessage( TBaseMessage& Ref )
:TMdMessage()
,FAccount( Ref.FAccount )
,FAccountFlag( Ref.FAccountFlag )
,FAE( Ref.FAE )
,FClOrdID( Ref.FClOrdID )
,FSymbol( Ref.FSymbol )
,FOrderID( Ref.FOrderID )
,FBrokerID( Ref.FBrokerID )
,FNID( Ref.FNID )
,FGroup( Ref.FGroup )
,FMarket( Ref.FMarket )
,FMessageType( Ref.FMessageType )
,FLocalIP( Ref.FLocalIP )
,FPeerIP( "" )
,FPriceBase( Ref.FPriceBase )
,FAENO("")
,FStrikePrice( 0.0 )
,FEventType( nsOrderMessageDefine::evtNone )
,FOrderChannel( Ref.FOrderChannel )
,FCASessionID( "" )
,FCASignature( "" )
,FCAPlainText( "" )
,FIsRequestSend(false)
,FDATA( Ref.FDATA ) ///< All Data
,FUDD( Ref.FUDD )  ///< User Define data
,FGDD( Ref.FGDD )  ///< Gateway Data
,FNVS( Ref.FNVS )  ///< Name values
{
    Attach( Ref.GetMTree() );
}
//--------------------------------------------------------------------------
TBaseMessage::~TBaseMessage(void)
{
}
//--------------------------------------------------------------------------
void TBaseMessage::SetAllUserData( const UFC::AnsiString& DataStr )
{
    if( DataStr.Length() >= 4  &&
        DataStr[ 0 ] == '[' && DataStr[ 1 ] == '<' &&
        DataStr[ DataStr.Length()-2 ] == '>' &&	DataStr[ DataStr.Length()-1 ] == ']' )
    {   ///< New Style User Data [<GDD=...^UDD=...^Name=Value^Name1=Value1>]
        UFC::AnsiString Data( DataStr.c_str() + 2, DataStr.Length() -4 );
        UFC::NameValueMessage NameValues( "^" );

        NameValues.FromString( Data );
        if( NameValues.IsExists( "GDD" ) == TRUE ) /// Allen Modify at 20190615
        {   ///< Get GDD
            NameValues.Get( "GDD", FGDD );
            NameValues.Delete( "GDD" );
        }
        if( NameValues.IsExists( "UDD" ) == TRUE ) /// Allen Modify at 20190615
        {   ///< Get UDD
            NameValues.Get( "UDD", FUDD );
            NameValues.Delete( "UDD" );
        }
        FNVS = NameValues.ToString();
    } ///< Old style, No GDD and name value pairs.
    else
        FUDD = DataStr;
    FDATA = DataStr;
}
//--------------------------------------------------------------------------
const char* TBaseMessage::GetAllUserData( void )
{
	return FDATA.c_str();
}
//--------------------------------------------------------------------------
void TBaseMessage::UpdateAllUserData( void )
{
	///< New Style UDD, Has GatewayData or Name Value Pairs.
	if( FNVS.Length() > 0 || FGDD.Length() > 0 )
	{
		if( FUDD.Length() > 0 && FGDD.Length() > 0  )
			FDATA.Printf("[<GDD=%s^UDD=%s^%s>]", FGDD.c_str(), FUDD.c_str(), FNVS.c_str());
		else if( FGDD.Length() > 0 && FUDD.Length() == 0  )
			FDATA.Printf("[<GDD=%s^%s>]",FGDD.c_str(), FNVS.c_str());
		else if( FGDD.Length() == 0 && FUDD.Length() > 0  )
			FDATA.Printf("[<UDD=%s^%s>]",FUDD.c_str(), FNVS.c_str());
		else
			FDATA.Printf("[<%s>]", FNVS.c_str());
	}
	else ///< Old style, No GDD and name value pairs.
		FDATA = FUDD;
}
//--------------------------------------------------------------------------
//
//  Get/Set Gateway data. ( for Yuanta)
//
//--------------------------------------------------------------------------
void TBaseMessage::SetGatewayData( const char* Value )
{
	FGDD = Value;
	UpdateAllUserData();
}
//--------------------------------------------------------------------------
const char* TBaseMessage::GetGatewayData( void )
{
	return FGDD.c_str();
}
//--------------------------------------------------------------------------
//
//  Get/Set User define data.
//
//--------------------------------------------------------------------------
void TBaseMessage::SetUserData( const char* Value )
{
	FUDD = Value;
	UpdateAllUserData();
}
//--------------------------------------------------------------------------
const char* TBaseMessage::GetUserData( void )
{
	return FUDD.c_str();
}
//--------------------------------------------------------------------------
//
// Get/Set name-value set
//
//--------------------------------------------------------------------------
void TBaseMessage::SetValue( const char* Name, const char* Value )
{
	UFC::NameValueMessage NameValues( "^" );

	NameValues.FromString( FNVS );
	NameValues.Append( Name, Value );
	FNVS = NameValues.ToString();
	UpdateAllUserData();
}
//--------------------------------------------------------------------------
bool  TBaseMessage::GetValue( const UFC::AnsiString& Name, UFC::AnsiString& Value )
{
	UFC::NameValueMessage NameValues( "^" );

	NameValues.FromString( FNVS );
	return NameValues.Get( Name, Value );
}
//--------------------------------------------------------------------------
bool TBaseMessage::ValueExists( const char* Name )
{
	UFC::AnsiString NameStr;

	NameStr.Printf( "%s=", Name );
	if( FNVS.AnsiPos( NameStr ) == -1 )
		return false;
	return true;
}
//--------------------------------------------------------------------------

