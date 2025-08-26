//------------------------------------------------------------------------------------------------------------------------
#ifndef __ThresholdManager_H
#define __ThresholdManager_H
//------------------------------------------------------------------------------------------------------------------------
#include "../RCRule/RCQGProtocol.h"
#include "../RCDataStore/mtdRC_ROUTING.h"
#include "../RCDataStore/mtdCUST_SUM.h"
#include "../RCDataStore/mtdTHRESHOLD.h"
//------------------------------------------------------------------------------------------------------------------------
typedef struct s_wsThresholdCfgParm {
        BOOL   IsActive;
        int    QueryGatewayPort;
	///< Initialize
        s_wsThresholdCfgParm( )
        {
            IsActive             = FALSE;
            QueryGatewayPort     = 9866;
        }           
} wsThresholdCfgParm;
//------------------------------------------------------------------------------------------------------------------------
// class ThresholdManager
//------------------------------------------------------------------------------------------------------------------------
class ThresholdManager
{
private:
    	int                             FRCGroup;
        UFC::AnsiString                 FMDSHomeDir;
        wsThresholdCfgParm              FThresholdCfgParm;
private:
        Int32				FTThreshold_PKSize;   
        MDS::MTable*			FTThreshold;
        MDS::MTable*                    FTRC_Routing;
private:
        void InitTableThreshold( void );
        BOOL ClearTableThreshold( void );
        void UpdateTableThreshold( MDS::MRow& RowThreshold, double CanOrder, double RiskRate );
        void NewTableThreshold( void );
        BOOL QueryRemoteThreshold( UFC::AnsiString& IB, UFC::AnsiString& Account, UFC::PSocket* pSocket, double& CanOrder, double& RiskRate );
        void QueryRCThreshod( UFC::PSocket* pSocket, UFC::AnsiString IP, UFC::AnsiString Key, int CheckLevel );
public:
        void ResetThreshold( MDS::MTable* TRC_Routing );
        void UpdateTableThresholdByPK(  MDS::MRow&  RowRCRouting,
                                        int    CheckLevel,
                                        double CanOrder,
                                        double RiskRate,
                                        BOOL IsInit = FALSE );
        BOOL CheckThreshold( const char* IB, const char* Account );
        void QueryRCThreshodByIP( const char* IP );
public:
	ThresholdManager( int                    RCGroup,
                          const UFC::AnsiString& MDSHomeDir,
                          wsThresholdCfgParm&    ThresholdCfgParm,
                          MDS::MTable*           TRC_Routing );
public:
	virtual ~ThresholdManager( void );
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
