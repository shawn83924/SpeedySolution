//////////////////////////////////////////////////////////
// File Name: jsonxml.h
//
// Author : Jerry Chio
// Created: 2013/3/25
// Last Modified : 2013/3/25
//////////////////////////////////////////////////////////

#ifndef __JSONXML_H__
#define __JSONXML_H__

#include "xmlnode.h"

/*
///////////////////////////////////////////////////////////////////
 class JsonXML: JSON data parser for XMLNode
///////////////////////////////////////////////////////////////////
*/
class JsonXML
{
public:

	/** Constructor, init with the target node. */
	JsonXML(XMLNode &node);

	/** Load from the in-stream object with JSON text data. */
	bool LoadFromStream(InStream &ins);

	/** Save to the out-stream object with JSON text data. */
	void SaveToStream(OutStream &outs, int indent = 0) const;

	/** Load XMLNode from JSON-format file. */
	bool LoadFromFile(const char *filename);

	/**
	* Save XMLNode to JSON-format file.
	* @param filename The name of file to save.
	* @param bUnicode Determine if save strings of the node in unicode format.
	*/
	bool SaveToFile(const char *filename, bool bUnicode = false) const;

protected:

	/** Read quoted string from stream. */
	bool ReadQuote(InStream &ins, StdString &value);

	/** Read data until meeting character c. */
	bool ReadChar(InStream &ins, _TCHAR c);

	/** Read string until meeting character comma (,). */
	bool ReadValue(InStream &ins, StdString &value);

	 /** Decode escape characters */
	 static StdString Decode(const StdString &str);

     /** Encode escape characters */
     static StdString Encode(const StdString &str);

	XMLNode &m_Node;
	_TCHAR m_ExpectedEndChar;
};

#endif
