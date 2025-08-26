#include "MultilegSymbol.h"
//------------------------------------------------------------------------------------------------------------------------
char *strncpy_zero(char *s1, const char *s2, size_t n)
{
    strncpy(s1, s2, n);
    s1[n] = 0;
    return s1;
}
//------------------------------------------------------------------------------------------------------------------------
// class BO_SymbolDeCompose
//------------------------------------------------------------------------------------------------------------------------
BO_SymbolDeCompose::BO_SymbolDeCompose()
{
    Initialize();
}
//------------------------------------------------------------------------------------------------------------------------
BO_SymbolDeCompose::~BO_SymbolDeCompose()
{
}
//------------------------------------------------------------------------------------------------------------------------
BO_SymbolDeCompose::BO_SymbolDeCompose( UFC::AnsiString Symbol, char BS_Code )
{
    Initialize();
    SetSymbol( Symbol, BS_Code );
}
//------------------------------------------------------------------------------------------------------------------------
void BO_SymbolDeCompose::Initialize( void )
{
    // Initial variant
    FStrategicType = stUnKnown;
    UFC::UDateTime Today;
    FCYear1Base = Today.getYear() % 10;
    FCYear3Base = Today.getYear() / 10;
    FNYear3Base = FCYear3Base + 1;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::GetExcercisePrice( const char* SymbolPos, const char* SymbolPricePos, char* ePrice )
{
    int i;

    for ( i=0; i<5; i++ )
    {
        if ( SymbolPricePos[i] < '0' || SymbolPricePos[i] > '9' )	// not equal '0' ~ '9'
                return FALSE;
    }
    if ( !memcmp(SymbolPos, "TXO", 3) || !memcmp(SymbolPos, "TFO", 3) || !memcmp(SymbolPos, "TGO", 3) || !memcmp(SymbolPos, "XIO", 3) )
    {
        memcpy( ePrice, "0", 1 );
        memcpy( ePrice+1, SymbolPricePos, 5 );
        strncpy_zero( ePrice+6, "000", 3 );
    }
    else
    {
        memcpy( ePrice, "00", 2 );
        memcpy( ePrice+2, SymbolPricePos, 5 );
        strncpy_zero( ePrice+7, "00", 2 );
    }
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::GetMonthAndCP( const char* SymbolMonthPos, char* SettleMonth, char* CP_Code )
{
    // SettleMonth & CP_Code
    if (SymbolMonthPos[1] == '0' && FCYear1Base == 9)
        sprintf(SettleMonth, "%d%c", FNYear3Base, SymbolMonthPos[1]);
    else
        sprintf(SettleMonth, "%d%c", FCYear3Base, SymbolMonthPos[1]);
    switch ( SymbolMonthPos[0] )
    {
        case 'A': strcat( SettleMonth, "01" ); *CP_Code = 'C'; break;
        case 'M': strcat( SettleMonth, "01" ); *CP_Code = 'P'; break;
        case 'B': strcat( SettleMonth, "02" ); *CP_Code = 'C'; break;
        case 'N': strcat( SettleMonth, "02" ); *CP_Code = 'P'; break;
        case 'C': strcat( SettleMonth, "03" ); *CP_Code = 'C'; break;
        case 'O': strcat( SettleMonth, "03" ); *CP_Code = 'P'; break;
        case 'D': strcat( SettleMonth, "04" ); *CP_Code = 'C'; break;
        case 'P': strcat( SettleMonth, "04" ); *CP_Code = 'P'; break;
        case 'E': strcat( SettleMonth, "05" ); *CP_Code = 'C'; break;
        case 'Q': strcat( SettleMonth, "05" ); *CP_Code = 'P'; break;
        case 'F': strcat( SettleMonth, "06" ); *CP_Code = 'C'; break;
        case 'R': strcat( SettleMonth, "06" ); *CP_Code = 'P'; break;
        case 'G': strcat( SettleMonth, "07" ); *CP_Code = 'C'; break;
        case 'S': strcat( SettleMonth, "07" ); *CP_Code = 'P'; break;
        case 'H': strcat( SettleMonth, "08" ); *CP_Code = 'C'; break;
        case 'T': strcat( SettleMonth, "08" ); *CP_Code = 'P'; break;
        case 'I': strcat( SettleMonth, "09" ); *CP_Code = 'C'; break;
        case 'U': strcat( SettleMonth, "09" ); *CP_Code = 'P'; break;
        case 'J': strcat( SettleMonth, "10" ); *CP_Code = 'C'; break;
        case 'V': strcat( SettleMonth, "10" ); *CP_Code = 'P'; break;
        case 'K': strcat( SettleMonth, "11" ); *CP_Code = 'C'; break;
        case 'W': strcat( SettleMonth, "11" ); *CP_Code = 'P'; break;
        case 'L': strcat( SettleMonth, "12" ); *CP_Code = 'C'; break;
        case 'X': strcat( SettleMonth, "12" ); *CP_Code = 'P'; break;
        default:  strcat( SettleMonth, "00" ); *CP_Code = ' '; return FALSE;
    }
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetSingle( void )
{
    FStrategicType = stSingle;
    // 1st foot
    strncpy_zero( FComm1, FSymbol.c_str(), 10 );
    if ( GetExcercisePrice( FSymbol.c_str(), FSymbol.c_str()+3, FExcercisePrice1 ) == FALSE )       
    {
        printf("ExcercisePrice:%s\n", FExcercisePrice1 );                
        return FALSE;
    }
    if ( GetMonthAndCP( FSymbol.c_str()+8, FSettleMonth1, &FCP_Code1 ) == FALSE )
    {
        printf("SettleMonth:%s Call,Put:%c \n", FSettleMonth1, FCP_Code1 );                        
        return FALSE;
    }
    FBS_Code1 = FBS_Code;
    // 2nd foot
    memset( FComm2, ' ', 10 ); FComm2[10] = 0;
    strcpy( FExcercisePrice2, "000000000" );
    strcpy( FSettleMonth2, "      " );
    FBS_Code2 = ' ';
    FCP_Code2 = ' ';
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetPriceSpread( void )
{
    // 1st foot
    memcpy( FComm1, FSymbol.c_str(), 8 );
    strncpy_zero( FComm1+8, FSymbol.c_str()+14, 2 );
    if ( GetExcercisePrice( FSymbol.c_str(), FSymbol.c_str()+3, FExcercisePrice1 ) == FALSE )
        return FALSE;
    if ( GetMonthAndCP( FSymbol.c_str()+14, FSettleMonth1, &FCP_Code1 ) == FALSE )
        return FALSE;
    FBS_Code1 = (FBS_Code == 'B') ? 'S' : 'B';
    // 2nd foot
    memcpy( FComm2, FSymbol.c_str(), 3 );
    strncpy_zero( FComm2+3, FSymbol.c_str()+9, 7 );
    if ( GetExcercisePrice( FSymbol.c_str(), FSymbol.c_str()+9, FExcercisePrice2 ) == FALSE )
        return FALSE;
    strcpy( FSettleMonth2, FSettleMonth1 );
    FCP_Code2 = FCP_Code1;
    FBS_Code2 = FBS_Code;
    // check 1.ExcercisePrice
    if ( FCP_Code1 == 'C' )
    {
        if ( atoi(FExcercisePrice1) <= atoi(FExcercisePrice2) )
            return FALSE;
        FStrategicType = stCallPriceSpread;
    }
    else
    {
        if ( atoi(FExcercisePrice2) <= atoi(FExcercisePrice1) )
            return FALSE;
        FStrategicType = stPutPriceSpread;
    }
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetTimeSpread( void )
{
    // 1st foot
    strncpy_zero( FComm1, FSymbol.c_str(), 10 );
    if ( GetExcercisePrice( FSymbol.c_str(), FSymbol.c_str()+3, FExcercisePrice1 ) == FALSE )
        return FALSE;
    if ( GetMonthAndCP( FSymbol.c_str()+8, FSettleMonth1, &FCP_Code1 ) == FALSE )
        return FALSE;
    FBS_Code1 = (FBS_Code == 'B') ? 'S' : 'B';
    // 2nd foot
    memcpy( FComm2, FSymbol.c_str(), 8 );
    strncpy_zero( FComm2+8, FSymbol.c_str()+11, 2 );
    strcpy( FExcercisePrice2, FExcercisePrice1 );
    if ( GetMonthAndCP( FSymbol.c_str()+11, FSettleMonth2, &FCP_Code2 ) == FALSE )
        return FALSE;
    FBS_Code2 = FBS_Code;
    // check 1.CP_Code 2.SettleMonth
    if ( FCP_Code1 != FCP_Code2 )
        return FALSE;
    if ( atoi(FSettleMonth2) <= atoi(FSettleMonth1) )
        return FALSE;
    if ( FCP_Code1 == 'C' )
        FStrategicType = stCallTimeSpread;
    else
        FStrategicType = stPutTimeSpread;
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetStraddle( void )
{
    FStrategicType = stStraddle;
    // 1st foot
    strncpy_zero( FComm1, FSymbol.c_str(), 10 );
    if ( GetExcercisePrice( FSymbol.c_str(), FSymbol.c_str()+3, FExcercisePrice1 ) == FALSE )
        return FALSE;
    if ( GetMonthAndCP( FSymbol.c_str()+8, FSettleMonth1, &FCP_Code1 ) == FALSE )
        return FALSE;
    FBS_Code1 = FBS_Code;
    // 2nd foot
    memcpy( FComm2, FSymbol.c_str(), 8 );
    strncpy_zero( FComm2+8, FSymbol.c_str()+11, 2 );
    strcpy( FExcercisePrice2, FExcercisePrice1 );
    if ( GetMonthAndCP( FSymbol.c_str()+11, FSettleMonth2, &FCP_Code2 ) == FALSE )
        return FALSE;
    FBS_Code2 = FBS_Code;
    // check 1.CP_Code 2.SettleMonth
    if ( FCP_Code1 != 'C' || FCP_Code2 != 'P' )
        return FALSE;
    if ( atoi(FSettleMonth1) != atoi(FSettleMonth2) )
        return FALSE;
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetStrangle( void )
{
    FStrategicType = stStrangle;
    // 1st foot
    strncpy_zero( FComm1, FSymbol.c_str(), 10 );
    if ( GetExcercisePrice( FSymbol.c_str(), FSymbol.c_str()+3, FExcercisePrice1 ) == FALSE )
        return FALSE;
    if ( GetMonthAndCP( FSymbol.c_str()+8, FSettleMonth1, &FCP_Code1 ) == FALSE )
        return FALSE;
    FBS_Code1 = FBS_Code;
    // 2nd foot
    memcpy( FComm2, FSymbol.c_str(), 3 );
    strncpy_zero( FComm2+3, FSymbol.c_str()+11, 7 );
    if ( GetExcercisePrice( FSymbol.c_str(), FSymbol.c_str()+11, FExcercisePrice2 ) == FALSE )
        return FALSE;
    if ( GetMonthAndCP( FSymbol.c_str()+16, FSettleMonth2, &FCP_Code2 ) == FALSE )
        return FALSE;
    FBS_Code2 = FBS_Code;
    // check 1.CP_Code 2.SettleMonth 3.ExcercisePrice
    if ( FCP_Code1 != 'C' || FCP_Code2 != 'P' )
        return FALSE;
    if ( atoi(FSettleMonth1) != atoi(FSettleMonth2) )
        return FALSE;
    if ( atoi(FExcercisePrice1) == atoi(FExcercisePrice2) )
        return FALSE;
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetConRev( void )
{
    FStrategicType = stConRev;
    // 1st foot
    strncpy_zero( FComm1, FSymbol.c_str(), 10 );
    if ( GetExcercisePrice( FSymbol.c_str(), FSymbol.c_str()+3, FExcercisePrice1 ) == FALSE )
        return FALSE;
    if ( GetMonthAndCP( FSymbol.c_str()+8, FSettleMonth1, &FCP_Code1 ) == FALSE )
        return FALSE;
    FBS_Code1 = (FBS_Code == 'B') ? 'S' : 'B';
    // 2nd foot
    memcpy( FComm2, FSymbol.c_str(), 8 );
    strncpy_zero( FComm2+8, FSymbol.c_str()+11, 2 );
    strcpy( FExcercisePrice2, FExcercisePrice1 );
    if ( GetMonthAndCP( FSymbol.c_str()+11, FSettleMonth2, &FCP_Code2 ) == FALSE )
        return FALSE;
    FBS_Code2 = FBS_Code;
    // check 1.CP_Code 2.SettleMonth
    if ( FCP_Code1 != 'C' || FCP_Code2 != 'P' )
        return FALSE;
    if ( atoi(FSettleMonth1) != atoi(FSettleMonth2) )
        return FALSE;
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetSymbol( UFC::AnsiString Symbol, char BS_Code )
{
    BOOL rtnVal;

    FSymbol = Symbol;
    FBS_Code = BS_Code;
    FSymbol.TrimRight();
    if( FSymbol.Length() == 10 )	// Single
        rtnVal = SetSingle();
    else if( FSymbol.Length() == 5 )	// Futures single (New Version)
        rtnVal = SetFuturesSingle();
    else if( FSymbol.Length() == 16 && FSymbol[8] == '/' ) // Price Spread
        rtnVal = SetPriceSpread();
    else if( FSymbol.Length() == 13 && FSymbol[10] == '/' )	// Time Spread
        rtnVal = SetTimeSpread();
    else if( FSymbol.Length() == 13 && FSymbol[10] == ':' )	// Straddle
        rtnVal = SetStraddle();
    else if( FSymbol.Length() == 18 && FSymbol[10] == ':' )	// Strangle
        rtnVal = SetStrangle();
    else if( FSymbol.Length() == 13 && FSymbol[10] == '-' )	// Conversion/Reversals
        rtnVal = SetConRev();
    else if( FSymbol.Length() == 8   && FSymbol[5] == '/' ) // Futures Time Spread
        rtnVal = SetFuturesTimeSpread();
    else
        rtnVal = FALSE;
    if ( rtnVal == FALSE )
        FStrategicType = stUnKnown;
    return rtnVal;
}
//------------------------------------------------------------------------------------------------------------------------
//  Functions to process Futures symbol. ( For TAIFEX protocol after version 20060607)
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetFuturesSingle( void )
{
    FStrategicType = stFuturesSingle;
    // 1st foot
    memset( FComm1, ' ', 10 ); FComm1[10] = 0;
    memcpy( FComm1, FSymbol.c_str(), 5 );
    strcpy( FExcercisePrice1, "000000000" );
    if ( GetMonthAndCP( FSymbol.c_str()+3, FSettleMonth1, &FCP_Code1 ) == FALSE )
        return FALSE;
    FBS_Code1 = FBS_Code;
    FCP_Code1 = ' ';
    // 2nd foot
    memset( FComm2, ' ', 10 ); FComm2[10] = 0;
    strcpy( FExcercisePrice2, "000000000" );
    strcpy( FSettleMonth2, "      " );
    FBS_Code2 = ' ';
    FCP_Code2 = ' ';
    return TRUE;    
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BO_SymbolDeCompose::SetFuturesTimeSpread( void )
{
    FStrategicType = stFuturesTimeSpread;
    ///< for example: TXFA7/C7
    //< 1st foot should be: TXFA7
    memset( FComm1, ' ', 10 ); FComm1[10] = 0;
    memcpy( FComm1, FSymbol.c_str(), 5 );       ///< Copy the TXFA7
    FBS_Code1 = (FBS_Code == 'B') ? 'S' : 'B';  ///< BS code is opposite to the B/S of this order.          	
    if( GetMonthAndCP( FSymbol.c_str()+3, FSettleMonth1, &FCP_Code1 ) == FALSE )
        return FALSE;        	
    FCP_Code1 = ' ';
    // 2nd foot should be: TXFC7
    memset( FComm2, ' ', 10 ); FComm2[10] = 0;
    memcpy( FComm2, FSymbol.c_str(), 3 );         ///< Copy the TXF
    memcpy( FComm2 + 3, FSymbol.c_str() + 6, 2 ); ///< Copy the C7
    FBS_Code2 = FBS_Code;                         ///< BS code equals the B/S of this order.        
    
    if( GetMonthAndCP( FSymbol.c_str()+6, FSettleMonth2, &FCP_Code2 ) == FALSE )
        return FALSE;
    FCP_Code2 = ' ';
    ///< Check SettleMonth	
    if( atoi( FSettleMonth2 ) <= atoi( FSettleMonth1 ) )
        return FALSE;    
    strcpy( FExcercisePrice1, "000000000" );
    strcpy( FExcercisePrice2, "000000000" );
    return TRUE;        
}
//------------------------------------------------------------------------------------------------------------------------
