//----------------------------------------------------------------------------------------------------------------------
#ifndef __CMESSAGES_H
#define __CMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "TFXMessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//----------------------------------------------------------------------------------------------------------------------
#define FCM_NO_LEN      7                         ///< FCM no length
#define ORDER_NO_LEN    5                         ///< order number length
#define ACC_NO_LEN      7                         ///< acc_no length
#define PROD_LEN        10                        ///< product id length
#define COMBINED_PROD_LEN 20                      ///< combined order id length
#define CM_NO_LEN       4                         ///< CM No.length
#define INST_LEN        10                        ///< match instance string length
//----------------------------------------------------------------------------------------------------------------------
class MessageC010 : public MessageHeader
{
    private:
            char BrokerID[ 7 ];
            char StartSeq[ 8 ];
    public:
            MessageC010( char* Data );
            MessageC010( char* BrokerID, int SeqNo );
            char* GetBrokerID(){ return BrokerID; }
            int   GetSeq();
            virtual void AsString( char* Buffer );
            virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class MessageC020 : public MessageHeader
{
    private:
            char BrokerID[ 7 ];
            char StartSeq[ 8 ];
    public:
            MessageC020( char* Data );
            MessageC020( char* BrokerID, int SeqNo );
            char* GetBrokerID(){ return BrokerID; }
            int   GetSeq();
            virtual void AsString( char* Buffer );
            virtual void print( void );
};
//---------------------------------------------------------------------------
class MessageC040 : public MessageHeader
{
   public:
        MessageC040( ):MessageHeader("50","00","04") {}
        MessageC040( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
            SocketPrintf("----------- Confirm Connection message C040 ------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageC050 : public MessageHeader
{
   public:
        MessageC050( ):MessageHeader("50","00","05") {}
        MessageC050( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
            SocketPrintf("-------- Confirm Connection reply message C050----------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageC060 : public MessageHeader
{
   private:
        char TotalRecord[8];
   public:
        MessageC060( int Total )
        :MessageHeader( "50","00","10" )
        {
            UFC::AnsiString CountStr;
            
            CountStr.Printf( "%08d",Total );
            memcpy( TotalRecord, CountStr.c_str(), 8 );
        }
        MessageC060( char* Data ):MessageHeader( Data )
        {
            memcpy( TotalRecord, Data + MESSAGE_HEADER_SIZE, 8 );
        }
        virtual void AsString( char* Buffer )
        {
            MessageHeader::AsString( Buffer );
            MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
            MemWriter.Write( TotalRecord, 8 );
            MemWriter.AppendZero();
        }
        virtual void print( void )
        {
            SocketPrintf("------------- End Confirm Message C060 -----------------\n" );
        }
};
//----------------------------------------------------------------------------------------------------------------------
class MessageC011 : public MessageHeader
{
    private:
        char FCMID[ 4 ];
        char FStartSeq[ 8 ];
    public:
        MessageC011( char* Data );
        MessageC011( char* CMID, int SeqNo );
        char* GetCMID(){ return FCMID; }
        int   GetSeq();
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class MessageC021 : public MessageHeader
{
    private:
        char FCMID[ 4 ];
        char FStartSeq[ 8 ];
    public:
    	MessageC021( char* Data );
        MessageC021( char* CMID, int SeqNo );
        char* GetCMID(){ return FCMID; }
        int   GetSeq();
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------
class C03XBody;
//----------------------------------------------------------------------------------------------------------------------
class MessageC030 : public MessageHeader
{
    private:
            char      FBrokerID[ 7 ];  ///< Broker ID.
            int       FMsgCount;       ///< Message Count.
            C03XBody* FMessages[ 3 ];
    public:
            MessageC030( );
            MessageC030( char* Buffer );
            MessageC030( int TotalBuy, int TotalSell, int Seq ); // For TSCD last message.
            virtual ~MessageC030( );
            void SetBroker( char *BrokerID ){ memcpy( FBrokerID ,BrokerID ,7 );}
            int  GetMessageCount( void ) { return FMsgCount; }
            C03XBody* GetMessage( int Index ) { return FMessages[Index]; }
            BOOL AddMessage( C03XBody* NewMessage );
            virtual void AsString( char* Buffer );
            virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class MessageC031 : public MessageHeader
{
    private:
            char      FBrokerID[ 4 ];  ///< Broker ID.
            int       FMsgCount;       ///< Message Count.
            C03XBody* FMessages[ 3 ];
    public:
            MessageC031( );
            MessageC031( char* Buffer );
            virtual ~MessageC031( );
            void SetBroker( char *BrokerID ){ memcpy( FBrokerID ,BrokerID ,4 );}
            int  GetMessageCount( void ) { return FMsgCount; }
            C03XBody* GetMessage( int Index ) { return FMessages[Index]; }
            BOOL AddMessage( C03XBody* NewMessage );
            virtual void AsString( char* Buffer );
            virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class MessageC032 : public MessageHeader
{
    private:
            char      FBrokerID[ 4 ];  ///< CM ID.
            int       FMsgCount;       ///< Message Count.
	    C03XBody* FMessages[ 3 ];
    public:
            MessageC032( );
            MessageC032( char* Buffer );
            virtual ~MessageC032( );
            void SetBroker( char *BrokerID ){ memcpy( FBrokerID ,BrokerID ,4 );}
            int  GetMessageCount( void ) { return FMsgCount; }
            C03XBody* GetMessage( int Index ) { return FMessages[ Index ]; }
            BOOL AddMessage( C03XBody* NewMessage );
            virtual void AsString( char* Buffer );
            virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C03XBody
{
    public:
        char FRecordType;    ///< 1:single order report 2:Combo order report 3:Order R/C report 4: Quote R/C report
        int  FProcessingTime;
        char FSeqNo[8];
    public:
        C03XBody( char RecordType ):FRecordType( RecordType ),FProcessingTime(0){}
        int ProcessingTime( void ) { return FProcessingTime; }
        Int32 GetSeq( void ) { return UFC::AnsiString( FSeqNo,8).ToInt(); }
        char  GetType( void ){ return FRecordType; }
        virtual int AsString( char* Buffer ) = 0;
        virtual void print( void ) = 0;
        virtual ~C03XBody( void ){}
    public:
#ifndef _FOR_FCM
        static C03XBody* CreateFromRPTLog( RPTLOG* rpt ,char APCode, BOOL IsTSCD = FALSE ); ///< Class factory.
#endif
        static C03XBody* CreateFromString( char* Str, int &offset, char APCode );  ///< Class factory.
};
//----------------------------------------------------------------------------------------------------------------------
class C030STSCDLastBody : public C03XBody
{
    public:
    	char      FTotalBuy[ 12 ];
        char      FTotalSell[ 12 ];
        char      FLastSeq[ 8 ];
    public:
    	C030STSCDLastBody( char* Data, int& Offset );
    	C030STSCDLastBody( int TotalBuy, int TotalSell, int Seq );
        virtual int AsString( char* Buffer );
        virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C030SingleBody : public C03XBody
{
    public:
 	    char FBroker[ 7 ];
	    char FOrderNo[ 5 ];
	    char FInvestorACNo[ 7 ];
	    char FInvestorFlag;
	    char FProdID[ 20 ];
  	    char FBuySellCode;
	    char FOrderType;
	    char FPrice[9];
	    char FQuntity[4];
	    char FOCCode;
	    char FCMID[4];
	    char FMatchTime[8];
            char FProdType;///< F: Future O:Options.
    public: ///< Fields for future
            char SettlementMonth[6]; ///< Settlement month format: yyyyMM
            char StrikePrice[8];     ///< Strike price
    public:
            C030SingleBody( void ):C03XBody( '1' ) {}
            C030SingleBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
            C030SingleBody( RPTLOG* rpt, BOOL IsTSCD );
#endif
            virtual int AsString( char* Buffer );
            virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C030ComboBody : public C03XBody
{
    public:
            char FBroker[ 7 ];
	    char FOrderNo[ 5 ];
	    char FInvestorACNo[ 7 ];
	    char FInvestorFlag;
	    char FProdID[ 20 ];
  	    char FProdID1[10];
	    char FBuySellCode1;
	    char FMatchPrice1[9];
	    char FMatchQuntity1[4];
	    char FProdID2[10];
	    char FBuySellCode2;
	    char FMatchPrice2[9];
	    char FMatchQuntity2[4];
	    char FBuySellCode;
	    char FOrderType;
	    char FPrice[9];
	    char FQuntity[4];
	    char FOCCode;
	    char FCMID[4];
	    char FMatchTime[8];
            char FProd1Type;///< F: Future O:Options.
            char FProd2Type;///< F: Future O:Options.
    public:
            C030ComboBody( void ):C03XBody( '2' ) {}
            C030ComboBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
            C030ComboBody(  RPTLOG* rpt, BOOL IsTSCD  );
#endif
            virtual int AsString( char* Buffer );
            virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C030ReduceCancelBody : public C03XBody
{
    public:
            char FStatusCode[ 2 ];
            char FBroker[ 7 ];
            char FOrderNo[ 5 ];
            char FInvestorACNo[ 7 ];
            char FInvestorFlag;
            char FProdID[ 20 ];
            char FBuySellCode;
            char FCMID[4];
            char FTransCode;		///< 2: Change Quntity 3:Cancel
            char FBeforeQuntity[4];
            char FAfterQuntity[4];
            char FReceiveTime[8];
    public: ///< Fields for future
            char SettlementMonth[6]; ///< Settlement month format: yyyyMM
            char StrikePrice[8];     ///< Strike price
    public:
            C030ReduceCancelBody( void ):C03XBody( '3' ) {}
            C030ReduceCancelBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
            C030ReduceCancelBody(  RPTLOG* rpt, BOOL IsTSCD  );
#endif
            virtual int AsString( char* Buffer );
            virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C030QuoteReduceCancelBody : public C03XBody
{
    public:
            char FStatusCode[ 2 ];
            char FBroker[ 7 ];
            char FOrderNo[ 5 ];
            char FInvestorACNo[ 7 ];
            char FInvestorFlag;
            char FProdID[ 20 ];
            char FBuySellCode;
            char FCMID[4];
            char FTransCode;       ///< 2: Change Quntity 3:Cancel
            char FBeforeQuntity[4];
            char FAfterQuntity[4];
            char FReceiveTime[8];
    public:
            C030QuoteReduceCancelBody( void ):C03XBody( '4' ) {}
            C030QuoteReduceCancelBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
            C030QuoteReduceCancelBody(  RPTLOG* rpt ,BOOL IsTSCD  );
#endif
            virtual int AsString( char* Buffer );
            virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C031FutureBody: public C03XBody
{
    public:
          char FBroker[ 7 ];
          char FInvestorACNo[ 7 ];
          char FInvestorFlag;
          char FProdID[ 7 ];
          char SettlementMonth[6];
          char StrikePrice[8];
          char TransCoode;
          char FBuySellCode;
          char OrderType;
          char OpenOffsetFlag;
          char OrdMthPrice[ 8 ];
          char OrdMthQty[ 4 ];
          char FReceiveTime[8];
    public:
          C031FutureBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
          C031FutureBody(  RPTLOG* rpt, char ReplyType  );
#endif
          virtual int AsString( char* Buffer );
          virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C031SingleBody: public C03XBody
{
	public:
		char FBroker[ 7 ];
		char FOrderNo[ 5 ];
		char FInvestorACNo[ 7 ];
		char FInvestorFlag;
		char FProdID[ 20 ];
		char FBuySellCode;
		char FOrderType;
		char FPrice[9];
		char FQuntity[4];
		char FOCCode;
		char FCMID[4];
		char FMatchTime[8];
		char FProdType;///< F: Future O:Options.
	public:
		C031SingleBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
		C031SingleBody(  RPTLOG* rpt  );
#endif
		virtual int AsString( char* Buffer );
		virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C031ComboBody : public C03XBody
{
	public:
		char FBroker[ 7 ];
		char FOrderNo[ 5 ];
		char FInvestorACNo[ 7 ];
		char FInvestorFlag;
		char FProdID[ 20 ];
		char FProdID1[10];
		char FBuySellCode1;
		char FMatchPrice1[9];
		char FMatchQuntity1[4];
		char FProdID2[10];
		char FBuySellCode2;
		char FMatchPrice2[9];
		char FMatchQuntity2[4];
		char FBuySellCode;
		char FOrderType;
		char FPrice[9];
		char FQuntity[4];
		char FOCCode;
		char FCMID[4];
		char FMatchTime[8];
		char FProd1Type;///< F: Future O:Options.
		char FProd2Type;///< F: Future O:Options.
	public:
		C031ComboBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
		C031ComboBody(  RPTLOG* rpt  );
#endif
		virtual int AsString( char* Buffer );
		virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C032OrderBody : public C03XBody
{
	public:
		char FBroker[ 7 ];
		char FOrderNo[ 5 ];
		char FInvestorACNo[ 7 ];
		char FInvestorFlag;
		char FProdID[ 20 ];
		char FBuySellCode;
		char FOrderType;
		char FPrice[9];
		char FQuntity[4];
		char FCondition;
		char FOCCode;
		char FTransCode;
		char FBeforeQuntity[4];
		char FAfterQuntity[4];
		char FReceiveTime[8];
	public:
		C032OrderBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
		C032OrderBody(  RPTLOG* rpt  );
#endif
		virtual int AsString( char* Buffer );
		virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class C032QuoteBody : public C03XBody
{
	public:
		char FBroker[ 7 ];
		char FOrderNo[ 5 ];
		char FInvestorACNo[ 7 ];
		char FInvestorFlag;
		char FProdID[ 20 ];
		char FBuySellCode;
		char FBidOrderPrice[9];
		char FAskOrderPrice[9];
		char FBidOrderQuntity[4];
		char FAskOrderQuntity[4];
		char FOrderCondition;
		char FOCCode;
		char FTransCode;
		char FBeforeQuntity[4];
		char FAfterQuntity[4];
		char FReceiveTime[8];
	public:
		C032QuoteBody(  char* Data, int& Offset );
#ifndef _FOR_FCM
		C032QuoteBody(  RPTLOG* rpt  );
#endif
		virtual int AsString( char* Buffer );
		virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
