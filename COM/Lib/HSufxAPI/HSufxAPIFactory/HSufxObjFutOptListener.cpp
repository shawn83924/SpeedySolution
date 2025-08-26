//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "HSufxObjFutOptListener.h"
#include "TTaifexConnection.h"
#include "APIParams.h"
//---------------------------------------------------------------------------
#include <WideStrUtils.hpp>
//---------------------------------------------------------------------------
//typedef AnsiStringT<936> GB2312String;
//---------------------------------------------------------------------------
//  - HSufx ExecID Rule
//  ---> Pending(None)  (NewOrder)             Key    : FrontID (0) + SessionID (session_no) + OrderRef (hSend)   <"%d|%d|%d">
//  <--- PendingNew     (PendingExecution)     ExecID : "P" + OrderLocalID (entrust_no)                           <"P_%d">
//  <--- New            (ConfirmExecution)     ExecID : "N" + OrderLocalID (entrust_no)                           <"N_%d">
//  <--- Filled         (FillExecution)        ExecID : "F" + OrderLocalID (entrust_no) + FillSeq (business_id)   <"F_%d_%s">
//  <--- Canceled       (CancelExecution)      ExecID : "C" + OrderLocalID (entrust_no)                           <"C_%d">
//  <--- Rejected ByHS  (HSufxRejectExecution) ExecID : "R" + SessionID (LoginTimes) + OrderRef (hSend)           <"R_%d_%d">
//  <--- Rejected ByExh (HSufxRejectByExh)     ExecID : "R" + OrderLocalID (entrust_no)                           <"R_%d">
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
//      ------> HSufx Confirm & Fill SendSubscribePush()
//                                        <------ HSufx OnReceivedBizMsg(620001) (Confirm & Fill)
//      <------ WndMsgRcvSubPush()
//
//  <3> TTaifexConnection::Recover(.) Recover(..)
//      <3.1>
//      ------> HSufx SendQryOrderConfirm()
//                                        <------ HSufx OnReceivedBizMsg(338301) (Confirm)
//      <------ WndMsg RcvQryOrderConfirm Finish
//      <3.2>
//      ------> HSufx SendQryOrderFill()
//                                        <------ HSufx OnReceivedBizMsg(338302) (Fill)
//      <------ WndMsg RcvQryOrderFill Finish
//    **2**Trigger IOrderConnectionEventListener::OnLogonReply()
//    **3**Trigger IOrderConnectionEventListener::OnRecoverFinished()
//      <3.3>
//      ------> HSufx SendQryExchStatus()
//      ------> HSufx SendQryExchTime()
//      ------> HSufx SendSubscribeExchStatus()
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
// class HSufxObjFutOpt
//
// Receiving data process implementation ( <--- )
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::InitMessageWindow( void )
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
		swprintf( MessageWindowsClassName, L"HSUFX_FutOpt_MsgWin_%10d", UFC::GetTickCountMS() );
	// Fill in the window class structure with parameters that describe the main window.
	wcx.cbSize = sizeof( wcx );                 // size of structure
	wcx.style = CS_HREDRAW |CS_VREDRAW;         // redraw if size changes
	wcx.lpfnWndProc   = HSufxObjFutOpt::MessageWndProc;// points to window procedure
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
BOOL HSufxObjFutOpt::CreateMessageWindow( void )
{
	FHwnd = CreateWindowW( MessageWindowsClassName,	// name of window class
						   L"HSUFX_FutOpt_App",		// title-bar string
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
LRESULT CALLBACK HSufxObjFutOpt::MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	HSufxObjFutOpt* HSufxObjPtr = (HSufxObjFutOpt*)wParam;
	switch ( Message )
	{
	case WM_HSUFX_EVENT+1  : HSufxObjPtr->HSufxOnConnect( );
							 return 0;
	case WM_HSUFX_EVENT+2  : HSufxObjPtr->HSufxOnClose( );
							 return 0;
	case WM_HSUFX_EVENT+3  : HSufxObjPtr->HSufxLoginResult( (HSufxFutOpt::dataLoginResult*)lParam );
							 delete (HSufxFutOpt::dataLoginResult*)lParam;
							 return 0;
//	case WM_HSUFX_EVENT+4  : HSufxObjPtr->HSufxLogoutResult( (HSufxFutOpt::dataLogoutResult*)lParam );
//							 delete (HSufxFutOpt::dataLogoutResult*)lParam;
//							 return 0;
	case WM_HSUFX_EVENT+5  : HSufxObjPtr->HSufxReplyHeartBeatMsg( (HSufxFutOpt::dataReplyHeartBeatMsg*)lParam );
							 delete [] ((HSufxFutOpt::dataReplyHeartBeatMsg*)lParam)->Buffer;
							 delete (HSufxFutOpt::dataReplyHeartBeatMsg*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+6  : HSufxObjPtr->HSufxChangePwdResult( (HSufxFutOpt::dataChangePwdResult*)lParam );
							 delete (HSufxFutOpt::dataChangePwdResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+7  : HSufxObjPtr->HSufxSubscribeResult( (HSufxFutOpt::dataSubscribeResult*)lParam );
							 delete (HSufxFutOpt::dataSubscribeResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+11 : HSufxObjPtr->HSufxPendingExecution( (HSufxFutOpt::dataPendingExecution*)lParam );
							 delete (HSufxFutOpt::dataPendingExecution*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+12 : HSufxObjPtr->HSufxPushReport( (HSufxFutOpt::dataPushReport*)lParam );
							 delete (HSufxFutOpt::dataPushReport*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+13 : HSufxObjPtr->HSufxRejectExecution( (HSufxFutOpt::dataRejectExecution*)lParam );
							 delete (HSufxFutOpt::dataRejectExecution*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+23 : HSufxObjPtr->HSufxQryAssetResult( (HSufxFutOpt::dataQryAssetResult*)lParam );
							 delete (HSufxFutOpt::dataQryAssetResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+24 : HSufxObjPtr->HSufxQryPositionResult( (HSufxFutOpt::dataQryPositionResult*)lParam );
							 delete (HSufxFutOpt::dataQryPositionResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+25 : HSufxObjPtr->HSufxExchStatusResult( (HSufxFutOpt::dataExchStatusResult*)lParam );
							 delete (HSufxFutOpt::dataExchStatusResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+26 : HSufxObjPtr->HSufxExchTimeResult( (HSufxFutOpt::dataExchTimeResult*)lParam );
							 delete (HSufxFutOpt::dataExchTimeResult*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+91 : HSufxObjPtr->HSufxReQueryCmd( (HSufxFutOpt::dataReQueryCmd*)lParam );
							 delete (HSufxFutOpt::dataReQueryCmd*)lParam;
							 return 0;
	case WM_HSUFX_EVENT+92 : HSufxObjPtr->QryOrderFinishProcess( );
							 return 0;
	case WM_HSUFX_EVENT+93 : HSufxObjPtr->QryFillFinishProcess( );
							 return 0;
	default : return DefWindowProc( hwnd, Message, wParam, lParam );
	}

	return 0;
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassOnConnect( void )
{
	PostMessage( FHwnd, WM_HSUFX_EVENT+1, (WPARAM)this, (LPARAM)NULL );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassOnClose( void )
{
	PostMessage( FHwnd, WM_HSUFX_EVENT+2, (WPARAM)this, (LPARAM)NULL );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassLoginResult( bool IsLoginOK, const char* ErrMsg,
									  int branch_no, const char* fund_account,
									  const char* user_token, const char* client_id,
									  int sysnode_id, int session_no, int init_date )
{
	// 1. Initial Data
	HSufxFutOpt::dataLoginResult* Data = new HSufxFutOpt::dataLoginResult();
	Data->IsLoginOK   = IsLoginOK;
	strcpy( Data->ErrMsg, ErrMsg );
	Data->branch_no   = branch_no;
	strcpy( Data->fund_account, fund_account );
	strcpy( Data->user_token, user_token );
	strcpy( Data->client_id, client_id );
	Data->sysnode_id  = sysnode_id;
	Data->session_no  = session_no;
	Data->init_date   = init_date;
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+3, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassLoginResult: IsLoginOK[%c] branch_no[%d] fund_account[%s] user_token[%s] client_id[%s]"
							  , IsLoginOK ? 'Y' : 'N', branch_no, fund_account, user_token, client_id );
	UFC::BufferedLog::Printf( " sysnode_id[%d] session_no[%d] init_date[%d] ErrMsg[%s]", sysnode_id, session_no, init_date, ErrMsg );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassLogoutResult( bool /*IsLogoutOK*/ )
{
	// no implementation
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassReplyHeartBeat( IBizMessage* lpMsg )
{
	//如果要把消息放到其他線程處理，必須自行拷貝，操作如下：
	//int iMsgLen = 0;
	//void * lpMsgBuffer = lpBizMessageRecv->GetBuff(iMsgLen);
	//將lpMsgBuffer拷貝走，然後在其他線程中恢復成消息可進行如下操作：
	//lpBizMessageRecv->SetBuff(lpMsgBuffer,iMsgLen);

	// 1. Initial Data;
	lpMsg->ChangeReq2AnsMessage();
	HSufxFutOpt::dataReplyHeartBeatMsg* Data = new HSufxFutOpt::dataReplyHeartBeatMsg();
	void* lpMsgBuffer = lpMsg->GetBuff( Data->BufferLen );
	Data->Buffer = new char[ Data->BufferLen ];
	memcpy( Data->Buffer, lpMsgBuffer, Data->BufferLen );
	PostMessage( FHwnd, WM_HSUFX_EVENT+5, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassChangePwdResult( bool IsChangePwdOK, const char* ErrMsg )
{
	// 1. Initial Data
	HSufxFutOpt::dataChangePwdResult* Data = new HSufxFutOpt::dataChangePwdResult();
	Data->IsChangePwdOK = IsChangePwdOK;
	strcpy( Data->ErrMsg, ErrMsg );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+6, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassChangePwdResult: IsChangePwdOK[%c] ErrMsg[%s]", IsChangePwdOK ? 'Y' : 'N', ErrMsg );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassSubscribeResult( bool IsSubscribeOK, int IssueType, const char* ErrMsg )
{
	// 1. Initial Data
	HSufxFutOpt::dataSubscribeResult* Data = new HSufxFutOpt::dataSubscribeResult();
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
void HSufxObjFutOpt::PassQryOrderResult( IF2UnPacker* Packet )
{
	int RowCount = Packet->GetRowCount();
	UFC::AnsiString PositionStr;

	for ( int i = 0; i < RowCount; i++ )
	{
		// 1. Get data from Packet
		HSufxFutOpt::dataQryOrderResult* Data = new HSufxFutOpt::dataQryOrderResult();
		Data->batch_no                 = Packet->GetInt( "batch_no" );
		Data->entrust_no               = Packet->GetInt( "entrust_no" );
		strcpy( Data->fund_account,      Packet->GetStr( "fund_account" ) );
		strcpy( Data->futu_exch_type,    Packet->GetStr( "futu_exch_type" ) );
		strcpy( Data->futures_account,   Packet->GetStr( "futures_account" ) );
		strcpy( Data->futu_code,         Packet->GetStr( "futu_code" ) );
		strcpy( Data->money_type,        Packet->GetStr( "money_type" ) );
		Data->futures_direction        = Packet->GetChar( "futures_direction" );
		Data->futu_entrust_price       = Packet->GetDouble( "futu_entrust_price" );
		Data->entrust_bs               = Packet->GetChar( "entrust_bs" );
		Data->hedge_type               = Packet->GetChar( "hedge_type" );
		Data->entrust_status           = Packet->GetChar( "entrust_status" );
		Data->entrust_time             = Packet->GetInt( "entrust_time" );
		Data->report_time              = Packet->GetInt( "report_time" );
		Data->business_amount          = Packet->GetInt( "business_amount" );
		Data->entrust_amount           = Packet->GetInt( "entrust_amount" );
		Data->withdraw_amount          = Packet->GetInt( "withdraw_amount" );
		Data->curr_entrust_margin      = Packet->GetDouble( "curr_entrust_margin" );
		Data->entrust_type             = Packet->GetChar( "entrust_type" );
		strcpy( Data->confirm_id,        Packet->GetStr( "confirm_id" ) );
		Data->curr_entrust_fare        = Packet->GetDouble( "curr_entrust_fare" );
		Data->forceclose_reason        = Packet->GetChar( "forceclose_reason" );
		HSufxStrcpyNoLineFeed( Data->error_message, Packet->GetStr( "error_message" ), 256 );
		Data->business_balance         = Packet->GetDouble( "business_balance" );
		strcpy( Data->entrust_prop,      Packet->GetStr( "entrust_prop" ) );
		strcpy( Data->arbit_code,        Packet->GetStr( "arbit_code" ) );
		strcpy( Data->second_code,       Packet->GetStr( "second_code" ) );
		Data->weave_type               = Packet->GetChar( "weave_type" );
		Data->spring_price             = Packet->GetDouble( "spring_price" );
		Data->time_condition           = Packet->GetChar( "time_condition" );
		Data->valid_date               = Packet->GetInt( "valid_date" );
		Data->volume_condition         = Packet->GetChar( "volume_condition" );
		Data->fex_min_volume           = Packet->GetInt( "fex_min_volume" );
		strcpy( Data->report_id,         Packet->GetStr( "report_id" ) );
		Data->curr_entrust_premium     = Packet->GetDouble( "curr_entrust_premium" );
		Data->session_no               = Packet->GetInt( "session_no" );
		strcpy( Data->entrust_reference, Packet->GetStr( "entrust_reference" ) );
		strcpy( Data->entrust_occasion,  Packet->GetStr( "entrust_occasion" ) );
		const char* strPos             = Packet->GetStr( "position_str" );
		if ( strPos == 0 )
		{
			Data->position_str[0] = 0;
			PositionStr = "";
		}
		else
		{
			strcpy( Data->position_str, strPos );
			PositionStr = strPos;
		}
		// 2. Packet to next row
		Packet->Next();
		// 3. add to FHSufxQryOrderVector
		FHSufxQryOrderVector.push_back( Data );
	}
	// 4. Post message
	if ( RowCount == FHSufxRequestNum && PositionStr.Length() != 0 )
	{
		// ReQueryCmd message
		HSufxFutOpt::dataReQueryCmd* DataReQuery = new HSufxFutOpt::dataReQueryCmd();
		DataReQuery->FunctionID = 338301;
		strcpy( DataReQuery->position_str, PositionStr.c_str() );
		PostMessage( FHwnd, WM_HSUFX_EVENT+91, (WPARAM)this, (LPARAM)DataReQuery );
	}
	else
	{
		// QryOrderResult finish message
		PostMessage( FHwnd, WM_HSUFX_EVENT+92, (WPARAM)this, (LPARAM)NULL );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassQryFillResult( IF2UnPacker* Packet )
{
	int RowCount = Packet->GetRowCount();
	UFC::AnsiString PositionStr;

	for ( int i = 0; i < RowCount; i++ )
	{
		// 1. Get data from Packet
		HSufxFutOpt::dataQryFillResult* Data = new HSufxFutOpt::dataQryFillResult();
		Data->entrust_no               = Packet->GetInt( "entrust_no" );
		strcpy( Data->futu_exch_type,    Packet->GetStr( "futu_exch_type" ) );
		strcpy( Data->futures_account,   Packet->GetStr( "futures_account" ) );
		strcpy( Data->futu_code,         Packet->GetStr( "futu_code" ) );
		strcpy( Data->money_type,        Packet->GetStr( "money_type" ) );
		Data->futures_direction        = Packet->GetChar( "futures_direction" );
		Data->entrust_bs               = Packet->GetChar( "entrust_bs" );
		Data->hedge_type               = Packet->GetChar( "hedge_type" );
		Data->futu_business_price      = Packet->GetDouble( "futu_business_price" );
		Data->entrust_type             = Packet->GetChar( "entrust_type" );
		Data->business_time            = Packet->GetInt( "business_time" );
		Data->business_amount          = Packet->GetInt( "business_amount" );
		Data->business_fare            = Packet->GetDouble( "business_fare" );
		strcpy( Data->business_id,       Packet->GetStr( "business_id" ) );
		strcpy( Data->confirm_id,        Packet->GetStr( "confirm_id" ) );
		Data->session_no               = Packet->GetInt( "session_no" );
		strcpy( Data->entrust_reference, Packet->GetStr( "entrust_reference" ) );
		strcpy( Data->entrust_occasion,  Packet->GetStr( "entrust_occasion" ) );
		const char* strPos             = Packet->GetStr( "position_str" );
		if ( strPos == 0 )
		{
			Data->position_str[0] = 0;
			PositionStr = "";
		}
		else
		{
			strcpy( Data->position_str, strPos );
			PositionStr = strPos;
		}
		// 2. Packet to next row
		Packet->Next();
		// 3. add to FHSufxQryOrderVector
		FHSufxQryFillVector.push_back( Data );
	}
	// 4. Post message
	if ( RowCount == FHSufxRequestNum && PositionStr.Length() != 0 )
	{
		// ReQueryCmd message
		HSufxFutOpt::dataReQueryCmd* DataReQuery = new HSufxFutOpt::dataReQueryCmd();
		DataReQuery->FunctionID = 338302;
		strcpy( DataReQuery->position_str, PositionStr.c_str() );
		PostMessage( FHwnd, WM_HSUFX_EVENT+91, (WPARAM)this, (LPARAM)DataReQuery );
	}
	else
	{
		// QryFillResult finish message
		PostMessage( FHwnd, WM_HSUFX_EVENT+93, (WPARAM)this, (LPARAM)NULL );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassQryAssetResult( IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	HSufxFutOpt::dataQryAssetResult* Data = new HSufxFutOpt::dataQryAssetResult();
	strcpy( Data->money_type,    Packet->GetStr( "money_type" ) );
	Data->current_balance      = Packet->GetDouble( "current_balance" );
	Data->enable_balance       = Packet->GetDouble( "enable_balance" );
	Data->fetch_balance        = Packet->GetDouble( "fetch_balance" );
	Data->frozen_balance       = Packet->GetDouble( "frozen_balance" );
	Data->pre_entrust_balance  = Packet->GetDouble( "pre_entrust_balance" );
	Data->entrust_balance      = Packet->GetDouble( "entrust_balance" );
	Data->hold_income          = Packet->GetDouble( "hold_income" );
	Data->hold_income_float    = Packet->GetDouble( "hold_income_float" );
	Data->begin_equity_balance = Packet->GetDouble( "begin_equity_balance" );
	Data->equity_balance       = Packet->GetDouble( "equity_balance" );
	Data->interest_balance     = Packet->GetDouble( "interest_balance" );
	Data->drop_income          = Packet->GetDouble( "drop_income" );
	Data->drop_income_float    = Packet->GetDouble( "drop_income_float" );
	Data->business_fare        = Packet->GetDouble( "business_fare" );
	Data->hold_margin          = Packet->GetDouble( "hold_margin" );
	Data->exch_hold_margin     = Packet->GetDouble( "exch_hold_margin" );
	Data->client_risk_rate     = Packet->GetDouble( "client_risk_rate" );
	Data->exch_risk_rate       = Packet->GetDouble( "exch_risk_rate" );
	Data->out_premium          = Packet->GetDouble( "out_premium" );
	Data->in_premium           = Packet->GetDouble( "in_premium" );
	Data->market_value         = Packet->GetDouble( "market_value" );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+23, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassQryPositionResult( IF2UnPacker* Packet )
{
	int RowCount = Packet->GetRowCount();
	UFC::AnsiString PositionStr;

	for ( int i = 0; i < RowCount; i++ )
	{
		// 1. Get data from Packet
		HSufxFutOpt::dataQryPositionResult* Data = new HSufxFutOpt::dataQryPositionResult();
		Data->IsEndNotify            = false;
		strcpy( Data->futu_exch_type,  Packet->GetStr( "futu_exch_type" ) );
		strcpy( Data->futures_account, Packet->GetStr( "futures_account" ) );
		strcpy( Data->futu_code,       Packet->GetStr( "futu_code" ) );
		strcpy( Data->money_type,      Packet->GetStr( "money_type" ) );
		Data->entrust_bs             = Packet->GetChar( "entrust_bs" );
		Data->begin_amount           = Packet->GetInt( "begin_amount" );
		Data->enable_amount          = Packet->GetInt( "enable_amount" );
		Data->real_enable_amount     = Packet->GetInt( "real_enable_amount" );
		Data->hold_income_float      = Packet->GetDouble( "hold_income_float" );
		Data->hold_income            = Packet->GetDouble( "hold_income" );
		Data->hold_margin            = Packet->GetDouble( "hold_margin" );
		Data->average_price          = Packet->GetDouble( "average_price" );
		Data->futu_last_price        = Packet->GetDouble( "futu_last_price" );
		Data->hedge_type             = Packet->GetChar( "hedge_type" );
		Data->real_amount            = Packet->GetInt( "real_amount" );
		Data->real_open_balance      = Packet->GetDouble( "real_open_balance" );
		Data->old_open_balance       = Packet->GetDouble( "old_open_balance" );
		Data->real_current_amount    = Packet->GetInt( "real_current_amount" );
		Data->old_current_amount     = Packet->GetInt( "old_current_amount" );
		Data->futu_product_type      = Packet->GetChar( "futu_product_type" );
		const char* strPos           = Packet->GetStr( "position_str" );
		if ( strPos == 0 )
		{
			Data->position_str[0] = 0;
			PositionStr = "";
		}
		else
		{
			strcpy( Data->position_str, strPos );
			PositionStr = strPos;
		}
		// 2. Packet to next row
		Packet->Next();
		// 3. Post message
		PostMessage( FHwnd, WM_HSUFX_EVENT+24, (WPARAM)this, (LPARAM)Data );
	}
	// 4. Post message
	if ( RowCount == FHSufxRequestNum && PositionStr.Length() != 0 )
	{
		// ReQueryCmd message
		HSufxFutOpt::dataReQueryCmd* DataReQuery = new HSufxFutOpt::dataReQueryCmd();
		DataReQuery->FunctionID = 338303;
		strcpy( DataReQuery->position_str, PositionStr.c_str() );
		PostMessage( FHwnd, WM_HSUFX_EVENT+91, (WPARAM)this, (LPARAM)DataReQuery );
	}
	else
	{
		// End notify message
		HSufxFutOpt::dataQryPositionResult* DataEnd = new HSufxFutOpt::dataQryPositionResult();
		DataEnd->IsEndNotify = true;
		PostMessage( FHwnd, WM_HSUFX_EVENT+24, (WPARAM)this, (LPARAM)DataEnd );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassQryExchStatusResult( IF2UnPacker* Packet )
{
	int RowCount = Packet->GetRowCount();

	for ( int i = 0; i < RowCount; i++ )
	{
		// 1. Get data from Packet
		HSufxFutOpt::dataExchStatusResult* Data = new HSufxFutOpt::dataExchStatusResult();
		strcpy( Data->futu_exch_type,  Packet->GetStr( "futu_exch_type" ) );
		Data->exchange_status        = Packet->GetChar( "exchange_status" );
		Data->exch_status            = Packet->GetChar( "exch_status" );
		// 2. Packet to next row
		Packet->Next();
		// 3. Post message
		PostMessage( FHwnd, WM_HSUFX_EVENT+25, (WPARAM)this, (LPARAM)Data );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassQryExchTimeResult( IF2UnPacker* Packet )
{
	int RowCount = Packet->GetRowCount();
	HSufxFutOpt::dataExchTimeResult* Data = new HSufxFutOpt::dataExchTimeResult();
	memset( Data, 0, sizeof(HSufxFutOpt::dataExchTimeResult) );

	for ( int i = 0; i < RowCount; i++ )
	{
		UFC::AnsiString StrExchangeType = Packet->GetStr( "futu_exch_type" );
		// 1. Get data from Packet
		if ( StrExchangeType == "F1" )
			Data->CZCE_curr_time = Packet->GetInt( "curr_time" );
		else if ( StrExchangeType == "F2" )
			Data->DCE_curr_time = Packet->GetInt( "curr_time" );
		else if ( StrExchangeType == "F3" )
			Data->SHFE_curr_time = Packet->GetInt( "curr_time" );
		else if ( StrExchangeType == "F4" )
			Data->CFFEX_curr_time = Packet->GetInt( "curr_time" );

		// 2. Packet to next row
		Packet->Next();
		// 3. Post message
	}
	PostMessage( FHwnd, WM_HSUFX_EVENT+26, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassPendingExecution( int hSend, IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	HSufxFutOpt::dataPendingExecution* Data = new HSufxFutOpt::dataPendingExecution();
	Data->hSend                    = hSend;
	Data->entrust_no               = Packet->GetInt( "entrust_no" );
	strcpy( Data->entrust_reference, Packet->GetStr( "entrust_reference" ) );
	//strcpy( Data->futu_exch_type,    Packet->GetStr( "futu_exch_type" ) ); ///< 有些環境(ex:魯証) 沒回這個欄位
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+11, (WPARAM)this, (LPARAM)Data );
	// 3. Write log
	UFC::BufferedLog::Printf( " PassPendingExecution: hSend[%d] entrust_no[%d] entrust_reference[%s]", hSend, Data->entrust_no, Data->entrust_reference );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassPushReport( IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	HSufxFutOpt::dataPushReport* Data = new HSufxFutOpt::dataPushReport();
	Data->LY        = Packet->GetChar( "LY" );
	strcpy( Data->QH, Packet->GetStr( "QH" ) );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+12, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassPushExchStatus( IF2UnPacker* Packet )
{
	// 1. Get data from Packet
	char strQH[32];
	strcpy( strQH, Packet->GetStr( "QH" ) );
	// 2. Parse QH Data
	char* strPos = strQH;
	char* futu_exch_type  = GetNextPushToken( strPos );
	char* exchange_status = GetNextPushToken( strPos );
	char* exch_status     = GetNextPushToken( strPos );
	HSufxFutOpt::dataExchStatusResult* Data = new HSufxFutOpt::dataExchStatusResult();
	strcpy( Data->futu_exch_type, futu_exch_type );
	Data->exchange_status       = exchange_status[0];
	Data->exch_status           = exch_status[0];
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+25, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::PassRejectExecution( int hSend, const char* ErrMsg, nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo )
{
	// 1. Get data from Packet
	HSufxFutOpt::dataRejectExecution* Data = new HSufxFutOpt::dataRejectExecution();
	Data->ResponseTo = ResponseTo;
	Data->hSend      = hSend;
	HSufxStrcpyNoLineFeed( Data->ErrMsg, ErrMsg, 500 );
	// 2. Post message
	PostMessage( FHwnd, WM_HSUFX_EVENT+13, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::RecvLogin( IF2UnPacker* Packet )
{
	int         branch_no    = Packet->GetInt( "branch_no" );
	const char* fund_account = Packet->GetStr( "fund_account" );
	const char* user_token   = Packet->GetStr( "user_token" );
	const char* client_id    = Packet->GetStr( "client_id" );
	int         sysnode_id   = Packet->GetInt( "sysnode_id" );
	int         session_no   = Packet->GetInt( "session_no" );
	int         init_date    = Packet->GetInt( "init_date" );
	int         error_no     = Packet->GetInt( "error_no" );
	const char* error_info   = Packet->GetStr( "error_info" );
	if ( error_no == 0 ||
		 error_no == -41 || ///< 資金密碼到期未修改 (提示,仍可登錄)
		 error_no == -42 || ///< 交易密碼到期未修改 (提示,仍可登錄)
		 error_no == -71 )  ///< 客戶登錄受限 (提示,仍可登錄)
	{
		UFC::BufferedLog::Printf( " RecvLogin() OK!! user_token[%s] client_id[%s] session_no[%d] error_no[%d] error_info[%s]"
								  , user_token, client_id, session_no, error_no, UTF8String(AnsiToUtf8Ex(error_info, 936)).c_str() );
		PassLoginResult( true, "", branch_no, fund_account , user_token, client_id, sysnode_id, session_no, init_date );
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
void HSufxObjFutOpt::RecvChangePwd( IF2UnPacker* Packet )
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
void HSufxObjFutOpt::MainBizMsgOK( int hSend, IBizMessage* lpMsg )
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
	if ( iFuncID == 620001 )
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
	// ====================== Future Options Biz functions ======================
	case 338202 : ///< New Order (single row)
		PassPendingExecution( hSend, Packet );
		break;
	case 338217 : ///< Cancel Order (single row)
		// do nothing, just do action by ReportFill
		break;
	case 620003 : ///< 33101-Push (Confirm & Fill) (single row)
		if ( iIssueType == 33101 )
			PassPushReport( Packet );
		else if ( iIssueType == 33105 )
			PassPushExchStatus( Packet );
		else
		{
			UFC::BufferedLog::Printf( " MainBizMsgOK() Unknow Push issueType error!! (fun=%d,ret=%d,err=%d,issue=%d)"
									  , iFuncID, iReturnCode, iErrorNo, iIssueType );
		}
		break;
	case 620001 : ///< SubscribePush [subscribe] (single row)
		if ( iIssueType == 33101 || ///< Report Push
			 iIssueType == 33105 )  ///< ExchStatus
			PassSubscribeResult( true, iIssueType, "" );
		else
		{
			UFC::BufferedLog::Printf( " MainBizMsgOK() Unknow Subscribe issueType error!! (fun=%d,ret=%d,err=%d,issue=%d)"
									  , iFuncID, iReturnCode, iErrorNo, iIssueType );
			PassSubscribeResult( false, iIssueType, "Unknow Subscribe issueType error!!" );
		}
		break;
	case 338301 : ///< Query Order (multi row)
		PassQryOrderResult( Packet );
		break;
	case 338302 : ///< Query Fill (multi row)
		PassQryFillResult( Packet );
		break;
	case 338300 : ///< Query Asset (single row)
		PassQryAssetResult( Packet );
		break;
	case 338303 : ///< Query Hold Postion (multi row)
		PassQryPositionResult( Packet );
		break;
	case 338313 : ///< Query ExchStatus (multi row)
		PassQryExchStatusResult( Packet );
		break;
	case 330852 : ///< Query ExchTime (multi row)
		PassQryExchTimeResult( Packet );
		break;
	//========================= General Biz functions ==========================
	case 331100 : ///< Login (single row)
		RecvLogin( Packet );
		break;
//	case 331102 : ///< Logout (single row)
//		PassLogoutResult( true );
//		break;
	case 331101 : ///< Change Password (single row)
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
void HSufxObjFutOpt::MainBizMsgErr( int hSend, IBizMessage* lpMsg )
{
	UTF8String* UTF8Str = NULL;
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

		error_no       = Packet->GetStr( "error_no" );
		error_info     = Packet->GetStr( "error_info" );
		IsBusinessError = TRUE;
		UFC::BufferedLog::Printf( " MainBizMsgErr() Reason:nResult = 1 error!! (fun=%d,ret=%d,err=%d,issue=%d,nResult:%d)"
								  , iFuncID, iReturnCode, iErrorNo, iIssueType, nResult );
		UTF8Str = new UTF8String( AnsiToUtf8Ex( error_info, 936 ) );
		Packet->Release();
	}

	// 2. Do Biz functionID
	if ( iFuncID == 338202 ) ///< New Order
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject NewOrder> SessionID[%d] OrderRef[%d] error_no[%s] error_info[%s]", FHSufxSessionNo, hSend, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject NewOrder> SessionID[%d] OrderRef[%d] Msg[%s]", FHSufxSessionNo, hSend, UTF8Str->c_str() );
		PassRejectExecution( hSend, UTF8Str->c_str(), nsOrderMessageDefine::crrNew );
	}
	else if ( iFuncID == 338217 ) ///< Cancel Order
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject Cancel> SessionID[%d] OrderRef[%d] error_no[%s] error_info[%s]", FHSufxSessionNo, hSend, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject Cancel> SessionID[%d] OrderRef[%d] Msg[%s]", FHSufxSessionNo, hSend, UTF8Str->c_str() );
		PassRejectExecution( hSend, UTF8Str->c_str(), nsOrderMessageDefine::crrCancel );
	}
	else if ( iFuncID == 331100 ) ///< Login
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject Login> hSend[%d] error_no[%s] error_info[%s]", hSend, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject Login> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
		PassLoginResult( false, UTF8Str->c_str() );
	}
//	else if ( iFuncID == 331102 ) ///< Logout
//	{
//		if ( IsBusinessError )
//			UFC::BufferedLog::Printf( " <Reject Logout> hSend[%d] error_no[%s] error_info[%s]", hSend, error_no, UTF8Str->c_str() );
//		else
//			UFC::BufferedLog::Printf( " <Reject Logout> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
//		PassLogoutResult( false );
//	}
	else if ( iFuncID == 331101 ) ///< Change Password
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject Change Password> hSend[%d] error_no[%s] error_info[%s]", hSend, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject ChangePassword> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
		PassChangePwdResult( false, UTF8Str->c_str() );
	}
	else if ( iFuncID == 620001 ) ///< SubscribePush
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject SubscribePush> hSend[%d] issurType[%d] error_no[%s] error_info[%s]", hSend, iIssueType, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject SubscribePush> hSend[%d] issurType[%d] Msg[%s]", hSend, iIssueType, UTF8Str->c_str() );
		PassSubscribeResult( false, iIssueType, UTF8Str->c_str() );
	}
	else if ( iFuncID == 338301 ) ///< Query Order
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] error_no[%s] error_info[%s]", hSend, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}
	else if ( iFuncID == 338302 ) ///< Query Fill
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject QueryFill> hSend[%d] error_no[%s] error_info[%s]", hSend, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject QueryOrder> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}
	else if ( iFuncID == 338300 ) ///< Query Asset
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject QueryAsset> hSend[%d] error_no[%s] error_info[%s]", hSend, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject QueryAsset> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}
	else if ( iFuncID == 338303 ) ///< Query Hold Postion
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject QueryHoldPostion> hSend[%d] error_no[%s] error_info[%s]", hSend, error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject QueryHoldPostion> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}
	else
	{
		if ( IsBusinessError )
			UFC::BufferedLog::Printf( " <Reject> error_no[%s] error_info[%s]", error_no, UTF8Str->c_str() );
		else
			UFC::BufferedLog::Printf( " <Reject> hSend[%d] Msg[%s]", hSend, UTF8Str->c_str() );
	}

	if ( UTF8Str )
		delete UTF8Str;
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjFutOpt::OnConnect(CConnectionInterface* /*lpConnection*/)
{
	PassOnConnect( );
	UFC::BufferedLog::Printf( " HSufxObjFutOpt::OnConnect() HSufx API call back" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjFutOpt::OnSafeConnect(CConnectionInterface* /*lpConnection*/)
{
	UFC::BufferedLog::Printf( " HSufxObjFutOpt::OnSafeConnect() HSufx API call back" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjFutOpt::OnRegister(CConnectionInterface* /*lpConnection*/)
{
	UFC::BufferedLog::Printf( " HSufxObjFutOpt::OnRegister() HSufx API call back" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjFutOpt::OnClose(CConnectionInterface* /*lpConnection*/)
{
	PassOnClose( );
	UFC::BufferedLog::Printf( " HSufxObjFutOpt::OnClose() HSufx API call back" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxObjFutOpt::OnReceivedBizMsg(CConnectionInterface* /*lpConnection*/, int hSend, IBizMessage* lpMsg)
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
