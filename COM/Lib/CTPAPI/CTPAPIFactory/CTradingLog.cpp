//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include "CTPConnectionImp.h"
//---------------------------------------------------------------------------
//	functions to keep trading log for recover.
//---------------------------------------------------------------------------
//	TradeLogNew
//	CTP implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogNew( int FID, int SID, CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "PNEW" );
	LogMessage.Append("FID", (long)FID );
	LogMessage.Append("SID", (long)SID );
	LogMessage.Append("OREF",(long)atoi( Ord.OrderRef ) );
	LogMessage.Append("BRK", Ord.BrokerID );
	LogMessage.Append("ACC", Ord.InvestorID );
	LogMessage.Append("SYM", Ord.InstrumentID );
	LogMessage.Append("PX",  Ord.LimitPrice );
	LogMessage.Append("QTY", (long)Ord.VolumeTotalOriginal );
	LogMessage.Append("UDD", UserDefine );
	if( Ord.OrderPriceType == THOST_FTDC_OPT_AnyPrice )
		LogMessage.Append("OT", "M" );
	else
		LogMessage.Append("OT", "L" );

	if( Ord.Direction == THOST_FTDC_D_Buy )
		LogMessage.Append("BS", "B" );
	else
		LogMessage.Append("BS", "S" );
	if(	Ord.CombOffsetFlag[0] == THOST_FTDC_OF_Open ) // Open
		LogMessage.Append("PE", "O" );
	else if( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_Close )//Close
		LogMessage.Append("PE", "C" );
	else if( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday )//Close Today
		LogMessage.Append("PE", "c" );
	if( Ord.TimeCondition == THOST_FTDC_TC_IOC )
		LogMessage.Append("TIF", "I" );
	else
		LogMessage.Append("TIF", "R" );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
//	TradeLogNew
//	Femas implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogNew( int FID, int SID, CUstpFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "PNEW" );
	LogMessage.Append("FID", (long)FID );
	LogMessage.Append("SID", (long)SID );
	LogMessage.Append("OREF",(long)atoi( Ord.UserOrderLocalID ) );
	LogMessage.Append("BRK", Ord.BrokerID );
	LogMessage.Append("ACC", Ord.InvestorID );
	LogMessage.Append("SYM", Ord.InstrumentID );
	LogMessage.Append("PX",  Ord.LimitPrice );
	LogMessage.Append("QTY", (long)Ord.Volume );
	LogMessage.Append("UDD", UserDefine );
	if( Ord.OrderPriceType == USTP_FTDC_OPT_AnyPrice )
		LogMessage.Append("OT", "M" );
	else
		LogMessage.Append("OT", "L" );

	if( Ord.Direction == USTP_FTDC_D_Buy )
		LogMessage.Append("BS", "B" );
	else
		LogMessage.Append("BS", "S" );
	if(	Ord.OffsetFlag == USTP_FTDC_OF_Open ) // Open
		LogMessage.Append("PE", "O" );
	else if( Ord.OffsetFlag == USTP_FTDC_OF_Close )//Close
		LogMessage.Append("PE", "C" );
	else if( Ord.OffsetFlag == USTP_FTDC_OF_CloseToday )//Close Today
		LogMessage.Append("PE", "c" );
	if( Ord.TimeCondition == USTP_FTDC_TC_IOC )
		LogMessage.Append("TIF", "I" );
	else
		LogMessage.Append("TIF", "R" );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
//	TradeLogNew
//	KSft / KStrd implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogNew( int FID, int SID, KS_::CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "PNEW" );
	LogMessage.Append( "FID", (long)FID );
	LogMessage.Append( "SID", (long)SID );
	LogMessage.Append( "OREF",(long)atoi( Ord.OrderRef ) );
	LogMessage.Append( "BRK", Ord.BrokerID );
	LogMessage.Append( "ACC", Ord.InvestorID );
	LogMessage.Append( "SYM", Ord.InstrumentID );
	LogMessage.Append( "PX",  Ord.LimitPrice );
	LogMessage.Append( "QTY", (long)Ord.VolumeTotalOriginal );
	LogMessage.Append( "UDD", UserDefine );
	// OrderType
	if ( Ord.OrderPriceType == THOST_FTDC_OPT_AnyPrice )
		LogMessage.Append( "OT", "M" );
	else
		LogMessage.Append( "OT", "L" );
	// Side
	if ( Ord.Direction == THOST_FTDC_D_Buy )
		LogMessage.Append( "BS", "B" );
	else
		LogMessage.Append( "BS", "S" );
	// PositionEffect
	if ( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_Open ) ///< Open
		LogMessage.Append( "PE", "O" );
	else if ( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_Close ) ///< Close
		LogMessage.Append( "PE", "C" );
	else if ( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday ) ///< Close Today
		LogMessage.Append( "PE", "c" );
	// TimeInForce
	if ( Ord.TimeCondition == THOST_FTDC_TC_IOC )
		LogMessage.Append( "TIF", "I" );
	else
		LogMessage.Append( "TIF", "R" );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
//	TradeLogCancel
//	CTP implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogCancel( int OrderLocalID, CThostFtdcInputOrderActionField& CancelOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "PCXL");
	LogMessage.Append("FID", (long)CancelOrder.FrontID );
	LogMessage.Append("SID", (long)CancelOrder.SessionID );
	LogMessage.Append("OREF",(long)CancelOrder.OrderActionRef );
	LogMessage.Append("COID",(long)OrderLocalID );
	LogMessage.Append("OSID", CancelOrder.OrderSysID  );
	LogMessage.Append("EX",   CancelOrder.ExchangeID  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
}
//---------------------------------------------------------------------------
//	TradeLogCancel
//	Femas implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogCancel( int OrderLocalID, CUstpFtdcOrderActionField& CancelOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "PCXL");
	LogMessage.Append("FID", (long)FCTPFrontID);
	LogMessage.Append("SID", (long)FCTPSessionID);
	LogMessage.Append("OREF",(long)CancelOrder.UserOrderLocalID );
	LogMessage.Append("COID",(long)OrderLocalID );
	LogMessage.Append("OSID", CancelOrder.OrderSysID  );
	LogMessage.Append("EX",   CancelOrder.ExchangeID  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
}
//---------------------------------------------------------------------------
//	TradeLogCancel
//	KSft / KStrd implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogCancel( int OrderLocalID, KS_::CThostFtdcInputOrderActionField& CancelOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "PCXL" );
	LogMessage.Append( "FID", (long)CancelOrder.FrontID );
	LogMessage.Append( "SID", (long)CancelOrder.SessionID );
	LogMessage.Append( "OREF",(long)CancelOrder.OrderActionRef );
	LogMessage.Append( "COID",(long)OrderLocalID );
	LogMessage.Append( "OSID", CancelOrder.OrderSysID );
	LogMessage.Append( "EX",   CancelOrder.ExchangeID );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
}
//---------------------------------------------------------------------------
//	TradeLogCTPConfirm
//	CTP implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogCTPConfirm( CThostFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "CCTP");
	LogMessage.Append("FID", (long)pOrder->FrontID );
	LogMessage.Append("SID", (long)pOrder->SessionID );
	LogMessage.Append("OREF", pOrder->OrderRef );
	LogMessage.Append("OLID", pOrder->OrderLocalID  );
	LogMessage.Append("EX",   pOrder->ExchangeID  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
}
//---------------------------------------------------------------------------
//	TradeLogCTPConfirm
//	CTP implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogCTPConfirm( CThostFtdcInputOrderField *pOrder, int Code )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "RCTP");
	LogMessage.Append("FID", (long)FCTPFrontID );
	LogMessage.Append("SID", (long)FCTPSessionID );
	LogMessage.Append("OREF", pOrder->OrderRef  );
	LogMessage.Append("ERR", (long)Code  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
}
//---------------------------------------------------------------------------
//	TradeLogFemasConfirm
//	Femas implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogFemasConfirm( CUstpFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "CCTP");
	LogMessage.Append("FID", (long)FCTPFrontID );
	LogMessage.Append("SID", (long)FCTPSessionID);
	LogMessage.Append("OREF", pOrder->UserOrderLocalID );
	LogMessage.Append("OLID", pOrder->OrderLocalID  );
	LogMessage.Append("EX",   pOrder->ExchangeID  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
}
//---------------------------------------------------------------------------
//	TradeLogFemasConfirm
//	Femas implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogFemasConfirm( CUstpFtdcInputOrderField *pOrder, int Code )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "RCTP");
	LogMessage.Append("FID", (long)FCTPFrontID );
	LogMessage.Append("SID", (long)FCTPSessionID );
	LogMessage.Append("OREF", pOrder->UserOrderLocalID  );
	LogMessage.Append("ERR", (long)Code  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
}
//---------------------------------------------------------------------------
//	TradeLogKSftConfirm
//	KSft implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogKSftConfirm( KS_::CThostFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "CCTP" );
	LogMessage.Append( "FID", (long)pOrder->FrontID );
	LogMessage.Append( "SID", (long)pOrder->SessionID );
	LogMessage.Append( "OREF", pOrder->OrderRef );
	LogMessage.Append( "OLID", pOrder->OrderLocalID );
	LogMessage.Append( "EX",   pOrder->ExchangeID );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
}
//---------------------------------------------------------------------------
//	TradeLogKStrdConfirm
//	KStrd implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogKStrdConfirm( KS_::CThostFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "CCTP" );
	LogMessage.Append( "FID", (long)pOrder->FrontID );
	LogMessage.Append( "SID", (long)pOrder->SessionID );
	LogMessage.Append( "OREF", pOrder->OrderRef );
	LogMessage.Append( "OLID", pOrder->OrderLocalID );
	LogMessage.Append( "EX",   pOrder->ExchangeID );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
}
//---------------------------------------------------------------------------
//	TradeLogKSftConfirm
//	KSft implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogKSftConfirm( KS_::CThostFtdcInputOrderField *pOrder, int Code )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "RCTP" );
	LogMessage.Append( "FID", (long)FCTPFrontID );
	LogMessage.Append( "SID", (long)FCTPSessionID );
	LogMessage.Append( "OREF", pOrder->OrderRef );
	LogMessage.Append( "ERR", (long)Code );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
}
//---------------------------------------------------------------------------
//	TradeLogKStrdConfirm
//	KStrd implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogKStrdConfirm( KS_::CThostFtdcInputOrderField *pOrder, int Code )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "RCTP" );
	LogMessage.Append( "FID", (long)FCTPFrontID );
	LogMessage.Append( "SID", (long)FCTPSessionID );
	LogMessage.Append( "OREF", pOrder->OrderRef );
	LogMessage.Append( "ERR", (long)Code );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
}
//---------------------------------------------------------------------------
//	TradeLogExConfirm
//	CTP implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogExConfirm( CThostFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "CEXH");
	LogMessage.Append("FID", (long)pOrder->FrontID );
	LogMessage.Append("SID", (long)pOrder->SessionID );
	LogMessage.Append("OREF", pOrder->OrderRef  );
	LogMessage.Append("OLID", pOrder->OrderLocalID  );
	LogMessage.Append("OSID", pOrder->OrderSysID   );
	LogMessage.Append("EX",   pOrder->ExchangeID  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
//	TradeLogExConfirm
//	Femas implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogExConfirm( CUstpFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "CEXH");
	LogMessage.Append("FID", (long)FCTPFrontID );
	LogMessage.Append("SID", (long)FCTPSessionID);
	LogMessage.Append("OREF", pOrder->UserOrderLocalID  );
	LogMessage.Append("OLID", pOrder->OrderLocalID  );
	LogMessage.Append("OSID", pOrder->OrderSysID   );
	LogMessage.Append("EX",   pOrder->ExchangeID  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
//	TradeLogExConfirm
//	KSft / KStrd implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogExConfirm( KS_::CThostFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "CEXH" );
	LogMessage.Append( "FID", (long)pOrder->FrontID );
	LogMessage.Append( "SID", (long)pOrder->SessionID );
	LogMessage.Append( "OREF", pOrder->OrderRef );
	LogMessage.Append( "OLID", pOrder->OrderLocalID );
	LogMessage.Append( "OSID", pOrder->OrderSysID );
	LogMessage.Append( "EX",   pOrder->ExchangeID );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
//	TradeLogCancel
//	CTP implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogCancel( CThostFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "CCXL");
	LogMessage.Append("FID", (long)pOrder->FrontID );
	LogMessage.Append("SID", (long)pOrder->SessionID );
	LogMessage.Append("OREF", pOrder->OrderRef  );
	LogMessage.Append("OLID", pOrder->OrderLocalID  );
	LogMessage.Append("OSID", pOrder->OrderSysID  );
	LogMessage.Append("EX",   pOrder->ExchangeID  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
}
//---------------------------------------------------------------------------
//	TradeLogCancel
//	Femas implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogCancel( CUstpFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "CCXL");
	LogMessage.Append("FID", (long)FCTPFrontID );
	LogMessage.Append("SID", (long)FCTPSessionID );
	LogMessage.Append("OREF", pOrder->UserOrderLocalID  );
	LogMessage.Append("OLID", pOrder->OrderLocalID  );
	LogMessage.Append("OSID", pOrder->OrderSysID  );
	LogMessage.Append("EX",   pOrder->ExchangeID  );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
}
//---------------------------------------------------------------------------
//	TradeLogCancel
//	KSft / KStrd implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogCancel( KS_::CThostFtdcOrderField *pOrder )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "CCXL" );
	LogMessage.Append( "FID", (long)pOrder->FrontID );
	LogMessage.Append( "SID", (long)pOrder->SessionID );
	LogMessage.Append( "OREF", pOrder->OrderRef );
	LogMessage.Append( "OLID", pOrder->OrderLocalID );
	LogMessage.Append( "OSID", pOrder->OrderSysID );
	LogMessage.Append( "EX",   pOrder->ExchangeID );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
}
//---------------------------------------------------------------------------
//	TradeLogFill
//	CTP implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogFill( CThostFtdcTradeField *pTrade )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "FILL");
	LogMessage.Append("OREF", pTrade->OrderRef  );
	LogMessage.Append("OLID", pTrade->OrderLocalID  );
	LogMessage.Append("OSID", pTrade->OrderSysID  );
	LogMessage.Append("FSEQ",(long)pTrade->SequenceNo  );
	LogMessage.Append("EX",   pTrade->ExchangeID );
	LogMessage.Append("LQTY",(long)pTrade->Volume );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
//	TradeLogFill
//	Femas implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogFill( CUstpFtdcTradeField *pTrade )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append("Time",Now );
	LogMessage.Append("Act", "FILL");
	LogMessage.Append("OREF", pTrade->UserOrderLocalID  );
//	LogMessage.Append("OLID", pTrade->OrderLocalID  );
	LogMessage.Append("OSID", pTrade->OrderSysID  );
	LogMessage.Append("FSEQ",(long)pTrade->TradeID  );
	LogMessage.Append("EX",   pTrade->ExchangeID );
	LogMessage.Append("LQTY",(long)pTrade->TradeVolume );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length());
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
//	TradeLogFill
//	KSft / KStrd implement
//---------------------------------------------------------------------------
void TTaifexConnection::TradeLogFill( KS_::CThostFtdcTradeField *pTrade )
{
	UFC::NameValueMessage LogMessage("^\n");
	UFC::AnsiString Now;

	UFC::GetTimeString( Now , true );
	LogMessage.Append( "Time",Now );
	LogMessage.Append( "Act", "FILL" );
	LogMessage.Append( "OREF", pTrade->OrderRef );
	LogMessage.Append( "OLID", pTrade->OrderLocalID );
	LogMessage.Append( "OSID", pTrade->OrderSysID );
	LogMessage.Append( "FSEQ",(long)pTrade->SequenceNo );
	LogMessage.Append( "EX",   pTrade->ExchangeID );
	LogMessage.Append( "LQTY",(long)pTrade->Volume );
	UFC::AnsiString Log = LogMessage.ToString( true );
	FCTPTransLog->Write( Log.c_str(), Log.Length() );
	FCTPTransLog->Flush();
}
//---------------------------------------------------------------------------
// Recover order status/data from trading log.
//---------------------------------------------------------------------------
void TTaifexConnection::CTPRecover( const UFC::AnsiString& FileName )
{
	FILE*  FFileHandle;
	char   Buffer[ LOG_MAX_LEN ];

	///< Delete all order objects.
	for( int i =0; i< FOrderRecords.ItemCount(); i ++ )
		 delete FOrderRecords[i];
	FOrderRecords.Clear();
	FOrdLocalIDToRecord.Clear();
	FOrderRefToRecord.Clear();
	FOrdSysIDToRecord.Clear();
	///< Clear ExecID set
	FCNExecIDSet.Clear();

	if( (FFileHandle = fopen( FileName.c_str(), "r" )) != NULL )
	{
		 while( fgets( Buffer, LOG_MAX_LEN, FFileHandle ) != NULL ) ///< Get the first execution.
		 {
			try
			{
				UFC::NameValueMessage TradeLog("^\n");
				UFC::AnsiString       ActCode;

				TradeLog.FromString( Buffer );
				if( TradeLog.Get( "Act", ActCode ) == true )
				{
					if( ActCode == "PNEW" ) ///< Pending New
						AddPendingOrder( TradeLog );
					else if( ActCode == "CCTP" )///< Confirm by CTP
						CTPConfirm( TradeLog );
					else if( ActCode == "CEXH" )///< Confirm by Exchange
						ExhConfirm( TradeLog );
					else if( ActCode == "CCXL" )///< Order Canceled
						ExhConfirm( TradeLog );
					else if( ActCode == "FILL" )///< Order filled
						OrderFill( TradeLog );
				}
			}
			catch( UFC::Exception& e )
			{
				UFC::BufferedLog::Printf( " Recover Exception:%s", e.what() );
				UFC::BufferedLog::Printf( " Data[%s]", Buffer );
			}
		 };
		 fclose( FFileHandle );
	}
	else
		UFC::BufferedLog::Printf(" Can't open Executions recover file %s", FileName.c_str() );
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddPendingOrder( UFC::NameValueMessage& TradeLog )
{
	COrderInfo* NewOrd = new COrderInfo( TradeLog );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", NewOrd->GetFrontID(), NewOrd->GetSessionID(), NewOrd->GetOrderRef() );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
}
//---------------------------------------------------------------------------
void  TTaifexConnection::CTPConfirm( UFC::NameValueMessage& TradeLog )
{
	UFC::AnsiString FrontID,SessionID,OrderRef,ExchangeID,OrderLocalID;
	COrderInfo*     Record;

	TradeLog.Get("FID",  FrontID, true );
	TradeLog.Get("SID",  SessionID, true );
	TradeLog.Get("OREF", OrderRef, true );
	TradeLog.Get("OLID", OrderLocalID, true );
	TradeLog.Get("EX",   ExchangeID, true );
	if( (Record = FindOrderByOrderRef( FrontID.ToInt(),SessionID.ToInt(),OrderRef.ToInt())) != NULL  ) ///< Update Record fields
	{
		if( Record->GetStatus() == cosPending && OrderLocalID.Length() > 0 && ExchangeID.Length() > 0  )
		{
			Record->CTPAccepted( ExchangeID, OrderLocalID );
			AddOrderLocalIDToMap( ExchangeID, OrderLocalID.ToInt(), Record );
		}
	}
}
//---------------------------------------------------------------------------
void  TTaifexConnection::ExhConfirm( UFC::NameValueMessage& TradeLog )
{
	UFC::AnsiString FrontID,SessionID,OrderRef,ExchangeID,OrderLocalID,OrderSysID;
	COrderInfo*     Record;

	TradeLog.Get("FID",  FrontID, true );
	TradeLog.Get("SID",  SessionID, true );
	TradeLog.Get("OREF", OrderRef, true );
	TradeLog.Get("EX",   ExchangeID, true );
	TradeLog.Get("OLID", OrderLocalID, true );
	TradeLog.Get("OSID", OrderSysID, false );
	if( (Record = FindOrderByOrderRef( FrontID.ToInt(),SessionID.ToInt(),OrderRef.ToInt())) != NULL || ///< Update Record fields
		(Record = FindOrderByOrderLocalID( ExchangeID, OrderLocalID.ToInt())) != NULL )
	{
		if( Record->GetStatus() == cosConfirm && OrderSysID.Length() > 0  )
		{
			Record->EachangeAccepted( OrderSysID );
			AddOrderSysIDToMap( ExchangeID, OrderSysID.ToInt64(), Record );
		}
	}
}
//---------------------------------------------------------------------------
void  TTaifexConnection::OrderFill( UFC::NameValueMessage& TradeLog )
{
	UFC::AnsiString ExchangeID,OrderLocalID,OrderSysID,FillSequence,ExceQty,ExecID;
	COrderInfo*     Ord;

	TradeLog.Get("OLID", OrderLocalID, false );
	TradeLog.Get("OSID", OrderSysID, true );
	TradeLog.Get("FSEQ", FillSequence, true );
	TradeLog.Get("EX",   ExchangeID, true );
	TradeLog.Get("LQTY", ExceQty, true );
	if( ( Ord = FindOrderByOrderSysID( ExchangeID, OrderSysID.ToInt64() ) ) != NULL ||
		( Ord = FindOrderByOrderLocalID( ExchangeID, OrderLocalID.ToInt() )) != NULL )
	{
		ExecID.Printf( "F_%s_%lld_%d", ExchangeID.c_str(), OrderSysID.ToInt64(), FillSequence.ToInt() );
		if( IsExecutionDup( nsOrderMessageDefine::mCNFutures, ExecID ) == false )
			Ord->Fill( ExceQty.ToInt() );
	}
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
