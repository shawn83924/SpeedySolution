//////////////////////////////////////////////////////////
// File Name: textfileio.cpp
//
// Author : Jerry Chio
// Created: 2013/4/8
//
//////////////////////////////////////////////////////////

#include "textfileio.h"
#include "osfunc.h"

//////////////////////////////////////////////////////////////////////////
// class TextFileIO: Text file reader/writer, handling UTF-8, UCS and MBS
/////////////////////////////////////////////////////////////////////////

bool TextFileIO::Load(FILE *fp, StdString &buf)
{
	fseek(fp, 0, SEEK_SET);

	// check if this file is unicoded...
	unsigned char header[4] = {0};
	fread(&header, 1, sizeof(header), fp);

	fseek(fp, 0, SEEK_END);
	long dataSize = ftell(fp);
	int headerSize = 0;

	if((headerSize = _UTF8_BOM_Inc(header)) != 0)
	{
		dataSize -= headerSize;
		fseek(fp, headerSize, SEEK_SET);

		char *as = new char[dataSize + 1];
		fread(as, 1, dataSize, fp);
		as[dataSize] = 0;

#if defined(_UNICODE)

		_A2W_UTF8(as, buf);

#elif !defined(_WIN32)

		// read UTF-8 directly
		buf = as;
#else
		// convert UTF-8 to multi-byte
		std::wstring ws;
		if(_A2W_UTF8(as, ws))
		{
			buf = _W2A(ws);
		}
#endif
		delete [] as;
	}
	else if((headerSize = _UTF16_BOM_Inc(header)) != 0)
	{
		dataSize -= headerSize;
		fseek(fp, headerSize, SEEK_SET);

		char *as = new char[dataSize + 1];
		fread(as, 1, dataSize, fp);
		as[dataSize] = 0;

#if defined(_UNICODE)

		_A2W_UTF16(as, dataSize, buf);

#elif !defined(_WIN32)
		
		// convert to UTF-8
		std::wstring ws;
		if(_A2W_UTF16(as, dataSize, ws))
		{
			_W2A_UTF8(ws, buf);
		}
#else
		// convert UTF-16 to multi-byte
		std::wstring ws;
		if(_A2W_UTF16(as, dataSize, ws))
		{
			buf = _W2A(ws);
		}
#endif
		delete [] as;
	}
	else
	{
		fseek(fp, 0, SEEK_SET);

		char *as = new char[dataSize + 1];
		fread(as, 1, dataSize, fp);
		as[dataSize] = 0;

#if defined(_UNICODE)

		buf = _A2W(as);

#elif !defined(_WIN32)

		// convert to UTF-8
		std::wstring ws = _A2W(as);
		_W2A_UTF8(ws, buf);

#else
		buf = as;
#endif
		delete [] as;
			
	}

	return buf.size() != 0 || dataSize == 0;
}

void TextFileIO::Save(FILE *fp, const StdString &buf, bool bUnicode)
{
	if(bUnicode)
	{
		// write out the header of UTF-8 file
		int header = 0xbfbbef;
		fwrite(&header, 1, 3, fp);

#ifdef _UNICODE

		// convert to UTF-8
		std::string as;
		_W2A_UTF8(buf, as);
		
		if(as.size()!=0)
		{
			fwrite(as.c_str(), sizeof(char), as.size(), fp);
		}

#elif !defined(_WIN32)

		// already UTF-8 format, write directly
		fwrite(buf.c_str(), sizeof(_TCHAR), buf.size(), fp);

#else
		// need to convert mbcs to UTF-8
		std::wstring ws = _A2W(buf);
		if(ws.size()!=0)
		{
			std::string as;
			_W2A_UTF8(ws, as);
			fwrite(as.c_str(), sizeof(char), as.size(), fp);
		}
#endif
	}
	else
	{
#ifdef _UNICODE

		// need to convert wcs to mbcs
		std::string as = _W2A(buf);
		if(as.size()!=0)
		{
			fwrite(as.c_str(), sizeof(char), as.size(), fp);
		}

#elif !defined(_WIN32)

		// need to convert UTF-8 to mbcs
		std::wstring ws;
		if(_A2W_UTF8(buf, ws))
		{
			std::string as = _W2A(ws);
			if(as.size()!=0)
			{
				fwrite(as.c_str(), sizeof(char), as.size(), fp);
			}
		}
#else
		// write mbcs directly
		fwrite(buf.c_str(), sizeof(_TCHAR), buf.size(), fp);
#endif
	}
}


