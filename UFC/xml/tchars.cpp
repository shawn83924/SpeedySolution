//////////////////////////////////////////////////////////
// File Name: tchars.cpp
//
// Author : Little Chio
// Created: 2013/4/8
//////////////////////////////////////////////////////////

#include "tchars.h"

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>

//======= Conversions between UNICODE and ANSI string =============
std::wstring _A2W(const std::string &as)
{
	std::wstring ws;
	int len = MultiByteToWideChar(CP_ACP, 0, as.c_str(), -1, NULL, 0) - 1; // skip null character

	if(len > 0)
	{
		ws.resize(len);
		MultiByteToWideChar(CP_ACP, 0, as.c_str(), -1, (wchar_t *)ws.c_str(), len);
	}

    return ws;
}

std::string _W2A(const std::wstring &ws)
{
	std::string as;
	int len = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, NULL, 0, NULL, NULL) - 1;
	
	if(len > 0)
	{
		as.resize(len);
		WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, (char *)as.c_str(), len, NULL, NULL);
	}

	return as;
}

std::wstring _A2W(const char *as)
{
	std::wstring ws;
	int len = MultiByteToWideChar(CP_ACP, 0, as, -1, NULL, 0) - 1;

	if(len > 0)
	{
		ws.resize(len);
		MultiByteToWideChar(CP_ACP, 0, as, -1, (wchar_t *)ws.c_str(), len);
	}

    return ws;
}

std::string _W2A(const wchar_t *ws)
{
	std::string as;
	int len = WideCharToMultiByte(CP_ACP, 0, ws, -1, NULL, 0, NULL, NULL) - 1;
	
	if(len > 0)
	{
		as.resize(len);
		WideCharToMultiByte(CP_ACP, 0, ws, -1, (char *)as.c_str(), len, NULL, NULL);
	}

	return as;
}

#else

//======= Conversions between UNICODE and ANSI string =============
std::wstring _A2W(const std::string &as)
{
	std::wstring ws;
	int len = (int)mbstowcs(0, as.c_str(), as.size());
    
	if(len > 0)
	{
		ws.resize(len);
		mbstowcs((wchar_t *)ws.c_str(), as.c_str(), len);
	}
    
    return ws;
}

std::string _W2A(const std::wstring &ws)
{
	std::string as;
	int len = (int)wcstombs(0, ws.c_str(), ws.size());
	
	if(len > 0)
	{
		as.resize(len);
		wcstombs((char *)as.c_str(), ws.c_str(), len);
	}
    
	return as;
}

std::wstring _A2W(const char *as)
{
	std::wstring ws;
	int len = (int)mbstowcs(0, as, strlen(as));
    
	if(len > 0)
	{
		ws.resize(len);
		mbstowcs((wchar_t *)ws.c_str(), as, len);
	}
    
    return ws;
}

std::string _W2A(const wchar_t *ws)
{
	std::string as;
	int len = (int)wcstombs(0, ws, wcslen(ws));
	
	if(len > 0)
	{
		as.resize(len);
		wcstombs((char *)as.c_str(), ws, len);
	}
    
	return as;
}

#endif

//===== Conversions between UNICODE, UTF-8 and UTF-16 string =======

/** convert UTF8 string to wide-character string */
bool _A2W_UTF8(const std::string &as, std::wstring &ws)
{
	unsigned int c = 0;
	unsigned int adv = 0;
	ws.clear();

	for(unsigned int i = 0; i< as.size(); i += adv)
	{
		if((as[i] & 0x80) != 0)
		{
			if((as[i] & 0xF8) == 0xF0) // 4 sequence characters
			{
				if((as[i+1] & 0xC0) != 0x80 ||
					(as[i+2] & 0xC0) != 0x80 ||
					(as[i+3] & 0xC0) != 0x80 )
				{
					// UTF-8 format is invalid
					return false;
				}

				unsigned int c1 = as[i] & 0x07;
				unsigned int c2 = as[i+1] & 0x3F;
				unsigned int c3 = as[i+2] & 0x3F;
				unsigned int c4 = as[i+3] & 0x3F;

				c = c4 | (c3<<6) | (c2<<12) | (c1<<18);
				adv = 4;
			}
			else if((as[i] & 0xF0) == 0XE0) // 3 sequence characters
			{
				if((as[i+1] & 0xC0) != 0x80 ||
					(as[i+2] & 0xC0) != 0x80 )
				{
					// UTF-8 format is invalid
					return false;
				}

				unsigned int c1 = as[i] & 0x0F;
				unsigned int c2 = as[i+1] & 0x3F;
				unsigned int c3 = as[i+2] & 0x3F;
					
				c = c3 | (c2<<6) | (c1<<12);
				adv = 3;
			}
			else if((as[i] & 0xE0) == 0xC0) // 2 sequence characters
			{
				if((as[i+1] & 0xC0) != 0x80)
				{
					// UTF-8 format is invalid
					return false;
				}

				unsigned int c1 = as[i] & 0x1F;
				unsigned int c2 = as[i+1] & 0x3F;

				c = c2 | (c1<<6);
				adv = 2;
			}
			else
			{
				// UTF-8 format is invalid
				return false;
			}
		}
		else
		{
			// ASCII
			c = as[i];
			adv = 1;
		}

		ws += (wchar_t)c;
	}

	return true;
}

/** convert wide-character string to UTF8 string */
void _W2A_UTF8(const std::wstring &ws, std::string &as)
{
	unsigned char uc[8];
	as.clear();

	for(unsigned int i = 0; i< ws.size(); i++)
	{
		unsigned int c = ws[i];

		if(c < 0x80)
		{
			// ASCII
			uc[0] = c;
			uc[1] = 0;
		}
		else if(c < 0x0800)
		{
			// 2 characters
			uc[0] = (c >> 6) | 0xC0;
			uc[1] = (c & 0x3F) | 0x80;
			uc[2] = 0;
		}
		else if(c < 0x10000)
		{
			// 3 characters
			uc[0] = (c >> 12) | 0xE0;
			uc[1] = ((c >> 6) & 0x3F) | 0x80;
			uc[2] = (c & 0x3F) | 0x80;
			uc[3] = 0;
		}
		else
		{
			// 4 characters
			uc[0] = (c >> 18) | 0xF0;
			uc[1] = ((c >> 12) & 0x3F) | 0x80;
			uc[2] = ((c >> 6) & 0x3F) | 0x80;
			uc[3] = (c & 0x3F) | 0x80;
			uc[4] = 0;
		}

		as += (const char *)uc;
	}
}

/** get the next character offset relative to current character */
int _UTF8_Inc(const char *as)
{
	int adv = 0;

	if((as[0] & 0x80) != 0)
	{
		if((as[0] & 0xF8) == 0xF0) // 4 sequence characters
		{
			if((as[1] & 0xC0) != 0x80 ||
				(as[2] & 0xC0) != 0x80 ||
				(as[3] & 0xC0) != 0x80 )
			{
				// UTF-8 format is invalid
				return 0;
			}

			adv = 4;
		}
		else if((as[0] & 0xF0) == 0XE0) // 3 sequence characters
		{
			if((as[1] & 0xC0) != 0x80 ||
				(as[2] & 0xC0) != 0x80 )
			{
				// UTF-8 format is invalid
				return 0;
			}

			adv = 3;
		}
		else if((as[0] & 0xE0) == 0xC0) // 2 sequence characters
		{
			if((as[1] & 0xC0) != 0x80)
			{
				// UTF-8 format is invalid
				return 0;
			}

			adv = 2;
		}
	}
	else
	{
		// ASCII
		adv = 1;
	}

	return adv;
}

/** Check ByteOrderMap signature of UTF-8 and return the length of BOM in bytes. */
int _UTF8_BOM_Inc(const void *data)
{
	static const unsigned char bom[] = { 0xEF, 0xBB, 0xBF, 0 };
	return memcmp(data, bom, 3) == 0 ? 3 : 0;
}

/** convert UTF16 string to wide-character string */
bool _A2W_UTF16(const void *data, unsigned int len, std::wstring &ws)
{
	unsigned int c = 0;
	unsigned int adv = 0;
	unsigned short *as = (unsigned short *)data;
	len /= sizeof(unsigned short);  // convert to characters
	ws.clear();

	for(unsigned int i= 0; i< len; i += adv)
	{
		unsigned short wc = as[i];

		if(wc < 0xD800)
		{
			c = wc;
			adv = 1;
		}
		else
		{
			unsigned short wc2 = as[i+1];
				
			if(wc2 >= 0xDC00)
			{
				unsigned int c1 = wc & 0x03FF;
				unsigned int c2 = wc2 & 0x03FF;
				c = ((c1 << 10) | c2) + 0x10000;
				adv = 2;
			}
			else
			{
				// it should be invalid, or used by UCS2 encoding
				c = wc;
				adv = 1;
			}
		}

		ws += (wchar_t)c;
	}

	return true;
}

/** convert wide-character string to UTF16 string */
unsigned short* _W2A_UTF16(const std::wstring &ws)
{
	unsigned short *as = new unsigned short[ws.size()*2+1];
	unsigned short *p = as;

	for(unsigned int i = 0; i< ws.size(); i++)
	{
		unsigned int c = ws[i];

		if(c < 0x10000)
		{
			// ucs-2
			*p = c;
			p ++;
		}
		else
		{
			// ucs-4
			c -= 0x10000;
			p[0] = (((c >> 10) & 0x03FF) | 0xD800);
			p[1] = ((c & 0x03FF) | 0xDC00);
			p += 2;
		}
	}

	*p = 0;

	return as;
}

/** get the next character offset relative to current character */
int _UTF16_Inc(const void *as)
{
	int adv = 0;

	unsigned short wc = *(unsigned short *)as;

	if(wc < 0xD800)
	{
		adv = 2;  // < U+D7FF
	}
	else
	{
		unsigned short wc2 = *((unsigned short *)as + 1);
				
		if(wc2 >= 0xDC00)
		{
			adv = 4;  // > U+D800
		}
	}

	return adv;
}

/** Check ByteOrderMap signature of UTF-16 and return the length of BOM in bytes. */
int _UTF16_BOM_Inc(const void *data)
{
	static const unsigned char bom[] = { 0xFF, 0xFE, 0 };
	return memcmp(data, bom, 2) == 0 ? 2 : 0;
}
