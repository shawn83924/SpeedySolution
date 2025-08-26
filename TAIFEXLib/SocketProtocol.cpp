#ident "@(#) $Id: SocketProtocol.cpp,v 1.3 2004/05/21 10:34:23 ap27 Exp $"
#include "SocketProtocol.h"
//-------------------------------------------------------------------------------------------------------------------
DataHeader::DataHeader( char Command, char Link, char PVC, int Size , void* Data  )
:FData( NULL )
{
	FHeader.Command = Command;
	FHeader.LinkNo  = Link;
	FHeader.PVCNo   = PVC;
	FHeader.SizeHi  = Size >> 8;
	FHeader.SizeLo  = Size - (((int)FHeader.SizeHi)<<8);
	if( Size != 0 )
	{
		FData = new char[ Size ];
		memcpy( FData, Data, Size );
	}
}
//-------------------------------------------------------------------------------------------------------------------
DataHeader::DataHeader( PClientSocket* Socket )
:FData( NULL )
{
	if( Socket->RecvBuffer( (UInt8*)&FHeader, sizeof( Header ) ) == sizeof( Header ) )
	{
		int Size = GetSize();
		if( Size != 0 )
		{
			FData = new char[ Size ];
			Socket->RecvBuffer( (UInt8*)FData, Size );
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------
DataHeader::~DataHeader( void )
{
	if( FData != NULL )
		delete [] FData;
}
//-------------------------------------------------------------------------------------------------------------------
BOOL DataHeader::Serialize( PClientSocket* Socket )
{
	int Size = GetSize();
	int TotalSize = Size + sizeof( Header );
	char *Buffer;

	Buffer = new char[ TotalSize ];
	memcpy( Buffer, &FHeader, sizeof( Header ) );
	if( Size != 0 )
		memcpy( Buffer + sizeof( Header ), FData, Size );
	if( Socket->SendBuffer( Buffer, TotalSize ) == TotalSize )
	{
		delete [] Buffer;
		return TRUE;
	}
	delete [] Buffer;
	return FALSE;
}
//-------------------------------------------------------------------------------------------------------------------
