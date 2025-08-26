//---------------------------------------------------------------------------
#include "TTaifexTMPConnection.h"
#include "../ExchangeUtility/Utility.h"
//---------------------------------------------------------------------------
void TTaifexTMPConnection::FillRejectExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,
												TMP::TMPMessage& R03,
												UFC::AnsiString StatusCode,
												UFC::AnsiString ErrMsg,
												TExecutionReportMessage* ExecutionReport )
{
	char            ExecType   = (char)R03["ExecType"].ToInteger();
	UFC::AnsiString BrokerID   = TMP::TMPMessage::GetBrokerID( (UInt16)(R03["fcm_id"].ToInteger()));

	ExecutionReport->SetOrderID( R03["order_no"].ToString().c_str() );
	ExecutionReport->SetBrokerID( BrokerID.c_str() );
	ExecutionReport->SetTMPBrokerID( R03["fcm_id"].ToInteger() );
	ExecutionReport->SetTMPExecType( (nsOrderMessageDefine::TMPExecTypeEnum)ExecType );
	switch( MessageType )
	{
		case nsOrderMessageDefine::mtNew:
			Reject( nsOrderMessageDefine::crrNew, StatusCode, ErrMsg, ExecutionReport );
			break;
		case nsOrderMessageDefine::mtReplace:
			Reject( nsOrderMessageDefine::crrReplace, StatusCode, ErrMsg, ExecutionReport );
			break;
		case nsOrderMessageDefine::mtCancel:
			Reject( nsOrderMessageDefine::crrCancel, StatusCode, ErrMsg, ExecutionReport );
			break;
		case nsOrderMessageDefine::mtOrderStatusRequest:
			Reject( nsOrderMessageDefine::crrOrderStatus, StatusCode, ErrMsg, ExecutionReport );
			break;
		case nsOrderMessageDefine::mtQuote:
			Reject( nsOrderMessageDefine::crrQuote, StatusCode, ErrMsg, ExecutionReport );
			break;
		case nsOrderMessageDefine::mtQuoteCancel:
			Reject( nsOrderMessageDefine::crrQuoteCancel, StatusCode, ErrMsg, ExecutionReport );
			break;
		case nsOrderMessageDefine::mtQuoteRequest:
			Reject( nsOrderMessageDefine::crrQuoteRequest, StatusCode, ErrMsg, ExecutionReport );
			break;
        default: break;
	}
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::FillExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,
										  nsOrderMessageDefine::MarketEnum Market,
										  TMP::TMPMessage& R02,
										  TExecutionReportMessage* ExecutionReport )
{
	UFC::AnsiString Account, Symbol, Time;
	UInt8			SymType;
	int             Precision;
	char            ExecType   = (char)R02["ExecType"].ToInteger();
	int             StatusCode = R02[ "status_code" ].ToInteger();
	UFC::AnsiString BrokerID   = TMP::TMPMessage::GetBrokerID( (UInt16)(R02["fcm_id"].ToInteger()));
	///< For R02 only.
	UFC::AnsiString CMID       = TMP::TMPMessage::GetBrokerID( (UInt16)(R02["cm_id"].ToInteger()));
	TMP::TMPSymbol* SymPtr     = (TMP::TMPSymbol*)R02.GetField( "Symbol" );

	CMID.PadThis( 4, '0' );
	Account.Printf( "%07d", R02["investor_acno"].ToInteger());
	ExecutionReport->SetAccount( Account.c_str() );
	ExecutionReport->SetAccountFlag( R02[ "investor_flag" ].ToString().c_str() );
	ExecutionReport->SetCMID( CMID.c_str() );
	if( Market == nsOrderMessageDefine::mTWFutures )
		SymPtr->GetStringSymbol( FALSE, SymType, Symbol );
	else
		SymPtr->GetStringSymbol( TRUE, SymType, Symbol );
	Symbol.TrimRight();
	ExecutionReport->SetSymbol( Symbol.c_str() );
	Precision = GetPricePrecision( Market, Symbol );
	ExecutionReport->SetPxDigit( Precision );
	ExecutionReport->SetOrderType( (nsOrderMessageDefine::OrderTypeEnum)R02[ "OrdType" ].ToInteger() );
	ExecutionReport->SetPrice( UFC::IntToDouble( R02[ "Price" ].ToInteger(), Precision ) );
	ExecutionReport->SetLastPx( UFC::IntToDouble( R02[ "LastPx" ].ToInteger(), Precision ) );
	ExecutionReport->SetOrderQty( R02[ "qty" ].ToInteger() );
	ExecutionReport->SetCumQty( R02[ "CumQty" ].ToInteger() );
	ExecutionReport->SetLastQty( R02[ "LastQty" ].ToInteger() );
	Time.Printf("%09d", R02[ "org_trans_time" ].ToInteger() );
	ExecutionReport->SetOrgTransactTime( Time.c_str() );
	ExecutionReport->SetLegSide1( (nsOrderMessageDefine::SideEnum)R02["leg_side1"].ToInteger() );
	ExecutionReport->SetLegSide2( (nsOrderMessageDefine::SideEnum)R02["leg_side2"].ToInteger() );
	///< For both R02,R22
	switch( (char)R02[ "PositionEffect" ].ToInteger() )
	{
		case 'O':ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peOpen );break;
		case 'C':ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peClose );break;
		case 'D':ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peRolled );break;
		case '9':ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peTMPMarketMaker );break;
		default: ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peOpen );break;
	}
	ExecutionReport->SetOrderID( R02["order_no"].ToString().c_str() );
	ExecutionReport->SetSide( (nsOrderMessageDefine::SideEnum)R02["Side"].ToInteger());
	Time.Printf("%09d", R02[ "TransactTime" ].ToInteger() );
	ExecutionReport->SetTransactTime( Time.c_str() );
	ExecutionReport->SetBrokerID( BrokerID.c_str() );
	ExecutionReport->SetBeforeQty( R02[ "before_qty" ].ToInteger());
	ExecutionReport->SetAfterQty( R02[ "LeavesQty" ].ToInteger() );
	ExecutionReport->SetLeavesQty( R02[ "LeavesQty" ].ToInteger() );
	ExecutionReport->SetLegQty1( R02[ "leg_qty1" ].ToInteger() );
	ExecutionReport->SetLegQty2( R02[ "leg_qty2" ].ToInteger() );
	ExecutionReport->SetLegPrice1( UFC::IntToDouble( R02[ "leg_px1" ].ToInteger(), Precision ) );
	ExecutionReport->SetLegPrice2( UFC::IntToDouble( R02[ "leg_px2" ].ToInteger(), Precision ) );
	ExecutionReport->SetTMPStstusCode( StatusCode );
	ExecutionReport->SetTMPBrokerID( R02["fcm_id"].ToInteger() );
	ExecutionReport->SetTMPExecType( (nsOrderMessageDefine::TMPExecTypeEnum)ExecType );
	switch( R02[ "TimeInForce" ].ToInteger() )
	{
		case 3:
		case 8: ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifIOC); break;
		case 4: ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifFOK); break;
		default:ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifROD); break;
	}
	switch( MessageType )
    {
        case nsOrderMessageDefine::mtNew:
                ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osNew );
                ExecutionReport->SetExecType( nsOrderMessageDefine::etNew );
                break;
        case nsOrderMessageDefine::mtReplace:
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osPendingReplace );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etPendingReplace );
				break;
		case nsOrderMessageDefine::mtCancel:
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osPendingCancel );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etPendingCancel );
				break;
		case nsOrderMessageDefine::mtOrderStatusRequest:
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osNew );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etOrderStatus );
				break;
		case nsOrderMessageDefine::mtQuoteRequest:
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osQuoteRequestAccept );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etQuoteRequestAccept );
				break;
		case nsOrderMessageDefine::mtQuote:
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osQuoteAccept );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etQuoteAccept );
				break;
		case nsOrderMessageDefine::mtQuoteCancel:
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osPendingCancel );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etPendingCancel );
				break;
		default: break;
	}
}
//------------------------------------------------------------------------------
//
// For Futures and Option
//
//------------------------------------------------------------------------------
void TTaifexTMPConnection::ReceiveFutConfirmMessage( MTree* pTree  )
{
	nsOrderMessageDefine::MessageTypeEnum MessageType;
	UFC::AnsiString CDKey, Key, AE, Data, ExecID, PBNO;
	Int32           NID,TMPSeq,DataSize,IsBin;
	char*           BinData;


	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::ReceiveFutConfirmMessage" );
	if( pTree->get( "BIN", IsBin ) && (IsBin == 1 ) &&
		pTree->get( "CONFIRM_ORDER", BinData, DataSize ) &&
		pTree->get( "NID", NID ) &&
		pTree->get( "KEY", Key ) &&
		pTree->get( "PBNO", PBNO ) && (NID != 0 ))
	{
		UFC::MemoryReadStream   Stream( BinData, DataSize );
		TMP::TMPMessage         R02( &Stream );
		TExecutionReportMessage ExecutionReport;
		int                     StatusCode = R02[ "status_code" ].ToInteger();

		MessageType = (nsOrderMessageDefine::MessageTypeEnum)( TNetworkID::GetMessageType( NID ) );
		TMPSeq = R02["rpt_seq"].ToInteger();
		if( pTree->get( "CKEY", CDKey ) == TRUE && ParseUserID( CDKey, AE, Data ) == TRUE )
		{
			ExecutionReport.SetData( Data.c_str());
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," CKEY[%s]", CDKey.c_str() );
		}
		else
		{
			if( ParseUserID( Key, AE, Data ) )///< Split UserID and user data.
				ExecutionReport.SetData( Data.c_str());
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," KEY[%s]", Key.c_str() );
		}
		ExecutionReport.SetMarket( nsOrderMessageDefine::mTWFutures );
		ExecutionReport.SetAE( AE.c_str() );
		ExecutionReport.SetNID( NID );
		ExecutionReport.SetLINBRN( PBNO.c_str() );
		ExecutionReport.SetPVC( UFC::AnsiString( R02.GetSessionID()).c_str() );
		UFC::BufferedLog::DebugPrintf( UFC::dlInformation," StatusCode[%d]", StatusCode );
		UFC::BufferedLog::DebugPrintf( UFC::dlInformation," PBNO[%s]", PBNO.c_str() );
		ExecutionReport.SetTMPStstusCode( StatusCode );
		if( StatusCode == 0 || StatusCode == 32 || StatusCode == 246 ||
			StatusCode == 247|| StatusCode == 248|| StatusCode == 249 || StatusCode == 250 ) ///< Succeed execution.
		{
			ExecID.Printf( "FUTC%s%d", PBNO.c_str(), TMPSeq );
			FillExecution( MessageType, nsOrderMessageDefine::mTWFutures, R02, &ExecutionReport );
		}
		else  ///< Reject, Status code != 0
		{
			UFC::AnsiString ErrMsg( "" );
			UFC::AnsiString ErrCode( StatusCode );

			pTree->get( "MSG", ErrMsg );
			ExecID.Printf( "FUTR%s%d", PBNO.c_str(), TMPSeq );
			FillRejectExecution( MessageType, R02, ErrCode, ErrMsg, &ExecutionReport );
		}
		ExecutionReport.SetExecID( ExecID.c_str() );
		TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTWFutures, ExecID ));
		if( UFC::BufferedLog::DebugLevelExist(UFC::dlInformation ) )
		{
			UFC::AnsiString TMPMsg;

			R02.ToString( TMPMsg );
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," Futures confirm[%s]",TMPMsg.c_str() );
		}
	}
}
//------------------------------------------------------------------------------
void TTaifexTMPConnection::ReceiveOptConfirmMessage( MTree* pTree )
{
	nsOrderMessageDefine::MessageTypeEnum MessageType;
	UFC::AnsiString CDKey, Key, AE, Data, ExecID, PBNO;
	Int32           NID,TMPSeq,DataSize,IsBin;
	char*           BinData;


	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::ReceiveOptConfirmMessage" );
	if( pTree->get( "BIN", IsBin ) && (IsBin == 1 ) &&
		pTree->get( "CONFIRM_ORDER", BinData, DataSize ) &&
		pTree->get( "NID", NID ) &&
		pTree->get( "KEY", Key ) &&
		pTree->get( "PBNO", PBNO ) && (NID != 0 ))
	{
		UFC::MemoryReadStream   Stream( BinData, DataSize );
		TMP::TMPMessage         R02( &Stream );
		TExecutionReportMessage ExecutionReport;
		int                     StatusCode = R02[ "status_code" ].ToInteger();

		MessageType = (nsOrderMessageDefine::MessageTypeEnum)( TNetworkID::GetMessageType( NID ) );
		TMPSeq = R02["rpt_seq"].ToInteger();
		if( pTree->get( "CKEY", CDKey ) == TRUE && ParseUserID( CDKey, AE, Data ) == TRUE )
		{
			ExecutionReport.SetData( Data.c_str());
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," CKEY[%s]", CDKey.c_str() );
		}
		else
		{
			if( ParseUserID( Key, AE, Data ) )///< Split UserID and user data.
				ExecutionReport.SetData( Data.c_str());
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," KEY[%s]", Key.c_str() );
		}
		ExecutionReport.SetMarket( nsOrderMessageDefine::mTWOptions );
		ExecutionReport.SetAE( AE.c_str() );
		ExecutionReport.SetNID( NID );
		ExecutionReport.SetLINBRN( PBNO.c_str() );
		ExecutionReport.SetPVC( UFC::AnsiString( R02.GetSessionID()).c_str() );
		UFC::BufferedLog::DebugPrintf( UFC::dlInformation," StatusCode[%d]", StatusCode );
		UFC::BufferedLog::DebugPrintf( UFC::dlInformation," PBNO[%s]", PBNO.c_str() );
		ExecutionReport.SetTMPStstusCode( StatusCode );
		if( StatusCode == 0  || StatusCode == 32 || StatusCode == 246 ||
			StatusCode == 247|| StatusCode == 248|| StatusCode == 249|| StatusCode == 250 ) ///< Succeed execution.
		{
			ExecID.Printf( "OPTC%s%d", PBNO.c_str(), TMPSeq );
			FillExecution( MessageType, nsOrderMessageDefine::mTWOptions, R02, &ExecutionReport );
		}
		else  ///< Reject, Status code != 0
		{
			UFC::AnsiString ErrMsg( "" );
			UFC::AnsiString ErrCode( StatusCode );

			pTree->get( "MSG", ErrMsg );
			ExecID.Printf( "OPTR%s%d", PBNO.c_str(), TMPSeq );
			FillRejectExecution( MessageType, R02, ErrCode, ErrMsg, &ExecutionReport );
		}
		ExecutionReport.SetExecID( ExecID.c_str() );
		TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTWOptions, ExecID ));
		if( UFC::BufferedLog::DebugLevelExist(UFC::dlInformation ) )
		{
			UFC::AnsiString TMPMsg;

			R02.ToString( TMPMsg );
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," Options confirm[%s]",TMPMsg.c_str() );
		}
	}
}
//------------------------------------------------------------------------------

