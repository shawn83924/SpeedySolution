#include "jsonxml.h"
#include "osfunc.h"
#include "textfileio.h"

// Get the offset from current position to the next character, in bytes.
static int GetNextCharOffset(const _TCHAR *str, int pos)
{
#ifdef _UNICODE
	
	int len = _tcslen(str);
	if(pos >= len) return 0;

	int mbsadv = 1;

#else

	int len = strlen(str);
	if(pos >= len) return 0;

	int mbsadv = _UTF8_Inc(str + pos);
	
	if(mbsadv == 0) // if it is not UTF-8 string, try multi-byte string...
	{
		for(int i=0; i< len; i += mbsadv)
		{
			const unsigned char *cp1 = (const unsigned char *)&str[i];
			const unsigned char *cp2 = (*cp1 & 0x80)!=0 ? cp1 + 2 : cp1 + 1;//_mbsinc(cp1);
			mbsadv = cp2 - cp1;

			if(i >= pos)
			{
				break;
			}
		}

		if(mbsadv == 0) mbsadv = 1;
	}

#endif

	return mbsadv;
}

///////////////////////////////////////////////////////////////////
// class JsonXML: JSON data parser for XMLNode
///////////////////////////////////////////////////////////////////
// Constructor, init with the target node
JsonXML::JsonXML(XMLNode &node):
m_Node(node),
m_ExpectedEndChar(_T(','))
{
}

// Load from the in-stream object with JSON text data
bool JsonXML::LoadFromStream(InStream &ins)
{
	StdString value;
	bool retval = false;

	if(ReadChar(ins, _T('{')) == false) return false;
	m_ExpectedEndChar = _T('}');

	while(ReadQuote(ins, value) && ReadChar(ins, _T(':')))   // read "xxxx": first
	{
		XMLNode node;
		node.m_Name = Decode(value);
		retval = true;
		int pos = (int)ins.tellg();
		
		if(ReadChar(ins, _T('{'))) // try to read object
		{
			JsonXML json(node);
			ins.clear();
			ins.seekg(pos, std::ios::beg);			
			json.LoadFromStream(ins);
			m_Node.m_ChildNodes.push_back(node);
		}
		else if(ReadChar(ins, _T('['))) // try to read array
		{
			m_ExpectedEndChar = _T(']');

			while(true)
			{
				int pos = (int)ins.tellg();
				if(ReadChar(ins, _T('{')))
				{			
					node.m_ChildNodes.clear();
					JsonXML json(node);
					ins.clear();
					ins.seekg(pos, std::ios::beg);					
					json.LoadFromStream(ins);
					m_Node.m_ChildNodes.push_back(node);
				}
				else if(ReadQuote(ins, value) || ReadValue(ins, value))
				{
					node.m_Text = Decode(value);
					m_Node.m_ChildNodes.push_back(node);
				}
				else break;

				ReadChar(ins, _T(','));
			}

			ReadChar(ins, _T(']'));
			m_ExpectedEndChar = _T('}');
		}
		else if(ReadQuote(ins, value)) // try to read string
		{
			node.m_Text = Decode(value);
			m_Node.m_ChildNodes.push_back(node);
		}
		else if(ReadValue(ins, value))
		{
			if(value != _T("null"))
			{
				node.m_Text = Decode(value);
				m_Node.m_ChildNodes.push_back(node);
			}
		}
		else
		{
			retval = false;
			break;
		}

		ReadChar(ins, _T(','));
	}

	ReadChar(ins, _T('}'));         
	return retval;
}

// Save to the out-stream object with JSON text data
void JsonXML::SaveToStream(OutStream &outs, int indent) const
{
    // prepare the indent string  
	_TCHAR *indent_str = new _TCHAR[indent+1];

	for(int i=0; i< indent; i++) indent_str[i] = _T(' ');
	indent_str[indent] = 0;

    if(m_Node.m_ChildNodes.size()!=0)
    {
		if(m_Node.m_Name.size()!=0)
		{
			outs << indent_str << _T("\"") + Encode(m_Node.m_Name) + _T("\":\r\n");
		}

		outs << indent_str << _T("{\r\n");

		for(unsigned int i=0; i< m_Node.m_ChildNodes.size(); i++)
		{
			StdString key = m_Node.m_ChildNodes[i].m_Name;
			XMLNode *pNode = m_Node.FindNode(key);

			// save unique key/value
			if(m_Node.FindNextNode() == NULL)
			{
				JsonXML json(*pNode);
				if(i!=0) outs << _T(",\r\n");
				json.SaveToStream(outs, indent + 2);
			}
			else if(pNode == &m_Node.m_ChildNodes[i]) // is the first node?
			{
				if(i!=0) outs << _T(",\r\n");
				// save array...
				outs << indent_str << _T("  ") << _T("\"") + key + _T("\":\r\n");
				outs << indent_str << _T("  ") << _T("[\r\n");
				int count = 0;

				XMLNode *pNode = m_Node.FindNode(key);
				while(pNode)
				{
					if(count > 0)
					{
						outs << _T(",\r\n");
					}

					if(pNode->m_ChildNodes.size() != 0)
					{
						StdString name = pNode->m_Name;
						pNode->m_Name.clear(); // don't save name

						JsonXML json(*pNode);
						json.SaveToStream(outs, indent + 4);

						pNode->m_Name = name; // restore name
					}
					else
					{
						outs << indent_str << _T("    \"") + Encode(pNode->m_Text) + _T("\"");
					}
					
					pNode = m_Node.FindNextNode();
					count ++;
				}

				outs << _T("\r\n") << indent_str << _T("  ]");
			}
		}

		outs << _T("\r\n") << indent_str << _T("}");
    }
    else
	{
		// write out single key-value pair
		outs << indent_str << _T("\"") + Encode(m_Node.m_Name) + _T("\":\"") + Encode(m_Node.m_Text) + _T("\"");
	}

	delete [] indent_str;
}

// Load XMLNode from JSON-format file
bool JsonXML::LoadFromFile(const char *filename)
{
	FILE *fp = fopen(filename, "rb");

    if(fp)
    {
		StdString buf;
		if(TextFileIO::Load(fp, buf))
		{
			fclose(fp);

			if(buf.size()!=0)
			{
				InStrStream iss(buf);
				LoadFromStream(iss);
			}

			return true;
		}

		fclose(fp);
    }

    return false;
}

// Save XMLNode to JSON-format file.
bool JsonXML::SaveToFile(const char *filename, bool bUnicode) const
{
    FILE *fp = fopen(filename, "wb");

    if(fp)
    {
		OutStrStream outs;
             
		SaveToStream(outs);
		StdString p = outs.str();	// get the full string of content
		
		TextFileIO::Save(fp, p, bUnicode);

		fclose(fp);
        return true;
    }

    return false;	
}

// Read quoted string from stream
bool JsonXML::ReadQuote(InStream &ins, StdString &value)
{
	_TCHAR buffer[256];
	value.clear();

	int pos = (int)ins.tellg();
            
    do
    {
		ins.clear();
        ins.getline(buffer, 256, _T('\"'));
		value += buffer;
    }
    while(ins.fail() && !ins.eof());

	// check if any break character
	for(size_t i=0; i< value.size();)
	{
		if(value[i] == m_ExpectedEndChar)
		{
			ins.clear();
			ins.seekg(pos + i);			
			return false;
		}
		
		if(value[i]!=_T(' ') && value[i]!=_T('\t') &&
			value[i]!=_T('\n') && value[i]!=_T('\r') && 
			value[i]!=_T(','))
		{
			ins.clear();
			ins.seekg(pos);
			return false;
		}

		int adv = GetNextCharOffset(value.c_str(), i);
		if(adv == 0) break;
		i += adv;
	}

	value.clear();

read_quote_end:

    do
	{
		buffer[0] = 0;
		ins.clear();
	    ins.getline(buffer, 256, _T('\"'));
		value += buffer;
	}
	while(ins.fail() && !ins.eof());

	// check if it is ended with \" and not \\"
	if(value.size() != 0 && value[value.size()-1] == _T('\\') &&
		(value.size() < 2 || (value[value.size()-2] != _T('\\') && 
		GetNextCharOffset(value.c_str(), value.size()-2) == 1)))
	{
		value += '\"';
		goto read_quote_end;
	}

    return ins.good();
}

// Read data until meeting character c
bool JsonXML::ReadChar(InStream &ins, _TCHAR c)
{
	_TCHAR buffer[256];
	StdString value;
	int pos = (int)ins.tellg();
            
    do
    {
		ins.clear();
        ins.getline(buffer, 256, c);
		value += buffer;
    }
    while(ins.fail() && !ins.eof());

	// check if any break character
	for(size_t i=0; i< value.size();)
	{
		if(value[i] == m_ExpectedEndChar && c != m_ExpectedEndChar)
		{
			ins.clear();
			ins.seekg(pos + i);			
			return false;
		}

		if(value[i]!=_T(' ') && value[i]!=_T('\t') &&
			value[i]!=_T('\n') && value[i]!=_T('\r') &&
			value[i]!=_T(','))
		{
			ins.clear();
			ins.seekg(pos);			
			return false;
		}

		int adv = GetNextCharOffset(value.c_str(), i);
		if(adv == 0) break;
		i += adv;
	}

	return ins.good();
}

// Read string until meeting character comma (,)
bool JsonXML::ReadValue(InStream &ins, StdString &value)
{
	_TCHAR buffer[256];
	StdString str;
	int pos = (int)ins.tellg();
            
    do
    {
		ins.clear();
        ins.getline(buffer, 256, _T(','));
		str += buffer;
    }
    while(ins.fail() && !ins.eof());

	size_t i= str.find(m_ExpectedEndChar);
	if(i != StdString::npos)
	{
		ins.clear();
		ins.seekg(pos + i);		
		str.resize(i);
	}

	if(i!=0 && ins.good())
	{
		InStrStream ins2(str);
		value.clear();
		ins2 >> value;
		return true;
	}

	return false;
}

static _TCHAR unescapes[] =
{
   _T('\\'), _T('\"'), _T('/'), _T('b'), _T('f'), _T('n'), _T('r'), _T('t'), _T('u'), 0
};

static _TCHAR escapes[] =
{
   _T('\\'), _T('\"'), _T('/'), _T('\b'), _T('\f'), _T('\n'), _T('\r'), _T('\t'), 0, 0
};

// Decode escape characters
StdString JsonXML::Decode(const StdString &str)
{
    StdString text = str;

	_TCHAR unescape_str[4] = {0};
	unescape_str[0] = _T('\\');
	_TCHAR escape_str[2] = {0};

	for(int i=0; unescapes[i]!=0; i++)
	{
		unescape_str[1] = unescapes[i];
		escape_str[0] = escapes[i];
		size_t pos = 0;

		while((pos = text.find(unescape_str, pos))!=text.npos)
		{
			if(unescape_str[1] == _T('u')) // unicode?
			{
				if(pos + 6 <= text.size())
				{
					char digit[8] = {0};
					int wc[2] = {0};
					memcpy(digit, &text[pos + 2], 4); // 4-digit unicode

					if(sscanf(digit, "%x", wc) == 1 && *wc < 0xD800)  // usc2 only for now
					{
						#ifdef _UNICODE
							text.replace(pos, 6, (const wchar_t *)wc);
						#else
							text.replace(pos, 6, _W2A((const wchar_t *)wc));
						#endif
					}
				}
			}
			else
			{
				text.replace(pos, 2, escape_str);
			}

			pos ++;
		}
	}

	return text;
}

// Encode escape characters
StdString JsonXML::Encode(const StdString &str)
{
    StdString text = str;

	_TCHAR unescape_str[4] = {0};
	unescape_str[0] = _T('\\');
	_TCHAR escape_str[2] = {0};

	for(int i=0; escapes[i]!=0; i++)
	{
		unescape_str[1] = unescapes[i];
		escape_str[0] = escapes[i];
		size_t pos = 0;

		while((pos = text.find(escape_str, pos))!=text.npos)
		{
			text.replace(pos, 1, unescape_str);
			pos += 2;
		}
	}

    return text;
}
