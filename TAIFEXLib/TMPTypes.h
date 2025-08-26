// 
// File:   TMPTypes.h
// Author: Simon Chang
//
// Created on June 22, 2008, 4:11 PM
//
//------------------------------------------------------------------------------------------------------
#ifndef _TMPTYPES_H
#define	_TMPTYPES_H
//------------------------------------------------------------------------------------------------------
#include "TMPType.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------
// Class TMPChar
// Wrap char type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPChar: public TMPType
{
private:    
    char FData;
public:
    TMPChar( void );
    TMPChar( char ch );
    TMPChar( const UFC::AnsiString& Str );
    TMPChar( UFC::PStream* Stream );
    TMPChar( const TMPChar& Val ):TMPType(ttChar),FData( Val.FData ){}                
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );    
    virtual Int32    ToString( char* Str, UInt32 DataLength );    
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return sizeof( char ); }
public: ///< Operator.
    TMPChar& operator = ( char Val );
    TMPChar& operator = ( const TMPChar& ref );
    operator char ( void ) { return FData; }
};
//------------------------------------------------------------------------------------------------------
// Class TMPUInt8
// Wrap UInt8 type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPUInt8: public TMPType
{
private:    
	UInt8 FData;
public:
    TMPUInt8( void );
    TMPUInt8( UInt8 Val );
    TMPUInt8( const UFC::AnsiString& Str );
    TMPUInt8( UFC::PStream* Stream );
    TMPUInt8( const TMPUInt8& Val ):TMPType(ttUInt8),FData( Val.FData ){}            
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );    
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32 GetSize( void  ) { return sizeof( UInt8 ); }
public: ///< Operator.
    TMPUInt8& operator = ( UInt8 Val );
    TMPUInt8& operator = ( const TMPUInt8& ref );
    operator UInt8 ( void )  { return FData; }
    UInt8    ToUInt8( void ) { return FData; }
};
//------------------------------------------------------------------------------------------------------
// Class TMPInt8
// Wrap Int8 type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPInt8: public TMPType
{
private:    
    Int8 FData;
public:
    TMPInt8( void );
    TMPInt8( Int8 Val );
    TMPInt8( const UFC::AnsiString& Str );
    TMPInt8( UFC::PStream* Stream );
    TMPInt8( const TMPInt8& Val ):TMPType(ttInt8),FData( Val.FData ){}        
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );    
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return sizeof( Int8 ); }
public: ///< Operator.
    TMPInt8& operator = ( Int8 Val );
    TMPInt8& operator = ( const TMPInt8& ref );
    operator Int8 ( void )  { return FData; }
    Int8     ToInt8( void ) { return FData; }
};
//------------------------------------------------------------------------------------------------------
// Class TMPCharArray
// Wrap Char array type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPCharArray: public TMPType
{
private:    
    UInt8*          FData;
    BOOL            FDynamicLen;
    Int32           FOtherFieldLen;
    BOOL            FIsBinary;    
    UInt16          FDataLen;        
private:    
    UInt8 HexChToUInt8( const char Ch );
    void  FromHexString( const char* Str, UInt8* Data, Int32 Len );
    BOOL  IsHexString( const UInt8* Data, int DataLen );
public:    
    void  SetBinaryData( const UInt8* Data, int DataLen );
    void  SetTextData( const char* Str, int StrLen );
public:
    TMPCharArray( Int32 Len, BOOL IsBin );
    TMPCharArray( const UFC::AnsiString& Str, BOOL IsBin );
    TMPCharArray( UFC::PStream* Stream, Int32 Len );
    TMPCharArray( const TMPCharArray& ref );
    ~TMPCharArray( void );
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );   
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return FDataLen; }
    void             SetBinary( BOOL IsBin ) { FIsBinary = IsBin; }
public: ///< Operator.
    TMPCharArray& operator = ( const char* Val );
    TMPCharArray& operator = ( const TMPCharArray& ref );
    operator const char* ( void );
};
//------------------------------------------------------------------------------------------------------
// Class TMPInt16
// Wrap Int16 type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPInt16: public TMPType
{
private:    
    UFC::NInt16 FData;
public:
    TMPInt16( void );
    TMPInt16( Int16 Val );
    TMPInt16( const UFC::AnsiString& Str );
    TMPInt16( UFC::PStream* Stream );
    TMPInt16( const TMPInt16& Val ):TMPType(ttInt16),FData( Val.FData.ToInt16() ){}        
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return sizeof(Int16); }    
public: ///< Operator.
    TMPInt16& operator = ( Int16 Val );
    TMPInt16& operator = ( const TMPInt16& ref );
    operator Int16 ( void ) { return FData.ToInt16(); }
};
//------------------------------------------------------------------------------------------------------
// Class TMPUInt16
// Wrap UInt16 type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPUInt16: public TMPType
{
private:    
    UFC::NInt16 FData;
public:
    TMPUInt16( void );
    TMPUInt16( UInt16 Val );
    TMPUInt16( const UFC::AnsiString& Str );
    TMPUInt16( UFC::PStream* Stream );
    TMPUInt16( const TMPUInt16& Val ):TMPType(ttUInt16),FData( (UInt16)Val.FData.ToInt16() ){}    
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return sizeof(UInt16); }    
public: ///< Operator.
    TMPUInt16& operator = ( UInt16 Val );
    TMPUInt16& operator = ( const TMPUInt16& ref );
    operator UInt16 ( void ) { return (UInt16)FData.ToInt16(); }
};
//------------------------------------------------------------------------------------------------------
// Class TMPInt32
// Wrap Int32 type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPInt32: public TMPType
{
private:    
    UFC::NInt32 FData;
public:
    TMPInt32( void );
    TMPInt32( Int32 Val );
    TMPInt32( const UFC::AnsiString& Str );
    TMPInt32( UFC::PStream* Stream );
    TMPInt32( const TMPInt32& Val ):TMPType(ttInt32),FData( Val.FData.ToInt32() ){}
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );    
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return sizeof(Int32); }
public: ///< Operator.
    TMPInt32& operator = ( Int32 Val );
    TMPInt32& operator = ( const TMPInt32& ref );
    operator Int32 ( void )  { return FData.ToInt32(); }
};
//------------------------------------------------------------------------------------------------------
// Class TMPUInt32
// Wrap UInt32 type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPUInt32: public TMPType
{
private:    
    UFC::NInt32 FData;
public:
    TMPUInt32( void );
    TMPUInt32( UInt32 Val );
    TMPUInt32( const UFC::AnsiString& Str );
    TMPUInt32( UFC::PStream* Stream );
    TMPUInt32( const TMPUInt32& Val ):TMPType(ttUInt32),FData((UInt32)Val.FData.ToInt32() ){}
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return sizeof(UInt32); }    
public: ///< Operator.
    TMPUInt32& operator = ( UInt32 Val );
    TMPUInt32& operator = ( const TMPUInt32& ref );
    operator UInt32 ( void ) { return (UInt32)FData.ToInt32(); }
};
//------------------------------------------------------------------------------------------------------
// Class TMPInt64
// Wrap Int64 type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPInt64: public TMPType
{
private:    
    UFC::NInt64 FData;
public:
    TMPInt64( void ):TMPType(ttInt64),FData( 0 ){}
    TMPInt64( Int64 Val ):TMPType(ttInt64),FData( Val ){}
    TMPInt64( const UFC::AnsiString& Str );
    TMPInt64( UFC::PStream* Stream );
    TMPInt64( const TMPInt64& Val ):TMPType(ttInt64),FData( Val.FData.ToInt64() ){}
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return FData.ToInt64(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return sizeof(Int64); }    
public: ///< Operator.
    TMPInt64& operator = ( Int64 Val );
    TMPInt64& operator = ( const TMPInt64& ref );
    operator Int64 ( void ) { return FData.ToInt64(); }
};
//------------------------------------------------------------------------------------------------------
class TMPUInt64: public TMPType
{
private:    
    UFC::NInt64 FData;
public:
    TMPUInt64( void ):TMPType(ttUInt64),FData( 0 ){}
    TMPUInt64( UInt64 Val ):TMPType(ttUInt64),FData( (Int64)Val ){}
    TMPUInt64( const UFC::AnsiString& Str );
    TMPUInt64( UFC::PStream* Stream );
    TMPUInt64( const TMPUInt64& Val ):TMPType(ttUInt64),FData( Val.FData.ToInt64() ){}
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return FData.ToInt64(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return sizeof(UInt64); }    
public: ///< Operator.
    TMPUInt64& operator = ( UInt64 Val );
    TMPUInt64& operator = ( const TMPUInt64& ref );
    operator UInt64 ( void ) { return FData.ToInt64(); }
};
//------------------------------------------------------------------------------------------------------
// Class TMPTime
// Wrap Time type for TMP protocol
//------------------------------------------------------------------------------------------------------
class TMPTime: public TMPType
{
private:    
    UFC::NInt32 FTime;
    UFC::NInt16 Fms;
public:
    TMPTime( void );
    TMPTime( Int32 epoch, UInt16 ms );
    TMPTime( Int8 HH, Int8 MM, Int8 SS, Int16 ms );    
    TMPTime( const UFC::AnsiString& Str );
    TMPTime( UFC::PStream* Stream );
    TMPTime( const TMPTime& Time );
public:    
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void ){ return (Int64)ToInteger(); }
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 ); 
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return 6; }    
    TMPTime& operator = ( const TMPTime& ref );    
    Int32            DiffMS( const TMPTime& PassTime );
    void             CurrentTime( void );
    Int32            GetHour( void ) const;
    Int32            GetMinute( void ) const;
    Int32            GetSecond( void ) const;
    Int32            GetMillisecond( void ) const;
};
//------------------------------------------------------------------------------------------------------
class TMPTimeEx: public TMPType
{
private:    
    UFC::NInt32 FTime;
    UFC::NInt32 Fns;
public:
    TMPTimeEx( void );
    TMPTimeEx( Int32 epoch, UInt32 ns );
    TMPTimeEx( Int8 HH, Int8 MM, Int8 SS, UInt32 ns );    
    TMPTimeEx( const UFC::AnsiString& Str );
    TMPTimeEx( UFC::PStream* Stream );
    TMPTimeEx( const TMPTimeEx& Time );
public:    
    virtual Int32    ToBinary( UInt8* Buffer );    
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void );
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 ); 
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return 8; }    
    TMPTimeEx& operator = ( const TMPTimeEx& ref );    
    Int64            DiffNS( const TMPTimeEx& PassTime );
    void             CurrentTime( void );
    Int32            GetHour( void ) const;
    Int32            GetMinute( void ) const;
    Int32            GetSecond( void ) const;
    UInt32           GetNanoSecond( void ) const;
};
//------------------------------------------------------------------------------------------------------
// Class TMPSymbol
// Wrap Char array type for TMP protocol
//------------------------------------------------------------------------------------------------------
#define TMP_SYMBOL_LEN 20
#define TMP_LONG_SYMBOL_LEN 40
#define TMP_BIN_SYMBOL_LEN 7
//------------------------------------------------------------------------------------------------------
typedef enum
{
    lsBuy  = 1,
    lsSell = 2
            
} TMPLegSide;
//------------------------------------------------------------------------------------------------------
typedef enum
{
    coSingle      = 0,
    coPriceSpread = 1,
    coTimeSpread  = 2,
    coStraddle    = 3,
    coStrangle    = 4,
    coConversion  = 5
            
} TMPCombOP;
//------------------------------------------------------------------------------------------------------
class TMPSymbol: public TMPType
{
private:  ///< For binary format
    UInt8  FSymbolType;   //-------- Field: symbol_type 1,binary 2,text    
private:  
    UInt8 FData[ 21 ];    //-------- Field: sym (For String format )
private:  
    UFC::NInt16 FLeg1ID;  //----|
    UFC::NInt16 FLeg2ID;  //    |
    UInt8  FLeg1Side;     //    |--- Field: sym (For binary format)
    UInt8  FLeg2Side;     //    |
    UInt8  FCombOP;       //    |
    /// Filler uint8[ 13 ]//----|      
public:
    ///< Constructor of String symbol.    
    TMPSymbol( const UFC::AnsiString& Symbol );
    ///< Constructor of binary symbol.
    TMPSymbol( const Int16 Leg1ID, TMPLegSide Leg1Side, const Int16 Leg2ID,TMPLegSide Leg2Side,TMPCombOP ComboOP );
    ///< Construct object from stream
    TMPSymbol( UFC::PStream* Stream );    
    TMPSymbol( UInt8 Type, UInt8* Buffer );
    TMPSymbol( const TMPSymbol& Symbol );    
public:     
    virtual Int32    ToBinary( UInt8* Buffer );
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void );
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return TMP_SYMBOL_LEN + 1; }    
    void SetSymbol( const Int16 Leg1ID, TMPLegSide Leg1Side, const Int16 Leg2ID,TMPLegSide Leg2Side,TMPCombOP ComboOP );
    BOOL IsBinSymbol( void ) { return ( FSymbolType == 1 ); }
    BOOL IsMultileg( void );
    TMPSymbol& operator = ( const TMPSymbol& ref );    
    TMPSymbol& operator = ( const char* Symbol );
    BOOL GetBinSymbol( UInt8& Type, UInt8 Side, UInt16& Leg1Seq, UInt8& Leg1Side, UInt16& Leg2Seq, UInt8& Leg2Side, UInt8& CombOP );
    void GetStringSymbol( BOOL IsOptions, BOOL IsOffHour, UInt8& Type,UFC::AnsiString& Symbol );
    void ToTextMode( BOOL IsOptions, BOOL IsOffHour);
    BOOL ToBinaryMode( UInt8 BS_Code = 1 );
    BOOL GetLegInfo( int LegIndex, UFC::AnsiString& Symbol,UFC::AnsiString& CFICode,UFC::AnsiString& Maturity );
private:
    void  GetLegInfo( const UFC::AnsiString& Symbol, UFC::AnsiString& CFICode, UFC::AnsiString& Maturity );
    void  GetMonth( const char MonthCode, Int32& Month, char& CallPut );
    Int32 GetYear( char YearChar );
};
//------------------------------------------------------------------------------------------------------
class TMPLongSymbol: public TMPType
{
private:  ///< For binary format
    UInt8  FSymbolType;   //-------- Field: symbol_type 3,binary 4,text    
private:  
    UInt8 FData[ 41 ];    //-------- Field: sym (For Long String format )
private:  
    UFC::NInt16 FLeg1ID;  //----|
    UFC::NInt16 FLeg2ID;  //    |
    UInt8  FLeg1Side;     //    |--- Field: sym (For binary format)
    UInt8  FLeg2Side;     //    |
    UInt8  FCombOP;       //    |
    /// Filler uint8[ 33 ]//----|      
public:
    ///< Constructor of String symbol.    
    TMPLongSymbol( const UFC::AnsiString& Symbol );
    ///< Constructor of binary symbol.
    TMPLongSymbol( const Int16 Leg1ID, TMPLegSide Leg1Side, const Int16 Leg2ID,TMPLegSide Leg2Side,TMPCombOP ComboOP );
    ///< Construct object from stream
    TMPLongSymbol( UFC::PStream* Stream );    
    TMPLongSymbol( const TMPLongSymbol& Symbol );    
public:     
    virtual Int32    ToBinary( UInt8* Buffer );
    virtual Int32    ToStream( UFC::PStream* Stream );
    virtual Int32    ToString( char* Str, UInt32 DataLength );
    virtual Int32    ToInteger( void );
    virtual Int64    ToInteger64( void );
    virtual void     FromString( const UFC::AnsiString& String );
    virtual void     FromInteger( Int32 );    
    virtual TMPType* FromStream( UFC::PStream* Stream  );
    virtual TMPType* Duplicate( void  );
    virtual void     Assign( TMPType& Src );
    virtual Int32    GetSize( void  ) { return TMP_LONG_SYMBOL_LEN + 1; }    
    void SetSymbol( const Int16 Leg1ID, TMPLegSide Leg1Side, const Int16 Leg2ID,TMPLegSide Leg2Side,TMPCombOP ComboOP );
    BOOL IsBinSymbol( void ) { return ( FSymbolType == 3 ); }
    BOOL IsMultileg( void );
    TMPLongSymbol& operator = ( const TMPLongSymbol& ref );    
    TMPLongSymbol& operator = ( const char* Symbol );
    BOOL GetBinSymbol( UInt8& Type, UInt8 Side, UInt16& Leg1Seq, UInt8& Leg1Side, UInt16& Leg2Seq, UInt8& Leg2Side, UInt8& CombOP );
    void GetStringSymbol( BOOL IsOptions, BOOL IsOffHour, UInt8& Type,UFC::AnsiString& Symbol );
    void ToTextMode( BOOL IsOptions, BOOL IsOffHour );
    BOOL ToBinaryMode( UInt8 BS_Code = 1 );
    BOOL GetLegInfo( int LegIndex, UFC::AnsiString& Symbol,UFC::AnsiString& CFICode,UFC::AnsiString& Maturity );
private:
    void  GetLegInfo( const UFC::AnsiString& Symbol, UFC::AnsiString& CFICode, UFC::AnsiString& Maturity );
    void  GetMonth( const char MonthCode, Int32& Month, char& CallPut );
    Int32 GetYear( char YearChar );
};
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------
#endif	/* _TMPTYPES_H */

