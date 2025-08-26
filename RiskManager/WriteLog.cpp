//----------------------------------------------------------------------------------------------------------------------
#include "RiskManager.h"
//----------------------------------------------------------------------------------------------------------------------
//  Functions to write ChanterFlow log
//----------------------------------------------------------------------------------------------------------------------
void LogRQ_Check( const UFC::AnsiString& Order )
{
	UFC::AnsiString Msg, TimeNow;

	GetTimeString( TimeNow );
	Msg.Printf( "%s [RQ_CHK] <DATA:%s> (%d)",
				TimeNow.c_str(),
				Order.c_str(),
				Order.Length() );
	gOrderLog->WriteString( Msg );
}
//----------------------------------------------------------------------------------------------------------------------
void LogSQ_CheckReply( const UFC::AnsiString& ResultMsg, int Result )
{
	UFC::AnsiString Msg, TimeNow;

	GetTimeString( TimeNow );
	Msg.Printf( "%s [RQ_CHK] <DATA:%s> (%d)",
				TimeNow.c_str(),
				ResultMsg.c_str(),
				Result );
	gOrderLog->WriteString( Msg );
}
//----------------------------------------------------------------------------------------------------------------------
