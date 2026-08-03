#ifndef __CONFIG_H__
#define __CONFIG_H__
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include "IdSSL.hpp"
#include "IdSSLOpenSSL.hpp"
#include "IdExplicitTLSClientServerBase.hpp"
#include <System.JSON.hpp>
#include <list>
#include "AnsiString.h"
#include "PHashedSet.h"
#include "PHashedList.h"
#include "iniFile.h"
#include "RoundFormEx.h"
#include "OrderMessageDefine.h"
#include "TTaifexConnection.h"
//---------------------------------------------------------------------------
#include "TradingInvestors.h"
const UFC::AnsiString FUNC_PINGPONG( L"1001" );
const UFC::AnsiString FUNC_MULTI_ACCOUNT( L"1002" );
const UFC::AnsiString FUNC_ENABLE_S8_API( L"1003" );
const UFC::AnsiString FUNC_OVER_SEA_FUT( L"1004" );
class TOrderStore;
class TCAChecker;
//---------------------------------------------------------------------------
const int HOT_KEY_COUNT = 56;
class TCAChecker;
//---------------------------------------------------------------------------
const WORD HotkeyCode[ HOT_KEY_COUNT ] =
{
	0,
	VK_UP,//             0x26
	VK_DOWN,//           0x28
	VK_LEFT,//           0x25
	VK_RIGHT,//          0x27
	VK_RETURN,//         0x0D
	VK_SPACE,//          0x20
	VK_HOME,//           0x24
	VK_END,//            0x23
	VK_PRIOR,//          0x21
	VK_NEXT,//           0x22
	VK_INSERT,//         0x2D
	VK_DELETE,//         0x2E
	VK_ADD,//            0x6B
	VK_SUBTRACT,//       0x6D
	VK_MULTIPLY,//       0x6A
	VK_DIVIDE,//         0x6F
	VK_F1,//             0x70
	VK_F2,//             0x71
	VK_F3,//             0x72
	VK_F4,//             0x73
	VK_F5,//             0x74
	VK_F6,//             0x75
	VK_F7,//             0x76
	VK_F8,//             0x77
	VK_F9,//             0x78
	VK_F10,//            0x79
	VK_F11,//            0x7A
	VK_F12,//            0x7B
	0x41, // 'A'
	0x42,
	0x43,
	0x44,
	0x45,
	0x46,
	0x47,
	0x48,
	0x49,
	0x4A,
	0x4B,
	0x4C,
	0x4D,
	0x4E,
	0x4F,
	0x50,
	0x51,
	0x52,
	0x53,
	0x54,
	0x55,
	0x56,
	0x57,
	0x58,
	0x59,
	0x5A, // 'Z'
	VK_RBUTTON //(0x02) Right mouse button
};
//---------------------------------------------------------------------------
class TNetPos
{
public:
	String Exchange;
	String Symbol;
	int    Position;
	double AvgPx;
};
//---------------------------------------------------------------------------
class TAccountInfo
{
public:
	String Account;
	String BrokerID;
	String BranchID;
	String DayTrade;
public:
	TAccountInfo( const String& account, const String& broker,const String& branch, const String& type )
	:Account( account ),
	 BrokerID( broker ),
	 BranchID( branch ),
	 DayTrade( type)
	 {}
};
//---------------------------------------------------------------------------
class TBrokerUser
{
public:
	String FName;
	String FIDNO;
	UFC::List<TAccountInfo*>  FFutAccounts;
	UFC::List<TAccountInfo*>  FStockAccounts;
public:
	void          ClearAccounts( void );
	const String& GetName( void ) { return FName; }
	const String& GetIDNO( void ) { return FIDNO; }
	int           FuturesAccountCount( void ) { return FFutAccounts.ItemCount(); }
	int           StockAccountCount( void )   { return FStockAccounts.ItemCount(); }
	TAccountInfo* FuturesAccount( int i )     { return FFutAccounts[ i ]; }
	TAccountInfo* StockAccount( int i )       { return FStockAccounts[ i ]; }
};
//---------------------------------------------------------------------------
class TBrokerService
{
public:
	virtual bool LoginBroker( const String& ID, const String& Password, String& Msg ) = 0;
	virtual bool ChangePassword( const String& NewPassword, String& Msg ) = 0;
	virtual bool GetPosition( bool IsTAIFEX, const String& Account, String& Msg ) = 0;
	virtual void ClearPosition( const String& ID ) = 0;
	virtual bool SignAgreememt( const String& ID, TCAChecker* CAChecker, String& Msg ) = 0;
	virtual bool SignRiskDisclosureStatement(String& Msg) = 0;
	virtual TBrokerUser* GetAccount( void ) = 0;
};
//---------------------------------------------------------------------------
class TMegaService : public TBrokerService
{
private:
	String  FBaseURL;
	String  FID;
	String  FPassword;
	TBrokerUser FAccounts;
	bool    FLoginOK;
	s888::CTaxRateObject* FStockTaxRate;
private:
    String FBrokerID;
private:
	const String& BrenchToBrokrtID( int Brench );
	bool ToErrorMessage( const String& Code, String& Msg );
	bool GetResponseJSON( TMemoryStream* Stream, String& ResponseJSON );
	void GenData( const String& ID, const String& Password, String& Out );
	void GenData( const String& ID, const String& Password, const String& IB, String& Out );
	void GenData2( const String& ID, const String& Password, const String& IB, String& Out );
	bool RequestLogon( TMemoryStream* OutStream, String& Msg );
	bool RequestPosition(  bool IsTAIFEX, const String& Account, TMemoryStream* OutStream, String& Msg );
	bool RequestRiskDisclosureStatement( const String& Branch_ID, const String& cust_id, TMemoryStream* OutStream, String& Msg );
	void AddPosition( const String& Account, TJSONArray *PosArray );
	void AddStockPosition( const String& Account, TJSONArray *PosArray );
	void AddTestPosition( bool IsTAIFEX );
	void ProcessHoldPositionData( s888::CTradingAccount* AccountPtr );
public:
	TMegaService( const String& BaseURL );
	virtual bool LoginBroker( const String& ID, const String& Password, String& Msg );
	virtual bool ChangePassword( const String& NewPassword, String& Msg ){return false;}
	virtual bool GetPosition( bool IsTAIFEX, const String& Account, String& Msg );
	virtual void ClearPosition( const String& ID );
	virtual bool SignAgreememt( const String& Account, TCAChecker* CAChecker, String& Msg );
	virtual bool SignRiskDisclosureStatement(String& Msg);
	virtual TBrokerUser* GetAccount( void );
};
//---------------------------------------------------------------------------
typedef enum
{
  hatTAIFEX,
  hatTWSE,
  hatBoth

} HasAccountType;
//---------------------------------------------------------------------------
class TUnifyUser
{
public:
	String UserID;
	String MemberID;
	String Mail;
	String Nickname;
	String ImageURL;
	String Token;
	String ClientIP;
	String LoginUserID;
	bool   IsAOEMember;
	HasAccountType AccountType;
};
//---------------------------------------------------------------------------
class TSpeedyConfig
{
public:
	String       Name;
	String       BrokerID;
	String       CMID;
	String       TWSEBrokerID;
	int          Port;
	int          VIPPort;
	bool         IsProxy;
private:
	int              FIndex;
	UFC::PStringList FIPs;
	UFC::PStringList FVIPIPs;
public:
	TSpeedyConfig();
	~TSpeedyConfig();
	void         SetIPS( const String& IPStr );
	void         SetVIPIPS( const String& IPStr );
	String       GetIP( void );
	String       GetVIPIP( void );
};
//---------------------------------------------------------------------------
class TBrokerConfig
{
private:
	String                     FBrokerName;
	String                     FBrokerID;
    String   				   FTWSEBrokerID;
	String                     FCMID;
	UFC::List<TSpeedyConfig*>  FConfigs;
	int                        FIconIndex;
	TBrokerService*            FService;
public:
	TBrokerConfig( UFC::Section* );
	~TBrokerConfig( void );
	int             Count( void )        { return FConfigs.ItemCount(); }
	TSpeedyConfig*  GetConfig( int i );
	const String&   GetName( void )      { return FBrokerName; }
	const String&   GetBrokerID( void )  { return FBrokerID; }
	const String&   GetCMID( void )      { return FCMID; }
	int             GetIconIndex( void ) { return FIconIndex; }
	TBrokerService* GetService( void )   { return FService; }
};
//---------------------------------------------------------------------------
class TUserAccount : public TComponent
{
private:
	String       FName;      ///< Sample: Simon Chang, Real name of this user.
	String       FCitizenID; ///< Sample: A123456789, Citizen ID of this user.
	String       FAE;        ///< Sample: 901, Active Executive of this user.
	String       FUniqueKey; ///< Unique ID for this logon session.
private:
	TOrderStore* FOrderStore;
private:
	String __fastcall GetUserName( void );
	void   __fastcall SetUserName( String );
	String __fastcall GetLogonID( void );
	void   __fastcall SetLogonID( String NewID );
	String __fastcall GetLogonPassword( void );
	void   __fastcall SetLogonPassword( String NewPassword );
	String __fastcall GetBrokerID( void );
	void   __fastcall SetBrokerID( String NewID );
	String __fastcall GetAccount( void );
	void   __fastcall SetAccount( String Account );
public:
	__fastcall TUserAccount( TComponent* Owner );
	__fastcall ~TUserAccount( void );
	///< Trading account information.
	__property String  LogonID         = { read = GetLogonID, write = SetLogonID };            ///< Logon ID
	__property String  Password        = { read = GetLogonPassword, write = SetLogonPassword };///< Logon Password
	__property String  Account         = { read = GetAccount, write = SetAccount };            ///< Default trading account.
	__property String  BrokerID        = { read = GetBrokerID, write = SetBrokerID };          ///< Trading broker ID.
	///< User information.
	__property String  UserName        = { read = GetUserName, write = SetUserName };
	__property String  UserCitizenID   = { read = FCitizenID, write = FCitizenID };
	__property String  UniqueKey       = { read = FUniqueKey, write = FUniqueKey };
	__property String  ActiveExecutive = { read = FAE, write = FAE }; ///< AE for this User.
	///< Objects
	__property TOrderStore* OrderStore = { read = FOrderStore };
//	__property TCAChecker*  CAChecker  = { read = FCAChecker };
};
//---------------------------------------------------------------------------
class Config
{
private:
    struct UpdateItem
    {
		DWORD           version;
		DWORD           filesize;
		UFC::AnsiString filename;
		UFC::AnsiString url;
		bool            autoreg;
		bool            needrestart;
		bool            compress;
	};
	String     m_CurrentDir;
	String     m_SpeedyIP;
	int        m_SpeedyPort;
	String     m_SpeedyOSIP;
	int        m_SpeedyOSPort;
	String	   m_PasswordURL;
	String	   m_UserKey;
	String     m_SetupURL;
	UFC::AnsiString  m_ConfigFile;
	bool       m_AutoUpdate;
	bool	   m_Channel;
	int        m_InternetConfig;
	bool       m_UseBackup;
	TIdHTTP*   m_pHTTP;
	TIdSSLIOHandlerSocketOpenSSL* m_SSLIOHandler;
	bool       m_bHTTPFail;
	bool       m_CancelUpdate;
	bool       m_UpdateMainProgram;
	int        m_UpdateVersion;
	DWORD      m_TotalBytes;
	DWORD      m_ThisFileBytes;
	DWORD      m_nBytesRecvd;
	DWORD      m_Downloaded;
	TStringList*                     m_StarWaveIPs;
	TStringList*                     m_ChartIPs;
	TStringList*                     m_VIPStarWaveIPs;
	TStringList*                     m_VIPChartIPs;
	TStringList*                     m_SimOrderIPs;
	TStringList*                     m_ChNames;
	TStringList*                     m_ChMaxLots;
	UFC::PInt32List                  m_SignalTime;
	UFC::PStringList                 m_Signal;
	UFC::PInt32List                  m_Throughput;
	UFC::PStringList                 m_ThroughputAccount;
	UFC::PStringList                 m_BaseURLs;
	UFC::PHashedSet<UFC::AnsiString> m_FuncSet;
	UFC::PHashedSet<UFC::AnsiString> m_LicenseIPSet;
	UFC::PHashedSet<UFC::AnsiString> m_TFTLicenseIPSet;
	UFC::PHashedSet<UFC::AnsiString> m_VIPSet;
	UFC::PHashMap<UFC::AnsiString,UFC::AnsiString*> m_Params;
	int        m_StarWaveIPIndex;
	int        m_ChartIPsIndex;
	int        m_VIPStarWaveIPIndex;
	int        m_VIPChartIPsIndex;
	int        m_BaseURLIndex;
	int        m_OrderStoreRecoverOverlapSecond;

	std::list<UpdateItem>           m_UpdateItems;
	std::list<UpdateItem>::iterator m_CurrentItemIter;
	UFC::List<TUserAccount*>        m_Users;
	UFC::List<TBrokerConfig*>       m_Brokers;
	UFC::UiniFile* m_ini;
	UFC::AnsiString m_RiskDisclosureContent;
	UFC::AnsiString m_RiskDisclosureUrl;
private:
	void 			GetProcessDir( void );
	DWORD           FileVersion( const char* FileName, const char* FileName2 = "" );
	DWORD 			FileCreateDate( const char* FileName );
	DWORD           FileCreateToday( void );
	void            SplitIPPort( const UFC::AnsiString& Str, UFC::AnsiString& IP, int& Port, int DefPort );
	int             GetUpdateList(  UFC::AnsiString BaseURL  );
	bool 			UpdateItemExists( const UFC::AnsiString& FileName );
	void            AddUpdateItem(const UpdateItem &item);
	void __fastcall Work(TObject *Sender, TWorkMode AWorkMode, __int64 AWorkCount);
	void __fastcall WorkEnd(TObject *Sender, TWorkMode AWorkMode);
	void __fastcall WorkBegin(TObject *Sender, TWorkMode AWorkMode, __int64 AWorkCountMax){}

	void             LoadUpdateSetting( const char* FileName );
	void 			 LoadSpeedySetting( const char* FileName  );
	void 			 LoadOpenCloseTime( const char* FileName );
	String           GetUpdateBaseURL( void );
	void             LoadBasicSetting( const char* FileName );
	void 			 LoadStarWaveSetting( const char* FileName );
	void 			 ClearBrokers( void );
	void 			 LoadParameter( UFC::UiniFile& SettingsFile );
	void 			 LoadSignal( UFC::UiniFile& SettingsFile );
	void             LoadThroughput( UFC::UiniFile& SettingsFile );
	void 			 LoadCHSetting( UFC::UiniFile& SettingsFile, const UFC::AnsiString& Session, const UFC::AnsiString& ValName, const UFC::AnsiString& DefVal, TStringList* mList );
	void             LoadBlackList( UFC::UiniFile& SettingsFile );
public:
	Config();
	~Config();
	void 			 LoadImageResource( void );
	void 			 ReleaseResource( void );
	int              RetriveUpdateList();
	int              GetUpdateVersion() { return m_UpdateVersion; }
	bool             UpdateMainProgram() { return m_UpdateMainProgram; }
	void 			 LoadUpdateDescription( TStrings* Lines );
	bool 			 NeedsForceUpdate( void );
	bool 			 NeedsReInstall( void );
	const String&    SetupURL( void );
	bool             StartUpdate( bool& );
	void             CancelUpdate();
	const String&    GetCurrentDir() const { return m_CurrentDir; }
	void             GetCurrentDir( char *cBuf );
	UFC::AnsiString  GetCurrentUpdateFile();
	int              GetTotalUpdateProgress();
	int              GetCurrentUpdateProgress();
	int              GetNumBytesRecvd() const { return m_nBytesRecvd; }
	int              GetVersion( void );
	void 			 FilterSlowServer(  void  );
	String 			 GetCTPIPs( void );
	String           GetStarWaveIP( int& Port, int DefPort = 34567 );
	String           GetChartServerIP(  int& Port, int DefPort = 34569  );
	String           GetVIPStarWaveIP( int& Port, int DefPort = 34567 );
	String           GetVIPChartServerIP(  int& Port, int DefPort = 34569  );
	String           GetSimOrderServerIP(  int& Port, const String& DefIP, int DefPort = 45678 );
	int              GetMaxLots( void );
	///< For different languages.
	String           GetNewsURL( void );
	String           GetContractListURL( void );
	String           GetDepositListURL( void );
	String           GetSupportMail( void );
	///< For different languages.
	const String&	 GetLogonURL( void );
	const String&	 GetPasswordURL() const { return m_PasswordURL; }
	const bool		 GetChannel()     const { return m_Channel; }
	const bool       IsBackup()       const { return m_UseBackup; }
	const bool       AutoUpdate()     const { return m_AutoUpdate; }
	String           RiskDisclosureContent(){ return m_RiskDisclosureContent.c_str(); }
	String           RiskDisclosureUrl()    { return m_RiskDisclosureUrl.c_str(); }

	int GetParamInt( const UFC::AnsiString& Name );
	double GetParamDouble( const UFC::AnsiString& Name );
public:
	const String&	 GetUserKey( int index );
	void			 SetUserKey( int index, const String& Key );
	String	         GetLogonID( int index );
	void			 SetLogonID( int index, const String& Account );
	String      	 GetAccount( int index );
	void			 SetAccount( int index, const String& Account );
	String      	 GetPassword( int index );
	void			 SetPassword( int index, const String& Password );
	String       	 GetBrokerID( int index );
	void			 SetBrokerID( int index, const String& Company );
	String       	 GetCitizenID( int index );
	void			 SetCitizenID( int index, const String& Company );
	String	         GetActiveExecutive( int index );
	void			 SetActiveExecutive( int index, const String& Company );
	String	         GetIB( int index );
	void 			 SetIB( int index, const String& IB );
	const String   	 GetName( int index );
	void			 SetName( int index, const String& Name );
public:
	TOrderStore*     GetOrderStore( int index );
	TOrderStore*     MainOrderStore( void ) { return GetOrderStore( 0 ); }
	int 			 GetAccountIndex( const String& Account );
	int              GetOrderStoreIndex( TObject* OrderStore );
	int              OrderStoreCount( void );
	void             SetInternetConfig( int internet ) { m_InternetConfig = internet; }
	TStringList*     GetInternetConfigNames( void ) { return m_ChNames; }
public:
	void             SetFunctionSet( const String& FuncStr );
	bool             IsSupportFunction( const char* FuncStr );
	bool             IsSupportFunction( const String& FuncStr );
	void		     LoadFromInIFile();
public:   ///< Registry access functions
	bool             GetBoolProperty( const String& Group, const String& Name, bool Default );
	void             SetBoolProperty( const String& Group, const String& Name, bool Value  );
	String           GetStringProperty( const String& Group, const String& Name, String Default );
	void             SetStringProperty( const String& Group, const String& Name, String Value );
	String           GetBase64StringProperty( const String& Group, const String& Name, String Default );
	void             SetBase64StringProperty( const String& Group, const String& Name, String Value );
	int              GetIntegerProperty( const String& Group, const String& Name, int Default );
	void             SetIntegerProperty( const String& Group, const String& Name, int Value );
	double			 GetDoubleProperty( const String& Group, const String& Name, double Default  );
	void 			 SetDoubleProperty( const String& Group, const String& Name, double Value  );
	bool             GetDesktopBool( const String& Group, const String& Name, bool Default );
	void             SetDesktopBool( const String& Group, const String& Name, bool Value  );
	String           GetDesktopString( const String& Group, const String& Name, String Default );
	void             SetDesktopString( const String& Group, const String& Name, String Value  );
	int              GetDesktopInteger( const String& Group, const String& Name, int Default );
	void             SetDesktopInteger( const String& Group, const String& Name, int Value  );
	double           GetDesktopDouble( const String& Group, const String& Name, double Default );
	void             SetDesktopDouble( const String& Group, const String& Name, double Value  );
	void             GetDesktopFormPos( const String& Group, const String& Name, TRoundFormEx* Obj, bool SetVisbale = true, int X = 2, int Y = 96, int W =320, int H = 200 );
	void             SetDesktopFormPos( const String& Group, const String& Name, TRoundFormEx* Obj );
	bool             DeleteProperty( const String& Group, const String& Name );
public:
	int              GetOrderStoreRecoverOverlapSecond() {return m_OrderStoreRecoverOverlapSecond;};
	int              GetBalanceDisplayDigi( void );
	///< Exchange support
	bool            SupportExchange( const String& Exchange );
	bool            FuturesExchange( const String& Exchange );
	bool            Save( void );
	bool            IamVIP( const String& IP );
	bool            IPLicense( const String& IP );
	bool            TFTLicense( const String& IP );
	int             BrokerCount( void )       { return m_Brokers.ItemCount(); }
	TBrokerConfig*  BrokerConfig( int Index ) { return m_Brokers.GetItem( Index ); }
	TSpeedyConfig*  SpeedyConfig( int Broker, int Config );
	void            LoadAllIni( void );
	void            LoadStarWaveSettingIni( void );
private:
	String	   m_RegisterURL;
	String	   m_LogonURL;
	String	   m_EditURL;
	String	   m_XFlashURL;
	String     m_PurchaseListURL;
	String	   m_ReportURL;
	String	   m_EditURLWithParams;
	String	   m_XFlashURLWithParams;
	String	   m_PurchaseListURLWithParams;
	String	   m_ForgetPassword;
	String	   m_MegaCAURL;
	String	   m_MegaBackendURL;
	bool GetResponseJSON( TMemoryStream* Stream, String& ResponseJSON );
	void GenData( String& Out );
	void GenData( String& Out, const String& Param );
	void GenData( String& Out, const String& Param, const String& Value );
	void GenUploadData( const String& Items, String& Out );
	bool MDBSRequest( TMemoryStream* OutStream, const String& Func, const String& Data );
	bool MDBSUploadRequest( TMemoryStream* OutStream, const String& FilledItems );
public:
	const String& GetRegisterURL( void );
	const String& GetForgetPasswordURL( void );
	const String& GetEditURL( void );
	const String& GetEditURL( const String& ID, const String& token );
	const String& GetXFlashURL( const String& ID, const String& token );
	const String& GePurchaseListURL( const String& ID, const String& token );
	const String& GetReportURL( void );
	const String& GetMegaCAURL( void );
	const String& GetMegaBackendURL( void );
public:
	bool HTTPDownload( const String& URL, const String& LocalFile );
	bool GetServerValue( const String& Name, String& Value, const String& Def );
	bool SetServerValue( const String& Name, const String& Value );
	bool TokenExpired( void );
	bool UploadFilled( const String& Items, int FC );
	bool ReloadUserInfo( void );
	void LoadUserInfo( TJSONObject *lpRoot );
	bool UserRightsInfo( void );
	void GetSignal( int h, int m, UFC::AnsiString& Signal );
	int  GetThroughput( const UFC::AnsiString& Account );

};
//---------------------------------------------------------------------------
extern Config g_Config;
//---------------------------------------------------------------------------
#endif

