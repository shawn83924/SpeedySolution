#ident "@(#) $Id: SocketProtocol.h,v 1.9 2004/05/21 10:34:23 ap27 Exp $"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __SOCKET_PROTOCOL_H
#define __SOCKET_PROTOCOL_H
//-----------------------------------------------------------------------------------------------------------
#include "../UFC/PClientSocket.h"
#include "../UFC/PServerSocket.h"

using namespace UFC;
//-----------------------------------------------------------------------------------------------------------
//                                         		   Server -> Client
//-----------------------------------------------------------------------------------------------------------
const unsigned char COMMAND_START       		 = 1;   //->
const unsigned char COMMAND_STOP        		 = 2;   //->
const unsigned char COMMAND_PAUSE       		 = 3;   //->

const unsigned char COMMAND_START_NORMAL		 = 4;   //-> **New** Sending orders according to the order time.

const unsigned char COMMAND_BROKEN     			 = 5;   //-> Data: secquence number, 8 digi string.( buffer length 9 )
const unsigned char COMMAND_FILE_TRANS  		 = 6;   //-> Data: FileData structure.
const unsigned char COMMAND_ALARM      			 = 7;   //<- Data: A String. ( Like COMMAND_MESSAGE )

const unsigned char COMMAND_REGISTER    		 = 8;   //<-
const unsigned char COMMAND_X25_READY   		 = 9;   //<-
const unsigned char COMMAND_X25_BROKEN  		 = 10;  //<-

const unsigned char COMMAND_TURNON_MSG   		 = 11;  //->
const unsigned char COMMAND_TURNOFF_MSG  		 = 12;  //->
const unsigned char COMMAND_FINISHED    		 = 16;  //<- **New** Finished sending all orders.
const unsigned char COMMAND_COMPETITION_FINISHED = 17;  //<- **New** Finished sending COMPETITION orders.
const unsigned char COMMAND_MESSAGE              = 128; //<- Data: A variant length string.

//-----------------------------------------------------------------------------------------------------------
//  Use by Simon only!!
//-----------------------------------------------------------------------------------------------------------
const unsigned char COMMAND_SEND_FILE   = 20;   //->
const unsigned char COMMAND_ORDER       = 21;   //-> Send a order (R010/R090/R070)
const unsigned char COMMAND_REPLY_ORDER = 22;   //<- Reply form (R020/R100/R080 or R030)
const unsigned char COMMAND_CONFIRM     = 23;   //<- Reply form (R020/R100/R080 or R030)
//-----------------------------------------------------------------------------------------------------------
typedef struct
{
	unsigned char Command;
	unsigned char LinkNo;
	unsigned char PVCNo;
	unsigned char SizeLo;
	unsigned char SizeHi;

} Header;
//-----------------------------------------------------------------------------------------------------------
typedef struct
{
	char APCode;
	char BrokerID[ 8 ];
	char IsHistroy;  // 1 for running history data PVC, 0 for running script data.

} RegData;

//-----------------------------------------------------------------------------------------------------------
typedef struct
{
	char FileCode[ 4 ]; // Default "R030"
	char StartSeq[ 9 ];
	char EndSeq[ 9 ];
	char BrokerID[ 8 ];

} FileData;
//-----------------------------------------------------------------------------------------------------------
class DataHeader
{
private:
	Header        FHeader;
	char*         FData;
public:
	DataHeader( char Command, char Link, char PVC, int Size = 0, void* Data = NULL );
	DataHeader( PClientSocket* Socket );
	~DataHeader( void );
	BOOL  Serialize( PClientSocket* Socket );
	unsigned char  GetCommand( void ) { return FHeader.Command; }
	unsigned char  GetPVC( void )     { return FHeader.PVCNo;   }
	unsigned char  GetLink( void )    { return FHeader.LinkNo;  }
	char* GetData( void )    { return FData; }
	int   GetSize( void )    { return (((int)FHeader.SizeHi)<< 8) + ((int)FHeader.SizeLo ); }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------
