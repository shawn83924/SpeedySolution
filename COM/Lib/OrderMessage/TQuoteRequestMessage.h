#ifndef _TQuoteRequestMessage_H_
#define _TQuoteRequestMessage_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
//--------------------------------------------------------------------------
class TQuoteRequestMessage :public TBaseMessage
{
private:    
        UFC::AnsiString FSrc;
public:
	TQuoteRequestMessage( void );
public:
	~TQuoteRequestMessage(void);        
	void SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value );
        void SetSrc( const char* Value )                                            { FSrc = Value; }
	nsOrderMessageDefine::TradingSessionIDEnum GetTradingSessionID( void );
        const char*                                GetSrc( void )                   { return FSrc.c_str(); }
};
//--------------------------------------------------------------------------
#endif

