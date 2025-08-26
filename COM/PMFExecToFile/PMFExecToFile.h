// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _SinoPacExecConnection_H
#define	_SinoPacExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
//--------------------------------------------------------------------------
class TExecFile
{
private:
    UFC::FileStream *FFutureFilePtr;
    UFC::FileStream *FOptionFilePtr;
    UFC::FileStream *FForeignConfirmFilePtr;
    UFC::FileStream *FForeignFillFilePtr;
    UFC::FileStream *FStockConfirmPtr;
    UFC::FileStream *FStockFillPtr;
public:
    TExecFile( const UFC::AnsiString& FutureFileName, const UFC::AnsiString& OptionFileName, const UFC::AnsiString& StockConfirmFileName = "", const UFC::AnsiString& StockFillFileName = "", const UFC::AnsiString& ForeignConfirmFileName = "", const UFC::AnsiString& ForeignFillFileName = "" );
    ~TExecFile();
    UFC::FileStream *OpenExecutionReportFile( const UFC::AnsiString& ExecRptFileName );
    void Write( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::ExecTypeEnum ExecType, nsOrderMessageDefine::OrderStatusEnum OrderStatus, const char* Data, int Length );
};

//--------------------------------------------------------------------------
const Int32 PATS_ORDER_LENGTH = 350;
const Int32 PATS_USERDATA_LENGTH = 270;
//--------------------------------------------------------------------------
class PMFExecToFile : public IOrderConnectionEventListener                                                  
{
public:
    static UFC::PInt32 FReportSeqNo;
    static UFC::AnsiString FReportSeqNoFileName;
    static UFC::FileStreamEx *FExecIDSeqNoFilePtr;
    static UFC::PHashMap<UFC::AnsiString, Int32> FSeqNoExecIDIndex;
    static UFC::AnsiString FPreRptSeqPrefix;
    static bool FIsRecovering;
    
public:    
    static Int32 GetReportSeqNo();
    static void SetReportSeqNo(Int32 NewValue);
    static Int32 IncreaseReportSeqNo(Int32 IncreaseValue);
    static Int32 GetNextReportSeqNo();
    static UFC::AnsiString GetOverseasFieldValueStr(const UFC::AnsiString& ReportStr, const UFC::AnsiString& FieldName);
    static Int32 PrepareExecIDSeqNoFile( int RecoverBeginTime );
    static UFC::AnsiString GenerateOverseasRptSeqNameValueStr(const UFC::AnsiString& SourceData);
    
private:    
    UFC::AnsiString       FDebug;   
private:
    int                   FRecoverBeginTime;    
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;    
    TExecFile*            FDefExecFile;
    UFC::PHashedList<UFC::AnsiString, TExecFile*> FBrokerToFile;
        
private:        
    bool IsStockExec( nsOrderMessageDefine::MarketEnum market );
    TExecFile* GetFileObj( const UFC::AnsiString& BrokerID );
    void GenUserDef( TExecutionReportMessage* Msg, UFC::AnsiString& UserDef );
    
private: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
    virtual void OnCancelWorking( CancelWorkingResult Result, int Count ) {};
    virtual void OnNews( TNewsMessage* Msg ) {}
        
public:
    PMFExecToFile( int RecoverBeginTime );
    virtual ~PMFExecToFile( void );       
    void Recover( UFC::AnsiString BeginTime );  
    void CheckConnection( void );
    void FillToFile( TExecutionReportMessage* Msg );
    void ConfirmToFile( TExecutionReportMessage* Msg );
    void StockConfirmToFile( TExecutionReportMessage* Msg );
};
//------------------------------------------------------------------------------
#endif
