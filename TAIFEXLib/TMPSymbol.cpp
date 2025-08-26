#include "TMPTypes.h"
#include "TMPMessage.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------
// Class TMPSymbol
// Implement Symbol class for TMP protocol.
// It has two mode, Binary mode and Text mode
//------------------------------------------------------------------------------------------------------
// Constructor of String symbol.  
// Fromat 1: TXO09000A8
// Fromat 2: 276,138,1,2,1
//------------------------------------------------------------------------------------------------------
TMPSymbol::TMPSymbol( const UFC::AnsiString& Symbol )
:TMPType(ttSymbol)
{
    FromString( Symbol );
}
//------------------------------------------------------------------------------------------------------
// Constructor of binary symbol.
//------------------------------------------------------------------------------------------------------
TMPSymbol::TMPSymbol( const Int16 Leg1ID, TMPLegSide Leg1Side, const Int16 Leg2ID, TMPLegSide Leg2Side, TMPCombOP ComboOP )
:TMPType(ttSymbol)
, FSymbolType( 1 )///< 1: binary, 2: text
, FLeg1ID( Leg1ID )
, FLeg2ID( Leg2ID )
, FLeg1Side( (UInt8)Leg1Side )
, FLeg2Side( (UInt8)Leg2Side )
, FCombOP( (UInt8)ComboOP ) 
{
}
//------------------------------------------------------------------------------------------------------
// Construct object from stream
//------------------------------------------------------------------------------------------------------
TMPSymbol::TMPSymbol( UFC::PStream* Stream )
:TMPType(ttSymbol) 
{
    UInt8 Buffer[ TMP_SYMBOL_LEN + 1 ];    
    
    if( (UInt32)Stream->Read( (void*)Buffer, TMP_SYMBOL_LEN + 1 ) !=  TMP_SYMBOL_LEN + 1 )
        throw( TMPBufferException( TMP_SYMBOL_LEN + 1 ) );
    
    FSymbolType = Buffer[ 0 ];
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {
        FLeg1ID   = UFC::NInt16::ToInt16( Buffer + 1);
        FLeg2ID   = UFC::NInt16::ToInt16( Buffer + 3 );
        FLeg1Side = Buffer[ 5 ];
        FLeg2Side = Buffer[ 6 ];
        FCombOP   = Buffer[ 7 ];
    }
    else 
    {
        memset( FData, ' ', TMP_SYMBOL_LEN );
        FData[ TMP_SYMBOL_LEN ] = 0;
        memcpy( FData, Buffer + 1, TMP_SYMBOL_LEN );
    }
}
//------------------------------------------------------------------------------------------------------
TMPSymbol::TMPSymbol( UInt8 Type, UInt8* Buffer )
:TMPType(ttSymbol) 
{
    FSymbolType = Type;
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {
        FLeg1ID   = UFC::NInt16::ToInt16( Buffer );
        FLeg2ID   = UFC::NInt16::ToInt16( Buffer + 2 );
        FLeg1Side = Buffer[ 4 ];
        FLeg2Side = Buffer[ 5 ];
        FCombOP   = Buffer[ 6 ];
    }
    else 
    {
        memcpy( FData, Buffer, TMP_SYMBOL_LEN );
        FData[ TMP_SYMBOL_LEN ] = 0;
    }
}
//------------------------------------------------------------------------------------------------------
TMPSymbol::TMPSymbol( const TMPSymbol& Symbol )
:TMPType(ttSymbol) 
{
    FSymbolType = Symbol.FSymbolType;   
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {
        FLeg1ID   = Symbol.FLeg1ID.ToInt16();
        FLeg2ID   = Symbol.FLeg2ID.ToInt16();
        FLeg1Side = Symbol.FLeg1Side;
        FLeg2Side = Symbol.FLeg2Side;
        FCombOP   = Symbol.FCombOP;
    }
    else
        memcpy( FData, Symbol.FData, TMP_SYMBOL_LEN );
}    
//------------------------------------------------------------------------------------------------------    
Int32 TMPSymbol::ToBinary( UInt8* Buffer )
{
    int Offset = 1;
        
    memcpy( Buffer, &FSymbolType, 1 );        
    if( FSymbolType == 1 ) ///< 1: binary, 2: text
    {
        memcpy( Buffer + Offset, FLeg1ID.GetBytes(), sizeof(Int16) );
        Offset += sizeof(Int16);
        memcpy( Buffer + Offset, FLeg2ID.GetBytes(), sizeof(Int16) );
        Offset += sizeof(Int16);
        memcpy( Buffer + Offset, &FLeg1Side, 1  );
        Offset += 1;
        memcpy( Buffer + Offset, &FLeg2Side, 1  );
        Offset += 1;
        memcpy( Buffer + Offset, &FCombOP, 1  );
        Offset += 1;
        memset( Buffer + Offset, 0, 13 );
        return TMP_SYMBOL_LEN + 1;
    }
    else
    {
        memcpy( Buffer + Offset, FData, TMP_SYMBOL_LEN );
        return TMP_SYMBOL_LEN + 1;
    }
}        
//------------------------------------------------------------------------------------------------------
Int32 TMPSymbol::ToStream( UFC::PStream* Stream ) 
{
    int WriteSize = 0;
        
    WriteSize += Stream->Write( &FSymbolType, 1 );    
    if( FSymbolType == 1 ) ///< 1: binary, 2: text
    {
        char Filler[ 16 ];        
        memset( Filler, 0, 16 );
        WriteSize += FLeg1ID.SaveToStream( Stream );
        WriteSize += FLeg2ID.SaveToStream( Stream );
        WriteSize += Stream->Write( &FLeg1Side, 1 );
        WriteSize += Stream->Write( &FLeg2Side, 1 );
        WriteSize += Stream->Write( &FCombOP, 1 );
        WriteSize += Stream->Write( Filler, 13 );
        return WriteSize;
    }
    else
        return Stream->Write( FData, TMP_SYMBOL_LEN ) + 1;
}
//------------------------------------------------------------------------------------------------------
Int32 TMPSymbol::ToString( char* Str, UInt32 DataLength ) 
{
    if( FSymbolType == 1 ) ///< 1: binary, 2: text
    {
        char tmpstr[24];///< "xxxxx,xxxxx,1,2,:"
        
        sprintf( tmpstr, "%u,%u,%u,%u,%u", FLeg1ID.ToInt16(), FLeg2ID.ToInt16(), FLeg1Side, FLeg2Side, FCombOP );
        if( DataLength < strlen(tmpstr) )
            throw( TMPBufferException( (Int32)strlen(tmpstr) ) );
        strcpy( Str, tmpstr );        
        return (Int32)strlen(tmpstr);
    }
    else 
    {
        UFC::AnsiString Symbol( (char*)FData, TMP_SYMBOL_LEN );
        
        Symbol.TrimRight( );
        if( Symbol.Length( ) == 0 )
            Symbol = "NULL";
        if( DataLength < (UInt32)Symbol.Length( ) )
            throw( TMPBufferException( Symbol.Length( ) ) );
        strcpy( Str, Symbol.c_str() );
        return Symbol.Length( );
    }
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPSymbol::ToInteger( void )
{
    throw( TMPFieldTypeException( "TMPSymbol", "Int32" ) );	   
}
//------------------------------------------------------------------------------------------------------
Int64 TMPSymbol::ToInteger64( void )
{
    throw( TMPFieldTypeException( "TMPSymbol", "Int64" ) );
}
//------------------------------------------------------------------------------------------------------        
void TMPSymbol::FromString( const UFC::AnsiString& Symbol )
{
    UFC::PStringList Strings;
    
    Strings.SetStrings( Symbol, ",\n" );
    if( Strings.ItemCount()  == 1 ) ///< String format.
    {
        FSymbolType = 2;///< 1: binary, 2: text
        memset( FData, ' ', TMP_SYMBOL_LEN );
        FData[ TMP_SYMBOL_LEN ] = 0;
        if( Symbol.Length() > TMP_SYMBOL_LEN )
            memcpy( FData, Symbol.c_str(), TMP_SYMBOL_LEN );
        else
            memcpy( FData, Symbol.c_str(), Symbol.Length() );
        UInt8  Type;
        UInt16 Leg1ID,Leg2ID;

        if( GetBinSymbol( Type, 1, Leg1ID,FLeg1Side,Leg2ID,FLeg2Side,FCombOP ) == TRUE )
        {
            FLeg1ID   = Leg1ID;
            FLeg2ID   = Leg2ID;
        }
    }
    else if( Strings.ItemCount() == 5 )/// Binary format 
    {
        FSymbolType = 1;///< 1: binary, 2: text
        FLeg1ID   = (UInt16)Strings[0].ToInt();
        FLeg2ID   = (UInt16)Strings[1].ToInt();
        FLeg1Side = (UInt8)Strings[2].ToInt();
        FLeg2Side = (UInt8)Strings[3].ToInt();
        FCombOP   = (UInt8)Strings[4].ToInt();
    }
    else
        throw( TMPFieldException( Symbol.c_str() ) );	    
}    
//------------------------------------------------------------------------------------------------------        
void TMPSymbol::FromInteger( Int32 )
{    
    throw( TMPFieldTypeException( "Int32", "TMPSymbol" ) );	   
}
//------------------------------------------------------------------------------------------------------
TMPType* TMPSymbol::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPSymbol( Stream ); 
}
//------------------------------------------------------------------------------------------------------
TMPType* TMPSymbol::Duplicate( void  ) 
{ 
    return new TMPSymbol( *this ); 
}
//------------------------------------------------------------------------------------------------------
void TMPSymbol::Assign( TMPType& ref )
{
    TMPSymbol* RefPtr = static_cast<TMPSymbol*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FSymbolType = RefPtr->FSymbolType;
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {
        FLeg1ID   = RefPtr->FLeg1ID.ToInt16();
        FLeg2ID   = RefPtr->FLeg2ID.ToInt16();
        FLeg1Side = RefPtr->FLeg1Side;
        FLeg2Side = RefPtr->FLeg2Side;
        FCombOP   = RefPtr->FCombOP;
    }
    else
        memcpy( FData, RefPtr->FData, TMP_SYMBOL_LEN );
}
//------------------------------------------------------------------------------------------------------
void TMPSymbol::SetSymbol( const Int16 Leg1ID, TMPLegSide Leg1Side, const Int16 Leg2ID, TMPLegSide Leg2Side, TMPCombOP ComboOP ) 
{    
    FSymbolType = 1;///< 1: binary, 2: text
    FLeg1ID   = Leg1ID;
    FLeg2ID   = Leg2ID;
    FLeg1Side = (UInt8)Leg1Side;
    FLeg2Side = (UInt8)Leg2Side;
    FCombOP   = (UInt8)ComboOP;
}
//------------------------------------------------------------------------------------------------------
TMPSymbol& TMPSymbol::operator = ( const char* Symbol ) 
{
    FSymbolType = 2;///< 1: binary, 2: text
    memset( FData, ' ', TMP_SYMBOL_LEN );
    FData[ TMP_SYMBOL_LEN ] = 0;
    if( strlen( Symbol ) > TMP_SYMBOL_LEN )
        memcpy( FData, Symbol, TMP_SYMBOL_LEN );
    else
        memcpy( FData, Symbol, strlen( Symbol ) );
    return *this;
}
//------------------------------------------------------------------------------------------------------
TMPSymbol& TMPSymbol::operator = ( const TMPSymbol& ref )
{
    FSymbolType = ref.FSymbolType;           
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {
        FLeg1ID   = ref.FLeg1ID.ToInt16();
        FLeg2ID   = ref.FLeg2ID.ToInt16();
        FLeg1Side = ref.FLeg1Side;
        FLeg2Side = ref.FLeg2Side;
        FCombOP   = ref.FCombOP;
    }
    else
        memcpy( FData, ref.FData, TMP_SYMBOL_LEN );
    return *this;
}
//------------------------------------------------------------------------------------------------------
BOOL TMPSymbol::GetBinSymbol( UInt8&  Type,
                              UInt8   BS_Code,  
                              UInt16& Leg1ID, UInt8& Leg1Side, 
                              UInt16& Leg2ID, UInt8& Leg2Side,
                              UInt8& CombOP )
{
    Type = FSymbolType;
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {
        Leg1ID   = (UInt16)FLeg1ID.ToInt16();
        Leg2ID   = (UInt16)FLeg2ID.ToInt16();
        Leg1Side = FLeg1Side;
        Leg2Side = FLeg2Side;
        CombOP   = FCombOP;
        ///< Both Leg2ID and CombOP for a single order should be zero.
        if( (FLeg2Side == 0 && CombOP != 0) || (FLeg2Side != 0 && CombOP == 0 ) )
            return FALSE;
    }    
    else///< 1: binary, 2: text
    {
        UFC::AnsiString Sym( (char*)FData, TMP_SYMBOL_LEN );
        UFC::AnsiString Sym1,Sym2;
        
        Sym.TrimRight();
        switch( Sym.Length() )
        {
            case 5:  ///< Futrues single prod.
            case 10: ///< Option single prod.
                     Leg1ID   = TMP::TMPMessage::GetSymbol( Sym );
                     Leg2ID   = 0;
                     Leg1Side = 0;
                     Leg2Side = 0;
                     CombOP   = 0;
                     if( Leg1ID == 0 )
                         return FALSE;
                     break;
            case 11: ///< Futures Time Spread.New Version 2013/05 for weekly MXF. 
                         ///< Example: MXFC3/MX4C3, MX1C3/MXFC3
                     Sym1     = Sym.SubString( 0, 5 );
                     Sym2     = Sym.SubString( 6, 5 );
                     Leg1ID   = TMP::TMPMessage::GetSymbol( Sym1 );
                     Leg2ID   = TMP::TMPMessage::GetSymbol( Sym2 );
                     Leg1Side = (UInt8)((BS_Code == 1)? 2:1);
                     Leg2Side = BS_Code;
                     CombOP   = 2;
                     if( Leg1ID == 0 || Leg2ID == 0 || Sym[5] != '/')
                         return FALSE;
                     break;                
            case 8:  ///< Futures Time Spread, Length 8  Ex:TXFC9/D9. MXFC3/D3
                     Sym1     = Sym.SubString( 0, 5 );
                     Sym2     = Sym.SubString( 0, 3 ) +  Sym.SubString( 6, 2 );
                     Leg1ID   = TMP::TMPMessage::GetSymbol( Sym1 );
                     Leg2ID   = TMP::TMPMessage::GetSymbol( Sym2 );
		     Leg1Side = (UInt8)((BS_Code == 1)? 2:1);
                     Leg2Side = BS_Code;
                     CombOP   = 2;
                     if( Leg1ID == 0 || Leg2ID == 0 || Sym[5] != '/')
                         return FALSE;
                     break;                
            case 16: ///< Option Price Spread, length 16 Ex:TXO06500/06600C6
                     if( Sym[8] == '/' )
                     {
                         Sym1     = Sym.SubString( 0, 8 ) +  Sym.SubString( 14, 2 );
                         Sym2     = Sym.SubString( 0, 3 ) +  Sym.SubString(  9, 7 );
                         CombOP   = 1;
                     }                        ///< New Version 2012/08/28 for weekly TX Options. 
                     else if( Sym[10] == '/' )///< Option Time Spread , length 16  Ex:TXO06500C6/TX1D6
                     {
                         Sym1     = Sym.SubString( 0, 10 );
                         Sym2     = Sym.SubString( 11, 3 )+  Sym.SubString( 3,5 ) +  Sym.SubString(  14, 2 );                         
                         CombOP   = 2;
                     }
                     else ///< Invalid Symbol.
                     {
                         Sym1        = Sym.SubString( 0, 10 );
                         Sym2        = "XXO12345A1";                         
                         CombOP   = 0;
                     }
                     Leg1ID   = TMP::TMPMessage::GetSymbol( Sym1 );
                     Leg2ID   = TMP::TMPMessage::GetSymbol( Sym2 );
                     Leg1Side = (UInt8)((BS_Code == 1)? 2:1);
                     Leg2Side = BS_Code;                                          
                     if( Leg1ID == 0 || Leg2ID == 0 )
                         return FALSE;
                     break;
            case 18: ///< Strangle, length 18                       
                     Sym1     = Sym.SubString( 0, 10 );
                     Sym2     = Sym.SubString( 0, 3 ) +  Sym.SubString(  11, 7 );
                     Leg1ID   = TMP::TMPMessage::GetSymbol( Sym1 );
                     Leg2ID   = TMP::TMPMessage::GetSymbol( Sym2 );
                     Leg1Side = BS_Code;
                     Leg2Side = BS_Code;
                     CombOP   = 4;
                     if( Leg1ID == 0 || Leg2ID == 0 || Sym[10] != ':')
                         return FALSE;
                     break;                
            case 13: ///< Time Spread, Straddle, Conversion & Reversals.
                     Sym1     = Sym.SubString( 0, 10 );
                     Sym2     = Sym.SubString( 0, 8 ) +  Sym.SubString(  11, 2 );
                     Leg1ID   = TMP::TMPMessage::GetSymbol( Sym1 );
                     Leg2ID   = TMP::TMPMessage::GetSymbol( Sym2 );                
                     Leg2Side = BS_Code;
                     if( Leg1ID == 0 || Leg2ID == 0 )
                         return FALSE;
                     if( Sym[10] == '/' ) ///< Time Spread.
                     {                        
			Leg1Side = (UInt8)((BS_Code == 1)? 2:1);
                        CombOP   = 2;
                     }
                     else if( Sym[10] == ':' ) ///< Straddle.
                     {                        
                        Leg1Side = BS_Code;
                        CombOP   = 3;
                     }
                     else if( Sym[10] == '-' ) ///< Conversion & Reversals
                     {                        
			Leg1Side = (UInt8)((BS_Code == 1)? 2:1);
                        CombOP   = 5;                         
                     }
                     else
                     {
                         CombOP   = 0;                                              
                         return FALSE;
                     }
                     break;
            default: return FALSE;
        }
    }
    return TRUE;
}
//------------------------------------------------------------------------------------------------------
BOOL TMPSymbol::IsMultileg( void )
{
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {    
        if( FLeg2ID.ToInt16() > 0 && FCombOP > 0)
            return TRUE;
    }
    else
    {
        UFC::AnsiString Sym( (char*)FData, TMP_SYMBOL_LEN );

        Sym.TrimRight();
        int Len = Sym.Length();
        if( Len != 5 && Len != 10 )///< Futrues single prod len = 5.
            return TRUE;           ///< Option single prod len = 10.
    }
    return FALSE;
}        
//------------------------------------------------------------------------------------------------------
void TMPSymbol::GetStringSymbol( BOOL IsOptions, BOOL IsOffHour, UInt8& Type, UFC::AnsiString& SymbolOut )
{
    Type = FSymbolType;
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {
        if( FLeg2ID.ToInt16() > 0 && FCombOP > 0)///< Multileg Symbol
        {
            UFC::AnsiString Sym1 = TMP::TMPMessage::GetSymbol( IsOptions, IsOffHour, FLeg1ID.ToInt16());
            UFC::AnsiString Sym2 = TMP::TMPMessage::GetSymbol( IsOptions, IsOffHour, FLeg2ID.ToInt16());

            Sym1.TrimRight();
            Sym2.TrimRight();
            switch( FCombOP )
            {
                case 1:///< Price Spread, length 16  Ex:TXO06500/06600C6
                       Sym1.SetLength( (UInt16)(Sym1.Length() -2) );
                       SymbolOut.Printf("%s/%s",Sym1.c_str(), Sym2.c_str()+3 );
                       break;
                case 2:///< Time Spread , length 13  Ex:TXO06500C6/D6
                       if( Sym1[2] == Sym2[ 2 ] ) 
                           SymbolOut.Printf("%s/%s",Sym1.c_str(), Sym2.c_str() + (Sym2.Length()-2));
                       else ///< Weekly Futures/Options 
                       {
                           if( IsOptions == TRUE)
                           {   ///< New Version 2012/08 for weekly TX Options.
                                ///< Time Spread , length 16  Ex:TXO06500C6/TX1D6
                                SymbolOut.Printf("%s/%s%s",Sym1.c_str(), Sym2.SubString(0,3).c_str(), Sym2.c_str() + (Sym2.Length()-2));
                           }
                           else
                           {    ///< New Version 2013/05 for weekly MX Futures.
                                ///< Time Spread , length 11  Ex:MX1C3/MXFC3 
                               SymbolOut.Printf("%s/%s",Sym1.c_str(), Sym2.c_str() );
                           }
                       }                       
                       break;
                case 3:///< Straddle, length 13      Ex:TXO06500C6:D6
					   SymbolOut.Printf("%s:%s",Sym1.c_str(), Sym2.c_str() + (Sym2.Length()-2));
                       break;
                case 4:///< Strangle, length 18      Ex:TXO06500C6:06400D6                      
                       SymbolOut.Printf("%s:%s",Sym1.c_str(), Sym2.c_str()+3 );
                       break;                    
                case 5:///< Conversion & 
                       ///< Reversals, length 13     Ex:Ex:TXO06500C6-D6  
                       SymbolOut.Printf("%s-%s",Sym1.c_str(), Sym2.c_str() + (Sym2.Length()-2));
                       break;
            }
        }
        else ///< Single
            SymbolOut = TMP::TMPMessage::GetSymbol( IsOptions, IsOffHour, FLeg1ID.ToInt16());
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation," Convert Index[%d] to text Symbol[%s] ", FLeg1ID.ToInt16() , SymbolOut.c_str() );
    }    
    else ///< String format 
        SymbolOut = UFC::AnsiString( (char*)FData, TMP_SYMBOL_LEN  );
    SymbolOut.PadThis( TMP_SYMBOL_LEN , ' ');
}
//------------------------------------------------------------------------------------------------------    
void TMPSymbol::ToTextMode( BOOL IsOptions, BOOL IsOffHour )
{
    if( FSymbolType == 1 )///< 1: binary, 2: text
    {
        UInt8           OrgType;
        UFC::AnsiString TextSymbol;        
        
        GetStringSymbol( IsOptions, IsOffHour, OrgType, TextSymbol );
        strcpy( (char*)FData, TextSymbol.c_str() );
        FSymbolType = 2;
    }
}
//------------------------------------------------------------------------------------------------------
BOOL TMPSymbol::ToBinaryMode( UInt8 BS_Code )
{
	if( FSymbolType == 2 )///< 1: binary, 2: text
	{
		UInt8   OrgType;
		UInt16  Leg1ID, Leg2ID;
		BOOL 	Succeed;

		Succeed = GetBinSymbol( OrgType, BS_Code, Leg1ID, FLeg1Side, Leg2ID, FLeg2Side, FCombOP );
		FLeg1ID = Leg1ID;
		FLeg2ID = Leg2ID;
		FSymbolType = 1;
		return Succeed;
	}
	else
		return TRUE;
}
//------------------------------------------------------------------------------------------------------
Int32 TMPSymbol::GetYear( char YearChar )
{
    UFC::UDateTime Today;

    int BaseYear = (Today.getYear()/10)*10;
    int YearDigi = Today.getYear()%10;
    int InYearDigi = YearChar - '0';
    int Year;

    if( ( YearDigi == 9 || YearDigi == 8 ) && InYearDigi == 0 ) ///< example This year is 2009
         Year = BaseYear + InYearDigi + 10; ///< Out 2010 (2000 + 0 + 10)
    else if( YearDigi == 0 && ( InYearDigi == 9 || InYearDigi == 8 ) ) ///< example This year is 2010
         Year = BaseYear + InYearDigi - 10; ///< Out 2008 (2010 + 8 - 10)
    else
         Year = BaseYear + InYearDigi;
    return Year;
}
//------------------------------------------------------------------------------------------------------    
void TMPSymbol::GetLegInfo( const UFC::AnsiString& Symbol,UFC::AnsiString& CFICode,UFC::AnsiString& Maturity )
{
    Int32 Month = 0;
    char  CallPut;

    if( Symbol.Length() == 5 ) ///< Futures symbol (XXFMY,XX1MY,XX2MY)
    {
        GetMonth( Symbol[ 3 ], Month, CallPut ); ///< Get Maturity month
        Maturity.Printf( "%04d%02d", GetYear( Symbol[4]), Month );
        CFICode = "FXXXXX";
    }
    else if( Symbol.Length() == 10 ) ///Options Symbol (XXO99999MY,TX199999MY,TX299999MY,TX499999MY,TX599999MY)
    {
        GetMonth( Symbol[ 8 ], Month, CallPut ); ///< Get Maturity month and call/put
        Maturity.Printf( "%04d%02d", GetYear( Symbol[9] ), Month );
        CFICode.Printf( "O%cXXXX", CallPut );
    }
    else ///< Unknown symbol
    {
        UFC::UDateTime Today;
        Maturity.Printf( "%04d%02d", Today.getYear(), Today.getMonth() );
        CFICode = "XXXXXX";
    }
}
//------------------------------------------------------------------------------------------------------------------------
void TMPSymbol::GetMonth( const char MonthCode, Int32& Month, char& CallPut )
{
    switch ( MonthCode )
    {
        case 'A': CallPut = 'C'; Month= 1; break;
        case 'M': CallPut = 'P'; Month= 1; break;
        case 'B': CallPut = 'C'; Month= 2; break;
        case 'N': CallPut = 'P'; Month= 2; break;
        case 'C': CallPut = 'C'; Month= 3; break;
        case 'O': CallPut = 'P'; Month= 3; break;
        case 'D': CallPut = 'C'; Month= 4; break;
        case 'P': CallPut = 'P'; Month= 4; break;
        case 'E': CallPut = 'C'; Month= 5; break;
        case 'Q': CallPut = 'P'; Month= 5; break;
        case 'F': CallPut = 'C'; Month= 6; break;
        case 'R': CallPut = 'P'; Month= 6; break;
        case 'G': CallPut = 'C'; Month= 7; break;
        case 'S': CallPut = 'P'; Month= 7; break;
        case 'H': CallPut = 'C'; Month= 8; break;
        case 'T': CallPut = 'P'; Month= 8; break;
        case 'I': CallPut = 'C'; Month= 9; break;
        case 'U': CallPut = 'P'; Month= 9; break;
        case 'J': CallPut = 'C'; Month= 10;break;
        case 'V': CallPut = 'P'; Month= 10;break;
        case 'K': CallPut = 'C'; Month= 11;break;
        case 'W': CallPut = 'P'; Month= 11;break;
        case 'L': CallPut = 'C'; Month= 12;break;
        case 'X': CallPut = 'P'; Month= 12;break;
        default:  CallPut = 'X'; Month= 0;break;
    }
}
//------------------------------------------------------------------------------------------------------
BOOL TMPSymbol::GetLegInfo( int LegIndex, UFC::AnsiString& Symbol,UFC::AnsiString& CFICode,UFC::AnsiString& Maturity )
{
    Int16 SymbolIndex;

    if( LegIndex == 0 )///< Get the first leg.
    {
       SymbolIndex = FLeg1ID.ToInt16();
    }
    else if( LegIndex == 1 ) ///< Get the second leg.
    {
       if( IsMultileg() == FALSE )
           return FALSE; ///< It's a Single Order.
       else
           SymbolIndex = FLeg2ID.ToInt16();
    }
    else ///< Error, TAIFEX supports 2 legs only.
        return FALSE;

    Symbol = TMP::TMPMessage::GetSymbol( SymbolIndex );
    Symbol.TrimRight( );
    GetLegInfo( Symbol, CFICode, Maturity );
    return TRUE;
}
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------



