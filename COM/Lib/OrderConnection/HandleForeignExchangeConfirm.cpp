//---------------------------------------------------------------------------
#include "TTaifexConnection.h"
#include "../ExchangeUtility/Utility.h"
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveForeignConfirmMessage( MTree* pTree  )
{
    UFC::AnsiString ConfirmMessage = "", Key = "", PVC = "";
    UFC::AnsiString AE = "", Data = "", StatusCode = "", Value = "", OrdType = "", ExecID = "";
    Int32 NID = 0;
    bool is_CONFIRM_ORDER_Received = false, is_NID_Received = false, is_KEY_Received = false, is_PVC_Received = false;
    if( pTree->get( "CONFIRM_ORDER", ConfirmMessage ) == TRUE ) is_CONFIRM_ORDER_Received = true;
    if( pTree->get( "NID", NID ) == TRUE ) is_NID_Received = true;
    if( pTree->get( "KEY", Key ) == TRUE ) is_KEY_Received = true;
    if( pTree->get( "PVC", PVC ) == TRUE ) is_PVC_Received = true;

    Glog->fprintf( " ------------------------ Foreign Confirm -----------------------" );
    if( is_CONFIRM_ORDER_Received && ( is_NID_Received && ( NID != 0 ) ) &&
        is_KEY_Received && is_PVC_Received )
    {
        Glog->fprintf( " CONFIRM[%u][%s]", (UInt32)NID, ConfirmMessage.c_str() );
        Glog->fprintf( " Key[%s]",  Key.c_str() );

        UFC::NameValueMessage   PATSConfirm( "^\n" );
        TExecutionReportMessage ExecutionReport;
        ExecutionReport.SetSrc( ConfirmMessage.c_str());
        ExecutionReport.SetPVC( PVC.c_str() );
        PATSConfirm.FromString( ConfirmMessage );
        if( ParseUserID( Key, AE, Data ) )
            ExecutionReport.SetAllUserData( Data.c_str());
        ExecutionReport.SetAE( AE.c_str() );
        if( ( PATSConfirm.Get("cd", StatusCode ) == FALSE ) || ( StatusCode.Length() <= 0 ) )
        {
            Glog->fprintf( " Message missing field Code[cd]" );
            Glog->FlushToFile();
            return;
        }
        else
            ExecutionReport.SetStatusCode( StatusCode.c_str() );

        Int32 qtyFieldValue = 0;
        BOOL isQtyFieldExist = PATSConfirm.Get( "qty", Value );
        if( ( isQtyFieldExist == TRUE ) && ( Value.Length() > 0 ) )
            qtyFieldValue = Value.ToInt();

        Int32 cumQtyFieldValue = 0;
        BOOL isCumQtyFieldExist = PATSConfirm.Get( "cqty", Value );
        if( ( isCumQtyFieldExist == TRUE ) && ( Value.Length() > 0 ) )
            cumQtyFieldValue = Value.ToInt();

        Int32 leavesQtyFieldValue = 0;
        BOOL isleavesQtyFieldExist = PATSConfirm.Get( "lqt", Value );
        if( ( isleavesQtyFieldExist == TRUE ) && ( Value.Length() > 0 ) )
            leavesQtyFieldValue = Value.ToInt();

        ///< Type: New,Cancel or Replace
        if( PATSConfirm.Get( "oty", OrdType ) == TRUE )
        {
            if( OrdType == "CXL" ) ///< Cancel Order
            {
                if( StatusCode == "0000" )
                {
                    ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
                    ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
                    if( isQtyFieldExist == TRUE ) ExecutionReport.SetOrderQty( qtyFieldValue );
                    ExecutionReport.SetLeavesQty( 0 );
                }
                else
                {
                    ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
                    ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
                    ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrCancel );
                    if( isQtyFieldExist ) ExecutionReport.SetOrderQty( qtyFieldValue );
                    ExecutionReport.SetLeavesQty( 0 );
                }

                if( isCumQtyFieldExist == TRUE ) ExecutionReport.SetCumQty( cumQtyFieldValue );
            }
            else if( OrdType == "UPD" ) ///< Change Qty
            {
                if( StatusCode == "0000" )
                {
                    ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osReplaced );
                    ExecutionReport.SetExecType( nsOrderMessageDefine::etReplaced );
                    if( isQtyFieldExist == TRUE ) ExecutionReport.SetOrderQty( qtyFieldValue );
                    if( isCumQtyFieldExist == TRUE ) ExecutionReport.SetCumQty( cumQtyFieldValue );

                    Int32 qty = ExecutionReport.GetOrderQty();
                    Int32 cumQty = ExecutionReport.GetCumQty();
                    Int32 leavesQty = qty - cumQty;
                    if( leavesQty < 0 )
                    {
                        Glog->fprintf( " Wrong Qty Value: Qty[%d] cumQty[%d]", qty, cumQty );
                        Glog->FlushToFile();
                        return;
                    }

                    if( ( isleavesQtyFieldExist == TRUE ) && ( leavesQtyFieldValue != leavesQty ) )
                        Glog->fprintf( " Wrong Leaves Qty[%d] Field Value != Qty[%d] - cumQty[%d]", leavesQtyFieldValue, qty, cumQty );
                    ExecutionReport.SetLeavesQty( leavesQty );
                }
                else
                {
                    ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
                    ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
                    ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrReplace );
                }
            }
            else ///< New Order
            {
                if( StatusCode == "0000" )
                {
                    ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
                    ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
                    if( isQtyFieldExist == TRUE )
                    {
                        ExecutionReport.SetOrderQty( qtyFieldValue );
                        ExecutionReport.SetLeavesQty( qtyFieldValue );
                    }
                }
                else
                {
                    ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
                    ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
                    ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrNew );
                    if( isQtyFieldExist == TRUE ) ExecutionReport.SetOrderQty( qtyFieldValue );
                    ExecutionReport.SetLeavesQty( 0 );
                }  //if( StatusCode == "0000" )
            }  //if( OrdType ==  "CXL" )
        }
        else
        {
            OrdType = "";
            Glog->fprintf( " Message missing field OrderType" );
            Glog->FlushToFile();
//            return;
        }

        if( ( PATSConfirm.Get("ct", Value) == TRUE ) && ( Value.Length() > 0 ) )
        {
            if( Value == "0" )
                ExecutionReport.SetMarket( nsOrderMessageDefine::mForeignFutures );
            else if( Value == "1" )
                ExecutionReport.SetMarket( nsOrderMessageDefine::mForeignOptions );
            else if( Value == "2" )
                ExecutionReport.SetMarket( nsOrderMessageDefine::mForeignStock );
            else
            {
                Glog->fprintf( " Incorrect Commodity Type value:%s",Value.c_str() );
                Glog->FlushToFile();
                return;
            }
        }
        else
        {
            Glog->fprintf( " Message missing field Commodity Type" );
            Glog->FlushToFile();
            return;
        }

        if( PATSConfirm.Get("tif", Value) == TRUE )
        {
            if( Value == "R" )
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifROD );   //1
            else if( Value == "I" )
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifIOC );   //2
            else if( Value == "F" )
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifFOK );   //3
            else if( Value == "4" )
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifGTC );   //4
            else if( Value == "5" )
            {
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifGTD );   //5
                UFC::AnsiString expireDate;
                if( PATSConfirm.Get("exdt", expireDate) == TRUE )
                    ExecutionReport.SetExpireDate( expireDate.c_str() );
            }
            else if( Value == "6" )
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifOPG );   //6
            else if( Value == "7" )
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifCLS );   //7
            else if( Value == "8" )
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifGTX );   //8
            else
                ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifNone );  //0
        }

        if( (PATSConfirm.Get( "oid", Value ) == FALSE ) || ( Value.Length() <= 0 ) )
        {
            Glog->fprintf( " Message missing field OrderID" );
            Glog->FlushToFile();
            return;
        }
        else
            ExecutionReport.SetOrderID( Value.c_str() );

        if( PATSConfirm.Get( "poid", Value ) == TRUE )
            ExecutionReport.SetClOrdID( Value.c_str() );
        if( PATSConfirm.Get( "bs", Value ) == TRUE )
        {
            if( Value == "B" )
                ExecutionReport.SetSide( nsOrderMessageDefine::sBuy );
            else if( Value == "S" )
                ExecutionReport.SetSide( nsOrderMessageDefine::sSell );
            else
            {
                Glog->fprintf( " Incorrect side value:%s",Value.c_str() );
                Glog->FlushToFile();
                return;
            }
        }
        else
        {
            Glog->fprintf( " Message missing field Side" );
            Glog->FlushToFile();
            return;
        }
        if( PATSConfirm.Get("pe", Value) == TRUE )
	{
            if( Value == "O" )
                ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peOpen );
            else if( Value == "C" )
                ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peClose );
            else if( Value == "D" )
                ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peDayTrade );
            else if( Value == "A" )
                ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peAuto );
            else if( Value == "a" )
                ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peAutoToday );
            else
            {
                Glog->fprintf( " Incorrect PositionEffect value:%s",Value.c_str() );
                return;
            }
	}
        if( ( PATSConfirm.Get( "fcm", Value ) == FALSE ) || ( Value.Length() <= 0 ) )
            Glog->fprintf( " Message missing field BrokerID" );
        else
            ExecutionReport.SetBrokerID( Value.c_str());
        
        bool isProxyAccountExist = true;
        UFC::AnsiString rptAccount = "";
        if( PATSConfirm.Get( "acc", Value ) == TRUE ) rptAccount = Value;
        if( rptAccount.Length() > 0 )
        {
            ExecutionReport.SetAccount( rptAccount.c_str() );
            if( FIsProxy && ( FAdmin == 0 ) )
            {
                if( FAccountSet.Exists( rptAccount ) == FALSE )
                    isProxyAccountExist = false;
            }
        }
        else
        {
            Glog->fprintf( " Message missing field Account" );
            if( FIsProxy && ( FAdmin == 0 ) )
                isProxyAccountExist = false;
        }
        if( PATSConfirm.Get( "ssacc", Value ) == TRUE && Value.Length() > 0 ) 
            ExecutionReport.SetSellSideAccount( Value.c_str() );
        if( PATSConfirm.Get( "eoid", Value ) == TRUE && Value.Length() > 0 ) 
            ExecutionReport.SetExchangeOrderID( Value.c_str() );
        if( ( PATSConfirm.Get( "exh", Value ) == FALSE ) || ( Value.Length() <= 0 ) )
            Glog->fprintf( " Message missing field ExchangeCode" );
        else
            ExecutionReport.SetExchangeCode( Value.c_str());
        if( ( PATSConfirm.Get( "sym", Value ) == FALSE ) || ( Value.Length() <= 0 ) )
            Glog->fprintf( " Message missing field Symbol" );
        else
            ExecutionReport.SetSymbol( Value.c_str() );
        if( ( PATSConfirm.Get( "mmy", Value ) == FALSE ) || ( Value.Length() <= 0 ) )
            Glog->fprintf( " Message missing field Maturity Month Year" );
        else
            ExecutionReport.SetMaturityMonthYear( Value.c_str() );
        if( ( PATSConfirm.Get("px", Value) == FALSE ) || ( Value.Length() <= 0 ) )
            Glog->fprintf( " Message missing field Price" );
        else
            ExecutionReport.SetPrice( Value.ToDouble() );
        if( ( PATSConfirm.Get( "spx", Value) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetStopPx( Value.ToDouble() );
        if( ( PATSConfirm.Get( "pxb", Value ) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetPxBase( ( nsOrderMessageDefine::PriceBaseEnum )Value.ToInt() );
        else
            ExecutionReport.SetPxBase( nsOrderMessageDefine::pbNone );
        if( ( PATSConfirm.Get( "stkpx", Value ) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetStrikePrice(Value.ToDouble());
        else
            ExecutionReport.SetStrikePrice(0);
        if( PATSConfirm.Get("apx", Value) == TRUE )
            ExecutionReport.SetAvgPx( Value.ToDouble());        
        if( PATSConfirm.Get( "cp", Value ) == TRUE )
        {
            if( Value == "C" )
                ExecutionReport.SetEventType( nsOrderMessageDefine::evtCall );
            else if( Value == "P" )
                ExecutionReport.SetEventType( nsOrderMessageDefine::evtPut );
            else
                ExecutionReport.SetEventType( nsOrderMessageDefine::evtNone );
        }
        if( ( PATSConfirm.Get( "txt", Value) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetText( Value.c_str());
        if( ( PATSConfirm.Get( "rct", Value) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetTransactTime( Value.c_str() );
        if ((PATSConfirm.Get("rlt", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetMessageTime(Value.c_str());
        if( ( PATSConfirm.Get( "eucd", Value ) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetEUCD( Value.c_str() );
        ExecutionReport.SetNID( (UInt32)NID );
        if ((PATSConfirm.Get( "eid", Value) == TRUE) && (Value.Length() > 0))
            ExecID = Value;
        else
        {
            UFC::UDateTime Now;
            ExecID.Printf( "C_%d_%010d%02d%03d",(int)ExecutionReport.GetMarket(), NID, Now.getSecond(), Now.getMillisecond());
        }
        ExecutionReport.SetExecID( ExecID.c_str() );
        if( ( PATSConfirm.Get( "erid", Value ) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetExecRefID( Value.c_str() );
        if( ( PATSConfirm.Get( "oeid", Value ) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetOrigExecID( Value.c_str() );
        if( ( PATSConfirm.Get( "och", Value ) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetOrderChannel( Value.c_str() );
        if( ( PATSConfirm.Get( "ett", Value ) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetExecTransType( static_cast<nsOrderMessageDefine::ExecTransTypeEnum>( Value.ToInt() ) );
        if( ( PATSConfirm.Get( "et", Value ) == TRUE ) && ( Value.Length() > 0 ) )
        {
            nsOrderMessageDefine::ExecTypeEnum execType = static_cast<nsOrderMessageDefine::ExecTypeEnum>( Value.ToInt() );
            if( execType != nsOrderMessageDefine::etNone )
            {
                ExecutionReport.SetExecType( execType );
                if( execType == nsOrderMessageDefine::etRejected )
                {
                    if( ( PATSConfirm.Get( "rjto", Value ) == TRUE ) && ( Value.Length() > 0 ) )
                    {
                        int cxlRejResponseTo = Value.ToInt();
                        if( cxlRejResponseTo >= 0  ) ExecutionReport.SetFixCxlRejResponseTo( cxlRejResponseTo );
                    }
                    
                    if( ( PATSConfirm.Get( "crjr", Value ) == TRUE ) && ( Value.Length() > 0 ) )
                    {
                        int cxlRejReason = Value.ToInt();
                        if( cxlRejReason >= 0  ) ExecutionReport.SetFixCxlRejReason( cxlRejReason );
                    }
                    
                    if( ( PATSConfirm.Get( "orjr", Value ) == TRUE ) && ( Value.Length() > 0 ) )
                    {
                        int fixOrdRejReason = Value.ToInt();
                        if( fixOrdRejReason >= 0  ) ExecutionReport.SetFixOrdRejReason( fixOrdRejReason );
                    }
                }  //if( execType == nsOrderMessageDefine::etRejected )
                
            }  //if( execType != nsOrderMessageDefine::etNone )
        }
        if( ( OrdType == "CXL" ) || ( OrdType == "UPD" ) || ExecutionReport.GetExecType( ) == nsOrderMessageDefine::etRestated )
        {
            UFC::AnsiString origOrdType = "";
            if( PATSConfirm.Get( "ooty", origOrdType ) == TRUE )
            {
                origOrdType.TrimLeft(' ');
                origOrdType.TrimRight(' ');
                if (origOrdType.Length() > 0)
                    OrdType = origOrdType;
                else
                    Glog->fprintf( " Field Original OrderType is NULL." );
            }
            else
                Glog->fprintf( " Message missing field Original OrderType" );
            Glog->FlushToFile();
        }
        if( OrdType == "LMT" )
            ExecutionReport.SetOrderType( nsOrderMessageDefine::otLimit);
        else if( OrdType == "MKT" )
            ExecutionReport.SetOrderType( nsOrderMessageDefine::otMarket);
        else if( OrdType == "STP" )
            ExecutionReport.SetOrderType( nsOrderMessageDefine::otStop);
        else if( OrdType == "SWL" )
            ExecutionReport.SetOrderType( nsOrderMessageDefine::otStopLimit);
        else if( OrdType == "MWP" )
            ExecutionReport.SetOrderType( nsOrderMessageDefine::otMarketWithProtection);
        else if( OrdType == "MOC" )
            ExecutionReport.SetOrderType( nsOrderMessageDefine::otMarketOnClose);
        else if( OrdType == "LOC" )
            ExecutionReport.SetOrderType( nsOrderMessageDefine::otLimitOnClose);
        else
        {
            ExecutionReport.SetOrderType( nsOrderMessageDefine::otNone);
            if( OrdType.Length() > 0 ) Glog->fprintf( " Unknown OrderType[%s]", OrdType.c_str() );
            Glog->FlushToFile();
        }
        
        if ( ( PATSConfirm.Get( "os", Value ) == TRUE ) && ( Value.Length() > 0 ) )
        {
            nsOrderMessageDefine::OrderStatusEnum orderStatus = static_cast<nsOrderMessageDefine::OrderStatusEnum>( Value.ToInt() );
            if( orderStatus != nsOrderMessageDefine::osNone )
            {
                ExecutionReport.SetOrderStatus( orderStatus );
                if( orderStatus == nsOrderMessageDefine::osExpired )
                {
                    if( isleavesQtyFieldExist == TRUE )
                        ExecutionReport.SetLeavesQty( leavesQtyFieldValue );
                    else
                        ExecutionReport.SetLeavesQty( 0 );
                }
            }
        }
        
        if( (PATSConfirm.Get( "stgy", Value ) == TRUE ) && ( Value.Length() > 0 ) )
            ExecutionReport.SetAttribute("stgy", Value);
        
        long RptSeq;
                
        if( ( PATSConfirm.Get( "rseq", Value ) == TRUE ) && ( (RptSeq = Value.ToInt()) > 0 ) )
            ExecutionReport.SetReportSequence( RptSeq );        
        
        Value = "";
        ExecDup rptExecDup = (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecID );
        if( PATSConfirm.Get( "pdup", Value ) == TRUE )
        {
            if( Value == "Y" )
                rptExecDup = edPossibleDuplicate;
            else if( Value == "N" )
                rptExecDup = edNewExecution;
        }
        if( isProxyAccountExist ) TrigerOnExecutionReport( &ExecutionReport, rptExecDup );
    }
    else
    {
        UFC::PStringBuffer missingBuff;
        if( !is_CONFIRM_ORDER_Received ) missingBuff.Append( "CONFIRM_ORDER" );
            
        if( !is_NID_Received )
        {
            if( missingBuff.Length() > 0 ) missingBuff.Append( ", " );
            missingBuff.Append( "NID" );
        }

        if( !is_KEY_Received )
        {
            if( missingBuff.Length() > 0 ) missingBuff.Append( ", " );
            missingBuff.Append( "KEY" );
        }
            
        if( !is_PVC_Received )
        {
            if( missingBuff.Length() > 0 ) missingBuff.Append( ", " );
            missingBuff.Append( "PVC" );
        }
        
        UFC::PStringBuffer logBuff;
        logBuff.AppendPrintf( "Ignore CONFIRM[%u][", (UInt32)NID );
        if( ConfirmMessage.Length() > 0 )
            logBuff.AppendPrintf( " %s]", ConfirmMessage.c_str() );
        else
            logBuff.Append( ']' );
        
        if( is_NID_Received && ( NID == 0 ) )
            logBuff.Append( ", NID is 0" );
        
        if( missingBuff.Length() > 0 )
            logBuff.AppendPrintf( ", Missing Node:%s", missingBuff.c_str() );
        
        Glog->fprintf( " %s.", logBuff.c_str() );
    }
    Glog->FlushToFile();
}
 //------------------------------------------------------------------------------
void TTaifexConnection::ReceiveCNConfirmMessage( MTree* pTree  )
{
	UFC::AnsiString ConfirmMessage, Key, AE, Data, PVC;
	Int32           NID;
	bool GetNIDSuccess            = pTree->get( "NID", NID );
	bool GetConfirmMessageSuccess = pTree->get( "CONFIRM_ORDER", ConfirmMessage );
	bool GetKeySuccess            = pTree->get( "KEY", Key );

	if ((GetNIDSuccess && (NID != 0)) && GetConfirmMessageSuccess && GetKeySuccess)
	{
		UFC::NameValueMessage   GLQHConfirm( "^\n" );
		TExecutionReportMessage ExecutionReport;

		Glog->fprintf( " ---------------------------- CN Confirm ------------------------" );
		Glog->fprintf( " CONFIRM[%u][%s]", (UInt32)NID,ConfirmMessage.c_str() );
		Glog->fprintf( " Key[%s]",  Key.c_str() );
		Glog->FlushToFile();
		ExecutionReport.SetSrc( ConfirmMessage.c_str());
		if( pTree->get( "PVC", PVC ) )
			ExecutionReport.SetPVC( PVC.c_str() );
		else
			ExecutionReport.SetPVC( "00" );
		GLQHConfirm.FromString( ConfirmMessage );
		if( ParseUserID( Key, AE, Data ) )
			ExecutionReport.SetAllUserData( Data.c_str());
		UFC::AnsiString Ex( ExecutionReport.GetExchangeCode() );
		UFC::AnsiString Sym( ExecutionReport.GetSymbol() );

		if( Ex == "CFFEX" && Sym[ 1 ] == 'O' ) ///< CFFEX IO
			ExecutionReport.SetMarket( nsOrderMessageDefine::mCNOptions );
		else
			ExecutionReport.SetMarket( nsOrderMessageDefine::mCNFutures );
		ExecutionReport.SetAE( AE.c_str() );
        ExecutionReport.SetNID((UInt32)NID);
		if( ParseCNExecution( ExecutionReport, GLQHConfirm ) == TRUE )
			TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() ));
		else
			Glog->fprintf( " Parse CN Confirm Message Falied" );
	}
	else
	{
		Glog->fprintf( " TTaifexConnection::ReceiveCNConfirmMessage Falied" );
		Glog->FlushToFile();
	}
}
//------------------------------------------------------------------------------
BOOL TTaifexConnection::ParseCNExecution( TExecutionReportMessage& ExecutionReport, UFC::NameValueMessage& GLQHConfirm )
{
    UFC::AnsiString  OrdStatus, Value;

    if( GLQHConfirm.Get("os",OrdStatus) == TRUE )
    {
        switch( OrdStatus[0] )
        {
                case '0':ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
                                 ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
                                 break;
                case '1':ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPartiallyFilled );
                                 ExecutionReport.SetExecType( nsOrderMessageDefine::etPartiallyFilled );
                                 break;
                case '2':ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osFilled );
                                 ExecutionReport.SetExecType( nsOrderMessageDefine::etFilled );
                                 break;
                case '4':ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
                                 ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
                                 break;
                case '8':ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
								 ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
								 ExecutionReport.SetStatusCode( "99" );
								 ExecutionReport.SetTMPStstusCode( 99 );
                                 if( GLQHConfirm.Get("rjt",Value) == TRUE && Value[0] == '1' )
                                 {
                                    ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrCancel);
                                    if( GLQHConfirm.Get("rjr",Value) == TRUE )
                                        ExecutionReport.SetStatusCode( Value.c_str() );
                                 }
                                 else
                                     ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrNew);
                                 break;
                case 'a':
                case 'A':ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
                                 ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
                                 break;
                default: Glog->fprintf( " Not handle Order Status[%s]",OrdStatus.c_str() );
                         return FALSE;
        }
    }
    else
    {
            Glog->fprintf( " Message missing Order Status" );
            return FALSE;
    }
	if( GLQHConfirm.Get("eid",Value) == FALSE )
	{
		Glog->fprintf( " Message missing field ExecID" );
		return FALSE;
	}
	else
		ExecutionReport.SetExecID( Value.c_str());
	if( GLQHConfirm.Get("oid",Value) == FALSE )
	{
		if( ExecutionReport.GetOrderStatus( ) == nsOrderMessageDefine::osNew )
		{
			Glog->fprintf( " Message missing field OrderID" );
			return FALSE;
		}
	}
	else
		ExecutionReport.SetOrderID( Value.c_str());
	if( GLQHConfirm.Get("cnid",Value) == TRUE )
		ExecutionReport.SetOrigClOrdID( Value.c_str());
	if( GLQHConfirm.Get("oty",Value) == TRUE )
	{
		if( Value ==  "MKT" )
			ExecutionReport.SetOrderType( nsOrderMessageDefine::otMarket);
		else if( Value ==  "LMT" )
			ExecutionReport.SetOrderType( nsOrderMessageDefine::otLimit);
		else
		{
			Glog->fprintf( " Incorrect OrdType value:%s",Value.c_str() );
			return FALSE;
		}
	}
	if( GLQHConfirm.Get("bs",Value) == TRUE )
	{
		if( Value == "B" )
			ExecutionReport.SetSide( nsOrderMessageDefine::sBuy );
		else if( Value == "S" )
			ExecutionReport.SetSide( nsOrderMessageDefine::sSell );
		else
		{
			Glog->fprintf( " Incorrect side value:%s",Value.c_str() );
			return FALSE;
		}
	}
	if( GLQHConfirm.Get("pe",Value) == TRUE )
	{
		if( Value == "O" )
			ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peOpen );
		else if( Value == "C" )
			ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peClose );
		else if( Value == "D" )
			ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peDayTrade );
		else if( Value == "A" )
			ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peAuto );
		else if( Value == "a" )
			ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peAutoToday );
		else
		{
			Glog->fprintf( " Incorrect PositionEffect value:%s",Value.c_str() );
			return FALSE;
		}
	}
	if( GLQHConfirm.Get("tif",Value) == TRUE )
	{
		if( Value == "F" )
			ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifFOK );
		else if( Value == "I" )
			ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifIOC );
		else
			ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifROD );
	}
	if( GLQHConfirm.Get("acc",Value) == TRUE )
		ExecutionReport.SetAccount( Value.c_str());
	if( GLQHConfirm.Get("exh",Value) == TRUE )
		ExecutionReport.SetExchangeCode( Value.c_str());
	if( GLQHConfirm.Get("sym",Value) == TRUE )
		ExecutionReport.SetSymbol( Value.c_str());
	///< Fields about qty
	if( GLQHConfirm.Get("qty",Value) == TRUE )
		ExecutionReport.SetOrderQty( Value.ToInt());
	if( GLQHConfirm.Get("cqt",Value) == TRUE )
		ExecutionReport.SetCumQty( Value.ToInt());
	if( GLQHConfirm.Get("lqt",Value) == TRUE )
		ExecutionReport.SetLeavesQty(Value.ToInt());
	if( GLQHConfirm.Get("eqt",Value) == TRUE )
		ExecutionReport.SetLastQty(Value.ToInt());
	///< Fields about Price
	if( GLQHConfirm.Get("px",Value) == TRUE )
		ExecutionReport.SetPrice( Value.ToDouble());
	if( GLQHConfirm.Get("apx",Value) == TRUE )
		ExecutionReport.SetAvgPx( Value.ToDouble());
	if( GLQHConfirm.Get("epx",Value) == TRUE )
		ExecutionReport.SetLastPx( Value.ToDouble() );
	///< Other fields
	if( GLQHConfirm.Get("txt",Value) == TRUE )
		ExecutionReport.SetText( Value.c_str());
	if( GLQHConfirm.Get("rct",Value) == TRUE )
		ExecutionReport.SetTransactTime( Value.c_str() );
	return TRUE;
}
//------------------------------------------------------------------------------
