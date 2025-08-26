//------------------------------------------------------------------------------------------------------------------------
#ifndef __SpeedyRCCommandReceiver_H
#define __SpeedyRCCommandReceiver_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "RCRouting.h"
#define COMMAND_KEY				 0x66668888 
#define COMMAND_MTYPE                            30020
//------------------------------------------------------------------------------------------------------------------------
using namespace UFC;
//------------------------------------------------------------------------------------------------------------------------
class SpeedyRCCommandReceiver : public UQueueListener 
{
private:
        static SpeedyRCCommandReceiver*	FInstance;
        static UFC::PCriticalSection    FCreateCS;
	UQueueSubject*	                FQueueSubject;
public:
        RCRoutingManager*       FChkRC;
        RCRoutingManager*       FRptRC;        
private: /// Implement interface UQueueListener
	void OnQueueDataArrived( UQueueStruct *QueueData, int Len );
private:        
        SpeedyRCCommandReceiver( );
public:
        static SpeedyRCCommandReceiver* GetInstance( );
	~SpeedyRCCommandReceiver(){};
public:
	void Run( void );
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
