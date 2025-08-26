//---------------------------------------------------------------------------
#include "TTaifexConnection.h"
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//---------------------------------------------------------------------------
BOOL TTaifexConnection::ParseUserID( const UFC::AnsiString& String, UFC::AnsiString& AE, UFC::AnsiString& Data)
{
    ///< Layout for the "KEY" field.
    ///< AE,Data
    Int32 SplitPos = String.AnsiPos(',');
    
    if( SplitPos !=  -1 ) 
    {
        AE   = String.SubString( 0, SplitPos );
        Data = String.SubString( SplitPos + 1, String.Length() - SplitPos );
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
UFC::TRecord* TTaifexConnection::ParseExecuteReport( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& ExecuteMessage )
{
	int Length = ExecuteMessage.Length();
	char Type = ExecuteMessage[0];

	if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
	{
		switch( Type )
		{
			case '1':///< Filled OPT Single
				if( FReportType == rdConfirm ) ///< Confirm Only! Skip Filled
				    return NULL;
				return FC030Parser.Parse( ExecuteMessage.c_str(), Length );
			case '2':///< Multi-leg order filled.
				if( FReportType == rdConfirm ) ///< Confirm Only! Skip Filled
				    return NULL;
				return FC030MParser.Parse( ExecuteMessage.c_str(), Length );
			case '3':///< Canceled/ Replace
				if( FReportType == rdFill ) ///< Filled Only! Skip Cancel.
				    return NULL;
				return FC030CRParser.Parse( ExecuteMessage.c_str(), Length );
			case '4':///< Quote Canceled/ Replace
				if( FReportType == rdFill ) ///< Filled Only! Skip Replace.
				    return NULL;
				return FC030QCRParser.Parse( ExecuteMessage.c_str(), Length );
		}
    }
	else if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mES )
	{
		if( FIsTWSENewVersion == true )
			return FTSER03ExParser.Parse( ExecuteMessage.c_str(), Length );
		else
			return FTSEC030Parser.Parse( ExecuteMessage.c_str(), Length );
	}
	else if( Market == nsOrderMessageDefine::mOTC )
	{
		if( FIsTWSENewVersion == true )
			return FOTCR03ExParser.Parse( ExecuteMessage.c_str(), Length );
		else
			return FOTCC030Parser.Parse( ExecuteMessage.c_str(), Length );
    }
    return NULL;
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillFilledExecution( UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport, int Precision)
{
    int Qty = 0;
    UFC::AnsiString Value;
    
    if( pRecord->GetField( "OpenOffsetFlag", Value ) )
    {
        if( Value == "0" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peOpen );
        else if( Value == "1" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peClose );
        else if( Value == "2" )
			ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peDayTrade );
        else if( Value == "A" )
			ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peTAIFEXAuto );
        else if( Value == "7" )
			ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peForceClose );        
        else if( Value == "9" )
			ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peTMPMarketMaker );
    }
    if( pRecord->GetField( "OrderType", Value ) ) 
    {
        if( Value == "1" || Value == "M")
                ExecutionReport->SetOrderType( nsOrderMessageDefine::otMarket );
        else if( Value == "2" || Value == "L")
                ExecutionReport->SetOrderType( nsOrderMessageDefine::otLimit );
        else if( Value == "3" || Value == "P")
                ExecutionReport->SetOrderType( nsOrderMessageDefine::otMarketWithProtection );
    }
    if( FX25StyleQtyPx == true )///< X.25 style Price/qty
    {   ///< 1. OrderQty, Last filled qty.
        ///< 2. Price, Last filled price.
        ///< 3. LastPx, Original order price.
        if( pRecord->GetField( "Price", Value ) )
            ExecutionReport->SetPrice( UFC::IntToDouble( Value.ToInt(), Precision ) );
        if( pRecord->GetField( "Qty", Value ) )
        {
            Qty = Value.ToInt();
            ExecutionReport->SetOrderQty( Qty );
        }
    }
    else
    {
        if( pRecord->GetField( "Price", Value ) )
            ExecutionReport->SetLastPx( UFC::IntToDouble( Value.ToInt(), Precision ) );
        if( pRecord->GetField( "Qty", Value ) )
        {
            Qty = Value.ToInt();
            ExecutionReport->SetLastQty( Qty );
        }
    }
    if( Qty == 0 && FFillZeroToCancel == TRUE ) ///< Fill Qty equals 0 means this order was been canceled.
    {
        ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
        ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
    }
    else 
    {
        ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osPartiallyFilled );
        ExecutionReport->SetExecType( nsOrderMessageDefine::etPartiallyFilled );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillMultilegExecution( UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport, int Precision)
{
    int Qty = 0;
    UFC::AnsiString Value;
    
    if( pRecord->GetField( "OpenOffsetFlag", Value ) )
    {
        if( Value == "0" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peOpen );
        else if( Value == "1" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peClose );
        else if( Value == "2" )
			ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peDayTrade );
		else if( Value == "A" )
			ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peTAIFEXAuto );
		else if( Value == "9" )
			ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peTMPMarketMaker );
    }    
    if( pRecord->GetField( "OrderType", Value ) ) 
    {
		if( Value == "1" || Value == "M")
			ExecutionReport->SetOrderType( nsOrderMessageDefine::otMarket );
		else if( Value == "2" || Value == "L")
			ExecutionReport->SetOrderType( nsOrderMessageDefine::otLimit );
		else if( Value == "3" || Value == "P")
			ExecutionReport->SetOrderType( nsOrderMessageDefine::otMarketWithProtection );
	}
	if( FX25StyleQtyPx == true )///< X.25 style Price/qty
	{   ///< 1. OrderQty, Last filled qty.
		///< 2. Price, Last filled price.
		///< 3. LastPx, Original order price.
		if( pRecord->GetField( "Price", Value ) )
                    ExecutionReport->SetPrice( UFC::IntToDouble( Value.ToInt(), Precision ) );
		if( pRecord->GetField( "Qty", Value ) )
		{
                    Qty = Value.ToInt();
                    ExecutionReport->SetOrderQty( Qty );
		}
	}
	else
	{
		if( pRecord->GetField( "Price", Value ) )
                    ExecutionReport->SetLastPx( UFC::IntToDouble( Value.ToInt(), Precision ) );
		if( pRecord->GetField( "Qty", Value ) )
		{
                    Qty = Value.ToInt();
                    ExecutionReport->SetLastQty( Qty );
		}
	}
	ExecutionReport->SetNoLegs( 2 );
    ///< Fill leg 1
    if( pRecord->GetField( "LegSymbol1", Value ) )
        ExecutionReport->SetLegSymbol1( Value.c_str() );
    if( pRecord->GetField( "LegSide1", Value ) )
        ExecutionReport->SetLegSide1( (Value == "B") ? (nsOrderMessageDefine::sBuy) : (nsOrderMessageDefine::sSell) );
    if( pRecord->GetField( "LegQty1", Value ) )
        ExecutionReport->SetLegQty1( Value.ToInt() );
    if( pRecord->GetField( "LegPrice1", Value ) )
        ExecutionReport->SetLegPrice1( UFC::IntToDouble( Value.ToInt(), Precision ) );
    ///< Fill leg 2
    if( pRecord->GetField( "LegSymbol2", Value ) )
        ExecutionReport->SetLegSymbol2( Value.c_str() );
    if( pRecord->GetField( "LegSide2", Value ) )
        ExecutionReport->SetLegSide2( (Value == "B") ? (nsOrderMessageDefine::sBuy) : (nsOrderMessageDefine::sSell) );
    if( pRecord->GetField( "LegQty2", Value ) )
        ExecutionReport->SetLegQty2( Value.ToInt() );
    if( pRecord->GetField( "LegPrice2", Value ) )
        ExecutionReport->SetLegPrice2( UFC::IntToDouble( Value.ToInt(), Precision ) );
    if( Qty == 0 && FFillZeroToCancel == TRUE ) ///< Fill Qty equals 0 means this order was been canceled.
    {
        ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
        ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
    }
    else 
    {
        ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osPartiallyFilled );
        ExecutionReport->SetExecType( nsOrderMessageDefine::etPartiallyFilled );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillCancelReplaceExecution( UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport )
{
    UFC::AnsiString StatusCode, TransCode, AfterQty, BeforeQty;

    if( pRecord->GetField( "AfterQty", AfterQty ) )
    {
        ExecutionReport->SetLeavesQty( AfterQty.ToInt() );
        ExecutionReport->SetAfterQty( AfterQty.ToInt() );
    }
    if( pRecord->GetField( "BeforeQty", BeforeQty ) )
    {
        ExecutionReport->SetOrderQty( BeforeQty.ToInt() - AfterQty.ToInt() );///< Qty changed.
        ExecutionReport->SetBeforeQty( BeforeQty.ToInt() );
    }    
    pRecord->GetField( "StatusCode", StatusCode );
    pRecord->GetField( "TransCode", TransCode );
    if( StatusCode == "00" || StatusCode == "32" )
    {
        if( TransCode == "2" ) ///< TransCode = 2 means Replace order
        {
            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osReplaced );
            ExecutionReport->SetExecType( nsOrderMessageDefine::etReplaced );
        }
        else if( TransCode == "3" )///< TransCode = 3 means Cancel order
        {
            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
            ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
        }
    }
    else
    {
    	UFC::AnsiString ErrMsg( "TAIFEX Error" );

        ExecutionReport->SetStatusCode( StatusCode.c_str() );
        if( TransCode == "2" ) ///< TransCode = 2 means Replace order
            FillRejectMsg( nsOrderMessageDefine::crrReplace, StatusCode, ErrMsg, ExecutionReport );
        else if( TransCode == "3" )///< TransCode = 2 means Cancel order
            FillRejectMsg( nsOrderMessageDefine::crrCancel, StatusCode, ErrMsg, ExecutionReport );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillExecutionBase( nsOrderMessageDefine::MarketEnum Market,
					   nsOrderMessageDefine::TradingSessionIDEnum TradingSession,
					   UFC::TRecord* pRecord,
					   TExecutionReportMessage* ExecutionReport,
										   int& Precision  )
{
    UFC::AnsiString Value, Symbol, OldSymbol, MaturityMonthYear;    
    
    if( pRecord->GetField( "OrderID", Value ) )
        ExecutionReport->SetOrderID( Value.c_str() );
    else
        ExecutionReport->SetText("Missing field OrderID");    
    if( pRecord->GetField( "Account", Value ) )
        ExecutionReport->SetAccount( Value.c_str() );
    else
        ExecutionReport->SetText("Missing field Account");    
    if( pRecord->GetField( "BrokerID", Value ) )
        ExecutionReport->SetBrokerID( Value.c_str() );
    else
        ExecutionReport->SetText("Missing field BrokerID");    
    if( pRecord->GetField( "Side", Value ) )
        ExecutionReport->SetSide( (Value == "B") ? (nsOrderMessageDefine::sBuy) : (nsOrderMessageDefine::sSell) );
    else
        ExecutionReport->SetText("Missing field Side");    
    if( pRecord->GetField( "AccountFlag", Value ) )
        ExecutionReport->SetAccountFlag( Value.c_str() );    
    if( pRecord->GetField( "ProcessTime", Value ) )
        ExecutionReport->SetTransactTime( Value.c_str() );
    if( pRecord->GetField( "MaturityMonthYear", Value ) ) ///< For old version futures only
        ExecutionReport->SetMaturityMonthYear( Value.c_str() );
    if( pRecord->GetField( "ClearMemberID", Value ) )
        ExecutionReport->SetCMID( Value );
    if( pRecord->GetField( "Symbol", Symbol ) ) 
    {
        Symbol.TrimRight();
        OldSymbol = Symbol;
	Precision = GetPricePrecision( Market, TradingSession, Symbol );
        pRecord->GetField( "MaturityMonthYear", MaturityMonthYear );///< For old version futures only
	if( Market == nsOrderMessageDefine::mTWFutures && FNewFutSymbol == FALSE )
            FFutSymbol.ConvertToOldSymbol( Symbol, OldSymbol, MaturityMonthYear );
        if( MaturityMonthYear.Length() == 6 )
            ExecutionReport->SetMaturityMonthYear( MaturityMonthYear.c_str() );
        ExecutionReport->SetSymbol( OldSymbol.c_str() );
    }
    else
        ExecutionReport->SetText("Missing field Symbol");
    ExecutionReport->SetPxDigit( Precision );
    if( pRecord->GetField( "RecordType", Value ) ) 
    {
        if( Value == "1" ) ///< Filled
            FillFilledExecution( pRecord, ExecutionReport, Precision );
        else if( Value == "2" )///< MultiLeg
            FillMultilegExecution( pRecord, ExecutionReport, Precision );
        else if( Value == "3" )///< Canceled/ Replace
            FillCancelReplaceExecution( pRecord, ExecutionReport );
        else if( Value == "4" )///< Quote Canceled/ Replace
            FillCancelReplaceExecution( pRecord, ExecutionReport );
    }
    else
        FillFilledExecution( pRecord, ExecutionReport, Precision );
}
//---------------------------------------------------------------------------
void TTaifexConnection::TAIFEXFilledExecID( bool IsFut, UFC::AnsiString& Seq, const UFC::AnsiString& PBNO, int NID, int PartID, UFC::AnsiString& ExecID )
{
    Seq.PadThis( 8, '0' );
    if( IsFut == true )
    {
        if( Seq.ToInt() == 0 ) ///< Fate Filled
            ExecID.Printf( "FUTf%s%10d", PBNO.c_str(), NID );
        else
            ExecID.Printf( "FUTF%s%d%s", PBNO.c_str(), PartID, Seq.c_str());
    }
    else
    {
        if( Seq.ToInt() == 0 ) ///< Fate Filled
            ExecID.Printf( "OPTf%s%10d", PBNO.c_str(), NID );
        else
            ExecID.Printf( "OPTF%s%d%s", PBNO.c_str(), PartID, Seq.c_str());
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::UpdateTAIFEXAEUDD( MTree* pTree, UFC::AnsiString& Key, UFC::AnsiString& AE, TExecutionReportMessage& ExecutionReport )
{
	UFC::AnsiString  CDKey,Data;

	if( pTree->get( "CKEY", CDKey ) == TRUE && ParseUserID( CDKey, AE, Data ) == TRUE )
	{
		ExecutionReport.SetAllUserData( Data.c_str());
		Glog->fprintf( " CKEY[%s]", CDKey.c_str() );
	}
	else
	{
		if( ParseUserID( Key, AE, Data ) )
			ExecutionReport.SetAllUserData( Data.c_str());
		Glog->fprintf( " KEY[%s]", Key.c_str() );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::UpdateTAIFEXTradingSession( MTree* pTree, TExecutionReportMessage& ExecutionReport )
{
	Int32 IsOffHour;

	if( pTree->get( "OFFHOUR", IsOffHour ) == TRUE && IsOffHour ) /// Allen Modify at 20190615
		ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );
	else
		ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
}
//---------------------------------------------------------------------------
void TTaifexConnection::UpdateTAIFEXTMPExt( MTree* pTree, TExecutionReportMessage& ExecutionReport, int Precision )
{
    UFC::AnsiString TMPExtStr;
    Int32           TMPSeq;

    if( pTree->get( "TMPExt", TMPExtStr ) == TRUE )
    {
        Glog->fprintf( " TMP[%s]", TMPExtStr.c_str() );
        UpdateTMPFields( TMPExtStr, ExecutionReport, Precision );
    }
    else if( pTree->get( "RPTSEQ", TMPSeq ) == TRUE )
        ExecutionReport.SetReportSequence( TMPSeq );
}
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveFutExecuteMessage( MTree* pTree )
{
    UFC::AnsiString ExecuteMessage, Key, AE, Seq, ExecID, PBNO, PVC,eTime;
    UFC::TRecord*   pRecord;
    Int32           NID;
    int             Precision = 2;

    Glog->fprintf( " --------------------- TAIFEX Futures Filled --------------------" );
    if( pTree->get( "FILL_ORDER", ExecuteMessage ) &&
        pTree->get( "NID", NID )   && pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) && pTree->get( "PVC", PVC ) )
    {
        pRecord = ParseExecuteReport( nsOrderMessageDefine::mTWFutures, ExecuteMessage );
        if( pRecord != NULL && pRecord->GetField( "Seq", Seq ) )
        {
            TExecutionReportMessage ExecutionReport;

            ExecutionReport.SetMarket( nsOrderMessageDefine::mTWFutures );
            Glog->fprintf( " FILL[%u][%s]", (UInt32)NID, ExecuteMessage.c_str() );
            UpdateTAIFEXAEUDD( pTree, Key, AE, ExecutionReport );
            Glog->fprintf( " PVC[%s] PBNO[%s]", PVC.c_str(), PBNO.c_str() );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            ExecutionReport.SetAE( AE.c_str() );
            ExecutionReport.SetNID( NID );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetSrc( ExecuteMessage );
            ExecutionReport.SetPVC( PVC.c_str() );
            UpdateTAIFEXTradingSession( pTree, ExecutionReport );
            FillExecutionBase( nsOrderMessageDefine::mTWFutures, nsOrderMessageDefine::tsNormal, pRecord, &ExecutionReport, Precision );
            UpdateTAIFEXTMPExt( pTree, ExecutionReport, Precision );
            TAIFEXFilledExecID( true, Seq, PBNO, NID, ExecutionReport.GetTMPPartID(), ExecID );
            ExecutionReport.SetExecID( ExecID.c_str() );
            
            bool isProxyAccountExist = true;
            if( FIsProxy && ( FAdmin == 0 ) )
            {
                UFC::AnsiString rptAccount( ExecutionReport.GetAccount() );
                if( rptAccount.Length() > 0 )
                {
                    if( FAccountSet.Exists( rptAccount ) == FALSE )
                        isProxyAccountExist = false;
                }
                else
                    isProxyAccountExist = false;
            }
            
            if( isProxyAccountExist )
            {
                if( ExecutionReport.GetReportSequence( ) == 0 )
                    TrigerOnExecutionReport( &ExecutionReport, edSpeedyGenerate );
                else
                    TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTWFutures, ExecID ) );
            }
            delete pRecord;
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveOptExecuteMessage( MTree* pTree )
{
    UFC::AnsiString ExecuteMessage, Key, AE, Seq, ExecID, PBNO, PVC, eTime;
    UFC::TRecord*   pRecord;
    Int32           NID;
    int             Precision = 2;

    Glog->fprintf( " --------------------- TAIFEX Options Filled --------------------" );
    if( pTree->get( "FILL_ORDER", ExecuteMessage ) &&
        pTree->get( "NID", NID )   && pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) && pTree->get( "PVC", PVC ) )
    {
        pRecord = ParseExecuteReport( nsOrderMessageDefine::mTWOptions, ExecuteMessage );

        if( pRecord !=NULL && pRecord->GetField( "Seq", Seq ) )
        {
            TExecutionReportMessage ExecutionReport;

            ExecutionReport.SetMarket( nsOrderMessageDefine::mTWOptions );            
            UpdateTAIFEXAEUDD( pTree, Key, AE, ExecutionReport );
            Glog->fprintf( " FILL[%u][%s]", (UInt32)NID, ExecuteMessage.c_str() );
            Glog->fprintf( " PVC[%s] PBNO[%s]", PVC.c_str(), PBNO.c_str() );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            ExecutionReport.SetAE( AE.c_str());
            ExecutionReport.SetNID( NID );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetSrc( ExecuteMessage );            
            ExecutionReport.SetPVC( PVC.c_str() );
            UpdateTAIFEXTradingSession( pTree, ExecutionReport );
            FillExecutionBase( nsOrderMessageDefine::mTWOptions, nsOrderMessageDefine::tsNormal, pRecord, &ExecutionReport, Precision );
            UpdateTAIFEXTMPExt( pTree, ExecutionReport, Precision );
            TAIFEXFilledExecID( false, Seq, PBNO, NID, ExecutionReport.GetTMPPartID(), ExecID );
            ExecutionReport.SetExecID( ExecID.c_str() );
            
            bool isProxyAccountExist = true;
            if( FIsProxy && ( FAdmin == 0 ) )
            {
                UFC::AnsiString rptAccount( ExecutionReport.GetAccount() );
                if( rptAccount.Length() > 0 )
                {
                    if( FAccountSet.Exists( rptAccount ) == FALSE )
                        isProxyAccountExist = false;
                }
                else
                    isProxyAccountExist = false;
            }

            if( isProxyAccountExist )
            {
                if( ExecutionReport.GetReportSequence( ) == 0 )
                    TrigerOnExecutionReport( &ExecutionReport, edSpeedyGenerate );
                else
                    TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTWOptions, ExecID ) );
            }
            delete pRecord;
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveTSEExecuteMessage( MTree* pTree )
{
    Glog->fprintf( " -------------------------- TSE Filled --------------------------" );
    UFC::AnsiString ExecuteMessage, Key, AE, Data, Seq, ExecID, RecNo, PBNO, PVC, TSEOrderType,PHost,Side,eTime,Excd("0");
    UFC::TRecord*   pRecord;
    Int32           NID;
    
    if( pTree->get( "FILL_ORDER", ExecuteMessage ) &&
        pTree->get( "NID", NID ) &&
        pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) &&
        pTree->get( "PVC", PVC ) )
    {
        Glog->fprintf( " FILL[%u][%s]", (UInt32)NID, ExecuteMessage.c_str() );
        Glog->fprintf( " KEY[%s]", Key.c_str() );
        Glog->fprintf( " PVC[%s] PBNO[%s]", PVC.c_str(), PBNO.c_str() );
        pRecord = ParseExecuteReport( nsOrderMessageDefine::mTSE, ExecuteMessage );

        if( pRecord != NULL &&
            pRecord->GetField( "Seq", Seq ) &&
            pRecord->GetField( "RecNo", RecNo ) &&
            pRecord->GetField( "Side", Side ) )
        {
            TExecutionReportMessage ExecutionReport;
            int Precision = (FIsTWSENewVersion == true ) ? 4 : 2;
            int SideInt;

            ExecutionReport.SetMarket( nsOrderMessageDefine::mTSE );
            if( ParseUserID( Key, AE, Data ) )
                ExecutionReport.SetAllUserData( Data.c_str());
            ExecutionReport.SetAE( AE.c_str());
            ExecutionReport.SetNID( NID );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetSrc( ExecuteMessage );
            ExecutionReport.SetExecID( ExecID.c_str() );
            ExecutionReport.SetRecNo( RecNo.c_str() );
            ExecutionReport.SetPVC( PVC.c_str() );
            ExecutionReport.SetReportSequence( Seq.ToInt() );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            if( pRecord->GetField( "TSEOrderType", TSEOrderType ) )
                ExecutionReport.SetTSEOrderType( TSEOrderType.c_str() );
            else
                ExecutionReport.SetTSEOrderType( "0" );
            if( pTree->get( "PHOST", PHost ) )
                ExecutionReport.SetProcessHost( PHost );
            if( pRecord->GetField( "EXCD", Excd )  && Excd == "2" )
            {
                FillExecutionBase( nsOrderMessageDefine::mTSE, nsOrderMessageDefine::tsOddLot, pRecord, &ExecutionReport, Precision );
                ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsOddLot );
                ExecutionReport.SetTSEExchangeCode( "2" );
            }
            else
            {
                FillExecutionBase( nsOrderMessageDefine::mTSE, nsOrderMessageDefine::tsNormal, pRecord, &ExecutionReport, Precision );
                ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
                ExecutionReport.SetTSEExchangeCode( Excd.c_str() );
            }
            if( Side[0] == 'B' )
                SideInt = 1000;
            else
                SideInt = 2000;
            ///< BrokerID + Buy/Sell + RecNo as unique ExecID.
            ///< FIX, Use Seq + RecNo map to ExecID.
            ExecID.Printf( "TSEF%s%d%s", ExecutionReport.GetBrokerID(), SideInt, RecNo.c_str());
            ExecutionReport.SetExecID( ExecID.c_str() );
            ExecutionReport.SetStatusCode( "00" );

            bool isProxyAccountExist = true;
            if( FIsProxy && ( FAdmin == 0 ) )
            {
                UFC::AnsiString rptAccount( ExecutionReport.GetAccount() );
                if( rptAccount.Length() > 0 )
                {
                    if( FAccountSet.Exists( rptAccount ) == FALSE )
                        isProxyAccountExist = false;
                }
                else
                    isProxyAccountExist = false;
            }
            if( isProxyAccountExist )
                TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTSE, ExecID ) );
            delete pRecord;
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveOTCExecuteMessage( MTree* pTree )
{
    Glog->fprintf( " -------------------------- OTC Filled --------------------------" );
    UFC::AnsiString ExecuteMessage, Key, AE, Data, Seq, ExecID, RecNo, PBNO, PVC, TSEOrderType,PHost,Side,eTime,Excd("0");
    UFC::TRecord*   pRecord;
    Int32           NID;

    if( pTree->get( "FILL_ORDER", ExecuteMessage ) &&
        pTree->get( "NID", NID ) &&
        pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) &&
        pTree->get( "PVC", PVC ) )
    {
        Glog->fprintf( " FILL[%u][%s]", (UInt32)NID, ExecuteMessage.c_str() );
	Glog->fprintf( " KEY[%s]", Key.c_str() );
	Glog->fprintf( " PVC[%s] PBNO[%s]", PVC.c_str(), PBNO.c_str() );
	pRecord = ParseExecuteReport( nsOrderMessageDefine::mOTC, ExecuteMessage );

        if( pRecord != NULL &&
            pRecord->GetField( "Seq", Seq ) &&
            pRecord->GetField( "RecNo", RecNo ) &&
            pRecord->GetField( "Side", Side ) )
        {
            TExecutionReportMessage ExecutionReport;
            int Precision = (FIsTWSENewVersion == true ) ? 4 : 2;
            int SideInt;

            ExecutionReport.SetMarket( nsOrderMessageDefine::mOTC );
            if( ParseUserID( Key, AE, Data ) )
                ExecutionReport.SetAllUserData( Data.c_str());
            ExecutionReport.SetAE( AE.c_str());
            ExecutionReport.SetNID( NID );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetSrc( ExecuteMessage );
            ExecutionReport.SetRecNo( RecNo.c_str() );
            ExecutionReport.SetPVC( PVC.c_str() );
            ExecutionReport.SetReportSequence( Seq.ToInt() );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            if( pRecord->GetField( "TSEOrderType", TSEOrderType ) )
                ExecutionReport.SetTSEOrderType( TSEOrderType.c_str() );
            else
                ExecutionReport.SetTSEOrderType( "0" );                
            if( pTree->get( "PHOST", PHost ) )
                ExecutionReport.SetProcessHost( PHost );
            if( pRecord->GetField( "EXCD", Excd )  && Excd == "2" )
            {
                FillExecutionBase( nsOrderMessageDefine::mOTC, nsOrderMessageDefine::tsOddLot, pRecord, &ExecutionReport, Precision );
                ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsOddLot );
                ExecutionReport.SetTSEExchangeCode( "2" );
            }
            else
            {
                FillExecutionBase( nsOrderMessageDefine::mOTC, nsOrderMessageDefine::tsNormal, pRecord, &ExecutionReport, Precision );
                ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
                    ExecutionReport.SetTSEExchangeCode( Excd.c_str() );
            }
            if( Side[0] == 'B' )
                SideInt = 1000;
            else
                SideInt = 2000;
            ///< BrokerID + Buy/Sell + RecNo as unique ExecID.
            ///< FIX, Use Seq + RecNo map to ExecID.
            ExecID.Printf( "OTCF%s%d%s", ExecutionReport.GetBrokerID(), SideInt, RecNo.c_str());
            ExecutionReport.SetExecID( ExecID.c_str() );
            ExecutionReport.SetStatusCode( "00" );

            bool isProxyAccountExist = true;
            if( FIsProxy && ( FAdmin == 0 ) )
            {
                UFC::AnsiString rptAccount( ExecutionReport.GetAccount() );
                if( rptAccount.Length() > 0 )
                {
                    if( FAccountSet.Exists( rptAccount ) == FALSE )
                        isProxyAccountExist = false;
                }
                else
                    isProxyAccountExist = false;
            }
            if( isProxyAccountExist )
                TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mOTC, ExecID ) );
            delete pRecord;
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveESExecuteMessage( MTree* pTree )
{
	Glog->fprintf( " -------------------------- ES Filled --------------------------" );
	UFC::AnsiString ExecuteMessage, Key, AE, Data, Seq, ExecID, RecNo, PBNO, PVC, TSEOrderType,PHost,Side,eTime,Excd("0");
	UFC::TRecord*   pRecord;
    Int32           NID;

    if( pTree->get( "FILL_ORDER", ExecuteMessage ) &&
		pTree->get( "NID", NID ) &&
		pTree->get( "KEY", Key ) &&
		pTree->get( "PBNO", PBNO ) &&
		pTree->get( "PVC", PVC ) )
    {
        Glog->fprintf( " FILL[%u][%s]", (UInt32)NID, ExecuteMessage.c_str() );
        Glog->fprintf( " KEY[%s]", Key.c_str() );
        Glog->fprintf( " PVC[%s] PBNO[%s]", PVC.c_str(), PBNO.c_str() );
        pRecord = ParseExecuteReport( nsOrderMessageDefine::mES, ExecuteMessage );

        if( pRecord != NULL &&
            pRecord->GetField( "Seq", Seq ) &&
            pRecord->GetField( "RecNo", RecNo ) &&
            pRecord->GetField( "Side", Side ) )
        {
            TExecutionReportMessage ExecutionReport;
            int Precision = (FIsTWSENewVersion == true )? 4 : 2;
            int SideInt;

            ExecutionReport.SetMarket( nsOrderMessageDefine::mES );
            if( ParseUserID( Key, AE, Data ) )
                ExecutionReport.SetAllUserData( Data.c_str());
            ExecutionReport.SetAE( AE.c_str() );
            ExecutionReport.SetNID(NID);
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetSrc( ExecuteMessage );
            ExecutionReport.SetRecNo( RecNo.c_str() );
            ExecutionReport.SetPVC( PVC.c_str() );
            ExecutionReport.SetReportSequence( Seq.ToInt() );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            if( pRecord->GetField( "TSEOrderType", TSEOrderType ) )
                ExecutionReport.SetTSEOrderType( TSEOrderType.c_str() );
            else
                ExecutionReport.SetTSEOrderType( "0" );            
            if( pTree->get( "PHOST", PHost ) )
                ExecutionReport.SetProcessHost( PHost );
            if( pRecord->GetField( "EXCD", Excd )  && Excd == "2" )
            {
                FillExecutionBase( nsOrderMessageDefine::mES, nsOrderMessageDefine::tsOddLot, pRecord, &ExecutionReport, Precision );
                ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsOddLot );
                ExecutionReport.SetTSEExchangeCode( "2" );
            }
            else
            {
                FillExecutionBase( nsOrderMessageDefine::mES, nsOrderMessageDefine::tsNormal, pRecord, &ExecutionReport, Precision );
                ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
                ExecutionReport.SetTSEExchangeCode( Excd.c_str() );
            }            
            if( Side[0] == 'B' )
                SideInt = 1000;
            else
                SideInt = 2000;
            ///< BrokerID + Buy/Sell + RecNo as unique ExecID.
            ///< FIX, Use Seq + RecNo map to ExecID.
            ExecID.Printf( "ESF%s%d%s", ExecutionReport.GetBrokerID(), SideInt, RecNo.c_str());
            ExecutionReport.SetExecID( ExecID.c_str() );
            ExecutionReport.SetStatusCode( "00" );
            TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mES, ExecID ) );
            delete pRecord;
        }
    }
}
//---------------------------------------------------------------------------

