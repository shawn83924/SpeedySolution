//---------------------------------------------------------------------------
#include "TTaifexTMPConnection.h"
#include "MemoryStream.h"
//---------------------------------------------------------------------------
BOOL TTaifexTMPConnection::ParseUserID( const UFC::AnsiString& String, UFC::AnsiString& AE, UFC::AnsiString& Data)
{	///< Layout for the "KEY" field: AE,Data
	Int32 SplitPos = String.AnsiPos(',');

	if( SplitPos !=  -1 )
	{
		AE   = String.SubString( 0, (UInt16)SplitPos );
		Data = String.SubString( (UInt16)(SplitPos + 1), (UInt16)(String.Length() - SplitPos) );
		return TRUE;
	}
	else
	{
		AE = String;
		Data = "";
		return FALSE;
	}
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::FillExecution( nsOrderMessageDefine::MarketEnum Market,
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

	if( ExecType == '6' || ExecType == 'F' )///< Filled or partially filled.
	{
		if( ExecutionReport->GetLeavesQty( ) == 0 )  ///< Leaves == 0 -> Canceled or Filled
		{
			if( ExecutionReport->GetLastQty() == 0 ) ///< FOK/IOC canceled
			{
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
			}
			else
			{
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osFilled );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etFilled );
			}
		}
		else  ///< Leaves > 0 -> Partially Filled
		{
			ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osPartiallyFilled );
			ExecutionReport->SetExecType( nsOrderMessageDefine::etPartiallyFilled );
		}
	}
	else ///< Handle Canceled, Replaceed
	{
		if( StatusCode == 0  || StatusCode == 32  || StatusCode == 246 || StatusCode == 247||
			StatusCode == 248|| StatusCode == 249 || StatusCode == 250 )
		{
			if( ExecType == '5' || ExecType == 'M' || ExecType == 'm' ) ///<  means Replace order
			{
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osReplaced );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etReplaced );
			}
			else if( ExecType == '4'  )///<  means Cancel order
			{
				ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
				ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
			}
		}
		else
		{
			UFC::AnsiString ErrMsg( "TAIFEX Error" );
			UFC::AnsiString ErrCode( StatusCode );

			if( ExecType == '5' || ExecType == 'M' || ExecType == 'm' ) ///<  means Reject Replace order
				Reject( nsOrderMessageDefine::crrReplace, ErrCode, ErrMsg, ExecutionReport );
			else if( ExecType == '4'  )///<  means Reject Cancel order
				Reject( nsOrderMessageDefine::crrCancel, ErrCode, ErrMsg, ExecutionReport );
		}
    }
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::ReceiveFutExecuteMessage( MTree* pTree )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::ReceiveFutExecuteMessage" );
	UFC::AnsiString ExecuteMessage, Key, AE, Data, ExecID, PBNO, CDKey, PVC, TMPExtStr;
	Int32           NID,TMPSeq,DataSize,IsBin;
	char            ExecType;
	char*           BinData;

	if( pTree->get( "BIN", IsBin ) && (IsBin == 1 ) &&
		pTree->get( "FILL_ORDER", BinData, DataSize ) &&
		pTree->get( "NID", NID ) &&
		pTree->get( "KEY", Key ) &&
		pTree->get( "PBNO", PBNO ))
	{
		UFC::MemoryReadStream   Stream( BinData, DataSize );
		TMP::TMPMessage         R02( &Stream );
		TExecutionReportMessage ExecutionReport;

		TMPSeq = R02["rpt_seq"].ToInteger();
		ExecType = (char)R02["ExecType"].ToInteger();
		ExecID.Printf( "FUT_F%c%s%08d", ExecType, PBNO.c_str(), TMPSeq );
		ExecutionReport.SetMarket( nsOrderMessageDefine::mTWFutures );
		ExecutionReport.SetReportSequence( TMPSeq );
		ExecutionReport.SetNID( NID );
		ExecutionReport.SetLINBRN( PBNO.c_str() );
		ExecutionReport.SetExecID( ExecID.c_str() );
		ExecutionReport.SetPVC( UFC::AnsiString( R02.GetSessionID()).c_str() );
		if( pTree->get( "CKEY", CDKey ) == TRUE && ParseUserID( CDKey, AE, Data ) == TRUE )
		{
			ExecutionReport.SetData( Data.c_str());
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " CKEY[%s]", CDKey.c_str() );
		}
		else
		{
			if( ParseUserID( Key, AE, Data ) )
				ExecutionReport.SetData( Data.c_str());
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," KEY[%s]", Key.c_str() );
		}
		ExecutionReport.SetAE( AE.c_str() );
		UFC::BufferedLog::DebugPrintf( UFC::dlInformation," PBNO[%s]", PBNO.c_str() );
		FillExecution( nsOrderMessageDefine::mTWFutures, R02, &ExecutionReport );
		TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTWFutures, ExecID ) );
		if( UFC::BufferedLog::DebugLevelExist(UFC::dlInformation ) )
		{
			UFC::AnsiString TMPMsg;

			R02.ToString( TMPMsg );
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," Futures Execution[%s]",TMPMsg.c_str() );
		}
	}
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::ReceiveOptExecuteMessage( MTree* pTree )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::ReceiveOptExecuteMessage" );
	UFC::AnsiString ExecuteMessage, Key, AE, Data, ExecID, PBNO, CDKey, PVC, TMPExtStr;
	Int32           NID,TMPSeq,DataSize,IsBin;
	char            ExecType;
	char*           BinData;

	if( pTree->get( "BIN", IsBin ) && (IsBin == 1 ) &&
		pTree->get( "FILL_ORDER", BinData, DataSize ) &&
		pTree->get( "NID", NID ) &&
		pTree->get( "KEY", Key ) &&
		pTree->get( "PBNO", PBNO ))
	{
		UFC::MemoryReadStream   Stream( BinData, DataSize );
		TMP::TMPMessage         R02( &Stream );
		TExecutionReportMessage ExecutionReport;

		TMPSeq = R02["rpt_seq"].ToInteger();
		ExecType = (char)R02["ExecType"].ToInteger();
		ExecID.Printf( "OPT_F%c%s%08d", ExecType, PBNO.c_str(), TMPSeq );
		ExecutionReport.SetMarket( nsOrderMessageDefine::mTWOptions );
		ExecutionReport.SetReportSequence( TMPSeq );
		ExecutionReport.SetNID( NID );
		ExecutionReport.SetLINBRN( PBNO.c_str() );
		ExecutionReport.SetExecID( ExecID.c_str() );
		ExecutionReport.SetPVC( UFC::AnsiString( R02.GetSessionID()).c_str() );
		if( pTree->get( "CKEY", CDKey ) == TRUE && ParseUserID( CDKey, AE, Data ) == TRUE )
		{
			ExecutionReport.SetData( Data.c_str());
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," CKEY[%s]", CDKey.c_str() );
		}
		else
		{
			if( ParseUserID( Key, AE, Data ) )
				ExecutionReport.SetData( Data.c_str());
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," KEY[%s]", Key.c_str() );
		}
		ExecutionReport.SetAE( AE.c_str());
		UFC::BufferedLog::DebugPrintf( UFC::dlInformation," PBNO[%s]", PBNO.c_str() );
		FillExecution( nsOrderMessageDefine::mTWOptions, R02, &ExecutionReport );
		TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTWOptions, ExecID ) );
		if( UFC::BufferedLog::DebugLevelExist(UFC::dlInformation ) )
		{
			UFC::AnsiString TMPMsg;

			R02.ToString( TMPMsg );
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," Options Execution[%s]",TMPMsg.c_str() );
		}
	}
}
//---------------------------------------------------------------------------

