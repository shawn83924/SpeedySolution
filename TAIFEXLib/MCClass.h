#ident "@(#) $Id: MCClass.h,v 1.7 2004/05/31 02:11:22 ap27 Exp $"
#ifndef __MCCCLIENT_H
#define __MCCCLIENT_H
#include <unistd.h>
#include <pthread.h> 
#include <string.h>
#include "../../inc/mcc.h"
#include "../UFC/Type.h"
#include "../UFC/PThread.h"
//----------------------------------------------------------------------------------------------------------
using namespace UFC;
//----------------------------------------------------------------------------------------------------------
typedef bool (*FStatisticCallback)( void );
//----------------------------------------------------------------------------------------------------------
class MCCClient
{
private:	
	static int         FMCCHandle;
	static int	       FInterval;
	static BOOL        FStop;
	static void*       FStatisticData;
	static PEvent      FStopEvent;
	static FStatisticCallback FCallback;
	static void* Execute( void* Param );
public:
	MCCClient( int ProgramID, int ProgMinorID, void* Data, int Size, int Interval = 20 );
	~MCCClient();
	
	void Run( void );
	void Stop( void );
	
	void SetInterval( int Sec );
	int  GetInterval( void ) { return MCCClient::FInterval; }
	
	void SetStatisticCallback( FStatisticCallback CallbackFunction ){ FCallback = CallbackFunction; }
	FStatisticCallback GetStatisticCallback( void ){ return FCallback; }
};
//----------------------------------------------------------------------------------------------------------
#endif

/*const int XBIOProgID = 10;
//----------------------------------------------------------------------------------------------------------
XBIOStatistic  StatisticData;
bool IsAlive = true;
//----------------------------------------------------------------------------------------------------------
bool StatisticCallback( void )
{
	StatisticData.OrdersPerSec = Orders / TimeSec;
	return IsAlive; 
}
//----------------------------------------------------------------------------------------------------------
int main( int argc,char** argv )
{
	MCCClient  MCCObj( XBIOProgID, &StatisticData, sizeof( XBIOStatistic ));
	
	MCCObj.SetInterval( 3 );                          // Set heartbeats interval to 3 second.
	MCCObj.SetStatisticCallback( StatisticCallback );
	MCCObj.Run();
	while( true )
	{
		// main loop...
		try
		{
		}
		catch( FatalExecption &FE )
		{
			IsAlive = false;
		}
		catch( Execption &E )
		{
		}
	};	
}*/ 
//----------------------------------------------------------------------------------------------------------


