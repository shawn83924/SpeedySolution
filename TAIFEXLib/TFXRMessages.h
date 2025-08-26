#ifndef __RMESSAGES_H
#define __RMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "TFXMessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  R010 Order Message
//---------------------------------------------------------------------------
class MessageR010 : public MessageHeader
{
    public:
        char FcmNo[7];           /* FCM ID */
        char OrderNo[5];         /* order # */
        char InvestorAcno[7];    /* investor account # */
        char InvestorFlag;       /* investor flag */
        char CommodityId[20];    /* Commodity ID ( Oprions 20 Futures 7) */
        char BuySellCode;        /* B/S code */
        char OrderType;          /* order type */
        char OrderPrice[9];      /* order price (OPTIONS 9, FUTURES 8)*/
        char OrderQty[4];        /* quantity */
        char OrderCondition;     /* order condition (Options only) */
        char OpenOffsetFlag;     /* open offset flag */
        char ClearMemberId[4];   /* clear member ID */
     //public: ///< Fields for future
        //char SettlementMonth[6]; ///< Settlement yyyyMM 200501( Futures only)
        //char StrikePrice[8];     ///< Strike price
     public:
        MessageR010( char* FunctionCode, char* Body );
        MessageR010( char* Data );
        void LoadBody( char *Body );
        ///< Modify by Simon 04/09/16
        const UFC::AnsiString GetFCM(){ return UFC::AnsiString( FcmNo,7); }
        const UFC::AnsiString GetOrderNo(){ return UFC::AnsiString( OrderNo,5); }
        const UFC::AnsiString GetCommodityId();
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
//  R020 Confirm Order Message
//---------------------------------------------------------------------------
class MessageR020 : public MessageHeader
{
    public:
        char FcmNo[ 7 ];
        char OrderNo[ 5 ];
        char InvestorAcno[ 7 ];
        char InvestorFlag;
        char CommodityId[ 20 ];  ///< Commodity ID ( Oprions 20 Futures 7)
        char BuySellCode;
        char OrderType;
        char OrderPrice[ 9 ];
        char OrderQty[ 4 ];
        char OrderCondition;
        char OpenOffsetFlag;
        char ClearMemberId[ 4 ];
        char BeforeQty[ 4 ];
        char AfterQty[ 4 ];
        char ReceiveTime[ 8 ];   ///<(Options only)
        char SequenceNo[ 8 ];
    public: ///< Fields for future
        char SettlementMonth[6]; ///< Settlement yyyyMM 200501( Futures only)
        char StrikePrice[8];     ///< Strike price
    public:
    	MessageR020( ):MessageHeader(  "30", "00", "01" ){}
        MessageR020( char *Data );
        MessageR020( char *Func, char *Body );
        void LoadBody( char* Data );
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
//  R030 Error message
//---------------------------------------------------------------------------
class MessageR030 : public MessageHeader
{
   public:
   		MessageR030( ):MessageHeader("30","00","03"){}
        MessageR030( char* Data ):MessageHeader( Data ){}
        virtual void print( void );
};
//---------------------------------------------------------------------------
//  R040 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageR040 : public MessageHeader
{
   public:
        MessageR040( ):MessageHeader("30","00","02"){}
        MessageR040( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("----------- Confirm connection message R040 ------------\n" );
        }
};
//---------------------------------------------------------------------------
//  R050 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageR050 : public MessageHeader
{
   public:
        MessageR050( ):MessageHeader("30","00","05"){}
        MessageR050( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("--------- Confirm connection reply message R050 ---------\n" );
        }
};
//---------------------------------------------------------------------------
//  R060 Reconnect query message
//---------------------------------------------------------------------------
class MessageR060 : public MessageHeader
{
   public:
        MessageR060( ):MessageHeader("30","00","04"){}
        MessageR060( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("--------- Reconnect query message R060 ---------\n" );
        }
};
//---------------------------------------------------------------------------
// From R070 to R100
// Options only !! Futures don't have these fratures.
//---------------------------------------------------------------------------
class MessageR070 : public MessageHeader
{
   public:
        char FcmNo[ 7 ];
        char CommodityId[20];
        char OrderID[5];
   public:
        MessageR070( char* Data );
        ///< Modify by Simon 04/09/16
        const UFC::AnsiString GetFCM(){ return UFC::AnsiString( FcmNo, 7 ); }
        const UFC::AnsiString GetCommodityId() { return UFC::AnsiString( CommodityId, 20 ); }
        const UFC::AnsiString GetOrderID() { return UFC::AnsiString( OrderID, 5 ); }
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
class MessageR080 : public MessageHeader
{
   public:
        char FcmNo[ 7 ];
        char CommodityId[20];
        char ReceiveTime[ 8 ];
        char SequenceNo[ 8 ];
        char OrderID[5];
   public:
        MessageR080( ):MessageHeader( "30","01","07" ){}
        MessageR080( char* Data );
        void LoadBody( char* Data );
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
class MessageR090 : public MessageHeader
{
    public:
        char FcmNo[7];     // broker ID
        char OrderNo[5];   // order No
        char IvacNo[7];    // account No
        char IvacNoFlag;   // flag of account No
        char ProdId[20];   // production ID
        char BuyPrice[9];  // buy order price
        char SellPrice[9]; // sell order price
        char BuyQty[4];    // buy order quantity
        char SellQty[4];   // sell order quantity
        char OrderCond;    // order condition
        char OpenOffCode;  // open offset code
        char Member[4];    // member no
    public:
    	MessageR090( ):MessageHeader( "30", "00", "08"){}
    	MessageR090( char* Data );
        ///< Modify by Simon 04/09/16
        const UFC::AnsiString GetFCM(){ return UFC::AnsiString(FcmNo,7); }
        const UFC::AnsiString GetOrderNo(){ return UFC::AnsiString(OrderNo,5); }
        const UFC::AnsiString GetCommodityId() { return UFC::AnsiString(ProdId,20); }
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
class MessageR100 : public MessageHeader
{
    public:
        char FcmNo[7];
        char OrderNo[5];
        char InvestorAcno[7];
        char InvestorFlag;
        char CommodityId[20];
        char BuyPrice[9];
        char SellPrice[9];
        char BuyQty[4];
        char SellQty[4];
        char OrderCondition;
        char OpenOffsetFlag;
        char ClearMemberId[4];
        char BeforeQty[4];
        char AfterQty[4];
        char ReceiveTime[8];
        char SequenceNo[8];
    public:
        MessageR100( ):MessageHeader( "30", "00", "09" ){}
    	MessageR100( char *Data );
        void LoadBody( char* Data );
      	virtual void AsString( char* Buffer );
        virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
// End Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
