//---------------------------------------------------------------------------
#pragma hdrstop
#include "CAChecker.h"
#include "iniFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdHTTP"
#pragma link "IdTCPClient"
#pragma link "IdTCPConnection"
//---------------------------------------------------------------------------
extern bool gCheckCA;
extern bool GSimMatch;
extern TOrderStore*   gOrderStore;
//---------------------------------------------------------------------------
const int ALGOR_SHA256 = 0x10004;
//---------------------------------------------------------------------------
TCAChecker::TCAChecker( TComponent* Owner )
:UFC::PThread( NULL, FALSE )
,FInitOK( false )
,FSync( true )
,FIsRunning( false )
,FCASerial("")
,FOwner( Owner )
,FCheckResult(false)
,FCAObject( NULL )
,FPFXFile( L"C:\\src\\S8Projects\\Win32\\Debug\\52883910.pfx" )
,FPFXFilePassword( "" )
,FLocalIP( L"127.0.0.1" )
,FURL( "http://ekeytest.emega.com.tw:8080/VA/StockP1VerifySign.jsp" )
{
	FHTTP = new TIdHTTP( FOwner );
	FSSLIOHandler = new TIdSSLIOHandlerSocketOpenSSL( FOwner );
	FSSLIOHandler->SSLOptions->Method = sslvSSLv23;
	FHTTP->IOHandler = FSSLIOHandler;
}
//---------------------------------------------------------------------------
TCAChecker::~TCAChecker( void )
{
	Terminate();
	WaitFor( 10 );
	if( FCAObject != NULL )
		delete FCAObject;
	if( FHTTP != NULL )
		delete FHTTP;
	if( FSSLIOHandler != NULL )
		delete FSSLIOHandler;
}
//---------------------------------------------------------------------------
bool TCAChecker::InitialCheckerAndTest( String& Reason /* out */ )
{
	if( GSimMatch == true )
	{
		Reason = L"虛擬交易CA無須驗證";
		return true;
    }
	FSignSubject.printf( L"CN=%s\nOU=MegaSecurities\tCN=%s\nOU=Mega Securities:ADMIN\tCN=%s\nOU=International Securities Co.,",FID,FID,FID);
	FLocalIP = gOrderStore->LocalIP;
	FAnsiLocalIP = gOrderStore->LocalIP;
	if( gCheckCA == true ) ///< Check CA
	{
		String sign;

		if( FCAObject == NULL )
			FCAObject = new TMEGASECCAPI( FOwner );
		if( FCAObject != NULL )
		{
			String Content( L"CATest" );
			int    Err = Sign( Content, sign );

			if( Err != 0 )
			{
				Reason = ToErrorMessage( Err );//L"讀取失敗,請確認CA憑證已正確安裝,並未過期.";
				FCheckResult = false;
				FInitOK      = false;
				FSync        = false;
				if( FOnCACheckFail != NULL )
					FOnCACheckFail( this, Reason ); ///<
				return false;
			}
			else
			{
				FCASerial    = FCAObject->CGCAPIStockGetSN();
				FInitOK      = Post( Content, sign, L"F", Reason, true );
				FCheckResult = FInitOK;
				FSync        = !FInitOK;
				if( FInitOK == true && FIsRunning == false )
					Start();
				return FInitOK;
			}
		}
	}
	else  ///< Skip CA check
	{
		FCheckResult = true;
		FInitOK      = true;
		if( FIsRunning == false )
			Start();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void TCAChecker::EscapeDataString( UTF8String& EscStr )
{
	UTF8String HexEsc;
	char CheckCh;

	for( register int i = EscStr.Length(); i >= 1; i -- )
	{
		CheckCh = EscStr[i];
		switch( CheckCh )
		{
			case '\n':
			case '\t':
			case ' ':
			case '+':
			case '@':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '=':
			case '[':
			case ']':
			case '{':
			case '}':
			case '?':
			case '/':
			case ':':
			case '<':
			case '>':
			case '|': HexEsc.printf( "%%%02.2X", CheckCh );
					  EscStr.Delete( i, 1 );
					  EscStr.Insert( HexEsc, i );
					  break;
			default: break;
		}
	}
}
//---------------------------------------------------------------------------
int TCAChecker::Sign( const String& Content, String& Sign )
{
	AnsiString AnsiContent( Content );

	if( FCAObject != NULL )
	{
		AnsiString AnsiData( Content );
		UFC::PLockObject Lock( FSignCS );

		AnsiString AnsiPFXFile( FPFXFile );
		AnsiString AnsiSubject( FSignSubject );
		UFC::BufferedLog::Printf( " [CA][Sign] Enter. Content[%s] PFXFile[%s] SubjectLen[%d]",
								   AnsiContent.c_str(), AnsiPFXFile.c_str(), FSignSubject.Length() );

		UTF8String Signed = FCAObject->CGCAPIStockPFXPureSign(FPFXFile.c_str(),
															 FPFXFilePassword.c_str(),
															 Content.c_str(),
															 FSignSubject.c_str(),
															 0x2,
															 0x10004, ///< USE CG_ALGOR_SHA256
															 0x80);

		int ErrCode = FCAObject->GetErrorCode();

		EscapeDataString( Signed );
		//UFC::BufferedLog::Printf( " CA data[%s] Sign[%s]", AnsiData.c_str(),  Signed.c_str( ));
		Sign = Signed;
		UFC::BufferedLog::Printf( " [CA][Sign] Done. ErrCode[%d] SignedLen[%d] Signed[%s]",
								   ErrCode, Signed.Length(), Signed.c_str() );
		UFC::BufferedLog::FlushToFile();
		return ErrCode;
	}
	UFC::BufferedLog::Printf( " [CA][Sign] FCAObject is NULL, return 0 without signing. Content[%s]",
							   AnsiContent.c_str() );
	UFC::BufferedLog::FlushToFile();
	return 0;
}
//---------------------------------------------------------------------------
bool TCAChecker::HandleResultDoc( TMemoryStream* Result, bool IsTest, String& Reason )
{
	UTF8String ResponseStr( (char*)Result->Memory, Result->Size );
	UTF8String ReturnCode;
	bool       Rtn = false;
	int        DPos = ResponseStr.AnsiPos("|");

	if( DPos > 0 )
	{
		ReturnCode = ResponseStr.SubString( 1, DPos - 1 );
		Reason     = ResponseStr.SubString( DPos + 1 , ResponseStr.Length() - DPos  );
		if( ReturnCode.ToInt() == 0 )
			Rtn = true;
	}
	else
		Reason = L"錯誤的傳回值";

	AnsiString AnsiResponse( ResponseStr );
	UFC::BufferedLog::Printf( " CA Response[%s]", AnsiResponse.c_str( ));

	return Rtn;
}
//---------------------------------------------------------------------------
void TCAChecker::EmptyQueueAndTriggerError( String& ErrReason )
{
	if( FOnCACheckFail != NULL )
		FOnCACheckFail( this, ErrReason );
}
//---------------------------------------------------------------------------
bool TCAChecker::Post( const String& Content, const String& sign, const String& BizCode, String& Reason, bool IsTest )
{
	AnsiString AnsiContent( Content );
	AnsiString AnsiBizCode( BizCode );

	UFC::BufferedLog::Printf( " [CA][Post] Enter. Content[%s] BizCode[%s] IsTest[%d] CASerial[%s] LocalIP[%s] FID[%s]",
							   AnsiContent.c_str(), AnsiBizCode.c_str(), IsTest,
							   AnsiString(FCASerial).c_str(), AnsiString(FLocalIP).c_str(), AnsiString(FID).c_str() );
	try
	{
		TMemoryStream*  Source = new TMemoryStream();
		TMemoryStream*  Result = new TMemoryStream();
		String          URL;
		bool            Rtn;

		URL.printf( L"%s?srccode=SP&bizcode=%s&loginip=%s&pid=%s&loginacc=mega&loginpass=123123&data=%s&sn=%s&p1sign=%s&operation=p1verifysign",
					  FURL, BizCode, FLocalIP, FID, Content, FCASerial, sign );
		FHTTP->Request->Method       = L"POST";
		FHTTP->Request->ContentType  = L"application/x-www-form-urlencoded";
		FHTTP->ConnectTimeout        = 90000; ///< 90 sec IIS default(Paul said)

		AnsiString AnsiURL( URL );
		UFC::BufferedLog::Printf( " CA Request[%s]", AnsiURL.c_str( ));

		FHTTP->Post( URL, Source, Result );
		UFC::BufferedLog::Printf( " [CA][Post] HTTP ResponseCode[%d] ResultSize[%d]",
								   FHTTP->ResponseCode, Result->Size );
		Rtn = HandleResultDoc( Result, IsTest, Reason );
		AnsiString AnsiReason( Reason );
		UFC::BufferedLog::Printf( " [CA][Post] Done. Rtn[%d] Reason[%s]", Rtn, AnsiReason.c_str() );
		UFC::BufferedLog::FlushToFile();
		FLastError = Reason;
		delete Result;
		delete Source;
		return Rtn;
	}
	catch( Exception& ex )
	{
		AnsiString AnsiExMsg( ex.Message );
		UFC::BufferedLog::Printf( " [CA][Post] Exception[%s]", AnsiExMsg.c_str() );
		Reason  = ex.Message;
		FLastError  = Reason;
		EmptyQueueAndTriggerError( Reason );
		return false;
	}
	catch( ... )
	{
		Reason = L"不明的HTTP錯誤!";
		FLastError = Reason;
		EmptyQueueAndTriggerError( Reason );// '不明的HTTP錯誤!'
		return false;
	}
}
//---------------------------------------------------------------------------
void TCAChecker::ClearAndLog( void )
{
	while( FQueue.ItemCount() > 0 )
	{
		CheckData* Data = FQueue.Dequeue();
		String Content = Data->GetContent();

		if( Data != NULL )
			delete Data;
	}
}
//---------------------------------------------------------------------------
void TCAChecker::Execute( void )
{
	CheckData* Data;

	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	FIsRunning = true;
	while( !IsTerminated() )
	{
		try
		{
			if( FQueue.ItemCount() > 0 )
			{
				if( (Data = FQueue.Dequeue()) != NULL )
				{
					Work( Data );
					delete Data;
				}
			}
			else
				UFC::SleepMS( 10 );
		}
		catch( Exception& ex )
		{
			FSync = false;
			EmptyQueueAndTriggerError( ex.Message );
		}
	}
	FIsRunning = false;
	CoUninitialize( );
}
//---------------------------------------------------------------------------
void TCAChecker::Work( CheckData* Data )
{
	String sign, Reason,BizCode;
	String Content = Data->GetContent();

	switch( Data->GetType() )
	{
		case ctTWSE:
		case ctOTC:           BizCode =	L"S";break;
		case ctFutures:
		case ctOptions:
		case ctInternational: BizCode =	L"F";break;
	}
	if( Sign( Content, sign ) != 0 )
		FCheckResult = false;
	else
		Post( Content, sign, BizCode, Reason );
}
//---------------------------------------------------------------------------
bool TCAChecker::SignAgrement( const String& Content, String& Reason )
{
	if( FInitOK == false )
	{
		Reason = L"憑證讀取失敗";
		UFC::BufferedLog::Printf( " [CA] Load CA Failed." );
		return false;
	}
	String sign;
	int    ErrorCode = Sign( Content, sign );

	if( ErrorCode ==0 )
	{
		UFC::BufferedLog::Printf( " [CA] Sync Signed OK!" );
		if( Post( Content, sign, L"F",Reason ) == true )
			return true;
	}
	else
	{
		AnsiString AnsiReason( ToErrorMessage( ErrorCode ) );

		Reason = ToErrorMessage( ErrorCode );
		UFC::BufferedLog::Printf( " [CA] Signed failed, Reason[%s]", AnsiReason.c_str() );
	}
	return false;
}
//---------------------------------------------------------------------------
bool TCAChecker::Check( ComType Type, const String& Content, String& Reason )
{
	if( gCheckCA == false ) ///< Skip CA check.
	{
		Reason = L"OK!";
		UFC::BufferedLog::Printf( " [CA] Bypass!" );
		return true;
	}
	if( FInitOK == false )
	{
		Reason = L"憑證讀取失敗";
		UFC::BufferedLog::Printf( " [CA] Load CA Failed." );
		return false;
	}
	if( FSync == false )
	{
		FQueue.Inqueue( new CheckData( Type, Content ) );
		UFC::BufferedLog::Printf( " [CA] Data in queue OK!" );
		return true;
	}
	else
	{
		 String sign,BizCode;
		 int ErrorCode = Sign( Content, sign );

		 if( ErrorCode ==0 )
		 {
			switch( Type )
			{
				case ctTWSE:
				case ctOTC:           BizCode =	L"S";break;
				case ctFutures:
				case ctOptions:
				case ctInternational: BizCode =	L"F";break;
			}
			UFC::BufferedLog::Printf( " [CA] Sync Signed OK!" );
			 if( Post( Content, sign, BizCode, Reason ) == true )
			 {
				FSync = false;
				return true;
			 }
			 else
				return false;
		 }
		 else
		 {
			AnsiString AnsiReason( ToErrorMessage( ErrorCode ) );

			Reason = ToErrorMessage( ErrorCode );
			UFC::BufferedLog::Printf( " [CA] Signed failed, Reason[%s]", AnsiReason.c_str() );
			return false;
		 }
	}
}
//---------------------------------------------------------------------------
void TCAChecker::OnCACheck( nsOrderMessageDefine::MarketEnum Market,
							SendMessageType SType,
							TBaseMessage* Msg,
							bool& CanSend,
							UFC::AnsiString& RejectMsg )
{
	if( GSimMatch == true )
	{
		RejectMsg = "OK!";
		CanSend = true;
		return;
	}
	if( SType == smtNew || SType == smtCancel || SType == smtReplace )
	{
		ComType CType;
		String Content,Reason;
		UFC::AnsiString Text, TIF, PE, ExMarket,ExecType;

		switch( SType )
		{
			case smtNew:     ExecType = "New";break;
			case smtCancel:  ExecType = "Cxl";break;
			case smtReplace: ExecType = "Rpx";break;
		}
		switch( Msg->GetTimeInForce() )
		{
			case nsOrderMessageDefine::tifIOC: TIF = "IOC";break;
			case nsOrderMessageDefine::tifFOK: TIF = "FOK";break;
			default:  TIF="ROD";break;
		}
		switch(  Msg->GetPositionEffect() )
		{
			case nsOrderMessageDefine::peClose:        PE ="C";break;
			case nsOrderMessageDefine::peDayTrade:
			case nsOrderMessageDefine::peDayTradeOpen: PE = "D";break;
			case nsOrderMessageDefine::peAuto:         PE = "a";break;
			case nsOrderMessageDefine::peTAIFEXAuto:   PE = "A";break;
			default: PE="O";break;
		}

		if( Market == nsOrderMessageDefine::mTWFutures )
		{
			CType = ctFutures;
			ExMarket = "FUT";
		}
		else if( Market == nsOrderMessageDefine::mTWOptions )
		{
			CType = ctOptions;
			ExMarket = "OPT";
		}
		else if( Market == nsOrderMessageDefine::mTSE )
		{
			CType = ctTWSE;
			ExMarket = "TSE";
		}
		else if( Market == nsOrderMessageDefine::mOTC )
		{
			CType = ctOTC;
			ExMarket = "OTC";
		}
		else if( Market == nsOrderMessageDefine::mForeignFutures || Market == nsOrderMessageDefine::mForeignOptions )
		{
			CType = ctInternational;
			ExMarket = "ForeignFUT";
		}
		if( SType == smtNew )
			Text.Printf( "SpeedyUnify_Mega_%s_%s_%s_%s_%s_%d_%0.2f_%s_%s_%s_%s",
					  ExMarket.c_str(),
					  ExecType.c_str(),
					  Msg->GetBrokerID(),
					  Msg->GetAccount(),
					  Msg->GetSymbol(),
					  Msg->GetOrderQty(),
					  Msg->GetPrice(),
					  (Msg->GetSide() == nsOrderMessageDefine::sBuy)?"B":"S",
					  TIF.c_str(), PE.c_str(), FAnsiLocalIP.c_str() );
		else
			Text.Printf( "SpeedyUnify_Mega_%s_%s_%s_%s_%s_%s_%d_%0.2f_%s_%s_%s_%s",
					  ExMarket.c_str(),
					  ExecType.c_str(),
                      Msg->GetOrderID(),
					  Msg->GetBrokerID(),
					  Msg->GetAccount(),
					  Msg->GetSymbol(),
					  Msg->GetOrderQty(),
					  Msg->GetPrice(),
					  (Msg->GetSide() == nsOrderMessageDefine::sBuy)?"B":"S",
					  TIF.c_str(), PE.c_str(), FAnsiLocalIP.c_str() );

		Content = Text.c_str();
		///< Send to CA queue
		if( Check( CType, Content, Reason ) )
		{
			RejectMsg = "OK!";
			CanSend = true;
		}
		else
		{
			AnsiString AnsiRejectMsg = Reason;

			RejectMsg = AnsiRejectMsg.c_str();
			CanSend = false;
		}
	}
	else ///< bypass CA
	{
		RejectMsg = "OK!";
		CanSend = true;
	}
}
//---------------------------------------------------------------------------
const String& TCAChecker::ToErrorMessage( int code )
{
	switch( code )
	{
		case 0   : FErrMsg = L"成功";break;
		case 5001: FErrMsg = L"一般錯誤";break;
		case 5002: FErrMsg = L"配置記憶體發生錯誤";break;
		case 5003: FErrMsg = L"空間不足";break;
		case 5004: FErrMsg = L"未支援功能";break;
		case 5005: FErrMsg = L"錯誤的參數";break;
		case 5006: FErrMsg = L"錯誤的handle";break;
		case 5007: FErrMsg = L"試用版期限已過";break;
		case 5008: FErrMsg = L"編碼錯誤";break;
		case 5010: FErrMsg = L"無法在MS CryptoAPIDatabase 中找到指定憑證";break;
		case 5011: FErrMsg = L"憑證已過期";break;
		case 5012: FErrMsg = L"憑證尚未合法無法使用";break;
		case 5013: FErrMsg = L"憑證可能過期或無法使用法使用";break;
		case 5014: FErrMsg = L"憑證主旨錯誤";break;
		case 5015: FErrMsg = L"無法找到憑證發行者者";break;
		case 5016: FErrMsg = L"不合法的憑證簽章";break;
		case 5017: FErrMsg = L"憑證用途(加解密,簽驗章)不合適";break;
		case 5020: FErrMsg = L"憑證已撤銷";break;
		case 5021: FErrMsg = L"憑證已撤銷(金鑰洩露)";break;
		case 5022: FErrMsg = L"憑證已撤銷(CA compromised)";break;
		case 5023: FErrMsg = L"憑證已撤銷(聯盟已變更)";break;
		case 5024: FErrMsg = L"憑證已撤銷(已取代)";break;
		case 5025: FErrMsg = L"憑證已撤銷(已停止)";break;
		case 5026: FErrMsg = L"憑證保留或暫禁";break;
		case 5028: FErrMsg = L"CG_RTN_CERT_REMOVEFROMCRL";break;
		case 5030: FErrMsg = L"CRL已過期已過期";break;
		case 5031: FErrMsg = L"不合法的CRL";break;
		case 5032: FErrMsg = L"無法找到法找到CRL";break;
		case 5034: FErrMsg = L"CRL簽章值不合法";break;
		case 5035: FErrMsg = L"Digest錯誤";break;
		case 5036: FErrMsg = L"不合法的簽章";break;
		case 5037: FErrMsg = L"內容錯誤";break;
		case 5038: FErrMsg = L"錯誤的驗證碼";break;
		case 5040: FErrMsg = L"憑證格式錯誤";break;
		case 5041: FErrMsg = L"CRL格式錯誤";break;
		case 5042: FErrMsg = L"錯誤的PKCS7格式";break;
		case 5043: FErrMsg = L"Key的格式錯誤";break;
		case 5044: FErrMsg = L"不合法的PKCS10格式";break;
		case 5045: FErrMsg = L"不合適的格式";break;
		case 5046: FErrMsg = L"不合法的PKCS12格式";break;
		case 5050: FErrMsg = L"找不到物件";break;
		case 5051: FErrMsg = L"PKCS7不存在原文";break;
		case 5052: FErrMsg = L"PKCS7不存在憑證";break;
		case 5053: FErrMsg = L"PKCS7不存在SINGERINFO";break;
		case 5060: FErrMsg = L"錯誤的憑證或金鑰";break;
		case 5061: FErrMsg = L"簽章失敗";break;
		case 5062: FErrMsg = L"驗章失敗";break;
		case 5063: FErrMsg = L"加密失敗";break;
		case 5064: FErrMsg = L"解密失敗";break;
		case 5065: FErrMsg = L"產生金鑰失敗";break;
		case 5066: FErrMsg = L"刪除使用者憑證錯誤";break;
		case 5067: FErrMsg = L"錯誤的演算法";break;
		case 5070: FErrMsg = L"操作已關閉";break;
		case 5071: FErrMsg = L"密碼錯誤";break;
		case 5072: FErrMsg = L"卡片鎖卡";break;
		case 5073: FErrMsg = L"拔插逾時";break;
		case 5074: FErrMsg = L"裝置已停用";break;
		case 5080: FErrMsg = L"無法剖析XML文件";break;
		case 5081: FErrMsg = L"無法在XML中,找到指定的標籤名稱";break;
		case 5201: FErrMsg = L"Store開啟失敗開啟失敗";break;
		case 5202: FErrMsg = L"憑證練建立失敗";break;
		case 5203: FErrMsg = L"CSP初始化錯誤";break;
		case 5204: FErrMsg = L"密鑰不存在";break;
		case 5205: FErrMsg = L"無法匯出";break;
		case 5206: FErrMsg = L"無法存取此store";break;
		case 5901: FErrMsg = L"轉型失敗";break;
		case 5902: FErrMsg = L"找不到檔案";break;
		case 5903: FErrMsg = L"不存在的路徑";break;
		case 5904: FErrMsg = L"不存在的網路路徑";break;
		case 5905: FErrMsg = L"登入失敗";break;
		case 5906: FErrMsg = L"不符權限";break;
		case 5909: FErrMsg = L"錯誤的密碼";break;
		default: FErrMsg = L"讀取失敗,請確認CA憑證已正確安裝,並未過期.";break;
	}
	return FErrMsg;
}
//---------------------------------------------------------------------------



