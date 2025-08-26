//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCQGProtocol_H
#define __RCQGProtocol_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "RCQGNameValueSet.h"
//------------------------------------------------------------------------------------------------------------------------
#define RCQGP_HEADER_SIZE			16
#define RCQGP_DEFAULT_BODY_SIZE		1024
//------------------------------------------------------------------------------------------------------------------------
#define RCQGP_QUERY_COMMAND			0x0201		// S <-- C
#define RCQGP_QUERY_RESULT			0x0204		// S --> C
#define RCQGP_LOGOUT				0x0901		// S --> C
//------------------------------------------------------------------------------------------------------------------------
const UFCType::Int32 RCQGP_CHECKNUMBER = 20090928;
//------------------------------------------------------------------------------------------------------------------------
class RCQGPacket
{
protected:
	UFCType::Int32				FAction;
	UFCType::Int32				FEntryCode;
	UFCType::Int32				FReserveBytes;
	RCQGNameValueSet			FData;
public:
	UFCType::Int32	GetAction( void )						{ return FAction; }
	UFCType::Int32	GetEntryCode( void )					{ return FEntryCode; }
	UFCType::Int32	GetReserveBytes( void )					{ return FReserveBytes; }
	void	SetAction( UFCType::Int32 Action )				{ FAction = Action; }
	void	SetEntryCode( UFCType::Int32 EntryCode )		{ FEntryCode = EntryCode; }
	void	SetReserveBytes( UFCType::Int32 ReserveBytes )	{ FReserveBytes = ReserveBytes; }
public:
	static RCQGPacket*	CreateFromStream( UFC::PSocket* Socket );
public:
    RCQGNameValueSet*	GetData( void ) { return &FData; }
	void				LoadFromSocket( UFC::PSocket* Socket );
	void				SerializeToSocket( UFC::PSocket* Socket );
	RCQGNameValueSet& operator = ( RCQGNameValueSet& rhs );
public:
	RCQGPacket( );
	RCQGPacket( UFCType::Int32 Action );
	RCQGPacket( UFCType::Int32 Action, RCQGNameValueSet& Data );
	virtual ~RCQGPacket();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
