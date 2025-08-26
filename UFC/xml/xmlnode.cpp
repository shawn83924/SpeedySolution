//////////////////////////////////////////////////////////
// File Name: xmlnode.cpp
//
// Author : Little Chio
// Created : 2002/7/15
// Last Modified : 2009/3/13
//
// Copyright (c) 2002-03, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#include "xmlnode.h"
#include "osfunc.h"
#include "textfileio.h"

// Helper function : load a string chunck from file
static void LoadStringChunkFromFile(StdString &str, FILE *fp)
{
	unsigned int count = 0;
	fread(&count, 4, 1, fp);

	bool bUnicode = ((count>>24)==0xff);
	count &= 0x00ffffff;

	if(count > 0)
	{
		char *buf = new char[count + 1];
		buf[count] = 0;				// set null at end of buffer
		fread(buf, 1, count, fp);	// load data to buffer

#ifdef _UNICODE

		if(bUnicode)
		{
			_A2W_UTF8(buf, str);
		}
		else 
		{
			str = _A2W(buf);  // need to convert mbcs to wcs
		}

#elif !defined(_WIN32)

		if(bUnicode)
		{
			// read UTF-8 directly
			str = buf;
		}
		else
		{
			// convert mbcs to UTF-8
			std::wstring ws = _A2W(buf);
			if(ws.size()!=0)
			{
				_W2A_UTF8(ws, str);
			}
		}
#else
		if(bUnicode)
		{
			// need to convert UTF8 to mbcs
			std::wstring ws;
			if(_A2W_UTF8(buf, ws))
			{
				str = _W2A(ws);
			}			
		}
		else
		{
			str = buf;
		}
#endif

		delete [] buf;
	}
	else str = _T("");
}

// Helper function : save a string chunk to file
static void SaveStringChunkToFile(const StdString &str, FILE *fp, bool bUnicode)
{
	std::string as;

#ifdef _UNICODE

	if(bUnicode)
	{
		// convert wcs to UTF-8
		_W2A_UTF8(str, as);
	}
	else
	{
		// need to convert wcs to mbcs
		as = _W2A(str);
	}

#elif !defined(_WIN32)

	if(bUnicode)
	{
		as = str; // use UTF-8 directly
	}
	else
	{
		// convert UTF-8 to mbcs
		std::wstring ws;
		_A2W_UTF8(str, ws);
		as = _W2A(ws);
	}

#else

	if(bUnicode)
	{
		// need to convert mbcs to UTF8
		std::wstring ws = _A2W(str);
		_W2A_UTF8(ws, as);
	}
	else
	{
		as = str;
	}

#endif

	unsigned int count = as.size();
	if(bUnicode) count |= 0xff000000;
	fwrite(&count, 4, 1, fp);
	fwrite(as.c_str(), sizeof(char), as.size(), fp);
}

//===========================
//  class XMLNode
//===========================
// Read XML's tag from stream. 'isbegin' indicates if the tag is begin tag or end tag
bool XMLNode::ReadTag(InStream &ins, StdString &name, bool &isbegin)
{
	_TCHAR buffer[256];
	StdString tag_name;
            
    do
    {
		ins.clear();
        ins.getline(buffer, 256, _T('<'));  // try to get tag's mark '<'
    }
    while(ins.fail() && !ins.eof());
  
    do
	{
		buffer[0] = 0;
		ins.clear();
	    ins.getline(buffer, 256, _T('>'));     // try to get tag's mark '>'
		tag_name += buffer;
	} 
	while(ins.fail() && !ins.eof());

	int taglen = (int)tag_name.size();

    if(ins.good() && taglen > 0)
    {
		if(taglen>4 && memcmp(tag_name.c_str(), _T("?xml"), 4)==0)    // check if it's XML header
        {
			return ReadTag(ins, name, isbegin);     // skip the header and read the next node
        }
        else if(taglen>4 && memcmp(tag_name.c_str(), _T("!--"),3)==0)  // check if it's comment
        {
			while(tag_name[taglen-2]!='-' || tag_name[taglen-1]!='-')
			{
				do
				{
					buffer[0] = 0;
				    ins.clear();
				    ins.getline(buffer, 256, _T('>'));     // try to get tag's mark '>'
				    tag_name += buffer;
				}
				while(ins.fail() && !ins.eof());
				taglen = (int)tag_name.size();
			}

			m_Comment.append(tag_name, 3, taglen-5);

			return ReadTag(ins, name, isbegin); // read the next node
			  
		}
        else if(tag_name[0]==_T('/'))        // check if it's node's end
        {
			name = tag_name.c_str() + 1;
            isbegin = false;
        }
        else if(tag_name[taglen-1]==_T('/'))   // check if it's node's immediated end
        {
			return ReadTag(ins, name, isbegin); // skip this node and read the next one
        }
        else                      // otherwise, it should be the node's begin
        {
            name = tag_name;
            isbegin = true;
        }
                
        return true;
	}
                
    return false;
}
                
// Read the node from stream                
bool XMLNode::LoadFromStream(InStream &ins)
{
	m_Name = m_Text = m_Comment = _T("");
    m_ChildNodes.clear();      // clear all members first

    StdString name;
    bool begin_tag;

    int prev_pos = (int)ins.tellg();     // push the current stream's position
          
    if(ReadTag(ins, name, begin_tag) && begin_tag)   // read tag's begin first
    {
		m_Name = name;

        XMLNode node;
        while(node.LoadFromStream(ins))     // try to retrive the child nodes
        {
			m_ChildNodes.push_back(node);
        }

        int data_start = (int)ins.tellg();

        // find ending tag

        if(ReadTag(ins, name, begin_tag) && name==m_Name && !begin_tag)
        {
			if(m_ChildNodes.size()==0) // end node, so we should catch the node's text
            {
				int cur_pos = (int)ins.tellg();
                int data_len = cur_pos - data_start - (int)name.size() - 3;
                _TCHAR *text = new _TCHAR[data_len+1];
                   
                ins.seekg(data_start);
                ins.read(text, data_len);     // read the node's text
                ins.seekg(cur_pos);
                
                text[data_len] = 0;
	            m_Text = Decode(text);      // decode the text
                delete [] text;
			}
                    
			return true;
        }
	}
         
    ins.seekg(prev_pos);
    return false;
}

// Write the node to stream 
void XMLNode::SaveToStream(OutStream &outs, int indent) const
{
    // prepare the indent string  
	_TCHAR *indent_str = new _TCHAR[indent+1];

    for(int i=0; i< indent; i++) indent_str[i] = _T(' ');
    indent_str[indent] = 0;

    // write out the comment first
    if(m_Comment.size()!=0)
       outs << indent_str << _T("<!--") + m_Comment + _T("-->\r\n");

    if(m_ChildNodes.size()!=0)     // it's the way we output the node with child nodes
    {
         outs << indent_str << _T("<") + m_Name + _T(">\r\n");
           
         for(unsigned int i=0; i< m_ChildNodes.size(); i++)
             m_ChildNodes[i].SaveToStream(outs, indent + 2);
              
         outs << indent_str << _T("</") + m_Name + _T(">\r\n");
    }
    else  outs << indent_str << _T("<") + m_Name + _T(">") + Encode(m_Text) + 
	     _T("</") + m_Name + _T(">\r\n");
           
    delete [] indent_str;
}

// Load XML from file in binary format
bool XMLNode::LoadFromBinaryStream(FILE *fp)
{
	// clear all contents
	m_Name = m_Text = m_Comment = _T("");
	m_ChildNodes.clear();

	// load the name, text and comment
	LoadStringChunkFromFile(m_Name, fp);
	LoadStringChunkFromFile(m_Text, fp);
	LoadStringChunkFromFile(m_Comment, fp);
	
	int count = 0;
	
	// get the number of child nodes
	if(fread(&count, sizeof(count), 1, fp) != 0)
	{
		m_ChildNodes.resize(count);

		for(int i=0; i< count; i++)
		{
			// load all child nodes
			if(!m_ChildNodes[i].LoadFromBinaryStream(fp)) return false;
		}

		return true;
	}
	
	return false;
}

// Save XML to file in binary format
void XMLNode::SaveToBinaryStream(FILE *fp, bool bUnicode) const
{
#ifdef _UNICODE
	bUnicode = true;
#endif
	
	SaveStringChunkToFile(m_Name, fp, bUnicode);
	SaveStringChunkToFile(m_Text, fp, bUnicode);
	SaveStringChunkToFile(m_Comment, fp, bUnicode);
	
	int count = (int)m_ChildNodes.size(); // save number of child nodes
	fwrite(&count, sizeof(count), 1, fp);

	for(int i=0; i< count; i++)
	{
		m_ChildNodes[i].SaveToBinaryStream(fp, bUnicode);		// save all child nodes
	}
}

// Decode the XML-style string into plain text
StdString XMLNode::Decode(const StdString &str)
{
	size_t pos = 0;
    StdString text = str;
		 
    while( (pos = text.find(_T("&lt;"), pos))!=text.npos)
        text.replace(pos, 4, _T("<"));

    pos = 0;
	while( (pos = text.find(_T("&gt;"), pos))!=text.npos)
          text.replace(pos, 4, _T(">"));

	pos = 0;
	while( (pos = text.find(_T("&amp;"), pos))!=text.npos)
        text.replace(pos, 5, _T("&"));

    return text;
}

// Encode the plain text into the XML-style string
StdString XMLNode::Encode(const StdString &str)
{
    size_t pos = 0;
    StdString text = str;

    while( (pos = text.find(_T("&"), pos))!=text.npos)
	{
		text.replace(pos, 1, _T("&amp;"));
		pos+=4;
	}
	  
    pos = 0;
    while( (pos = text.find(_T("<"), pos))!=text.npos)
	text.replace(pos, 1, _T("&lt;"));

    pos = 0;
    while( (pos = text.find(_T(">"), pos))!=text.npos)
	    text.replace(pos, 1, _T("&gt;"));

    return text;
}

// Load XML from file
bool XMLNode::LoadFromFile(const char *filename)
{
	FILE *fp = fopen(filename, "rb");

    if(fp)
    {
		StdString buf;

		// check if this file is unicoded...
		unsigned char header[4] = {0};
		fread(&header, 1, sizeof(header), fp);

		if(header[0] == 0xff && header[1] == 0xff) // binary format
		{
			fseek(fp, 2, SEEK_SET);
			bool result = LoadFromBinaryStream(fp);
			fclose(fp);
			return result;
		}
		else if(TextFileIO::Load(fp, buf))
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

// Save XML to file
bool XMLNode::SaveToFile(const char *filename, bool bBinary, bool bUnicode) const
{
    FILE *fp = fopen(filename, "wb");

    if(fp)
    {
		if(bBinary)
		{
			// write out the header of binary file
			int header = 0xffff;
			fwrite(&header, 1, 2, fp);
			SaveToBinaryStream(fp, bUnicode);
		}
		else
		{
			OutStrStream outs;
             
			// write out the header first
			outs << _T("<?xml version='1.0'?>\r\n"); 
              
			SaveToStream(outs);
			StdString p = outs.str();	// get the full string of content

			TextFileIO::Save(fp, p, bUnicode);
		}

        fclose(fp);
        return true;
    }

    return false;	
}

// Reference the child nodes by name
XMLNode &XMLNode::operator[](const StdString &name)
{
    // find the node by name

    for(unsigned int i=0; i< m_ChildNodes.size(); i++)
    {
		if(m_ChildNodes[i].m_Name==name)
            return m_ChildNodes[i];
    }
       
    // we should create a new node if no match node

    XMLNode node;
    node.m_Name = name;
    m_ChildNodes.push_back(node);
    return m_ChildNodes.back();
}

// Reference the child nodes by name
XMLNode &XMLNode::operator[](const _TCHAR *name)
{
    // find the node by name

    for(unsigned int i=0; i< m_ChildNodes.size(); i++)
    {
		if(m_ChildNodes[i].m_Name == name)
            return m_ChildNodes[i];
    }
       
    // we should create a new node if no match node

    XMLNode node;
    node.m_Name = name;
    m_ChildNodes.push_back(node);
    return m_ChildNodes.back();
}

// Find child node by name. Return 0 if no match node found.
XMLNode *XMLNode::FindNode(const StdString &name)
{
	m_LastNodeIndex = -1;   // reset the index

    for(unsigned int i=0; i< m_ChildNodes.size(); i++)
    {
		if(m_ChildNodes[i].m_Name==name)
		{
			m_LastNodeIndex = i;
			return &m_ChildNodes[i];
		}
    }

	return 0;
}

// Find child node by name. Return 0 if no match node found.
XMLNode *XMLNode::FindNode(const _TCHAR *name)
{
	m_LastNodeIndex = -1;   // reset the index

    for(unsigned int i=0; i< m_ChildNodes.size(); i++)
    {
		if(m_ChildNodes[i].m_Name == name)
		{
			m_LastNodeIndex = i;
			return &m_ChildNodes[i];
		}
    }

	return 0;
}

// Find next child node if there're more child nodes with the same name
XMLNode *XMLNode::FindNextNode()
{
	if(m_LastNodeIndex>=0)
	{
		// find the node by name
		for(unsigned int i=m_LastNodeIndex+1; i< m_ChildNodes.size(); i++)
		{
			if(m_ChildNodes[i].m_Name==m_ChildNodes[m_LastNodeIndex].m_Name)
			{
				m_LastNodeIndex = i;
				return &m_ChildNodes[i];
			}
		}

		m_LastNodeIndex = -1;
	}

	return 0;
}
