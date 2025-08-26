//-----------------------------------------------------------------------------------------
/**@file QueryFCM.cp
*  @brief Header file for QueryFCM.cp
*  @author Simon Chaang
*
*/
//-----------------------------------------------------------------------------------------
#ifndef _QUERYPVC_H
#define _QUERYPVC_H
//-----------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
//-----------------------------------------------------------------------------------------
const int MAX_LINES = 1024; ///< Support 1024 PVCs(Perment Virtual Circuit)
//-----------------------------------------------------------------------------------------
class TSECLines
{
private:
    UFC::AnsiString BROKER_IDs[ MAX_LINES ];
    UFC::AnsiString PASSWORDs[ MAX_LINES ];
    UFC::AnsiString PEER_IPs[ MAX_LINES ];
    UFC::AnsiString PVC_IDs[ MAX_LINES ];
    int             PORT_NOs[ MAX_LINES ];
    int             PVC_NOs[ MAX_LINES ];
    int             LOCAL_PORT_NOs[ MAX_LINES ];
    int             GROUP[ MAX_LINES ];
    BOOL            ACTIVE[ MAX_LINES ];
    BOOL            IsFIX[ MAX_LINES ];
    int             THROUGHPUT[ MAX_LINES ];
private:
    UFC::AnsiString FTMPCfgFileName;
    UFC::AnsiString FFIXCfgFileName;    
    UFC::AnsiString FMarketCfgFile;
    UFC::AnsiString FTargetCompID;  
    char            FExCode;
    BOOL            FIsTWSE;
private:
    BOOL ParseLine( char* Line, char APCode, int Index  );
    BOOL ParseFIXLine( char* Line, int Index );
    int  GetFIXLines( void );
public:
    TSECLines( BOOL IsTWSE );
    int   SaveFIXConfig( void );    
    int   GetLines( char ApCode );
public:    
    const UFC::AnsiString& GetBrokerID( int Index ) { return BROKER_IDs[ Index ]; }
    const UFC::AnsiString& GetPassword( int Index ) { return PASSWORDs[ Index ]; }
    const UFC::AnsiString& GetIP( int Index )       { return PEER_IPs[ Index ]; }
    const UFC::AnsiString& GetPVCID( int Index )    { return PVC_IDs[ Index ]; }
    int   GetPort( int Index )                      { return PORT_NOs[ Index ]; }
    int   GetPVC( int Index )                       { return PVC_NOs[ Index ]; }
    int   GetLocalPort( int Index )                 { return LOCAL_PORT_NOs[ Index ]; }
    int   GetGroup( int Index )                     { return GROUP[ Index ]; }
    int   GetThreughput( int Index )                { return THROUGHPUT[ Index ]; }
    BOOL  GetActive( int Index )                    { return ACTIVE[ Index ]; }
    BOOL  IsFIXSession( int Index )                 { return IsFIX[ Index ]; }
};
//-----------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------
