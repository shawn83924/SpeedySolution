#include "TFXRMessages.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetTransactionSubsystemMessage( int FuncCode, int MsgType )
{
	switch( MsgType )
	{
		case 0: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 5  ||  FuncCode == 6 )
					return mtR010;
				else
					return mtUnknown;
		case 1: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 5  ||  FuncCode == 6 )
					return mtR020;
				else
					return mtUnknown;
		case 2: if( FuncCode == 0 )
					return mtR040;
				else
					return mtUnknown;
		case 3: if( FuncCode == 0 || FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 5 )
					return mtR030;
				else
					return mtUnknown;
		case 4: if( FuncCode == 0 )
					return mtR060;
				else
					return mtUnknown;
		case 5: if( FuncCode == 0 )
					return mtR050;
				else
					return mtUnknown;
		case 6: if( FuncCode == 1 )
					return mtR070;
				else
					return mtUnknown;
		case 7: if( FuncCode == 1 )
					return mtR080;
				else
					return mtUnknown;
		case 8: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 5 )
					return mtR090;
				else
					return mtUnknown;
		case 9: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 5 )
					return mtR100;
				else
					return mtUnknown;
		default: return mtUnknown;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR010::MessageR010( char* FunctionCode,char* Body )
:MessageHeader("30",FunctionCode,"00")
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR010::MessageR010( char* Buffer )
:MessageHeader( Buffer )
{
    LoadBody( Buffer  + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR010::LoadBody( char *Body )
{
    MemoryStreamReader MemReader( Body );
    MemReader.Read( FcmNo,7);          /* FCM ID */
    MemReader.Read( OrderNo,5);        /* order # */
    MemReader.Read( InvestorAcno,7);   /* investor account # */
    MemReader.Read( &InvestorFlag,1);  /* investor flag */    
    MemReader.Read( CommodityId,20);   /* Commodity ID */
    //memset( SettlementMonth,' ', 6);   ///< Settlement yyyyMM 200501( Futures only)
    //memset( StrikePrice, ' ', 8);      ///< Strike price( Futures only)
    MemReader.Read( &BuySellCode,1);   /* B/S code */
    MemReader.Read( &OrderType,1);     /* order type */
    MemReader.Read( OrderPrice,9);     /* order price */
    MemReader.Read( OrderQty,4);       /* quantity */
    MemReader.Read( &OrderCondition,1);/* order condition */    
    MemReader.Read( &OpenOffsetFlag,1);/* open offset flag */
    MemReader.Read( ClearMemberId,4);  /* clear member ID */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR010::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( FcmNo,7);          /* FCM ID */
    MemWriter.Write( OrderNo,5);        /* order # */
    MemWriter.Write( InvestorAcno,7);   /* investor account # */
    MemWriter.Write( &InvestorFlag,1);  /* investor flag */
    MemWriter.Write( CommodityId,20);   /* Commodity ID */
    MemWriter.Write( &BuySellCode,1);   /* B/S code */
    MemWriter.Write( &OrderType,1);     /* order type */
    MemWriter.Write( OrderPrice,9);     /* order price */
    MemWriter.Write( OrderQty,4);       /* quantity */
    MemWriter.Write( &OrderCondition,1);/* order condition */
    MemWriter.Write( &OpenOffsetFlag,1);/* open offset flag */
    MemWriter.Write( ClearMemberId,4);  /* clear member ID */
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR010::print( void )
{
     SocketPrintf("---------------------- Order message R010 ---------------------\n" );
     SocketPrintf("[ FCM-No %s ]\n",UFC::AnsiString(FcmNo,7).c_str());
     SocketPrintf("[ Order_No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
     SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
     SocketPrintf("[ Inv_flag %c ]\n",InvestorFlag );
     SocketPrintf("[ Prod_ID %s ]\n",UFC::AnsiString(CommodityId,20).c_str());
     SocketPrintf("[ B/S Code %c ]\n",BuySellCode);
     SocketPrintf("[ Order_Type %c ]\n",OrderType);
     SocketPrintf("[ Order_Price %s ]\n",UFC::AnsiString(OrderPrice,9).c_str());
     SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,4).c_str());
     SocketPrintf("[ Order Condition %c ]\n",OrderCondition);
     SocketPrintf("[ O/C Code %c ]\n",OpenOffsetFlag);
     SocketPrintf("[ Clear_Mem_ID %s ]\n",UFC::AnsiString(ClearMemberId,4).c_str());
     SocketPrintf("-------------------------- End of R010 ------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString MessageR010::GetCommodityId()
{
    return UFC::AnsiString( CommodityId,20);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR020::MessageR020( char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//---------------------------------------------------------------------------
MessageR020::MessageR020( char* Func, char *Body )
:MessageHeader( "30",Func,"01" )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR020::LoadBody( char *Body )
{
    MemoryStreamReader MemReader( Body );
    MemReader.Read(FcmNo,7);
    MemReader.Read(OrderNo,5);
    MemReader.Read(InvestorAcno,7);
    MemReader.Read(&InvestorFlag,1);
    MemReader.Read(CommodityId,20);
    MemReader.Read(&BuySellCode,1);
    MemReader.Read(&OrderType,1);
    MemReader.Read(OrderPrice,9);
    MemReader.Read(OrderQty,4);
    MemReader.Read(&OrderCondition,1);
    MemReader.Read(&OpenOffsetFlag,1);
    MemReader.Read(ClearMemberId,4);
    MemReader.Read(BeforeQty,4);
    MemReader.Read(AfterQty,4);
    MemReader.Read(ReceiveTime,8);        
    MemReader.Read(SequenceNo,8);
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR020::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write(FcmNo,7);
    MemWriter.Write(OrderNo,5);
    MemWriter.Write(InvestorAcno,7);
    MemWriter.Write(&InvestorFlag,1);
    MemWriter.Write(CommodityId,20);
    MemWriter.Write(&BuySellCode,1);
    MemWriter.Write(&OrderType,1);
    MemWriter.Write(OrderPrice,9);
    MemWriter.Write(OrderQty,4);
    MemWriter.Write(&OrderCondition,1);
    MemWriter.Write(&OpenOffsetFlag,1);
    MemWriter.Write(ClearMemberId,4);
    MemWriter.Write(BeforeQty,4);
    MemWriter.Write(AfterQty,4);
    MemWriter.Write(ReceiveTime,8);
    MemWriter.Write(SequenceNo,8);
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageR020::print( void )
{
    SocketPrintf("--------------- Accept order report mesage R020 ---------------\n" );
    SocketPrintf("[ FCM-No %s ]\n",UFC::AnsiString(FcmNo,7).c_str());
    SocketPrintf("[ Order_No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
    SocketPrintf("[ Inv_flag %c ]\n",InvestorFlag );
    SocketPrintf("[ Prod_ID %s ]\n",UFC::AnsiString(CommodityId,20).c_str());
    SocketPrintf("[ B/S Code %c ]\n",BuySellCode);
    SocketPrintf("[ Order_Type %c ]\n",OrderType);
    SocketPrintf("[ Order_Price %s ]\n",UFC::AnsiString(OrderPrice,9).c_str());
    SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,4).c_str());
    SocketPrintf("[ Order Condition %c ]\n",OrderCondition);
    SocketPrintf("[ O/C Code %c ]\n",OpenOffsetFlag);
    SocketPrintf("[ Clear_Mem_ID %s ]\n",UFC::AnsiString(ClearMemberId,4).c_str());
    SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,4).c_str());
    SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,4).c_str());
    SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(ReceiveTime,8).c_str());
    SocketPrintf("[ Seq_no %s ]\n",UFC::AnsiString(SequenceNo,8).c_str());
    SocketPrintf("----------------------- End of R020 ---------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR030::print( void )
{
    char TmpStr[3];

    strncpynt( TmpStr, GetStatusCodeStr(), 2 );
    SocketPrintf("----- Error Order message R030 code: %s -----------------\n",TmpStr );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Quote Request message
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR070::MessageR070( char* Data ):MessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
    MemReader.Read(FcmNo,7);
    MemReader.Read(CommodityId,20);
    MemReader.Read(OrderID,5);    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR070::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( FcmNo, 7 );
    MemWriter.Write( CommodityId, 20 );
    MemWriter.Write( OrderID, 5 );    
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR070::print( void )
{
    SocketPrintf("------------------ Request message R070 -----------------------\n");
    SocketPrintf("[ FCM-No %s ]\n",UFC::AnsiString(FcmNo,7).c_str());
    SocketPrintf("[ Prod_ID %s ]\n",UFC::AnsiString(CommodityId,20).c_str());
    SocketPrintf("[ OrderID %s ]\n",UFC::AnsiString(OrderID,5).c_str());
    SocketPrintf("----------------------- End of R070 ---------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Quote Request Confirm message
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR080::MessageR080( char* Data ):MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR080::LoadBody( char* Data )
{
    MemoryStreamReader MemReader( Data  );
    MemReader.Read(FcmNo,7);
    MemReader.Read(CommodityId,20);
    MemReader.Read(ReceiveTime, 8 );
    MemReader.Read(SequenceNo, 8 );
    MemReader.Read(OrderID,5);    
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR080::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write(FcmNo,7);
    MemWriter.Write(CommodityId,20);
    MemWriter.Write(ReceiveTime, 8 );
    MemWriter.Write(SequenceNo, 8 );
    MemWriter.Write(OrderID, 5 );    
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR080::print( void )
{
    SocketPrintf("------------------ Request message R080 -----------------------\n");
    SocketPrintf("[ FCM-No %s ]\n",UFC::AnsiString(FcmNo,7).c_str());
    SocketPrintf("[ Prod_ID %s ]\n",UFC::AnsiString(CommodityId,20).c_str());
    SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(ReceiveTime,8).c_str());
    SocketPrintf("[ Seq_no %s ]\n",UFC::AnsiString(SequenceNo,8).c_str());
    SocketPrintf("[ OrderID %s ]\n",UFC::AnsiString(OrderID,5).c_str());
    SocketPrintf("----------------------- End of R080 ---------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR090::MessageR090( char* Buffer )
:MessageHeader( Buffer )
{
    MemoryStreamReader MemReader( Buffer + MESSAGE_HEADER_SIZE );
    MemReader.Read( FcmNo        , 7);
    MemReader.Read( OrderNo      , 5);
    MemReader.Read( IvacNo       , 7);
    MemReader.Read( &IvacNoFlag  , 1);
    MemReader.Read( ProdId       ,20);
    MemReader.Read( BuyPrice     , 9);
    MemReader.Read( SellPrice    , 9);
    MemReader.Read( BuyQty       , 4);
    MemReader.Read( SellQty      , 4);
    MemReader.Read( &OrderCond   , 1);
    MemReader.Read( &OpenOffCode , 1);
    MemReader.Read( Member       , 4);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR090::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( FcmNo       , 7);
    MemWriter.Write( OrderNo     , 5);
    MemWriter.Write( IvacNo      , 7);
    MemWriter.Write( &IvacNoFlag , 1);
    MemWriter.Write( ProdId      ,20);
    MemWriter.Write( BuyPrice    , 9);
    MemWriter.Write( SellPrice   , 9);
    MemWriter.Write( BuyQty      , 4);
    MemWriter.Write( SellQty     , 4);
    MemWriter.Write( &OrderCond  , 1);
    MemWriter.Write( &OpenOffCode, 1);
    MemWriter.Write( Member      , 4);
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR090::print( void )
{
    SocketPrintf("----------------------- Input Data R090 -------------------\n" );
    SocketPrintf("[ FCM-No %s ]\n",UFC::AnsiString(FcmNo,7).c_str());
    SocketPrintf("[ Order_No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(IvacNo,7).c_str());
    SocketPrintf("[ Inv_flag %c ]\n",IvacNoFlag);
    SocketPrintf("[ Prod_ID %s ]\n",UFC::AnsiString(ProdId,20).c_str());
    SocketPrintf("[ Buy_Price %s ]\n",UFC::AnsiString(BuyPrice,9).c_str());
    SocketPrintf("[ Sell_Price %s ]\n",UFC::AnsiString(SellPrice,9).c_str());
    SocketPrintf("[ Buy_QNTY %s ]\n",UFC::AnsiString(BuyQty,4).c_str());
    SocketPrintf("[ Sell_Qnty %s ]\n",UFC::AnsiString(SellQty,4).c_str());
    SocketPrintf("[ Order Condition %c ]\n",OrderCond);
    SocketPrintf("[ O/C Code %c ]\n",OpenOffCode);
    SocketPrintf("[ Clear_Mem_ID %s ]\n",UFC::AnsiString(Member,4).c_str());
    SocketPrintf("----------------------- End of Data R090 -------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR100::MessageR100( char* Buffer )
:MessageHeader( Buffer )
{
    LoadBody( Buffer + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR100::LoadBody( char* Body )
{
    MemoryStreamReader MemReader( Body );
    MemReader.Read( FcmNo, 7 );
    MemReader.Read( OrderNo,5);
    MemReader.Read( InvestorAcno,7);
    MemReader.Read( &InvestorFlag,1);
    MemReader.Read( CommodityId,20);
    MemReader.Read( BuyPrice,9);
    MemReader.Read( SellPrice,9);
    MemReader.Read( BuyQty, 4 );
    MemReader.Read( SellQty,4 );
    MemReader.Read( &OrderCondition, 1);
    MemReader.Read( &OpenOffsetFlag, 1);
    MemReader.Read( ClearMemberId, 4);
    MemReader.Read( BeforeQty,4 );
    MemReader.Read( AfterQty,4 );
    MemReader.Read( ReceiveTime,8 );
    MemReader.Read( SequenceNo,8 );
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR100::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( FcmNo,7);
    MemWriter.Write( OrderNo,5);
    MemWriter.Write( InvestorAcno,7);
    MemWriter.Write( &InvestorFlag,1);
    MemWriter.Write( CommodityId,20);
    MemWriter.Write( BuyPrice,9);
    MemWriter.Write( SellPrice,9);
    MemWriter.Write( BuyQty, 4 );
    MemWriter.Write( SellQty,4 );
    MemWriter.Write( &OrderCondition, 1);
    MemWriter.Write( &OpenOffsetFlag, 1);
    MemWriter.Write( ClearMemberId, 4);
    MemWriter.Write( BeforeQty,4 );
    MemWriter.Write( AfterQty,4 );
    MemWriter.Write( ReceiveTime,8 );
    MemWriter.Write( SequenceNo,8 );
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR100::print( void )
{
    SocketPrintf("----------------------- Input Data R100 -------------------\n" );
    SocketPrintf("[ FCM-No %s ]\n",UFC::AnsiString(FcmNo,7).c_str());
    SocketPrintf("[ Order_No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
    SocketPrintf("[ Inv_flag %c ]\n",InvestorFlag );
    SocketPrintf("[ Prod_ID %s ]\n",UFC::AnsiString(CommodityId,20).c_str());
    SocketPrintf("[ Buy_Price %s ]\n",UFC::AnsiString(BuyPrice,9).c_str());
    SocketPrintf("[ Sell_Price %s ]\n",UFC::AnsiString(SellPrice,9).c_str());
    SocketPrintf("[ Buy_QNTY %s ]\n",UFC::AnsiString(BuyQty,4).c_str());
    SocketPrintf("[ Sell_Qnty %s ]\n",UFC::AnsiString(SellQty,4).c_str());
    SocketPrintf("[ Order Condition %c ]\n",OrderCondition);
    SocketPrintf("[ O/C Code %c ]\n",OpenOffsetFlag);
    SocketPrintf("[ Clear_Mem_ID %s ]\n",UFC::AnsiString(ClearMemberId,4).c_str());
    SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,4).c_str());
    SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,4).c_str());
    SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(ReceiveTime,8).c_str());
    SocketPrintf("[ Seq_no %s ]\n",UFC::AnsiString(SequenceNo,8).c_str());
    SocketPrintf("----------------------- End of Data R100 -------------------\n");
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
