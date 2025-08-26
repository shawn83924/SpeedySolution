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
#include "Sigo.h" 
#include "UFC.h" 
#include "iniFile.h"
#include "PEasyFTPClient.h"
//---------------------------------------------------------------------------
#include "TFixedFormat.h"
#include "TParseData.h"
//---------------------------------------------------------------------------
class SecurityParser : public UFC::TFixedFormat
{
private:
  UFC::AnsiString	FFilePath;
  UFC::TParseData	FParseData;
  UFC::TDataset*	FDataset;
  virtual void LoadConfigSettingEx(UFC::UiniFile* pIniFile);
public:
  SecurityParser(const UFC::AnsiString & iniFilePath);
  UFC::TRecord*  GetRecord( UFCType::Int32 RecordIndex);
  UFCType::Int32 GetRecordCount();
};
//--------------------------------------------------------------------------
class GCRMConnection :  public MessageListener
{
private:    
    MessageObject*        FMessageObject;
    UFC::AnsiString       FIP;
    int                   FPort;
    UFC::UiniFile*        FConfig;    
    BOOL                  FDownloaded;
private:            
    UFC::AnsiString         FFTPIP;
    Int32                   FFTPPort;
    UFC::AnsiString         FFTPAccount;
    UFC::AnsiString         FFTPPassword;
    UFC::AnsiString         FRemote;
    UFC::AnsiString         FLocal;
    UFC::AnsiString         FAlertSubject;
    UFC::AnsiString         FAlertKey;       
private:    
    UFC::PHashMap<UFC::AnsiString, double> FFutDeposit;
    UFC::PHashMap<UFC::AnsiString, double> FOptAValue;
    UFC::PHashMap<UFC::AnsiString, double> FOptCM;
private:
    void LoadSetting( void );    
    void ParseMargin( void ); 
    void ParseHoldingSetting( SecurityParser& P09F, SecurityParser& P09O );
    void ParseManualSetting( SecurityParser& P09F, SecurityParser& P09O );
    void FindReleated( BOOL IsFut, SecurityParser& P09, const UFC::AnsiString& Symbol, UFC::PStringList& Holding, UFC::PStringSet& SymSet );
    bool FindStockID( SecurityParser& P09, const UFC::AnsiString& Symbol, UFC::AnsiString& StockID );
private: 
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );    
public:
    GCRMConnection( );
    virtual ~GCRMConnection( void );
    void Download( void );   
    void DownloadFailed( const UFC::AnsiString& AlertMessage  );
    void SendAlert(const UFC::AnsiString& AlertMessage );
};
//------------------------------------------------------------------------------
#endif
