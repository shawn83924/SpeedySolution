// 
// File:   MessageDispatcher.h
// Author: yuan
//
// Created on January 4, 2010, 11:01 AM
//

#ifndef _MESSAGEDISPATCHER_H
#define	_MESSAGEDISPATCHER_H

#include "SpeedyQueueRiskManager.h"
//------------------------------------------------------------------------------
class MessageDispatcher : public MessageListener
{
private:
    MessageObject*         FMessageObject;
    SpeedyQueueConnection* FTAIFEXConnection;
    SpeedyQueueConnection* FTSEOTCConnection;
    SpeedyQueueConnection* FPATSConnection;
public:
    MessageDispatcher( void );
private: 
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );    
    void LoadConfig( void );  
};
//------------------------------------------------------------------------------
#endif	/* _MESSAGEDISPATCHER_H */

