//---------------------------------------------------------------------------
#ifndef RecoverThreadH
#define RecoverThreadH
#include "Sigo.h"
#include "PInt32.h"
#include "TTaifexConnection.h"
#include "../ExchangeUtility/Utility.h"
//------------------------------------------------------------------------------
class APIRecoverExecution : public UFC::PThread
{
private:
    UFC::AnsiString    FStartTime;
    UFC::AnsiString    FEndTime;
    UFC::AnsiString    FAE;
    UFC::AnsiString    FReplyKey;
    UFC::AnsiString    FKey;
    RecoverMarket      FMarket;
    RecoverDataType    FDataType;
    RecoverSession     FSessionType;
    Int32              FCID;
    Int32              FCount;
    Int32              FAELength;
    BOOL               FIsAdmin;
    Int32              FExecOutCount;
    Int32              FFirstExecTick;
    Int32              FThroughputLimit;
    TTaifexConnection* FObj;        
private:
    void ControlTroughput( void );
    void RecoverIt( RecoverMarket Market, ExecutionParser& Exec  );
    void Recover( RecoverMarket Market, char* Buffer );
    void GetRecoverFileName( RecoverMarket Market, UFC::AnsiString& RecoverFileName );
    void RecoverFromFile(  RecoverMarket Market );
    void SplitUserID( const UFC::AnsiString& String, UFC::AnsiString& AE );
    void Execute( void );
public:
    APIRecoverExecution( const UFC::AnsiString& STime,
                         const UFC::AnsiString& Key,
                         const UFC::AnsiString& AE,
                         Int32 CID,
                         RecoverMarket Market,
                         RecoverDataType DataType,                        
                         RecoverSession  SessionType,                        
                         BOOL IsAdmin,
                         Int32 RecoverSpeedy,
                         TTaifexConnection* ExecCallBack );    
};
//---------------------------------------------------------------------------
#endif
