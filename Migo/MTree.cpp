#include "MTree.h"
#include "MNode.h"
#include <algorithm>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Class MTreeEnumerator
// An enumerator class for MNodes
//------------------------------------------------------------------------------
MTreeEnumerator::MTreeEnumerator()
:FCurrent(0)
{
}
//------------------------------------------------------------------------------
MTreeEnumerator::~MTreeEnumerator()
{
}
//------------------------------------------------------------------------------
void MTreeEnumerator::append( MNode* pNode )
{
    FList.Add((void*)pNode);
}
//------------------------------------------------------------------------------
MNode* MTreeEnumerator::next()
{
    MNode* RtnNode = NULL;

    if( FCurrent < FList.ItemCount() )
    {
        RtnNode = (MNode*)FList.GetItem( FCurrent );
        FCurrent++;
    }
    return RtnNode;
}
//------------------------------------------------------------------------------
MNode* MTreeEnumerator::node( UFCType::Int32 Index ) const
{
    return (MNode*)FList.GetConstItem( Index );
}
//------------------------------------------------------------------------------
void MTreeEnumerator::reset()
{
    FCurrent = 0;
}
//------------------------------------------------------------------------------
void MTreeEnumerator::clear()
{
    FCurrent = 0;
    FList.Clear();
}
//------------------------------------------------------------------------------
// Class MTree
// Class to hold data.
//------------------------------------------------------------------------------
MTree::MTree()
:MNode( "MTree", M_TREE, 0, NULL )
,FDirty( FALSE ) /// Allen modify at 20190614
{
}
//------------------------------------------------------------------------------
MTree::~MTree()
{
   DeleteAllNodes();
}
//------------------------------------------------------------------------------
MTree::MTree( const MTree& CopyTree )
:MNode( "MTree", M_TREE, 0, NULL )
,FDirty( FALSE ) /// Allen modify at 20190614
{
   MNode* CopyNode;
   FSize = sizeof( UFC::NInt32 );
   for( register UFCType::Int32 i = 0; i < CopyTree.getNodeCount(); i++ )
   {
        if( (CopyNode = CopyTree.FEnumerator.node( i ))!=NULL )
        {
                append( new MNode( *CopyNode ) );
                FSize += CopyNode->getSize();
        }
   }
}
//------------------------------------------------------------------------------
void MTree::DeleteAllNodes( void )
{
   if( FNodes.ItemCount() > 0 )
   {
       register MNode* DelNode ;
       for( register UFCType::Int32 i = 0; i < getNodeCount(); i++ )
       {
            if( (DelNode = FEnumerator.node( i ))!=NULL )
                delete DelNode;
       }
       FEnumerator.clear();
       FNodes.Clear();
   }
   FSize = sizeof( UFC::NInt32 );
   FDirty = FALSE; /// Allen modify at 20190614
}
//------------------------------------------------------------------------------
void MTree::Copy( const MTree& CopyTree )
{
   DeleteAllNodes();
   MNode* CopyNode;
   FSize = sizeof( UFC::NInt32 );
   for( register UFCType::Int32 i = 0; i < CopyTree.getNodeCount(); i++ )
   {
        if( (CopyNode = CopyTree.FEnumerator.node( i ))!=NULL )
        {
            append( new MNode( *CopyNode ) );
            FSize += CopyNode->getSize();
        }
   }
}
//------------------------------------------------------------------------------
void MTree::Clear( void )
{
   DeleteAllNodes();
}
//------------------------------------------------------------------------------
void MTree::LoadFromStream( UFC::PStream* Stream )
{
    UFC::NInt32  NodeCount;

    NodeCount.LoadFromStream( Stream );
    for( register UFCType::Int32 i = NodeCount.ToInt32() - 1; i >= 0; i-- )
	 append( new MNode( Stream ) );
    FSize = Stream->GetSize();
    FDirty = FALSE; /// Allen modify at 20190614
}
//------------------------------------------------------------------------------
void MTree::SaveToStream( UFC::PStream* Stream )
{
    UFC::NInt32  NodeCount( getNodeCount() );
    MNode* Node;
    int OrigSize = Stream->GetSize();
    
    NodeCount.SaveToStream( Stream );  ///< write the Count of Nodes in MTree
    for( register UFCType::Int32 i = 0; i < getNodeCount(); i++ )
    {
    	Node = FEnumerator.node( i );
	Node->SaveToStream( Stream );
    }
    FSize = Stream->GetSize() - OrigSize;
    FDirty = FALSE; /// Allen modify at 20190614
}
//------------------------------------------------------------------------------
void MTree::append( MNode* MNodePtr )
{
    if( MNodePtr != NULL )
    {
        MNode*         OrigMNode;
        const MString& NodeName = MNodePtr->getName();
        
        if( FNodes.GetObjectByKey( NodeName, OrigMNode ) == FALSE )         
        {
            FEnumerator.append( MNodePtr );
            FNodes.Add( NodeName, MNodePtr );
        }
        else
            *OrigMNode = *MNodePtr; /// Update the orig node
    }
    FDirty = TRUE; /// Allen modify at 20190614
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, short s)
{
	append( new MNode(rsAttributeName,s) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, int i)
{
	append( new MNode(rsAttributeName, i) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, long l)
{
	append( new MNode(rsAttributeName, l) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, unsigned short us)
{
	append( new MNode(rsAttributeName, us) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, unsigned int ui)
{
	append( new MNode(rsAttributeName, ui) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, unsigned long ul)
{
	append( new MNode(rsAttributeName,ul) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, long long ll)
{
	append( new MNode(rsAttributeName, ll) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, float f)
{
	append( new MNode(rsAttributeName,f) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, double d)
{
	append( new MNode(rsAttributeName, d) );
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, const MString& rsStr)
{
    if( rsStr.Length( ) <= 0 )
    {
        UFC::AnsiString ErrMsg;
                
        ErrMsg.Printf( "Can not append field[%s] with NULL value to MTree.", rsAttributeName.c_str() );
        UFC::BufferedLog::Printf( " %s", ErrMsg.c_str() );
        throw( UFC::Exception( ErrMsg.c_str() ));
    }
    append( new MNode(rsAttributeName, rsStr));
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, const char* szStr )
{
    if( strlen( szStr ) <= 0 )
    {
        UFC::AnsiString ErrMsg;

        ErrMsg.Printf( "Can not append field[%s] with NULL value to MTree.", rsAttributeName.c_str() );
        UFC::BufferedLog::Printf( " %s", ErrMsg.c_str() );
        throw( UFC::Exception( ErrMsg.c_str() ));
    }
    append( new MNode(rsAttributeName, szStr));
}
//------------------------------------------------------------------------------
void MTree::append( const MString& rsAttributeName, const unsigned char* szBuffer, int iSize, BOOL Compress )
{
    if( szBuffer == NULL )
    {
        UFC::AnsiString ErrMsg;
        
        ErrMsg.Printf( "Can not append field[%s] with NULL pointer to MTree.", rsAttributeName.c_str() );
        UFC::BufferedLog::Printf( " %s", ErrMsg.c_str() );
        throw( UFC::Exception( ErrMsg.c_str() ));
    }
    if( iSize <= 0 )
    {
        UFC::AnsiString ErrMsg;

        ErrMsg.Printf( "Can not append field[%s] with zero length data to MTree.", rsAttributeName.c_str() );
        UFC::BufferedLog::Printf( " %s", ErrMsg.c_str() );
        throw( UFC::Exception( ErrMsg.c_str() ));
    }
    append( new MNode(rsAttributeName, szBuffer, iSize, Compress ));
}
//------------------------------------------------------------------------------
BOOL MTree::update( const MString& rsAttributeName, int Value )
{
    MNode* OrigMNode;
    BOOL   RtnVal = FALSE;
        
    if( FNodes.GetObjectByKey( rsAttributeName, OrigMNode ) == TRUE )         
    {
        RtnVal = OrigMNode->SetInt( Value );/// Update the original node	        
        FDirty = RtnVal;    
    }
    else
        append( new MNode(rsAttributeName, Value));
    return RtnVal;
}
//------------------------------------------------------------------------------
BOOL MTree::update( const MString& rsAttributeName, const MString& rsStr)
{
    MNode* OrigMNode;
    BOOL   RtnVal = FALSE;
        
    if( FNodes.GetObjectByKey( rsAttributeName, OrigMNode ) == TRUE )         
    {
        RtnVal = OrigMNode->SetString( rsStr );/// Update the original node	        
        FDirty = RtnVal;            
    }
    else
        append( new MNode(rsAttributeName, rsStr));
    return RtnVal;
}
//------------------------------------------------------------------------------
MNode* MTree::get( const MString& rsAttributeName)    
{ 
    MNode* Node;
    
    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE )
        return Node;
    return NULL;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, MNode& refMNode)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE )
    {
        refMNode = *Node;
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, char*& szBuffer, int& iSize)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE )
    {
        iSize    = Node->getSize();
        szBuffer =  (char*)Node->getData();
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, int& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE  && Node->getType() == M_INT)
    {
        refValue = *((int*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, short& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE  && Node->getType() == M_SHORT)
    {
        refValue = *((short*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, unsigned short& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE && Node->getType() == M_USHORT)
    {
        refValue = *((unsigned short*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, unsigned int& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE && Node->getType() == M_UINT)
    {
        refValue = *((unsigned int*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName,  long& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE && Node->getType() == M_LONG)
    {
        refValue = *((long*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, unsigned long& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE && Node->getType() == M_ULONG)
    {
        refValue = *((unsigned long*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, long long& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE && Node->getType() == M_INT64)
    {
        refValue = *((long long*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, float& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE && Node->getType() == M_FLOAT)
    {
        refValue = *((float*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, double& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE && Node->getType() == M_DOUBLE)
    {
        refValue = *((double*)(Node->getData()));
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL MTree::get( const MString& rsAttributeName, MString& refValue)
{
    MNode* Node;

    if( FNodes.GetObjectByKey( rsAttributeName, Node ) == TRUE )
    {
        if( Node->getType() == M_STRING )
        {
            refValue = ( const char*)( Node->getData() );
            return TRUE;
        }
        else if( Node->getType() == M_ROWDATA || Node->getType() == M_COMPRESSEDDATA )
        {
            refValue = UFC::AnsiString( ( const char*) Node->getData(), Node->getSize() -1 );
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
UFCType::Int32 MTree::getSize()
{
    if( FDirty )
    {
        MNode* Node;

        FSize = sizeof( UFC::NInt32 );  ///< write the Count of Nodes in MTree
        for( register UFCType::Int32 i = 0; i < getNodeCount(); i++ )
        {
                Node = FEnumerator.node( i );
                FSize += Node->getSize();
        }
    }
    return FSize;
}
//------------------------------------------------------------------------------


