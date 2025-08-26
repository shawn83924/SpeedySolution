//---------------------------------------------------------------------------

#ifndef TFixedFormatH
#define TFixedFormatH
//---------------------------------------------------------------------------
#include "TDataFormat.h"
#include "iniFile.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class TFixedFormat : public TDataFormat
{
protected:
    Int32 FLineLength;
    UFC::AnsiString FErrorMsg;

protected:
    void InitialFromIniFile(UiniFile* pIniFile);

private:
    static const UFC::AnsiString DEFAULT_CONFIG_SECTION_NAME;
    void LoadConfigSetting(UiniFile* pIniFile);
    void LoadFieldSetting(UiniFile* pIniFile);
    virtual void LoadConfigSettingEx(UiniFile* pIniFile) {}
    virtual bool IsConfigSection(const UFC::AnsiString& Value) { return false; }

private:
    TFieldDataType ReadDataType( Section* pSection );
    void CopyFieldValueToBuffer( TFieldOffset* pFieldOffset, const UFC::AnsiString& FieldValue, char* Buff );
    void FillZeroChar( UFC::AnsiString& Value, Int32 Size );

public:
    TFixedFormat();
    TFixedFormat(const UFC::AnsiString& IniFilePath);
    virtual ~TFixedFormat();

public:
    UFC::AnsiString GetErrorMsg() { return FErrorMsg; };
    
    virtual bool Parse( const char* inBUFF, int Size, TRecord* outREC );
    virtual void LoadStreamToStringList( PStream* pStream, PtrList<AnsiString>* pStringList );
    virtual AnsiString Render( TRecord* inREC );

public:
    bool SetIniFile(const UFC::AnsiString& IniFilePath);
    void SetLineLength(Int32 Value) { FLineLength = Value; };
    Int32 GetLineLength() { return FLineLength; };
    const UFC::AnsiString& GetDefaultConfigSectionName();
    BOOL IsValidStream( UFC::PStream* );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
