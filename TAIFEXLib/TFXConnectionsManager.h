#ifndef _CONNECTIONMANAGER_H
#define _CONNECTIONMANAGER_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
//---------------------------------------------------------------------------
using namespace UFC;
//--------------------------------------------------------------------------------------------------------------------
class LinkSubSystem;
//--------------------------------------------------------------------------------------------------------------------
class ConnectionsManager
{
private:
	AnsiString FLine;
	PList*	   FConnections;	
public:
	ConnectionsManager( AnsiString Line ):FLine(Line){}
	~ConnectionsManager( ){}
	void CreateConnectionObjects( int APCode ){}
	int RegisterConnectionObject( LinkSubSystem* ConnObj ){ return -1; }
	int UnregisterConnectionObject( LinkSubSystem* ConnObj ){ return -1; }
	void Run(){};
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
