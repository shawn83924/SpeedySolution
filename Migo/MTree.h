#ifndef _MTree_h_
#define _MTree_h_
#include "MNode.h"
//------------------------------------------------------------------------------
class MTreeEnumerator
{
private:
    UFC::PList FList;
    UFCType::Int32 FCurrent;
public:
    MTreeEnumerator();
    ~MTreeEnumerator();
public:
    void   clear();
    void   append( MNode* pNode );
    void   reset();
    MNode* next();
    MNode* node( UFCType::Int32 Index ) const;
};
//------------------------------------------------------------------------------
class MTree:public MNode
{
private:
    MTreeEnumerator                        FEnumerator;
    UFC::PHashMap<UFC::AnsiString,MNode*>  FNodes;
    void DeleteAllNodes( void );
    UFCType::Int32  FSize;
    BOOL FDirty; /// Allen modify at 20190614
public:
    MTree( const MTree& );
    MTree();
    ~MTree();
    void  LoadFromStream( UFC::PStream* Stream );
    void  SaveToStream( UFC::PStream* s );
    void  append( MNode* MNodePtr );
    MTreeEnumerator* newEnumerator()
    {
        FEnumerator.reset();
		return &FEnumerator;
    }
public:
    void Clear();
    void Copy( const MTree& CopyTree );
    void append( const MString& rsAttributeName, short s);
    void append( const MString& rsAttributeName, int i);
    void append( const MString& rsAttributeName, long l);
    void append( const MString& rsAttributeName, unsigned short us);
    void append( const MString& rsAttributeName, unsigned int ui);
    void append( const MString& rsAttributeName, unsigned long ul);
    void append( const MString& rsAttributeName, long long ul);
    void append( const MString& rsAttributeName, float f);
    void append( const MString& rsAttributeName, double d);
    void append( const MString& rsAttributeName, const MString& rsStr);
    void append( const MString& rsAttributeName, const char* szStr);
    void append( const MString& rsAttributeName, const unsigned char* szBuffer, int iSize, BOOL Compress = FALSE );
    BOOL update( const MString& rsAttributeName, int i);
    BOOL update( const MString& rsAttributeName, const MString& rsStr);
    MNode* get( UFCType::Int32 Index )             { return FEnumerator.node( Index ); }
    MNode* get( const MString& rsAttributeName);
    BOOL   Exists( const MString& rsAttributeName) { return FNodes.IsExists(rsAttributeName); }
    BOOL   get( const MString& rsAttributeName, MNode& refMNode);
    BOOL   get( const MString& rsAttributeName, char*& szBuffer, int& iSize);
    BOOL   get( const MString& rsAttributeName, int& refValue);
    BOOL   get( const MString& rsAttributeName, short& refValue);
    BOOL   get( const MString& rsAttributeName, unsigned short& refValue);
    BOOL   get( const MString& rsAttributeName, unsigned int& refValue);
    BOOL   get( const MString& rsAttributeName, long& refValue);
    BOOL   get( const MString& rsAttributeName, unsigned long&  refValue);
    BOOL   get( const MString& rsAttributeName, long long&      refValue);        
    BOOL   get( const MString& rsAttributeName, float& refValue);
    BOOL   get( const MString& rsAttributeName, double& refValue);
    BOOL   get( const MString& rsAttributeName, MString& refValue);
    UFCType::Int32  getNodeCount() const { return FNodes.ItemCount(); }
    virtual UFCType::Int32 getSize();
}; // class MTree
//------------------------------------------------------------------------------
#endif // _MTree_h_

