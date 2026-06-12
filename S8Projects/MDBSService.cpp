//---------------------------------------------------------------------------
#include "FMTConfig.h"
#include "MD5.h"
#include "main.h"
#include "UnifyUtility.h"
#include "UnifyDlg.h"
#include <System.NetEncoding.hpp>
#include <System.Win.Registry.hpp>
#include <System.IOUtils.hpp>
//---------------------------------------------------------------------------
extern bool        GIsTestingMode;
extern bool        GSimMatch;
extern bool        gNuclear;
extern TUnifyUser  gUser;
extern bool        gTFT;
extern bool        GVIPServer;
//---------------------------------------------------------------------------
const String& Config::GetRegisterURL( void )
{
	return m_RegisterURL;
}
//---------------------------------------------------------------------------
const String& Config::GetForgetPasswordURL( void )
{
	return m_ForgetPassword;
}
//---------------------------------------------------------------------------
const String& Config::GetLogonURL( void )
{
	return m_LogonURL;
}
//---------------------------------------------------------------------------
const String& Config::GetReportURL( void )
{
	return m_ReportURL;
}
//---------------------------------------------------------------------------
const String& Config::GetMegaCAURL( void )
{
	return m_MegaCAURL;
}
//---------------------------------------------------------------------------
const String& Config::GetMegaBackendURL( void )
{
	return m_MegaBackendURL;
}
//---------------------------------------------------------------------------
const String& Config::GetEditURL( void )
{
	return m_EditURL;
}
//---------------------------------------------------------------------------
const String& Config::GetEditURL( const String& ID, const String& token )
{
	m_EditURLWithParams.printf( L"%s?mobile=%s&login_token=%s", m_EditURL, ID,token );
	return m_EditURLWithParams;
}
//---------------------------------------------------------------------------
const String& Config::GePurchaseListURL( const String& ID, const String& token )
{
	AnsiString CheckSumData( ID + token  );
	UFC::MD5   ChkSum( (const UFC::UInt8*)CheckSumData.c_str(), CheckSumData.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );
	String     json;

	json.printf( L"{\"account\":\"%s\",\"token\":\"%s\",\"checksum\":\"%s\"}", ID, token,CheckSum );
	m_PurchaseListURLWithParams.printf( L"%s?data=%s&txid=IUN_O_19&vender=UnifyAP", m_PurchaseListURL, json);
	return m_PurchaseListURLWithParams;
}
//---------------------------------------------------------------------------
const String& Config::GetXFlashURL( const String& ID, const String& token )
{
	AnsiString CheckSumData( ID + token + L"1" );
	UFC::MD5   ChkSum( (const UFC::UInt8*)CheckSumData.c_str(), CheckSumData.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );
	String     json;

	json.printf( L"{\"account\":\"%s\",\"token\":\"%s\",\"cp\":\"1\",\"checksum\":\"%s\"}", ID,token,CheckSum );

	m_XFlashURLWithParams.printf( L"%s?data=%s&txid=IUN_O_09&vender=UnifyAP", m_XFlashURL, json);//Base64Encode( json ));
	return m_XFlashURLWithParams;
}
//---------------------------------------------------------------------------
bool Config::HTTPDownload( const String& URL, const String& LocalFile )
{
	TFileStream* SaveFile = NULL;
	try
	{
		SaveFile = new TFileStream( LocalFile, fmCreate );
		m_pHTTP->Get( URL, SaveFile );
		delete SaveFile;
		return true;
	}
	catch(...)
	{
		if( SaveFile )
			delete SaveFile;
		return false;
	}
}
//---------------------------------------------------------------------------
bool Config::GetResponseJSON( TMemoryStream* Stream, String& ResponseJSON )
{
	UTF8String  JsonTxt( (char*)Stream->Memory, Stream->Size );
	TJSONValue *lpJson = TJSONObject::ParseJSONValue( JsonTxt );
	TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
	if( lpRoot != NULL )
	{
		ResponseJSON = lpRoot->Values[ "response_data" ]->Value();
		delete lpJson;
		return true;
	}
	delete lpJson;
	return false;
}
//---------------------------------------------------------------------------
void Config::GenData( String& Out )
{
	AnsiString CheckSumData( gUser.UserID + gUser.Token );
	UFC::MD5   ChkSum( (const UFC::UInt8*)CheckSumData.c_str(), CheckSumData.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );

	Out.printf( L"{\"account\":\"%s\",\"token\":\"%s\",\"checksum\":\"%s\"}",gUser.UserID,gUser.Token,CheckSum );
}
//---------------------------------------------------------------------------
void Config::GenData(  String& Out, const String& Param )
{
	AnsiString CheckSumData( gUser.UserID + gUser.Token + Param );
	UFC::MD5   ChkSum( (const UFC::UInt8*)CheckSumData.c_str(), CheckSumData.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );

	Out.printf( L"{\"account\":\"%s\",\"token\":\"%s\",\"parm\":\"%s\",\"checksum\":\"%s\"}",gUser.UserID,gUser.Token,Param,CheckSum );
}
//---------------------------------------------------------------------------
void Config::GenData(  String& Out, const String& Param, const String& Value )
{
	AnsiString CheckSumData( gUser.UserID + gUser.Token + Param + Value );
	UFC::MD5   ChkSum( (const UFC::UInt8*)CheckSumData.c_str(), CheckSumData.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );

	Out.printf( L"{\"account\":\"%s\",\"token\":\"%s\",\"parm\":\"%s\",\"value\":\"%s\",\"checksum\":\"%s\"}",gUser.UserID,gUser.Token,Param,Value,CheckSum );
}
//---------------------------------------------------------------------------
void Config::GenUploadData( const String& Items, String& Out )
{
	UFC::AnsiString Today;
	AnsiString      Account( gUser.LoginUserID );
	AnsiString      Type = GSimMatch? "0":"1";
	AnsiString      CheckSumData( gUser.UserID + gUser.Token + Account + Type );
	UFC::MD5        ChkSum( (const UFC::UInt8*)CheckSumData.c_str(), CheckSumData.Length() );
	String          CheckSum( ChkSum.ToString().c_str() );
	String          FileName;

	Out.printf( L"data={\"account\":\"%s\",\"token\":\"%s\",\"txAccount\":\"%s\",\"txType\":\"%s\",\"productList\":%s,\"checksum\":\"%s\"}",
				gUser.UserID,gUser.Token,gUser.LoginUserID,String(Type),Items,CheckSum );

	TStringList* strings = new TStringList();
	strings->Add( Out );
	UFC::GetYYYYMMDD( Today );
	FileName.printf( L"%slog\\FilledList_%s_%s.txt", g_Config.GetCurrentDir(), gUser.UserID,String(Today.c_str()) );
	strings->SaveToFile( FileName );
	delete strings;
}
//---------------------------------------------------------------------------
bool Config::MDBSRequest( TMemoryStream* OutStream, const String& Func, const String& Data )
{
	TIdSSLIOHandlerSocketOpenSSL* SSLIOHandler = new TIdSSLIOHandlerSocketOpenSSL( NULL );
	TIdHTTP*                      pHTTP        = new TIdHTTP( NULL );
	TMemoryStream*                SourceStream = new TMemoryStream();
	bool                          Result = true;

	try
	{
		String URLPrefix = GetLogonURL();
		String URL;

		URL.printf( L"%s?data=%s&txid=%s&vender=UnifyAP",URLPrefix, Data, Func );
		if( URL.Pos( L"https" ) != 0 ) ///< Use https
		{
			SSLIOHandler->SSLOptions->Method = sslvSSLv23;
			pHTTP->IOHandler = SSLIOHandler;
		}
		pHTTP->ConnectTimeout = 90000; ///< 90 sec IIS default(Paul said)
		pHTTP->Post( pHTTP->URL->URLEncode( URL ), SourceStream, OutStream );
	}
	catch( Exception& ex )
	{
		Result = false;
	}
	delete SSLIOHandler;
	delete pHTTP;
	delete SourceStream;;
	return Result;
}
//---------------------------------------------------------------------------
bool Config::MDBSUploadRequest( TMemoryStream* OutStream, const String& FilledItems )
{
	TIdSSLIOHandlerSocketOpenSSL* SSLIOHandler = new TIdSSLIOHandlerSocketOpenSSL( NULL );
	TIdHTTP*                      pHTTP        = new TIdHTTP( NULL );
	TMemoryStream*                SourceStream = new TMemoryStream();
	TStringStream*                StreamParams;
	bool                          Result = true;

	try
	{
		String URL = GetLogonURL();
		String UploadData,StringParams;

		GenUploadData( FilledItems, UploadData );
		StringParams.printf( L"%s&txid=IUN_O_06&vender=UnifyAP",UploadData );
		StreamParams = new TStringStream( StringParams, TEncoding::UTF8, true );
		if( URL.Pos( L"https" ) != 0 ) ///< Use https
		{
			SSLIOHandler->SSLOptions->Method = sslvSSLv23;
			pHTTP->IOHandler = SSLIOHandler;
		}
		pHTTP->Request->ContentType = L"application/x-www-form-urlencoded";
		pHTTP->Request->CharSet = L"utf-8";
		pHTTP->ConnectTimeout = 90000; ///< 90 sec IIS default(Paul said)
		pHTTP->Post( pHTTP->URL->URLEncode( URL ), StreamParams, OutStream );
	}
	catch( Exception& ex )
	{
		Result = false;
		TUnifyDlgs::MessageDialog( "Upload failed", ex.Message );
	}
	delete StreamParams;
	delete SSLIOHandler;
	delete pHTTP;
	delete SourceStream;;
	return Result;
}
//---------------------------------------------------------------------------
bool Config::GetServerValue( const String& Name, String& Value, const String& Def )
{
	TMemoryStream* ResultStream = new TMemoryStream();
	String         ResponseJSON,Data;
	bool           IsOk = false;

	Value = Def;
	try
	{
		GenData( Data, Name );
		if( MDBSRequest( ResultStream, L"IUN_O_03", Data ) &&
			GetResponseJSON( ResultStream, ResponseJSON ) )
		{
			TJSONValue *lpJson = TJSONObject::ParseJSONValue( ResponseJSON );
			TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
			if( lpRoot != NULL )
			{
				String ResultStr  = lpRoot->Values[L"result"]->Value();
				if( ResultStr == L"0" )
				{
					Value = lpRoot->Values[L"value"]->Value();
					IsOk = true;
				}
				else
					IsOk = false;
			}
		}
	}
	catch( ... )
	{
		IsOk = false;
	}
	delete ResultStream;
	return IsOk;
}
//---------------------------------------------------------------------------
bool Config::SetServerValue( const String& Name, const String& Value )
{
	TMemoryStream* ResultStream = new TMemoryStream();
	String         ResponseJSON,Data;
	bool           IsOk = false;

	try
	{
		GenData( Data, Name, Value );
		if( MDBSRequest( ResultStream, L"IUN_O_04", Data ) &&
			GetResponseJSON( ResultStream, ResponseJSON ) )
		{
			TJSONValue* lpJson = TJSONObject::ParseJSONValue( ResponseJSON );
			TJSONObject* lpRoot = dynamic_cast<TJSONObject *>(lpJson);
			if( lpRoot != NULL )
			{
				String ResultStr  = lpRoot->Values[L"result"]->Value();
				if( ResultStr == L"0" )
					IsOk = true;
			}
		}
	}
	catch( ... ){}
	delete ResultStream;
	return IsOk;
}
//---------------------------------------------------------------------------
bool Config::TokenExpired( void )
{
	TMemoryStream* ResultStream = new TMemoryStream();
	String ResponseJSON,Data;
	bool IsOk = true;

	try
	{
		GenData( Data );
		if( MDBSRequest( ResultStream, L"IUN_O_05", Data ) )
		{
			if( GetResponseJSON( ResultStream, ResponseJSON ) )
			{
				TJSONValue *lpJson = TJSONObject::ParseJSONValue( ResponseJSON );
				TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
				if( lpRoot != NULL )
				{
					String ResultStr  = lpRoot->Values[L"result"]->Value();
					String MessageStr = lpRoot->Values[L"message"]->Value();
					if( ResultStr == L"0" )
						IsOk = true;
					else
						IsOk = false;
				}
			}
		}
	}
	catch( ... )
	{
		IsOk = true;
	}
	delete ResultStream;
	return IsOk;
}
//---------------------------------------------------------------------------
bool Config::UploadFilled( const String& Items, int FC )
{
	TMemoryStream* ResultStream = new TMemoryStream();
	if( MDBSUploadRequest( ResultStream, Items ) )
	{
		bool IsOk = false;
		String ResponseJSON,ErrMsg;

		if( GetResponseJSON( ResultStream, ResponseJSON ) )
		{
			TJSONValue *lpJson = TJSONObject::ParseJSONValue( ResponseJSON );
			TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
			if( lpRoot != NULL )
			{
				String ResultStr  = lpRoot->Values[L"result"]->Value();
				String MessageStr = lpRoot->Values[L"message"]->Value();
				if( ResultStr == L"0" )
				{
					ErrMsg.printf( L"成功上傳%d筆成交資料!", FC );
					TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, ErrMsg );
					IsOk = true;
				}
				else
				{
					ErrMsg.printf( L"上傳%d筆成交資料失敗:%s", FC, MessageStr );
					TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, ErrMsg );
				}

			}
		}
		delete ResultStream;
		return IsOk;
	}
	delete ResultStream;
	return true;
}
//---------------------------------------------------------------------------

