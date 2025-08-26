//////////////////////////////////////////////////////////
// File Name: xmlnode.h
//
// Author : Little Chio
// Created: 2002/7/15
// Last Modified : 2009/3/13
//
// Copyright (c) 2002-2007, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#ifndef __XMLNODE_H__
#define __XMLNODE_H__

#include "tchars.h"
#include <vector>
#include <stdio.h>


#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__GNUC__)	// If using Microsoft or Borland or GNU C++ compiler, do the following.
	#pragma pack(push, 8)						// To confirm the alignment of the structure members is the same under any case. Otherwise, strange bugs will be resulted under multiple projects of different alignment.
												// (Please place this line after all other #include<>.)
#endif


///////////////////////////////////////////////////////////////////
/// class XMLNode is a XML-parser and stores the pairs of 'name'
/// and 'text' in hierarchy structures. It is useful for serialization
/// because most of the data types can be represented in string 
/// form. So most of Pyxis class use XMLNode as data stream.
///////////////////////////////////////////////////////////////////

class XMLNode
{
public:

     /** Load the node from the in-stream object. */
	 bool LoadFromStream(InStream &ins);

	 /** Save the node to the out-stream object. */
     void SaveToStream(OutStream &outs, int indent=0) const;

	 /** Load the node from the binary file stream. */
	 bool LoadFromBinaryStream(FILE *fp);

	 /** Save the node to the binary file stream. */
	 void SaveToBinaryStream(FILE *fp, bool bUnicode) const;

	 /** Load the node from a file. */
     bool LoadFromFile(const char *filename);

	 /** 
	  * Save the node to a file. 
	  * @param filename The name of file to save.
	  * @param bBinary Determine if save hierarchy of the node in binary format.
	  * @param bUnicode Determine if save strings of the node in unicode format.
	  */
     bool SaveToFile(const char *filename, bool bBinary = false, bool bUnicode = false) const;

	 /** 
	  * Get the reference of the child node by name.
	  * @param name The child node's name
	  * @return Return the reference of node we found. If there's no match node
	  * in the child list, a new child node will be added and return its
	  * reference.
	  */
     XMLNode &operator[](const StdString &name);
	 XMLNode &operator[](const _TCHAR *name);

     /**
	  * Find a child node by name.
	  * @param name The child node's name.
	  * @return Return NULL if there's no match node found.
	  */
	 XMLNode *FindNode(const StdString &name);
	 XMLNode *FindNode(const _TCHAR *name);

	 /**
	  * Find the next child node.
	  * @return Return NULL if no more child node after last found node.
	  */
	 XMLNode *FindNextNode();

	 /** Decode a string to restore to normal characters. */
	 static StdString Decode(const StdString &str);

     /** Encode a string which has reserved characters. */
     static StdString Encode(const StdString &str);
     
	 StdString m_Name;    /**< The node's name. */
     StdString m_Text;    /**< The node's text. */
     StdString m_Comment;    /**< The node's comment. */
     std::vector<XMLNode> m_ChildNodes;  /**< The array of child nodes. */

private:
     
     bool ReadTag(InStream &ins, StdString &name, bool &isbegin);

	 int m_LastNodeIndex;

};


#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__GNUC__)	// If using Microsoft or Borland or GNU C++ compiler, do the following.
	#pragma pack(pop)							// Restore member alignment size. (Do not affect outside world)
#endif


///////////////////////////////////////////////////////////////////
/// template for output value to string
///////////////////////////////////////////////////////////////////
template<class T>
StdString &operator<<(StdString &str, const T &value)
{
	OutStrStream os;
    os << value;
    if(str.size()) str += _T(' ');
    return str += os.str();
}

///////////////////////////////////////////////////////////////////
/// template for input value from string
///////////////////////////////////////////////////////////////////
template<class T>
StdString operator>>(const StdString &str, T &value)
{
    InStrStream is(str);
    is >> value;

	if(!is.fail())
	{
		return StdString(str.c_str() + (int)is.tellg());
	}
	else
	{
		return StdString();
	}
}

///////////////////////////////////////////////////////////////////
/// special version of operator<< & >> for string
///////////////////////////////////////////////////////////////////
inline
StdString &operator<<(StdString &str, const StdString &value)
{
    return str = value;
}

inline
StdString &operator<<(StdString &str, const _TCHAR *value)
{
    return str = value;
}

inline
StdString &operator<<(StdString &str, const unsigned char &value)
{
    return str << (unsigned int)value;
}

inline
StdString &operator<<(StdString &str, const unsigned short &value)
{
    return str << (unsigned int)value;
}


inline
StdString operator>>(const StdString &str, StdString &value)
{
	value = str;
	return str;
}

inline
StdString operator>>(const StdString &str, unsigned char &value)
{
    unsigned int c = value;

    InStrStream is(str);
    is >> c;
    value = c;

	if(!is.fail())
	{
		return StdString(str.c_str() + (int)is.tellg());
	}
	else
	{
		return StdString();
	}
}

inline
StdString operator>>(const StdString &str, unsigned short &value)
{
    unsigned int c = value;

    InStrStream is(str);
    is >> c;
    value = c;

	if(!is.fail())
	{
		return StdString(str.c_str() + (int)is.tellg());
	}
	else
	{
		return StdString();
	}
}

#endif    // end of __XMLNODE_H__

