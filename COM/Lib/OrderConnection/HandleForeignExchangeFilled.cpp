//---------------------------------------------------------------------------
#include "TTaifexConnection.h"
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveForeignExecuteMessage( MTree* pTree )
{
    UFC::AnsiString ExecuteMessage, Key, AE, Data, StatusCode, OrdType, Value, ExecID, PVC;
    Int32           NID;

    Glog->fprintf( " ------------------------ Foreign Filled -------------------------" );
    if( ( pTree->get( "FILL_ORDER", ExecuteMessage ) == TRUE ) &&
        ( pTree->get( "NID", NID ) == TRUE ) &&
        ( pTree->get( "PVC", PVC ) == TRUE ) &&
        ( pTree->get( "KEY", Key ) == TRUE ) )
    {
        nsOrderMessageDefine::OrderStatusEnum ExeRptOrderStatus = nsOrderMessageDefine::osNone;
        nsOrderMessageDefine::ExecTypeEnum    ExeRptExecType = nsOrderMessageDefine::etNone;
        UFC::NameValueMessage PATSConfirm( "^\n" );
        TExecutionReportMessage ExecutionReport;

        Glog->fprintf( " FILL[%u][%s]", (UInt32)NID, ExecuteMessage.c_str() );
        Glog->fprintf( " KEY[%s]", Key.c_str() );
        ExecutionReport.SetSrc( ExecuteMessage.c_str() );
        ExecutionReport.SetPVC( PVC.c_str() );
        PATSConfirm.FromString( ExecuteMessage );
        if( ParseUserID( Key, AE, Data ) )
            ExecutionReport.SetAllUserData( Data.c_str());
        ExecutionReport.SetAE( AE.c_str() );
        ExecutionReport.SetNID( NID );
        if ((PATSConfirm.Get("cd", StatusCode) == FALSE) || (StatusCode.Length() <= 0))
        {
            Glog->fprintf( " Message missing field Code" );
            Glog->FlushToFile();
            return;
        }
        else
            ExecutionReport.SetStatusCode( StatusCode.c_str() );

        if( PATSConfirm.Get("oty", OrdType) == TRUE )
        {
            if( OrdType ==  "MKT" )
                ExecutionReport.SetOrderType( nsOrderMessageDefine::otMarket);
            else if( OrdType ==  "STP" )
                ExecutionReport.SetOrderType( nsOrderMessageDefine::otStop);
            else if( OrdType ==  "SWL" )
                ExecutionReport.SetOrderType( nsOrderMessageDefine::otStopLimit);
            else if( OrdType == "MWP" )
                ExecutionReport.SetOrderType( nsOrderMessageDefine::otMarketWithProtection);
            else
                ExecutionReport.SetOrderType( nsOrderMessageDefine::otLimit);
            if (StatusCode == "0000")
            {
                if (OrdType == "CXL")
                {
                    ExeRptOrderStatus = nsOrderMessageDefine::osCanceled;
                    ExeRptExecType = nsOrderMessageDefine::etCanceled;
                }
                else if (OrdType == "UDP")
                {
                    ExeRptOrderStatus = nsOrderMessageDefine::osReplaced;
                    ExeRptExecType = nsOrderMessageDefine::etReplaced;
                }
            }
        }
        else
        {
            Glog->fprintf( " Message missing field OrderType" );
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

        if ((PATSConfirm.Get("oid", Value) == FALSE ) || (Value.Length() <= 0))
        {
            Glog->fprintf( " Message missing field OrderID" );
            Glog->FlushToFile();
            return;
        }
        else
            ExecutionReport.SetOrderID( Value.c_str());
        if( PATSConfirm.Get("poid", Value) == TRUE )
            ExecutionReport.SetClOrdID( Value.c_str());

        if (PATSConfirm.Get("bs", Value) == TRUE )
        {
            if( Value == "B" )
                ExecutionReport.SetSide( nsOrderMessageDefine::sBuy );
            else if( Value == "S" )
                ExecutionReport.SetSide( nsOrderMessageDefine::sSell );
            else
            {
                Glog->fprintf( " Incorrect side value:%s", Value.c_str() );
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

        if ((PATSConfirm.Get("fcm", Value) == FALSE) || (Value.Length() <= 0))
            Glog->fprintf( " Message missing field BrokerID" );
        else
            ExecutionReport.SetBrokerID(Value.c_str());

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

        if ((PATSConfirm.Get("exh", Value) == FALSE) || (Value.Length() <= 0))
            Glog->fprintf( " Message missing field ExchangeCode" );
        else
            ExecutionReport.SetExchangeCode(Value.c_str());

        if ((PATSConfirm.Get("sym", Value) == FALSE) || (Value.Length() <= 0))
            Glog->fprintf( " Message missing field Symbol" );
        else
            ExecutionReport.SetSymbol(Value.c_str());

        if ((PATSConfirm.Get("mmy", Value) == FALSE) || (Value.Length() <= 0))
            Glog->fprintf( " Message missing field Maturity Month Year" );
        else
            ExecutionReport.SetMaturityMonthYear( Value.c_str());

        if( ExeRptOrderStatus != nsOrderMessageDefine::osCanceled )
        {
            if ((PATSConfirm.Get("qty", Value) == FALSE) || (Value.Length() <= 0))
            {
                Glog->fprintf( " Message missing field Quantity" );
                Glog->FlushToFile();
                return;
            }
            else
            {
                ExecutionReport.SetOrderQty(Value.ToInt());
                ExecutionReport.SetLastQty(Value.ToInt());
            }

            if ((PATSConfirm.Get("cqty", Value) == FALSE) || (Value.Length() <= 0))
            {
                Glog->fprintf( " Message missing field CumQuantity" );
                Glog->FlushToFile();
                return;
            }
            else
                ExecutionReport.SetCumQty( Value.ToInt());

            if ((PATSConfirm.Get("lqt", Value) == FALSE) || (Value.Length() <= 0))
            {
                Glog->fprintf( " Message missing field Leaves Quantity" );
                Glog->FlushToFile();
                return;
            }
            else
                ExecutionReport.SetLeavesQty(Value.ToInt());
        }

        if ((PATSConfirm.Get("px", Value) == FALSE ) || (Value.Length() <= 0))
            Glog->fprintf( " Message missing field Price" );
        else
        {
            ExecutionReport.SetPrice( Value.ToDouble());
            ExecutionReport.SetLastPx( Value.ToDouble());
        }

        if ((PATSConfirm.Get("pxb", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetPxBase((nsOrderMessageDefine::PriceBaseEnum)Value.ToInt());
        else
            ExecutionReport.SetPxBase( nsOrderMessageDefine::pbNone );

        if ((PATSConfirm.Get("stkpx", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetStrikePrice(Value.ToDouble());
        else
            ExecutionReport.SetStrikePrice(0);

        if ((PATSConfirm.Get("cp", Value) == TRUE) && (Value.Length() > 0))
        {
            if( Value == "C" )
                ExecutionReport.SetEventType( nsOrderMessageDefine::evtCall );
            else if( Value == "P" )
                ExecutionReport.SetEventType( nsOrderMessageDefine::evtPut );
            else
                ExecutionReport.SetEventType( nsOrderMessageDefine::evtNone );
        }

        if ((PATSConfirm.Get("txt", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetText(Value.c_str());

        if ((PATSConfirm.Get("rct", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetTransactTime(Value.c_str());

        if ((PATSConfirm.Get("rlt", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetMessageTime(Value.c_str());

        if ((ExeRptOrderStatus != nsOrderMessageDefine::osCanceled) &&
            (ExeRptOrderStatus != nsOrderMessageDefine::osReplaced))
        {
            if( FEnableFilledState &&
                ( PATSConfirm.Get("clx", Value) == TRUE ) &&
                ( Value == "Y" ) )
            {
                ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osFilled );
                ExecutionReport.SetExecType( nsOrderMessageDefine::etFilled );
            }
            else
            {
                ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPartiallyFilled );
                ExecutionReport.SetExecType( nsOrderMessageDefine::etPartiallyFilled );
            }
        }
        else
        {
            ExecutionReport.SetOrderStatus(ExeRptOrderStatus);
            ExecutionReport.SetExecType(ExeRptExecType);
        }

        if ((PATSConfirm.Get("eucd", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetEUCD( Value.c_str() );
        if( PATSConfirm.Get("apx",Value) == TRUE )
            ExecutionReport.SetAvgPx( Value.ToDouble());
        if( PATSConfirm.Get("eid", ExecID) == FALSE )
        {
            UFC::UDateTime Now;
            ExecID.Printf("F_%d_%010d%02d%03d", (int)ExecutionReport.GetMarket(), NID, Now.getSecond(), Now.getMillisecond());
        }
        ExecutionReport.SetExecID( ExecID.c_str() );

        if ((PATSConfirm.Get("erid", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetExecRefID( Value.c_str() );

        if ((PATSConfirm.Get("oeid", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetOrigExecID( Value.c_str() );

        if ((PATSConfirm.Get("och", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetOrderChannel( Value.c_str() );

        if ((PATSConfirm.Get("ett", Value) == TRUE) && (Value.Length() > 0))
            ExecutionReport.SetExecTransType(static_cast<nsOrderMessageDefine::ExecTransTypeEnum>(Value.ToInt()));

        if ((PATSConfirm.Get("et", Value) == TRUE) && (Value.Length() > 0))
        {
            nsOrderMessageDefine::ExecTypeEnum execType = static_cast<nsOrderMessageDefine::ExecTypeEnum>(Value.ToInt());
            if (execType != nsOrderMessageDefine::etNone)
                ExecutionReport.SetExecType(execType);
        }

        if ((PATSConfirm.Get("os", Value) == TRUE) && (Value.Length() > 0))
        {
            nsOrderMessageDefine::OrderStatusEnum orderStatus = static_cast<nsOrderMessageDefine::OrderStatusEnum>(Value.ToInt());
            if (orderStatus != nsOrderMessageDefine::osNone)
                ExecutionReport.SetOrderStatus(orderStatus);
        }
        long RptSeq;
        
        if( ( PATSConfirm.Get( "rseq", Value ) == TRUE ) && ( (RptSeq = Value.ToInt()) > 0 ) )
            ExecutionReport.SetReportSequence( RptSeq );        
        Value = "";
        ExecDup rptExecDup = (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecID );
        if (PATSConfirm.Get("pdup", Value) == TRUE)
        {
            if (Value == "Y")
                rptExecDup = edPossibleDuplicate;
            else if (Value == "N")
                rptExecDup = edNewExecution;
        }
        if( isProxyAccountExist ) TrigerOnExecutionReport( &ExecutionReport, rptExecDup );
    }
    Glog->FlushToFile();
}
//------------------------------------------------------------------------------
void TTaifexConnection::ReceiveCNExecuteMessage( MTree* pTree  )
{
	UFC::AnsiString ExecuteMessage, Key, AE, Data, PVC;
	Int32           NID;
	bool GetNIDSuccess         = pTree->get( "NID", NID );
	bool GetKeySuccess         = pTree->get( "KEY", Key );
	bool GetFillMessageSuccess = pTree->get( "FILL_ORDER", ExecuteMessage );

	if ((GetNIDSuccess && (NID != 0)) && GetFillMessageSuccess && GetKeySuccess)
	{
		UFC::NameValueMessage   GLQHConfirm( "^\n" );
		TExecutionReportMessage ExecutionReport;

		if( pTree->get( "PVC", PVC ) )
			ExecutionReport.SetPVC( PVC.c_str() );
		else
			ExecutionReport.SetPVC( "00" );
		Glog->fprintf( " --------------------------- CN Filled --------------------------" );
		Glog->fprintf( " FILL[%u][%s]", (UInt32)NID, ExecuteMessage.c_str() );
		Glog->fprintf( " KEY[%s] PVC[%s]",  Key.c_str(), PVC.c_str() );
		Glog->FlushToFile();
		ExecutionReport.SetSrc( ExecuteMessage.c_str());
		GLQHConfirm.FromString( ExecuteMessage );
		if( ParseUserID( Key, AE, Data ) )
			ExecutionReport.SetAllUserData( Data.c_str());
		UFC::AnsiString Ex( ExecutionReport.GetExchangeCode() );
		UFC::AnsiString Sym( ExecutionReport.GetSymbol() );

		if( Ex == "CFFEX" && Sym[ 1 ] == 'O' )
			ExecutionReport.SetMarket( nsOrderMessageDefine::mCNOptions );
		else
			ExecutionReport.SetMarket( nsOrderMessageDefine::mCNFutures );
		ExecutionReport.SetAE( AE.c_str() );
		ExecutionReport.SetNID(NID);
		if( ParseCNExecution( ExecutionReport, GLQHConfirm ) == TRUE )
			TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() ));
	}
	else
	{
		Glog->fprintf( " TTaifexConnection::ReceiveCNExecuteMessage Falied" );
		Glog->FlushToFile();
		return;
	}
}
//---------------------------------------------------------------------------
