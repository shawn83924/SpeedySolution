//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "HSufxObjStockOptListener.h"
#include "TTaifexConnection.h"
#include "APIParams.h"
//---------------------------------------------------------------------------
#include <WideStrUtils.hpp>
//---------------------------------------------------------------------------
//typedef AnsiStringT<936> GB2312String;
//---------------------------------------------------------------------------
//  ExecID Rule
//
// HSufx
//  Pending      Key    : FrontID (0) + SessionID (LoginTimes) + OrderRef (hSend)   <"%d|%d|%d">
//  PendingNew   ExecID : "P" + OrderLocalID (entrust_no)                           <"P_%d">
//  NewOrder     ExecID : "N" + OrderLocalID (entrust_no)                           <"N_%d">
//  Fill         ExecID : "F" + OrderLocalID (entrust_no) + FillSeq (business_id)   <"F_%d_%s">
//  Canceled     ExecID : "C" + OrderLocalID (entrust_no)                           <"C_%d">
//  Reject ByHS  ExecID : "R" + SessionID (LoginTimes) + OrderRef (hSend)           <"R_%d_%d">
//  Reject ByExh ExecID : "R" + OrderLocalID (entrust_no)                           <"R_%d">
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//  Logon Procedure
//
//  <1> TTaifexConnection::Connect()
//      ------> HSufx Connect() (Sync.)
//                                        <------ HSufx OnConnect()
//                                        <------ HSufx OnSafeConnect()
//      ------> HSufx Connect() return.
//    **1**Trigger IOrderConnectionEventListener::OnConnected()
//
//  <2> TTaifexConnection::Logon() TTaifexConnection::SpeedyProxyLogon()
//      <2.1>
//      ------> HSufx SendLogin()
//                                        <------ HSufx OnRegister()
//                                        <------ HSufx OnReceivedBizMsg(331100)
//      <------ WndMsgRcvLogin()
//      <2.2>
//      ------> HSufx SendSubscribeConfirm()
//      ------> HSufx SendSubscribeFill()
//                                        <------ HSufx OnReceivedBizMsg(620001) (Confirm)
//      <------ WndMsgRcvSubConfirm()
//                                        <------ HSufx OnReceivedBizMsg(620001) (Fill)
//      <------ WndMsgRcvSubFill()
//
//  <3> TTaifexConnection::Recover(.) Recover(..)
//      ------> HSufx SendQryConfirm()
//                                        <------ HSufx OnReceivedBizMsg(338020) (Confirm)
//      <------ WndMsgRcvQryConfirm()
//      ------> HSufx SendQryFill()
//                                        <------ HSufx OnReceivedBizMsg(338020) (Fill)
//      <------ WndMsgRcvQryFill()
//    **2**Trigger IOrderConnectionEventListener::OnLogonReply()
//    **3**Trigger IOrderConnectionEventListener::OnRecoverFinished()
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void HSufxStrcpyNoLineFeed( char* dest, const char* src, int dest_len )
{
	register int i;
	register const char* PtrSrc  = src;
	register char* PtrDest = dest;

	for ( i = 0; i < dest_len; i++ )
	{
		if ( *PtrSrc == 0 )
			break;
		if ( *PtrSrc != 0x0A && *PtrSrc != 0x0D )
			*(PtrDest++) = *(PtrSrc++);
		else
			PtrSrc++;
	}
	*PtrDest = 0;
}
//---------------------------------------------------------------------------
// class HSufxObjStockOpt
//
// Receiving data process implementation ( <--- )
//---------------------------------------------------------------------------
BOOL HSufxObjStockOpt::InitMessageWindow( void )
{
	GUID        Guid;
	WNDCLASSEXW wcx;
	if ( CoCreateGuid( &Guid ) == S_OK )
	{
		swprintf( MessageWindowsClassName, L"%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X"
										, Guid.Data1, Guid.Data2, Guid.Data3
										, Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], Guid.Data4[3]
										, Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7] );
	}
	else
		swprintf( MessageWindowsClassName, L"HSUFX_StockOpt_MsgWin_%10d", UFC::GetTickCountMS() );
	// Fill in the window class structure with parameters that describe the main window.
	wcx.cbSize = sizeof( wcx );                 // size of structure
	wcx.style = CS_HREDRAW |CS_VREDRAW;         // redraw if size changes
	wcx.lpfnWndProc   = HSufxObjStockOpt::MessageWndProc;// points to window procedure
	wcx.cbClsExtra    = 0;                      // no extra class memory
	wcx.cbWndExtra    = 0;                      // no extra window memory
	wcx.hInstance     = FInstance;              // handle to instance
	wcx.hIcon         = NULL;                   // predefined app. icon
	wcx.hCursor       = NULL;                   // predefined arrow
	wcx.hbrBackground = NULL;                   // background brush
	wcx.lpszMenuName  = NULL;                   // name of menu resource
	wcx.lpszClassName = MessageWindowsClassName;// name of window class
	wcx.hIconSm = NULL;
	if ( RegisterClassExW( &wcx ) == 0 )			// Register the window class.
		return FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------
BOOL HSufxObjStockOpt::CreateMessageWindow( void )
{
	FHwnd = CreateWindowW( MessageWindowsClassName,	// name of window class
						   L"HSUFX_StockOpt_App",		// title-bar string
						   WS_OVERLAPPEDWINDOW ,	// top-level window
						   10,						// horizontal position
						   10,						// vertical position
						   100,						// width
						   100,						// height
						   HWND_MESSAGE,			// Message only window
						   NULL,					// no class menu
						   NULL,					// handle to application instance
						   NULL );					// no window-creation data
	if ( !FHwnd )
		return FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK HSufxObjStockOpt::MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	HSufxObjStockOpt* HSufxObjPtr = (HSufxObjStockOpt*)wParam;
	switch ( Message )
	{
	case WM_HSUFX_EVENT+1  : HSufxObjPtr->HSufxOnConnect( );
							 return 0;
	case WM_HSUFX_EVENT+2  : HSufxObjPtr->HSufxOnClose( );
							 return 0;
	case WM_HSUFX_EVENT+3  : HSufxObjPtr->HSufxLoginResult( (HSufxStockOpt::dataLoginResult*)lParam );
							 delete (HSufxStockOpt::dataLoginResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+4  : HSufxObjPtr->HSufxLogoutResult( (HSufxStockOpt::dataLogoutResult*)lParam );
							 delete (HSufxStockOpt::dataLogoutResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+5  : HSufxObjPtr->HSufxReplyHeartBeatMsg( (HSufxStockOpt::dataReplyHeartBeatMsg*)lParam );
							 delete [] ((HSufxStockOpt::dataReplyHeartBeatMsg*)lParam)->Buffer;
							 delete (HSufxStockOpt::dataReplyHeartBeatMsg*)lParam;
							 return 0;
							 //HSufxReplyHeartBeat( (HSufxStockOpt::dataReplyHeartBeat*)lParam );
							 //delete (HSufxStockOpt::dataReplyHeartBeat*)lParam;
							 //return 0;
	case WM_HSUFX_EVENT+6  : HSufxObjPtr->HSufxChangePwdResult( (HSufxStockOpt::dataChangePwdResult*)lParam );
							 delete (HSufxStockOpt::dataChangePwdResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+7  : HSufxObjPtr->HSufxSubscribeResult( (HSufxStockOpt::dataSubscribeResult*)lParam );
							 delete (HSufxStockOpt::dataSubscribeResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+11 : HSufxObjPtr->HSufxPendingExecution( (HSufxStockOpt::dataPendingExecution*)lParam );
							 delete (HSufxStockOpt::dataPendingExecution*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+12 : HSufxObjPtr->HSufxConfirmExecution( (HSufxStockOpt::dataConfirmExecution*)lParam );
							 delete (HSufxStockOpt::dataConfirmExecution*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+13 : HSufxObjPtr->HSufxRejectExecution( (HSufxStockOpt::dataRejectExecution*)lParam );
							 delete (HSufxStockOpt::dataRejectExecution*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+14 : HSufxObjPtr->HSufxRejectByExh( (HSufxStockOpt::dataRejectByExh*)lParam );
							 delete (HSufxStockOpt::dataRejectByExh*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+15 : HSufxObjPtr->HSufxCanceledExecution( (HSufxStockOpt::dataCanceledExecution*)lParam );
							 delete (HSufxStockOpt::dataCanceledExecution*)lParam;
							 break;
	case WM_HSUFX_EVENT+16 : HSufxObjPtr->HSufxFillExecution( (HSufxStockOpt::dataFillExecution*)lParam );
							 delete (HSufxStockOpt::dataFillExecution*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+21 : HSufxObjPtr->HSufxQryOrderResult( (HSufxStockOpt::dataQryOrderResult*)lParam );
							 delete (HSufxStockOpt::dataQryOrderResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+22 : HSufxObjPtr->HSufxQryFillResult( (HSufxStockOpt::dataQryFillResult*)lParam );
							 delete (HSufxStockOpt::dataQryFillResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+23 : HSufxObjPtr->HSufxQryAssetResult( (HSufxStockOpt::dataQryAssetResult*)lParam );
							 delete (HSufxStockOpt::dataQryAssetResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+24 : HSufxObjPtr->HSufxQryPositionResult( (HSufxStockOpt::dataQryPositionResult*)lParam );
							 delete (HSufxStockOpt::dataQryPositionResult*)lParam;
							 return 0;
	default : return DefWindowProc( hwnd, Message, wParam, lParam );
	}

	return 0;
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassOnConnect( void )
{
	PostMessage( FHwnd, WM_HSUFX_EVENT+1, (WPARAM)this, (LPARAM)NULL );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassOnClose( void )
{
	PostMessage( FHwnd, WM_HSUFX_EVENT+2, (WPARAM)this, (LPARAM)NULL );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassLoginResult( bool IsLoginOK, const char* ErrMsg,
										int branch_no, const char* fund_account,
										const char* user_token, const char* client_id,
										int sysnode_id, int login_times, int init_date )
{
	// 1. Initial Data
	HSufxStockOpt::dataLoginResult* Data = new HSufxStockOpt::dataLoginResult();
	Data->IsLoginOK   = IsLoginOK;
	strcpy( Data->ErrMsg, ErrMsg );
	Data->branch_no   = branch_no;
	strcpy( Data->fund_account, fund_account );
	strcpy( Data->user_token, user_token );
	strcpy( Data->client_id, client_id );
	Data->sysnode_id  = sysnode_id;
	Data->login_times = login_times;
	Data->init_date   = init_date;
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+3, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassLoginResult: IsLoginOK[%c] branch_no[%d] fund_account[%s] user_token[%s] client_id[%s]"
							  , IsLoginOK ? 'Y' : 'N', branch_no, fund_account, user_token, client_id );
	UFC::BufferedLog::Printf( " sysnode_id[%d] login_times[%d] init_date[%d] ErrMsg[%s]", sysnode_id, login_times, init_date, ErrMsg );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassLogoutResult( bool IsLogoutOK )
{
	// 1. Initial Data
	HSufxStockOpt::dataLogoutResult* Data = new HSufxStockOpt::dataLogoutResult();
	Data->IsLogoutOK = IsLogoutOK;
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+4, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassReplyHeartBeat( LPRET_DATA lpRetData )
{
	// 1. Initial Data
	HSufxStockOpt::dataReplyHeartBeat* Data = new HSufxStockOpt::dataReplyHeartBeat();
	Data->ReqData.issueType  = 0;	///<此處需要確認，心跳應答是否區分訂閱類型
	Data->ReqData.packetType = 1;	///<轉換為應答包
	memcpy( &Data->ReqData.routeInfo, &lpRetData->sendInfo, sizeof(Route_Info) );
	Data->ReqData.lpKeyInfo = new char[lpRetData->keyInfoLen+1];
	memcpy( Data->ReqData.lpKeyInfo, lpRetData->lpKeyInfo, lpRetData->keyInfoLen );
	((char*)Data->ReqData.lpKeyInfo)[lpRetData->keyInfoLen] = 0;
	// 2. Post message
	//PostMessage( FHwnd, WM_HSUFX_EVENT+5, (WPARAM)this, (LPARAM)Data );

	// ????????-------------
	int RtnVal = FHSufxConnection->SendBizEx( 620000, NULL, NULL, HSufx_ASyncSend, FHSufxSysNodeID, 0, FHSufxBranchNo, &Data->ReqData );
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxReplyHeartBeat SendBizEx(620000) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxReplyHeartBeat SendBizEx(620000) to HS UFX Server OK." );
	delete Data;
	// ????????-------------
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassReplyHeartBeat( IBizMessage* lpMsg )
{
	//如果要把消息放到其他線程處理，必須自行拷貝，操作如下：
	//int iMsgLen = 0;
	//void * lpMsgBuffer = lpBizMessageRecv->GetBuff(iMsgLen);
	//將lpMsgBuffer拷貝走，然後在其他線程中恢復成消息可進行如下操作：
	//lpBizMessageRecv->SetBuff(lpMsgBuffer,iMsgLen);

	// 1. Initial Data;
	lpMsg->ChangeReq2AnsMessage();
	HSufxStockOpt::dataReplyHeartBeatMsg* Data = new HSufxStockOpt::dataReplyHeartBeatMsg();
	void* lpMsgBuffer = lpMsg->GetBuff( Data->BufferLen );
	Data->Buffer = new char[ Data->BufferLen ];
	memcpy( Data->Buffer, lpMsgBuffer, Data->BufferLen );
	PostMessage( FHwnd, WM_HSUFX_EVENT+5, (WPARAM)this, (LPARAM)Data );

	// ????????-------------
	/*
	int RtnVal = FHSufxConnection->SendBizMsg( lpMsg, HSufx_ASyncSend );
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxReplyHeartBeat SendBizMsg(620000) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxReplyHeartBeat SendBizMsg(620000) to HS UFX Server OK." );
	*/
	// ????????-------------
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassChangePwdResult( bool IsChangePwdOK, const char* ErrMsg )
{
	// 1. Initial Data
	HSufxStockOpt::dataChangePwdResult* Data = new HSufxStockOpt::dataChangePwdResult();
	Data->IsChangePwdOK = IsChangePwdOK;
	strcpy( Data->ErrMsg, ErrMsg );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+6, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassChangePwdResult: IsChangePwdOK[%c] ErrMsg[%s]", IsChangePwdOK ? 'Y' : 'N', ErrMsg );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassSubscribeResult( bool IsSubscribeOK, int IssueType, const char* ErrMsg )
{
	// 1. Initial Data
	HSufxStockOpt::dataSubscribeResult* Data = new HSufxStockOpt::dataSubscribeResult();
	Data->IsSubscribeOK = IsSubscribeOK;
	Data->IssueType     = IssueType;
	strcpy( Data->ErrMsg, ErrMsg );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+7, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassSubscribeResult: IsSubscribeOK[%c] issueType[%d] ErrMsg[%s]"
							  , IsSubscribeOK ? 'Y' : 'N', IssueType, ErrMsg );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassQryOrderResult( IF2UnPacker* Packet )
{
	int RowCount = Packet->GetRowCount();

	for ( int i = 0; i < RowCount; i++ )
	{
		// 1. Get data from Packet
		HSufxStockOpt::dataQryOrderResult* Data = new HSufxStockOpt::dataQryOrderResult();
		Data->IsEndNotify = false;
		Data->init_date             = Packet->GetInt( "init_date" );
		Data->batch_no              = Packet->GetInt( "batch_no" );
		Data->entrust_no            = Packet->GetInt( "entrust_no" );
		strcpy( Data->exchange_type,  Packet->GetStr( "exchange_type" ) );
		strcpy( Data->fund_account,   Packet->GetStr( "fund_account" ) );
		strcpy( Data->option_account, Packet->GetStr( "option_account" ) );
		strcpy( Data->option_code,    Packet->GetStr( "option_code" ) );
		strcpy( Data->optcontract_id, Packet->GetStr( "optcontract_id" ) );
		strcpy( Data->stock_code,     Packet->GetStr( "stock_code" ) );
		Data->entrust_bs            = Packet->GetChar( "entrust_bs" );
		Data->entrust_oc            = Packet->GetChar( "entrust_oc" );
		Data->covered_flag          = Packet->GetChar( "covered_flag" );
		Data->opt_entrust_price     = Packet->GetDouble( "opt_entrust_price" );
		Data->entrust_amount        = Packet->GetDouble( "entrust_amount" );
		Data->business_amount       = Packet->GetDouble( "business_amount" );
		Data->opt_business_price    = Packet->GetDouble( "opt_business_price" );
		Data->report_no             = Packet->GetInt( "report_no" );
		Data->report_time           = Packet->GetInt( "report_time" );
		Data->entrust_type          = Packet->GetChar( "entrust_type" );
		Data->entrust_status        = Packet->GetChar( "entrust_status" );
		Data->entrust_time          = Packet->GetInt( "entrust_time" );
		Data->entrust_date          = Packet->GetInt( "entrust_date" );
		strcpy( Data->entrust_prop,   Packet->GetStr( "entrust_prop" ) );
		Data->entrust_src           = Packet->GetChar( "entrust_src" );
		strcpy( Data->trade_name,     Packet->GetStr( "trade_name" ) );
		strcpy( Data->option_name,    Packet->GetStr( "option_name" ) );
		HSufxStrcpyNoLineFeed( Data->cancel_info, Packet->GetStr( "cancel_info" ), 256 );
		Data->withdraw_amount       = Packet->GetDouble( "withdraw_amount" );
		Data->withdraw_flag         = Packet->GetChar( "withdraw_flag" );
		strcpy( Data->position_str,   Packet->GetStr( "position_str" ) );
		// 2. Packet to next row
		Packet->Next();
		// 3. Post message
		PostMessage( FHwnd, WM_HSUFX_EVENT+21, (WPARAM)this, (LPARAM)Data );
	}
	// 4. Post message (End notify message)
	HSufxStockOpt::dataQryOrderResult* DataEnd = new HSufxStockOpt::dataQryOrderResult();
	DataEnd->IsEndNotify = true;
	PostMessage( FHwnd, WM_HSUFX_EVENT+21, (WPARAM)this, (LPARAM)DataEnd );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassQryFillResult( IF2UnPacker* Packet )
{
	int RowCount = Packet->GetRowCount();

	for ( int i = 0; i < RowCount; i++ )
	{
		// 1. Get data from Packet
		HSufxStockOpt::dataQryFillResult* Data = new HSufxStockOpt::dataQryFillResult();
		Data->IsEndNotify = false;
		Data->init_date             = Packet->GetInt( "init_date" );
		Data->serial_no             = Packet->GetInt( "serial_no" );
		strcpy( Data->exchange_type,  Packet->GetStr( "exchange_type" ) );
		//strcpy( Data->fund_account,   Packet->GetStr( "fund_account" ) );
		strcpy( Data->option_account, Packet->GetStr( "option_account" ) );
		strcpy( Data->option_code,    Packet->GetStr( "option_code" ) );
		strcpy( Data->optcontract_id, Packet->GetStr( "optcontract_id" ) );
		strcpy( Data->stock_code,     Packet->GetStr( "stock_code" ) );
		Data->entrust_bs            = Packet->GetChar( "entrust_bs" );
		Data->entrust_oc            = Packet->GetChar( "entrust_oc" );
		Data->covered_flag          = Packet->GetChar( "covered_flag" );
		Data->opt_business_price    = Packet->GetDouble( "opt_business_price" );
		Data->business_amount       = Packet->GetDouble( "business_amount" );
		Data->business_time         = Packet->GetInt( "business_time" );
		Data->real_type             = Packet->GetChar( "real_type" );
		Data->real_status           = Packet->GetChar( "real_status" );
		Data->business_times        = Packet->GetInt( "business_times" );
		Data->entrust_no            = Packet->GetInt( "entrust_no" );
		Data->business_balance      = Packet->GetDouble( "business_balance" );
		strcpy( Data->option_name,    Packet->GetStr( "option_name" ) );
		strcpy( Data->trade_name,     Packet->GetStr( "trade_name" ) );
		Data->report_no             = Packet->GetInt( "report_no" );
		strcpy( Data->entrust_prop,   Packet->GetStr( "entrust_prop" ) );
		strcpy( Data->business_id,    Packet->GetStr( "business_id" ) );
		strcpy( Data->position_str,   Packet->GetStr( "position_str" ) );
		// 2. Packet to next row
		Packet->Next();
		// 3. Post message
		PostMessage( FHwnd, WM_HSUFX_EVENT+22, (WPARAM)this, (LPARAM)Data );
	}
	// 4. Post message (End notify message)
	HSufxStockOpt::dataQryFillResult* DataEnd = new HSufxStockOpt::dataQryFillResult();
	DataEnd->IsEndNotify = true;
	PostMessage( FHwnd, WM_HSUFX_EVENT+22, (WPARAM)this, (LPARAM)DataEnd );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassQryAssetResult( IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	HSufxStockOpt::dataQryAssetResult* Data = new HSufxStockOpt::dataQryAssetResult();
	Data->total_asset         = Packet->GetDouble( "total_asset" );
	Data->fund_asset          = Packet->GetDouble( "fund_assetv" );
	Data->current_balance     = Packet->GetDouble( "current_balance" );
	Data->enable_balance      = Packet->GetDouble( "enable_balance" );
	Data->enable_bail_balance = Packet->GetDouble( "enable_bail_balance" );
	Data->used_bail_balance   = Packet->GetDouble( "used_bail_balance" );
	Data->used_pur_balance    = Packet->GetDouble( "used_pur_balance" );
	Data->enable_pur_balance  = Packet->GetDouble( "enable_pur_balance" );
	Data->pur_quota           = Packet->GetDouble( "pur_quota" );
	Data->income_balance      = Packet->GetDouble( "income_balance" );
	//Data->frozen_balance      = Packet->GetDouble( "frozen_balance" ); ///< 文件沒有
	Data->risk_degree         = Packet->GetDouble( "risk_degree" );
	Data->real_risk_degree    = Packet->GetDouble( "real_risk_degree" );
	Data->dyna_market_value   = Packet->GetDouble( "dyna_market_value" );
	Data->real_used_bail      = Packet->GetDouble( "real_used_bail" );
	Data->optrisk_type        = Packet->GetChar( "optrisk_type" );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+23, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassQryPositionResult( IF2UnPacker* Packet )
{
	int RowCount = Packet->GetRowCount();

	for ( int i = 0; i < RowCount; i++ )
	{
		// 1. Get data from Packet
		HSufxStockOpt::dataQryPositionResult* Data = new HSufxStockOpt::dataQryPositionResult();
		Data->IsEndNotify = false;
		strcpy( Data->fund_account,   Packet->GetStr( "fund_account" ) );
		strcpy( Data->exchange_type,  Packet->GetStr( "exchange_type" ) );
		strcpy( Data->option_account, Packet->GetStr( "option_account" ) );
		//Data->opthold_type          = Packet->GetChar( "opthold_type " );  ///< GetChar( "opthold_type " ) 抓不到 改用 GetCharByIndex( 4 )
		Data->opthold_type          = Packet->GetCharByIndex( 4 );
		strcpy( Data->option_code,    Packet->GetStr( "option_code" ) );
		Data->stock_code            = Packet->GetChar( "stock_code" );
		strcpy( Data->optcontract_id, Packet->GetStr( "optcontract_id" ) );
		strcpy( Data->option_name,    Packet->GetStr( "option_name" ) );
		Data->option_type           = Packet->GetChar( "option_type" );
		Data->current_amount        = Packet->GetDouble( "current_amount" );
		Data->hold_amount           = Packet->GetDouble( "hold_amount" );
		Data->enable_amount         = Packet->GetDouble( "enable_amount" );
		Data->real_open_amount      = Packet->GetDouble( "real_open_amount" );
		Data->real_drop_amount      = Packet->GetDouble( "real_drop_amount" );
		Data->entrust_drop_amount   = Packet->GetDouble( "entrust_drop_amount" );
		Data->last_price            = Packet->GetDouble( "last_price" );
		Data->opt_last_price        = Packet->GetDouble( "opt_last_price" );
		Data->opt_cost_price        = Packet->GetDouble( "opt_cost_price" );
		Data->exercise_price        = Packet->GetDouble( "exercise_price" );
		Data->market_value          = Packet->GetDouble( "market_value" );
		Data->av_buy_optprice       = Packet->GetDouble( "av_buy_optprice" );
		Data->av_income_balance     = Packet->GetDouble( "av_income_balance" );
		Data->cost_balance          = Packet->GetDouble( "cost_balance" );
		Data->income_balance        = Packet->GetDouble( "income_balance" );
		Data->exercise_income       = Packet->GetDouble( "exercise_income" );
		Data->duty_used_bail        = Packet->GetDouble( "duty_used_bail" );
		Data->exercise_date         = Packet->GetInt( "exercise_date" );
		Data->amount_per_hand       = Packet->GetInt( "amount_per_hand" );
		strcpy( Data->position_str,   Packet->GetStr( "position_str" ) );
		// 2. Packet to next row
		Packet->Next();
		// 3. Post message
		PostMessage( FHwnd, WM_HSUFX_EVENT+24, (WPARAM)this, (LPARAM)Data );
	}
	// 4. Post message (End notify message)
	HSufxStockOpt::dataQryPositionResult* DataEnd = new HSufxStockOpt::dataQryPositionResult();
	DataEnd->IsEndNotify = true;
	PostMessage( FHwnd, WM_HSUFX_EVENT+24, (WPARAM)this, (LPARAM)DataEnd );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassPendingExecution( int hSend, IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	HSufxStockOpt::dataPendingExecution* Data = new HSufxStockOpt::dataPendingExecution();
	Data->hSend = hSend;
	Data->entrust_no = Packet->GetInt( "entrust_no" );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+11, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassPendingExecution: hSend[%d] entrust_no[%d]", hSend, Data->entrust_no );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassConfirmExecution( IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	HSufxStockOpt::dataConfirmExecution* Data = new HSufxStockOpt::dataConfirmExecution();
	Data->branch_no                = Packet->GetInt( "branch_no" );
	strcpy( Data->fund_account,      Packet->GetStr( "fund_account" ) );
	Data->entrust_no               = Packet->GetInt( "entrust_no" );
	Data->entrust_status           = Packet->GetChar( "entrust_status" );
	Data->entrust_type             = Packet->GetChar( "entrust_type" );
	int         report_no          = Packet->GetInt( "report_no" );
	const char* option_code        = Packet->GetStr( "option_code" );
	char        entrust_bs         = Packet->GetChar( "entrust_bs" );
	char        entrust_oc         = Packet->GetChar( "entrust_oc" );
	char        covered_flag       = Packet->GetChar( "covered_flag" );
	double      business_amount    = Packet->GetDouble( "business_amount" );
	double      opt_business_price = Packet->GetDouble( "opt_business_price" );
	double      entrust_amount     = Packet->GetDouble( "entrust_amount" );
	int         batch_no           = Packet->GetInt( "batch_no" );
	double      opt_entrust_price  = Packet->GetDouble( "opt_entrust_price" );
	int         error_no           = Packet->GetInt( "error_no" );
	const char* error_info         = Packet->GetStr( "error_info" );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+12, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassConfirmExecution: branch_no[%d] fund_account[%s] entrust_no[%d] entrust_status[%c] entrust_type[%c] report_no[%d]"
							  , Data->branch_no, Data->fund_account, Data->entrust_no, Data->entrust_status, Data->entrust_type, report_no );
	UFC::BufferedLog::Printf( " option_code[%s] entrust_bs[%c] entrust_oc[%c] covered_flag[%c] business_amount[%.2f] opt_business_price[%.4f] entrust_amount[%.2f]"
							  , option_code, entrust_bs, entrust_oc, covered_flag, business_amount, opt_business_price, entrust_amount );
	UFC::BufferedLog::Printf( " batch_no[%d] opt_entrust_price[%.4f] error_no[%d] error_info[%s]"
							  , batch_no, opt_entrust_price, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassRejectExecution( int hSend, const char* ErrMsg, nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo )
{
	// 1. Get data from Packet
	HSufxStockOpt::dataRejectExecution* Data = new HSufxStockOpt::dataRejectExecution();
	Data->ResponseTo = ResponseTo;
	Data->hSend      = hSend;
	HSufxStrcpyNoLineFeed( Data->ErrMsg, ErrMsg, 500 );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+13, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassRejectByExh( IF2UnPacker* Packet, nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo )
{
	// 1. Get data from Packet
	HSufxStockOpt::dataRejectByExh* Data = new HSufxStockOpt::dataRejectByExh();
	Data->ResponseTo            = ResponseTo;
	Data->branch_no             = Packet->GetInt( "branch_no" );
	strcpy( Data->fund_account,   Packet->GetStr( "fund_account" ) );
	Data->entrust_no            = Packet->GetInt( "entrust_no" );
	Data->entrust_status        = Packet->GetChar( "entrust_status" );
	char        real_type       = Packet->GetChar( "real_type" );
	char        real_status     = Packet->GetChar( "real_status" );
	strcpy( Data->extern_code,    Packet->GetStr( "extern_code" ) );
	const char* report_seat     = Packet->GetStr( "report_seat" );
	int         report_no       = Packet->GetInt( "report_no" );
	const char* option_code     = Packet->GetStr( "option_code" );
	char        option_type     = Packet->GetChar( "option_type" );
	double      business_amount = Packet->GetDouble( "business_amount" );
	Data->business_time         = Packet->GetInt( "business_time" );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+14, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassRejectByExh: branch_no[%d] fund_account[%s] entrust_no[%d] entrust_status[%c] real_type[%c] real_status[%c] extern_code[%s]"
							  , Data->branch_no, Data->fund_account, Data->entrust_no, Data->entrust_status, real_type, real_status, Data->extern_code );
	UFC::BufferedLog::Printf( " report_seat[%s] report_no[%d] option_code[%s] option_type[%c] business_amount[%.2f] business_time[%d]"
							  , report_seat, report_no, option_code, option_type, business_amount, Data->business_time );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassCanceledExecution( IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	HSufxStockOpt::dataCanceledExecution* Data = new HSufxStockOpt::dataCanceledExecution();
	Data->branch_no             = Packet->GetInt( "branch_no" );
	strcpy( Data->fund_account,   Packet->GetStr( "fund_account" ) );
	Data->entrust_no            = Packet->GetInt( "entrust_no" );
	Data->entrust_status        = Packet->GetChar( "entrust_status" );
	char        real_type       = Packet->GetChar( "real_type" );
	char        real_status     = Packet->GetChar( "real_status" );
	const char* extern_code     = Packet->GetStr( "extern_code" );
	const char* report_seat     = Packet->GetStr( "report_seat" );
	int         report_no       = Packet->GetInt( "report_no" );
	const char* option_code     = Packet->GetStr( "option_code" );
	char        option_type     = Packet->GetChar( "option_type" );
	double      business_amount = Packet->GetDouble( "business_amount" );
	Data->business_time         = Packet->GetInt( "business_time" );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+15, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassCanceledExecution: branch_no[%d] fund_account[%s] entrust_no[%d] entrust_status[%c] real_type[%c] real_status[%c] extern_code[%s]"
							  , Data->branch_no, Data->fund_account, Data->entrust_no, Data->entrust_status, real_type, real_status, extern_code );
	UFC::BufferedLog::Printf( " report_seat[%s] report_no[%d] option_code[%s] option_type[%c] business_amount[%.2f] business_time[%d]"
							  , report_seat, report_no, option_code, option_type, business_amount, Data->business_time );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::PassFillExecution( IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	HSufxStockOpt::dataFillExecution* Data = new HSufxStockOpt::dataFillExecution();
	Data->branch_no            = Packet->GetInt( "branch_no" );
	strcpy( Data->fund_account,  Packet->GetStr( "fund_account" ) );
	Data->entrust_no           = Packet->GetInt( "entrust_no" );
	Data->entrust_status       = Packet->GetChar( "entrust_status" );
	char        real_type      = Packet->GetChar( "real_type" );
	char        real_status    = Packet->GetChar( "real_status" );
	strcpy( Data->business_id,   Packet->GetStr( "business_id" ) );
	Data->business_amount      = Packet->GetDouble( "business_amount" );
	Data->opt_business_price   = Packet->GetDouble( "opt_business_price" );
	Data->business_time        = Packet->GetInt( "business_time" );
	const char* client_id      = Packet->GetStr( "client_id" );
	const char* report_account = Packet->GetStr( "report_account" );
	const char* option_code    = Packet->GetStr( "option_code" );
	char        option_type    = Packet->GetChar( "option_type" );
	Data->entrust_bs           = Packet->GetChar( "entrust_bs" );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+16, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassFillExecution: branch_no[%d] fund_account[%s] entrust_no[%d] entrust_status[%c] real_type[%c] real_status[%c]"
							  , Data->branch_no, Data->fund_account, Data->entrust_no, Data->entrust_status, real_type, real_status );
	UFC::BufferedLog::Printf( " business_id[%s] business_amount[%.2f] opt_business_price[%.4f] business_time[%d] client_id[%s] report_account[%s]"
							  , Data->business_id, Data->business_amount, Data->opt_business_price, Data->business_time, client_id, report_account );
	UFC::BufferedLog::Printf( " option_code[%s] option_type[%c] entrust_bs[%c]", option_code, option_type, Data->entrust_bs );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::RecvReportConfirm( IF2UnPacker* Packet )
{
	char entrust_type = Packet->GetChar( "entrust_type" );
	if ( entrust_type == '0' ) ///< 委託
		PassConfirmExecution( Packet );
	//else if ( entrust_type == '2' ) ///< 撤單 ///< do nothing, just do action by ReportFill
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::RecvReportFill( IF2UnPacker* Packet )
{
	char real_type   = Packet->GetChar( "real_type" );
	char real_status = Packet->GetChar( "real_status" );
	if ( real_type == '0' ) ///< 委託
	{
		if ( real_status == '0' ) ///< 成交
			PassFillExecution( Packet );
		else if ( real_status == '2' ) ///< 廢單 ///< 委託失敗
			PassRejectByExh( Packet, nsOrderMessageDefine::crrNew );
		else
		{
			UFC::BufferedLog::Printf( " RecvReportFill() Unhandle real_status=%c when real_type=0 error!!", real_status );
		}
	}
	else if ( real_type == '2' ) ///< 撤單
	{
		if ( real_status == '0' ) ///< 成交 ///< 撤單成功
			PassCanceledExecution( Packet );
		else if ( real_status == '2' ) ///< 廢單 ///< 撤單失敗
			PassRejectByExh( Packet, nsOrderMessageDefine::crrCancel );
		else
		{
			UFC::BufferedLog::Printf( " RecvReportFill() Unhandle real_status=%c when real_type=2 error!!", real_status );
		}
	}
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::RecvLogin( IF2UnPacker* Packet )
{
	int         branch_no    = Packet->GetInt( "branch_no" );
	const char* fund_account = Packet->GetStr( "fund_account" );
	const char* user_token   = Packet->GetStr( "user_token" );
	const char* client_id    = Packet->GetStr( "client_id" );
	int         sysnode_id   = Packet->GetInt( "sysnode_id" );
	int         login_times  = Packet->GetInt( "login_times" );
	int         init_date    = Packet->GetInt( "init_date" );
	int         error_no     = Packet->GetInt( "error_no" );
	const char* error_info   = Packet->GetStr( "error_info" );
	if ( error_no == 0 ||
		 error_no == -41 || ///< 資金密碼到期未修改 (提示,仍可登錄)
		 error_no == -42 || ///< 交易密碼到期未修改 (提示,仍可登錄)
		 error_no == -71 )  ///< 客戶登錄受限 (提示,仍可登錄)
	{
		UFC::BufferedLog::Printf( " RecvLogin() OK!! user_token[%s] client_id[%s] login_times[%d] error_no[%d] error_info[%s]"
								  , user_token, client_id, login_times, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
		PassLoginResult( true, "", branch_no, fund_account , user_token, client_id, sysnode_id, login_times, init_date );
	}
	else
	{
		UTF8String UTF8Str( AnsiToUtf8Ex(error_info, 936) );
		UFC::AnsiString ErrMsg;
		ErrMsg.Printf( "Login failed! Code[%d] Msg[%s]", error_no, UTF8Str.c_str() );
		UFC::BufferedLog::Printf( " RecvLogin() login failed!! error_no[%d] error_info[%s]", error_no, UTF8Str.c_str() );
		PassLoginResult( false, ErrMsg.c_str() );
	}
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::RecvChangePwd( IF2UnPacker* Packet )
{
	int         error_no   = Packet->GetInt( "error_no" );
	const char* error_info = Packet->GetStr( "error_info" );
	if ( error_no == 0 )
	{
		UFC::BufferedLog::Printf( " RecvChangePwd() OK!! error_no[%d]", error_no );
		PassChangePwdResult( true, "" );
	}
	else
	{
		UTF8String UTF8Str( AnsiToUtf8Ex(error_info, 936) );
		UFC::AnsiString ErrMsg;
		ErrMsg.Printf( "Change password failed! Code[%d] Msg[%s]", error_no, UTF8Str.c_str() );
		UFC::BufferedLog::Printf( " RecvChangePwd() Change password failed!! error_no[%d] error_info[%s]", error_no, UTF8Str.c_str() );
		PassChangePwdResult( false, ErrMsg.c_str() );
	}
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::MainBizEx04( int hSend, LPRET_DATA lpRetData, const void *lpUnPackerOrStr, int nResult )
{
	int          iFuncID     = lpRetData->functionID;
	IF2UnPacker* Packet      = NULL;

	// 1. Check if HeartBeat
	if ( iFuncID == 620000 )
	{
		PassReplyHeartBeat( lpRetData );
		return;
	}

	// 1. initial Packet
	if ( nResult == 0 )
	{
		Packet = (IF2UnPacker*)lpUnPackerOrStr;
		if ( Packet == NULL )
		{
			UFC::BufferedLog::Printf( " MainBizEx04() lpUnpackerOrStr = NULL error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
									  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
			return;
		}
	}
	else ///< nResult == 4
	{
		Packet = gHSufxAPI.pNewUnPacker( lpRetData->lpKeyInfo, lpRetData->keyInfoLen );
		if ( Packet == NULL )
		{
			UFC::BufferedLog::Printf( " MainBizEx04() NewUnPacker = NULL error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
									  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
			return;
		}
		Packet->AddRef();
	}

	// 2. Do Biz functionID
	switch ( iFuncID )
	{
	// ====================== Stock Options Biz functions ======================
	case 338011 : ///< New Order [func_opt_entrust] (single row)
		PassPendingExecution( hSend, Packet );
		break;
	case 338012 : ///< Cancel Order [func_opt_withdraw] (single row)
		// do nothing, just do action by ReportFill
		break;
	case 620003 : ///< 33011-Fill , 33012-OrderConfirm [subscribe data report] (single row)
		if ( lpRetData->issueType == 33011 )
			RecvReportFill( Packet );
		else if ( lpRetData->issueType == 33012 )
			RecvReportConfirm( Packet );
		else
		{
			UFC::BufferedLog::Printf( " MainBizEx04() Unknow Push issueType error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
									  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
		}
		break;
	case 620001 : ///< SubscribePush [subscribe] (single row)
		if ( lpRetData->issueType != 33011 && lpRetData->issueType != 33012 )
		{
			UFC::BufferedLog::Printf( " MainBizEx04() Unknow Subscribe issueType error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
									  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
			PassSubscribeResult( false, lpRetData->issueType, "Unknow Subscribe issueType error!!" );
		}
		else
			PassSubscribeResult( true, lpRetData->issueType, "" );
		break;
	case 338020 : ///< Query Order [func_opt_entrust_qry] (multi row)
		PassQryOrderResult( Packet );
		break;
	case 338021 : ///< Query Fill [func_opt_realtime_qry] (multi row)
		PassQryFillResult( Packet );
		break;
	case 338022 : ///< Query Asset [func_opt_asset_qry] (single row)
		PassQryAssetResult( Packet );
		break;
	case 338023 : ///< Query Hold Postion [func_opt_hold_qry] (multi row)
		PassQryPositionResult( Packet );
		break;
	//========================= General Biz functions ==========================
	case 331100 : ///< Login [func_client_login] (single row)
		RecvLogin( Packet );
		break;
	case 331102 : ///< Logout [func_client_exit] (single row)
		PassLogoutResult( true );
		break;
	case 331101 : ///< Change Password [func_client_pwd_mod] (single row)
		RecvChangePwd( Packet );
		break;
	// =================== Unhandle(or Unknow) Biz functions ===================
	default :
		UFC::BufferedLog::Printf( " MainBizEx04() Unhandle functionID functionality!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
								  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
		break;
	}

	// 3. Release if need
	if ( nResult == 4 && Packet )
		Packet->Release();
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::MainBizEx12( int hSend, LPRET_DATA lpRetData, const void *lpUnPackerOrStr, int nResult )
{
	int          iFuncID     = lpRetData->functionID;
	IF2UnPacker* Packet      = NULL;

	if ( nResult == 1 )
	{
		Packet = (IF2UnPacker*)lpUnPackerOrStr;
		if ( Packet == NULL )
		{
			UFC::BufferedLog::Printf( " MainBizEx12() lpUnpackerOrStr = NULL error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
									  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
			return;
		}
		// Get data from Packet
		const char* error_pathinfo = Packet->GetStr( "error_pathinfo" );
		const char* error_no       = Packet->GetStr( "error_no" );
		const char* error_info     = Packet->GetStr( "error_info" );
		UFC::BufferedLog::Printf( " MainBizEx12() Reason:nResult = 1 error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
								  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
		if ( iFuncID == 338011 ) ///< New Order
		{
			UTF8String UTF8Str( AnsiToUtf8Ex(error_info, 936) );
			UFC::BufferedLog::Printf( " <Reject NewOrder> SessionID[%d] OrderRef[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , FHSufxLoginTimes, hSend, error_pathinfo, error_no, UTF8Str.c_str() );
			PassRejectExecution( hSend, UTF8Str.c_str(), nsOrderMessageDefine::crrNew );
			/*
			UFC::BufferedLog::Printf( " <Reject NewOrder> SessionID[%d] OrderRef[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , FHSufxLoginTimes, hSend, error_pathinfo, error_no, error_info );
			PassRejectExecution( hSend, error_info, nsOrderMessageDefine::crrNew );
			*/
		}
		else if ( iFuncID == 338012 ) ///< Cancel Order
		{
			UFC::BufferedLog::Printf( " <Reject Cancel> SessionID[%d] OrderRef[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , FHSufxLoginTimes, hSend
									  , error_pathinfo, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
		}
		else if ( iFuncID == 331100 ) ///< Login
		{
			UTF8String UTF8Str( AnsiToUtf8Ex(error_info, 936) );
			UFC::BufferedLog::Printf( " <Reject Login> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str.c_str() );
			PassLoginResult( false, UTF8Str.c_str() );
		}
		else if ( iFuncID == 331102 ) ///< Logout
		{
			UFC::BufferedLog::Printf( " <Reject Logout> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
			PassLogoutResult( false );
		}
		else if ( iFuncID == 331101 ) ///< Change Password
		{
			UTF8String UTF8Str( AnsiToUtf8Ex(error_info, 936) );
			UFC::BufferedLog::Printf( " <Reject Change Password> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str.c_str() );
			PassChangePwdResult( false, UTF8Str.c_str() );
		}
		else if ( iFuncID == 620001 ) ///< SubscribePush
		{
			UTF8String UTF8Str( AnsiToUtf8Ex(error_info, 936) );
			UFC::BufferedLog::Printf( " <Reject SubscribePush> hSend[%d] issurType[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, lpRetData->issueType, error_pathinfo, error_no, UTF8Str.c_str() );
			PassSubscribeResult( false, lpRetData->issueType, UTF8Str.c_str() );
		}
		else if ( iFuncID == 338020 ) ///< Query Order
		{
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
		}
		else if ( iFuncID == 338021 ) ///< Query Fill
		{
			UFC::BufferedLog::Printf( " <Reject QueryFill> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
		}
		else if ( iFuncID == 338022 ) ///< Query Asset
		{
			UFC::BufferedLog::Printf( " <Reject QueryAsset> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
		}
		else if ( iFuncID == 338023 ) ///< Query Hold Postion
		{
			UFC::BufferedLog::Printf( " <Reject QueryHoldPostion> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
		}
		else
		{
			UFC::BufferedLog::Printf( " <Reject> error_pathinfo[%s] error_no[%s] error_info[%s]", error_pathinfo, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
		}
	}
	else ///< nResult == 2
	{
		UFC::BufferedLog::Printf( " MainBizEx12() Reason:nResult = 2 error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
								  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
		if ( iFuncID == 338011 ) ///< New Order
		{
			UTF8String UTF8Str( AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936));
			UFC::BufferedLog::Printf( " <Reject NewOrder> SessionID[%d] OrderRef[%d] Msg[%s]"
									  , FHSufxLoginTimes, hSend, UTF8Str.c_str() );
			PassRejectExecution( hSend, UTF8Str.c_str(), nsOrderMessageDefine::crrNew );
			/*
			UFC::BufferedLog::Printf( " <Reject NewOrder> SessionID[%d] OrderRef[%d] Msg[%s]"
									  , FHSufxLoginTimes, hSend, (const char*)lpUnPackerOrStr );
			PassRejectExecution( hSend, (const char*)lpUnPackerOrStr, nsOrderMessageDefine::crrNew );
			*/
		}
		else if ( iFuncID == 338012 ) ///< Cancel Order
		{
			UFC::BufferedLog::Printf( " <Reject Cancel> SessionID[%d] OrderRef[%d] Msg[%s]"
									  , FHSufxLoginTimes, hSend, UTF8String(AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936)).c_str() );
		}
		else if ( iFuncID == 331100 ) ///< Login
		{
			UTF8String UTF8Str( AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936) );
			UFC::BufferedLog::Printf( " <Reject Login> hSend[%d] Msg[%s]", hSend, UTF8Str.c_str() );
			PassLoginResult( false, UTF8Str.c_str() );
		}
		else if ( iFuncID == 331102 ) ///< Logout
		{
			UFC::BufferedLog::Printf( " <Reject Logout> hSend[%d] Msg[%s]", hSend, UTF8String(AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936)).c_str() );
			PassLogoutResult( false );
		}
		else if ( iFuncID == 331101 ) ///< Change Password
		{
			UTF8String UTF8Str( AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936) );
			UFC::BufferedLog::Printf( " <Reject ChangePassword> hSend[%d] Msg[%s]", hSend, UTF8Str.c_str() );
			PassChangePwdResult( false, UTF8Str.c_str() );
		}
		else if ( iFuncID == 620001 ) ///< SubscribePush
		{
			UTF8String UTF8Str( AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936) );
			UFC::BufferedLog::Printf( " <Reject SubscribePush> hSend[%d] issurType[%d] Msg[%s]"
									  , hSend, lpRetData->issueType, UTF8Str.c_str() );
			PassSubscribeResult( false, lpRetData->issueType, UTF8Str.c_str() );
		}
		else if ( iFuncID == 338020 ) ///< Query Order
		{
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] Msg[%s]", hSend, UTF8String(AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936)).c_str() );
		}
		else if ( iFuncID == 338021 ) ///< Query Fill
		{
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] Msg[%s]", hSend, UTF8String(AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936)).c_str() );
		}
		else if ( iFuncID == 338022 ) ///< Query Asset
		{
			UFC::BufferedLog::Printf( " <Reject QueryAsset> hSend[%d] Msg[%s]", hSend, UTF8String(AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936)).c_str() );
		}
		else if ( iFuncID == 338023 ) ///< Query Hold Postion
		{
			UFC::BufferedLog::Printf( " <Reject QueryHoldPostion> hSend[%d] Msg[%s]", hSend, UTF8String(AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936)).c_str() );
		}
		else
		{
			UFC::BufferedLog::Printf( " <Reject> hSend[%d] Msg[%s]", hSend, UTF8String(AnsiToUtf8Ex((const char*)lpUnPackerOrStr, 936)).c_str() );
		}
	}
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::MainBizMsgOK( int hSend, IBizMessage* lpMsg )
{
	int iFuncID     = lpMsg->GetFunction();
	int iIssueType  = lpMsg->GetIssueType();
	int iErrorNo    = lpMsg->GetErrorNo();
	int iReturnCode = lpMsg->GetReturnCode();
	int iLen;
	const void*  lpBuffer;

	// 1. Check if HeartBeat
	if ( iFuncID == 620000 )
	{
		PassReplyHeartBeat( lpMsg );
		return;
	}

	// 2. initial Packet
	if ( iFuncID == 620001 || iFuncID == 620003 )
	{
		lpBuffer = lpMsg->GetKeyInfo( iLen );
		if ( lpBuffer == NULL )
		{
			UFC::BufferedLog::Printf( " MainBizMsgOK() lpMsg->GetKeyInfo() = NULL error!! (fun=%d,ret=%d,err=%d,issue=%d,hSend:%d)"
									  , iFuncID, iReturnCode, iErrorNo, iIssueType, hSend );
			return;
		}
	}
	else
	{
		lpBuffer = lpMsg->GetContent( iLen );
		if ( lpBuffer == NULL )
		{
			UFC::BufferedLog::Printf( " MainBizMsgOK() lpMsg->GetContent() = NULL error!! (fun=%d,ret=%d,err=%d,issue=%d,hSend:%d)"
									  , iFuncID, iReturnCode, iErrorNo, iIssueType, hSend );
			return;
		}
	}
	IF2UnPacker* Packet = gHSufxAPI.pNewUnPacker( (void*)lpBuffer, iLen );
	if ( Packet == NULL )
	{
		UFC::BufferedLog::Printf( " MainBizMsgOK() NewUnPacker = NULL error!! (fun=%d,ret=%d,err=%d,issue=%d,hSend:%d)"
								  , iFuncID, iReturnCode, iErrorNo, iIssueType, hSend );
		return;
	}
	Packet->AddRef();

	// 2. Do Biz functionID
	switch ( iFuncID )
	{
	// ====================== Stock Options Biz functions ======================
	case 338011 : ///< New Order [func_opt_entrust] (single row)
		PassPendingExecution( hSend, Packet );
		break;
	case 338012 : ///< Cancel Order [func_opt_withdraw] (single row)
		// do nothing, just do action by ReportFill
		break;
	case 620003 : ///< 33011-Fill , 33012-OrderConfirm [subscribe data report] (single row)
		if ( iIssueType == 33011 )
			RecvReportFill( Packet );
		else if ( iIssueType == 33012 )
			RecvReportConfirm( Packet );
		else
		{
			UFC::BufferedLog::Printf( " MainBizMsgOK() Unknow Push issueType error!! (fun=%d,ret=%d,err=%d,issue=%d)"
									  , iFuncID, iReturnCode, iErrorNo, iIssueType );
		}
		break;
	case 620001 : ///< SubscribePush [subscribe] (single row)
		if ( iIssueType != 33011 && iIssueType != 33012 )
		{
			UFC::BufferedLog::Printf( " MainBizMsgOK() Unknow Subscribe issueType error!! (fun=%d,ret=%d,err=%d,issue=%d)"
									  , iFuncID, iReturnCode, iErrorNo, iIssueType );
			PassSubscribeResult( false, iIssueType, "Unknow Subscribe issueType error!!" );
		}
		else
			PassSubscribeResult( true, iIssueType, "" );
		break;
	case 338020 : ///< Query Order [func_opt_entrust_qry] (multi row)
		PassQryOrderResult( Packet );
		break;
	case 338021 : ///< Query Fill [func_opt_realtime_qry] (multi row)
		PassQryFillResult( Packet );
		break;
	case 338022 : ///< Query Asset [func_opt_asset_qry] (single row)
		PassQryAssetResult( Packet );
		break;
	case 338023 : ///< Query Hold Postion [func_opt_hold_qry] (multi row)
		PassQryPositionResult( Packet );
		break;
	//========================= General Biz functions ==========================
	case 331100 : ///< Login [func_client_login] (single row)
		RecvLogin( Packet );
		break;
	case 331102 : ///< Logout [func_client_exit] (single row)
		PassLogoutResult( true );
		break;
	case 331101 : ///< Change Password [func_client_pwd_mod] (single row)
		RecvChangePwd( Packet );
		break;
	// =================== Unhandle(or Unknow) Biz functions ===================
	default :
		UFC::BufferedLog::Printf( " MainBizMsgOK() Unhandle functionID functionality!! (fun=%d,ret=%d,err=%d,issue=%d)"
								  , iFuncID, iReturnCode, iErrorNo, iIssueType );
		break;
	}

	// 3. Release if need
	if ( Packet )
		Packet->Release();
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::MainBizMsgErr( int hSend, IBizMessage* lpMsg )
{
	UTF8String* UTF8Str = NULL;
	const char* error_pathinfo;
	const char* error_no;
	const char* error_info;
	BOOL IsBusinessError = FALSE;

	IF2UnPacker* Packet = NULL;
	int iFuncID     = lpMsg->GetFunction();
	int iIssueType  = lpMsg->GetIssueType();
	int iErrorNo    = lpMsg->GetErrorNo();
	int iReturnCode = lpMsg->GetReturnCode();
	int nResult;

	// 1. Get Error Message
	if ( iErrorNo != 0 ) ///< Check if ErrorNo error
	{
		IsBusinessError = FALSE;
		UFC::BufferedLog::Printf( " MainBizMsgErr() Reason:nResult = 2 error!! (fun=%d,ret=%d,err=%d,issue=%d,hSend:%d)"
								  , iFuncID, iReturnCode, iErrorNo, iIssueType, hSend );
		UTF8Str = new UTF8String( AnsiToUtf8Ex( lpMsg->GetErrorInfo(), 936 ) );
	}
	else if ( iReturnCode != 0 ) ///< Check if ReturnCode error
	{
		int iLen;
		const void* lpBuffer;

		lpBuffer = lpMsg->GetContent( iLen );
		if ( lpBuffer == NULL )
		{
			lpBuffer = lpMsg->GetKeyInfo( iLen );
			if ( lpBuffer == NULL )
			{
				UFC::BufferedLog::Printf( " MainBizMsgErr() lpMsg->GetKeyInfo() = NULL error!! (fun=%d,ret=%d,err=%d,issue=%d,hSend:%d)"
										  , iFuncID, iReturnCode, iErrorNo, iIssueType, hSend );
				return;
			}
		}
		Packet = gHSufxAPI.pNewUnPacker( (void*)lpBuffer, iLen );
		if ( Packet == NULL )
		{
			UFC::BufferedLog::Printf( " MainBizMsgErr() NewUnPacker = NULL error!! (fun=%d,ret=%d,err=%d,issue=%d,hSend:%d)"
									  , iFuncID, iReturnCode, iErrorNo, iIssueType, hSend );
			return;
		}
		Packet->AddRef();

		error_pathinfo = Packet->GetStr( "error_pathinfo" );
		error_no       = Packet->GetStr( "error_no" );
		error_info     = Packet->GetStr( "error_info" );
		IsBusinessError = TRUE;
		UFC::BufferedLog::Printf( " MainBizMsgErr() Reason:nResult = 1 error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
								  , iFuncID, iReturnCode, iErrorNo, iIssueType, nResult );
		UTF8Str = new UTF8String( AnsiToUtf8Ex( error_info, 936 ) );
		Packet->Release();
	}

	// 2. Do Biz functionID
	if ( iFuncID == 338011 ) ///< New Order
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject NewOrder> SessionID[%d] OrderRef[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , FHSufxLoginTimes, hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject NewOrder> SessionID[%d] OrderRef[%d] Msg[%s]"
									  , FHSufxLoginTimes, hSend, UTF8Str->c_str() );
		PassRejectExecution( hSend, UTF8Str->c_str(), nsOrderMessageDefine::crrNew );
	}
	else if ( iFuncID == 338012 ) ///< Cancel Order
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject Cancel> SessionID[%d] OrderRef[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , FHSufxLoginTimes, hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject Cancel> SessionID[%d] OrderRef[%d] Msg[%s]"
									  , FHSufxLoginTimes, hSend, UTF8Str->c_str() );
		PassRejectExecution( hSend, UTF8Str->c_str(), nsOrderMessageDefine::crrCancel );
	}
	else if ( iFuncID == 331100 ) ///< Login
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject Login> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject Login> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
		PassLoginResult( false, UTF8Str->c_str() );
	}
	else if ( iFuncID == 331102 ) ///< Logout
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject Logout> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject Logout> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
		PassLogoutResult( false );
	}
	else if ( iFuncID == 331101 ) ///< Change Password
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject Change Password> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject ChangePassword> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
		PassChangePwdResult( false, UTF8Str->c_str() );
	}
	else if ( iFuncID == 620001 ) ///< SubscribePush
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject SubscribePush> hSend[%d] issurType[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, iIssueType, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject SubscribePush> hSend[%d] issurType[%d] Msg[%s]"
									  , hSend, iIssueType, UTF8Str->c_str() );
		PassSubscribeResult( false, iIssueType, UTF8Str->c_str() );
	}
	else if ( iFuncID == 338020 ) ///< Query Order
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}
	else if ( iFuncID == 338021 ) ///< Query Fill
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject QueryFill> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}
	else if ( iFuncID == 338022 ) ///< Query Asset
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject QueryAsset> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject QueryAsset> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}
	else if ( iFuncID == 338023 ) ///< Query Hold Postion
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject QueryHoldPostion> hSend[%d] error_pathinfo[%s] error_no[%s] error_info[%s]"
									  , hSend, error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject QueryHoldPostion> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}
	else
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject> error_pathinfo[%s] error_no[%s] error_info[%s]", error_pathinfo, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}

	if ( UTF8Str )
		delete UTF8Str;
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjStockOpt::OnConnect(CConnectionInterface */*lpConnection*/)
{
	PassOnConnect( );
	UFC::BufferedLog::Printf( " HSufxObjStockOpt::OnConnect() HSufx API call back" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjStockOpt::OnSafeConnect(CConnectionInterface */*lpConnection*/)
{
	UFC::BufferedLog::Printf( " HSufxObjStockOpt::OnSafeConnect() HSufx API call back" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjStockOpt::OnRegister(CConnectionInterface */*lpConnection*/)
{
	UFC::BufferedLog::Printf( " HSufxObjStockOpt::OnRegister() HSufx API call back" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjStockOpt::OnClose(CConnectionInterface */*lpConnection*/)
{
	PassOnClose( );
	UFC::BufferedLog::Printf( " HSufxObjStockOpt::OnClose() HSufx API call back" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjStockOpt::OnReceivedBizEx(CConnectionInterface */*lpConnection*/, int hSend, LPRET_DATA lpRetData, const void *lpUnPackerOrStr, int nResult)
{
	// Check lpRetData if NULL
	if ( lpRetData == NULL )
	{
		UFC::BufferedLog::Printf( " OnReceivedBizEx() error!! <Reason:lpRetData is NULL.>" );
		return;
	}

	// Check nResult
	switch ( nResult )
	{
	case 0 : ///< 表示業務數據接收成功，並且業務操作成功，lpUnpackerOrStr指向一個解包器
	case 4 : ///< 表示業務包為空，lpUnpackerOrStr指向 NULL。lpRetData這部分結果還會存在
		MainBizEx04( hSend, lpRetData, lpUnPackerOrStr, nResult );
		break;
	case 1 : ///< 表示業務數據接收成功，但業務操作失敗了，lpUnpackerOrStr指向一個解包器
	case 2 : ///< 表示收到非業務錯誤信息，lpUnpackerOrStr指向一個可讀的字符串錯誤信息
		MainBizEx12( hSend, lpRetData, lpUnPackerOrStr, nResult );
		break;
	case 3 : ///< 表示業務包解包失敗，lpUnpackerOrStr指向 NULL
		UFC::BufferedLog::Printf( " OnReceivedBizEx() nResult=3 Unpack business packet failed error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
								  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
		break;
	default :
		UFC::BufferedLog::Printf( " OnReceivedBizEx() nResult=%d Unhandle nResult error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
								  , lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->issueType, nResult );
		break;
	}
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjStockOpt::OnReceivedBizMsg(CConnectionInterface */*lpConnection*/, int hSend, IBizMessage* lpMsg)
{
	// Check lpMsg if NULL
	if ( lpMsg == NULL )
	{
		UFC::BufferedLog::Printf( " OnReceivedBizMsg() Reason:lpMsg is NULL error!! (hSend=%d)", hSend );
		return;
	}

	if ( lpMsg->GetErrorNo() == 0 && lpMsg->GetReturnCode() == 0 )
		MainBizMsgOK( hSend, lpMsg );
	else
		MainBizMsgErr( hSend, lpMsg );
}
//---------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
