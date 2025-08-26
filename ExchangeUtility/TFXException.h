//--------------------------------------------------------------------------------------------------------------------
#ifndef __TFX_EXCEPTION_H
#define __TFX_EXCEPTION_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "../UFC/Exception.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//using name space UFCType;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25Exception : public UFC::Exception
{
private:    
    void ErrorToWhat( int ErrCode, const char* Detail )
    { 
        sprintf( Message, " Connection Error:(%d) Reason:%s",ErrCode, Detail );
    }        
public:
    X25Exception( int ErrCode, const char* Str )
    :UFC::Exception( )
    { 
       ErrorToWhat(ErrCode,Str); 
    }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25ReconnectException : public UFC::Exception
{
private:     
    void ErrorToWhat( const char* Detail )
    {
        sprintf( Message, " Reconnect exception:%s", Detail );			
    }    
public:
    X25ReconnectException( const char* Str )
    :UFC::Exception( )
    {
        ErrorToWhat( Str );
    }	
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25RehandshakeException : public UFC::Exception
{
private:
    UFCType::UInt8 FStatusCode;    
private:    
    void ErrorToWhat( const char* Detail )
    {
       sprintf( Message, " Re-handshake exception:%s", Detail );
    }
public:
    X25RehandshakeException( const char* Str, UFCType::UInt8 Err = 0 )
    :UFC::Exception( )
    ,FStatusCode( Err )
    {
        ErrorToWhat( Str );
    }	
    UFCType::UInt8 GetErrorCode( void ) {return FStatusCode;}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPRehandshakeException : public UFC::Exception
{
private:
    UFCType::UInt8 FStatusCode;    
private:    
    void ErrorToWhat( const char* Detail )
    {		
	sprintf( Message, " TMP Re-handshake exception:%s", Detail );
    }
public:
    TMPRehandshakeException( const char* Str, UFCType::UInt8 Code = 0 )
    :UFC::Exception( )
    ,FStatusCode( Code )
    {
        ErrorToWhat( Str );
    }	
	UFCType::UInt8 GetErrorCode( void ) {return FStatusCode;}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MarketClosedException : public UFC::Exception
{
public:
    MarketClosedException( const char* Str ):UFC::Exception( Str ){}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CCHException : public UFC::Exception
{
public:
    CCHException( const char* Str ):UFC::Exception( Str ){}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ConfigFileException : public UFC::Exception
{
public:
    ConfigFileException( const char* Str ):UFC::Exception( Str ){}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TMP Exception
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPBufferException : public UFC::Exception
{
private:        
    void ErrorToWhat( int NeedSize )
    {		
	sprintf( Message, " TMP Buffer size not enough, Require:%d", NeedSize );
    }
public:
    TMPBufferException( int NeedSize )
    :UFC::Exception( )
    {
        ErrorToWhat( NeedSize );
    }	
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPConfigException : public UFC::Exception
{
private:    
    void ErrorToWhat( const char* FileName )
    {
        sprintf( Message, " File[%s] not a valid TMP format file", FileName );    
    }
public:
    TMPConfigException( const char* FileName )
    :UFC::Exception( )
    {
        ErrorToWhat( FileName );
    }	
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPFieldException : public UFC::Exception
{
private:    
    void ErrorToWhat( const char* FieldVal )
    {                
        sprintf( Message, " Data[%s] is not a valid field value", FieldVal );
    }    
public:
    TMPFieldException( const char* FieldVal )
    :UFC::Exception( )
    {
        ErrorToWhat( FieldVal );
    }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPCheckSumException : public UFC::Exception
{
private:    
    void ErrorToWhat( UFCType::UInt8 Src, UFCType::UInt8 Right )
    {           
        sprintf( Message, " TMP checksum error, should be[%d] but get[%d]",Right,Src);
    }    
public:
    TMPCheckSumException( UFCType::UInt8 Src, UFCType::UInt8 Right )
    :UFC::Exception( )
    {
        ErrorToWhat( Src, Right );
    }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPDataErrorException : public UFC::Exception
{
private:    
    void ErrorToWhat( UFCType::UInt16 Size, UFCType::UInt16 RecvSize )
    {
        sprintf( Message, " TMP message length should be[%d] but only received[%d] bytes.",Size,RecvSize);
    }    
public:
    TMPDataErrorException( UFCType::UInt16 Size, UFCType::UInt16 RecvSize )
    :UFC::Exception( )
    {
        ErrorToWhat( Size, RecvSize );
    }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPStringParserException : public UFC::Exception        
{
public:
	TMPStringParserException( const char* Msg ):UFC::Exception( Msg ){}
};    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPFieldNotFountException : public UFC::Exception        
{
private:    
    void ErrorToWhat( const char* FieldName )
    {        
        sprintf( Message, " Field[%s] not found in this TMP message", FieldName );    
    }
    void ErrorToWhat( UFCType::Int32 Index )
    {
        sprintf( Message, " Field index[%d] not found in this TMP message", Index );    
    }    
public:
    TMPFieldNotFountException( const char* FieldName )
    :UFC::Exception( )
    {
        ErrorToWhat( FieldName );
    }
    TMPFieldNotFountException( UFCType::Int32 Index )
    :UFC::Exception( )
    {
        ErrorToWhat( Index );
    }
};    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPFieldTypeException : public UFC::Exception        
{
private:    
    void ErrorToWhat( const char* ValueType, const char* FieldType )
    {   
        sprintf( Message, " Can not assign value type[%s] to TMP field type [%s].", ValueType,FieldType);    
    }    
public:
    TMPFieldTypeException( const char* ValType, const char* FieldType  )
    :UFC::Exception( )
    {
        ErrorToWhat( ValType,  FieldType );
    }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPValueRangeException : public UFC::Exception        
{
private:    
    void ErrorToWhat(  UFCType::Int32 Val, const char* FieldType )
    {           
        sprintf( Message, " Value[%d] out of TMP type [%s] range.", Val, FieldType );    
    }    
public:    
    TMPValueRangeException( UFCType::Int32 Val, const char* FieldType )
    :UFC::Exception( )
    {
        ErrorToWhat( Val,  FieldType );
    }	
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPIndexOutOfBoundException : public UFC::Exception        
{
private:    
    void ErrorToWhat( UFCType::Int32 Index, UFCType::Int32 Max )
    {
        sprintf( Message, " Index[%d] out of bound, should between 0~%d",Index, Max);    
    }    
public:    
    TMPIndexOutOfBoundException( UFCType::Int32 Index, UFCType::Int32 Max  )
    :UFC::Exception( )
    {
        ErrorToWhat( Index, Max );
    }	
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

