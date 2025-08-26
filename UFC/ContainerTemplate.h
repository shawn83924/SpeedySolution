 /*
 * File:   ContainerTemplate.h
 * Author: Zhen Fan
 *
 * Created on 2017/04/21 00:02
 */

#ifndef CONTAINERTEMPLATE_H
#define CONTAINERTEMPLATE_H
#include <vector>
#include <map>
#include <algorithm>
#include <queue>

#include "PHashedSet.h"
#include "PHashedList.h"
#include "AnsiString.h"
#include "PThread.h"

namespace UFC
{
const bool cstCheck = true;
const bool cstNotCheck = false;
const bool cstFlush = true;
const bool cstNotFlush = false;
const bool cstCreate = true;
const bool cstNotCreate = false;
const bool cstDelete = true;
const bool cstNotDelete = false;
const bool cstKeepDeciamlPt = true;
const bool cstNotKeepDeciamlPt = false;
const bool cstSort = true;
const bool cstNotSort = false;
const int  cst_1_or_0 = 0;
const int  cst_Y_or_N = 1;
const int  cst_true_or_false = 2;

//---------------------------------------------------------------------------
template<typename VectorElementType>
int GetVectorElementPtrCount_NoLock(std::vector<VectorElementType*>& ElementVector)
{
    int elementCount = static_cast<int>(ElementVector.size());
    return elementCount;
}  //GetVectorElementPtrCount_NoLock()
//---------------------------------------------------------------------------
template<typename VectorElementType>
int GetVectorElementPtrCount(std::vector<VectorElementType*>& ElementVector, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = static_cast<int>(ElementVector.size());
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetVectorElementPtrCount()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
VectorElementType *GetElementPtrFromVector_NoLock(std::vector<VectorElementType*>& ElementVector, int ElementIndex)
{
    VectorElementType *elementPtr = 0;
    if ((ElementIndex >= 0) && (ElementIndex < static_cast<int>(ElementVector.size())))
        elementPtr = ElementVector[ElementIndex];
    return elementPtr;
}  //GetElementPtrFromVector_NoLock()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
VectorElementType *GetElementPtrFromVector(std::vector<VectorElementType*>& ElementVector, int ElementIndex, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    VectorElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if ((ElementIndex >= 0) && (ElementIndex < static_cast<int>(ElementVector.size())))
        elementPtr = ElementVector[ElementIndex];
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //GetElementPtrFromVector()
//---------------------------------------------------------------------------
template<typename VectorElementType, typename KeyType, typename FindType>
VectorElementType *FindElementPtrFromVector_NoLock(std::vector<VectorElementType*>& ElementVector, const KeyType& KeyValue)
{
    VectorElementType *elementPtr = 0;
    typename std::vector<VectorElementType*>::iterator it = std::find_if(ElementVector.begin(), ElementVector.end(), FindType(KeyValue));
    if (it != ElementVector.end()) elementPtr = *it;
    return elementPtr;
}  //FindElementPtrFromVector_NoLock()
//---------------------------------------------------------------------------
template<typename VectorElementType, typename KeyType, typename FindType>
VectorElementType *FindElementPtrFromVector(std::vector<VectorElementType*>& ElementVector, const KeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    VectorElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    typename std::vector<VectorElementType*>::iterator it = std::find_if(ElementVector.begin(), ElementVector.end(), FindType(KeyValue));
    if (it != ElementVector.end()) elementPtr = *it;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //FindElementPtrFromVector()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
bool AppendElementPtrToVector_NoLock(std::vector<VectorElementType*>& ElementVector, VectorElementType *ElementPtr)
{
    if (ElementPtr == 0) return false;
    ElementVector.push_back(ElementPtr);
    return true;
}  //AppendElementPtrToVector_NoLock()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
bool AppendElementPtrToVector(std::vector<VectorElementType*>& ElementVector, VectorElementType *ElementPtr, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    if (ElementPtr == 0) return false;
    UFC::DoReadWriteLock(LockPtr, LockType);
    ElementVector.push_back(ElementPtr);
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return true;
}  //AppendElementPtrToVector()
//---------------------------------------------------------------------------
template<typename VectorElementType, typename KeyType, typename FindType>
VectorElementType *RemoveElementPtrFromVector_NoLock(std::vector<VectorElementType*>& ElementVector, const KeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    VectorElementType *elementPtr = 0;
    typename std::vector<VectorElementType*>::iterator it = std::find_if(ElementVector.begin(), ElementVector.end(), FindType(KeyValue));
    if (it != ElementVector.end())
    {
        elementPtr = *it;
        ElementVector.erase(it);
    }
    return elementPtr;
}  //RemoveElementPtrFromVector_NoLock()
//---------------------------------------------------------------------------
template<typename VectorElementType, typename KeyType, typename FindType>
VectorElementType *RemoveElementPtrFromVector(std::vector<VectorElementType*>& ElementVector, const KeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    VectorElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    typename std::vector<VectorElementType*>::iterator it = std::find_if(ElementVector.begin(), ElementVector.end(), FindType(KeyValue));
    if (it != ElementVector.end())
    {
        elementPtr = *it;
        ElementVector.erase(it);
    }
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //RemoveElementPtrFromVector()
//---------------------------------------------------------------------------
template<typename VectorElementType, typename CompareType>
void SortElementPtrVector_NoLock(std::vector<VectorElementType*>& ElementVector)
{
    VectorElementType *elementPtr = 0;
    std::sort(ElementVector.begin(), ElementVector.end(), CompareType());
}  //SortElementPtrVector_NoLock()
//---------------------------------------------------------------------------
template<typename VectorElementType, typename CompareType>
void SortElementPtrVector(std::vector<VectorElementType*>& ElementVector, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    VectorElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    std::sort(ElementVector.begin(), ElementVector.end(), CompareType());
    UFC::DoReadWriteUnlock(LockPtr, LockType);
}  //SortElementPtrVector()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
int ClearElementPtrVector_NoLock(std::vector<VectorElementType*>& ElementVector, bool DoDelete)
{
    int deleteCount = 0;
    if (DoDelete)
    {
        for (unsigned int i = 0; i < ElementVector.size(); i++)
        {
            VectorElementType *curElementPtr = ElementVector[i];
            if (curElementPtr != 0)
            {
                delete curElementPtr;
                deleteCount++;
            }
        }  //for (unsigned int i = 0; i < ElementVector.size(); i++)
    }  //if (DoDelete)
    ElementVector.clear();
    return deleteCount;
}  //ClearElementPtrVector_NoLock()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
int ClearElementPtrVector(std::vector<VectorElementType*>& ElementVector, bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    int deleteCount = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoDelete)
    {
        for (unsigned int i = 0; i < ElementVector.size(); i++)
        {
            VectorElementType *curElementPtr = ElementVector[i];
            if (curElementPtr != 0)
            {
                delete curElementPtr;
                deleteCount++;
            }
        }  //for (unsigned int i = 0; i < ElementVector.size(); i++)
    }  //if (DoDelete)
    ElementVector.clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementPtrVector()
//---------------------------------------------------------------------------
template<typename VectorElementType>
int GetVectorElementCount(std::vector<VectorElementType>& ElementVector, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = static_cast<int>(ElementVector.size());
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetVectorElementCount()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
VectorElementType GetElementFromVector(std::vector<VectorElementType>& ElementVector, int ElementIndex, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType, VectorElementType DefaultValue)
{
    VectorElementType element = DefaultValue;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if ((ElementIndex >= 0) && (ElementIndex < static_cast<int>(ElementVector.size())))
        element = ElementVector[ElementIndex];
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return element;
}  //GetElementFromVector()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
bool AppendElementToVector(std::vector<VectorElementType>& ElementVector, VectorElementType Element, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    ElementVector.push_back(Element);
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return true;
}  //AppendElementToVector()
//---------------------------------------------------------------------------
template<typename VectorElementType> 
int ClearElementVector(std::vector<VectorElementType>& ElementVector, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int deleteCount = ElementVector.size();
    ElementVector.clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementVector()

//declare Hash Map Function template
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int GetHMapElementCount(UFC::PHashMap<MapKeyType, MapElementType>& ElementMap, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = ElementMap.ItemCount();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetHMapElementCount()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool FindElementFromHMap(UFC::PHashMap<MapKeyType, MapElementType>& ElementMap, const MapKeyType& KeyValue, 
                            MapElementType& Element,
                            UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    bool isExist = true;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (ElementMap.GetObjectByKey(KeyValue, Element) == FALSE) isExist = false;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isExist;
}  //FindElementFromHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool InsertElementIntoHMap(UFC::PHashMap<MapKeyType, MapElementType>& ElementMap, 
                           const MapKeyType& KeyValue, const MapElementType& Element, bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    bool isSuccess = false;    
    UFC::DoReadWriteLock(LockPtr, LockType);
    bool isExist = false;
    if (DoCheckExist)
    {
        MapElementType existElement;
        if (ElementMap.GetObjectByKey(KeyValue, existElement) == TRUE) isExist = true;
    }
    
    if (!isExist)
    {
        if (ElementMap.Add(KeyValue, Element) == TRUE) isSuccess = true;
    }
    
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isSuccess;
}  //InsertElementIntoHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool RemoveElementFromHMap(UFC::PHashMap<MapKeyType, MapElementType>& ElementMap, const MapKeyType& KeyValue, 
                           UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    bool isExist = false;
    UFC::DoReadWriteLock(LockPtr, LockType);
    MapElementType existElement;
    if (ElementMap.GetObjectByKey(KeyValue, existElement) == TRUE) 
    {
        isExist = true;
        ElementMap.DeleteByKey(KeyValue);
    }
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isExist;
}  //RemoveElementFromHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int ClearElementHMap(UFC::PHashMap<MapKeyType, MapElementType>& ElementMap, 
                     UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int deleteCount = ElementMap.ItemCount();    
    ElementMap.Clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementHMap() 
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int GetHMapElementPtrCount_NoLock(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap)
{
    int elementCount = ElementMap.ItemCount();
    return elementCount;
}  //GetHMapElementPtrCount_NoLock()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int GetHMapElementPtrCount(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = ElementMap.ItemCount();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetHMapElementPtrCount()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
MapElementType *FindElementPtrFromHMap_NoLock(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, const MapKeyType& KeyValue)
{
    MapElementType *elementPtr = 0;
    if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == FALSE) elementPtr = 0;
    return elementPtr;
}  //FindElementPtrFromHMap_NoLock()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
MapElementType *FindElementPtrFromHMap(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                                       const MapKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    MapElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == FALSE) elementPtr = 0;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //FindElementPtrFromHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool InsertElementPtrIntoHMap_NoLock(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                                     const MapKeyType& KeyValue, MapElementType *ElementPtr, bool DoCheckExist)
{
    bool isSuccess = false;
    if (ElementPtr == 0) return isSuccess;
    
    if (DoCheckExist && (FindElementPtrFromHMap_NoLock(ElementMap, KeyValue) != 0)) 
        return isSuccess;
    if (ElementMap.Add(KeyValue, ElementPtr) == TRUE) isSuccess = true;
    return isSuccess;
}  //InsertElementPtrIntoHMap_NoLock()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool InsertElementPtrIntoHMap(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                              const MapKeyType& KeyValue, MapElementType *ElementPtr, bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    bool isSuccess = false;
    if (ElementPtr == 0) return isSuccess;
    
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoCheckExist && (FindElementPtrFromHMap_NoLock(ElementMap, KeyValue) != 0)) 
    {
        UFC::DoReadWriteUnlock(LockPtr, LockType);
        return isSuccess;
    }
    if (ElementMap.Add(KeyValue, ElementPtr) == TRUE) isSuccess = true;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isSuccess;
}  //InsertElementPtrIntoHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
MapElementType *RemoveElementPtrFromHMap_NoLock(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, const MapKeyType& KeyValue)
{
    MapElementType *elementPtr = 0;
    if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == TRUE) 
        ElementMap.DeleteByKey(KeyValue);
    else
        elementPtr = 0;
    return elementPtr;
}  //RemoveElementPtrFromHMap_NoLock()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
MapElementType *RemoveElementPtrFromHMap(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                                         const MapKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    MapElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == TRUE) 
        ElementMap.DeleteByKey(KeyValue);
    else
        elementPtr = 0;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //RemoveElementPtrFromHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int ClearElementPtrHMap_NoLock(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, bool DoDelete)
{
    int deleteCount = 0;
    if (DoDelete)
    {
        MapElementType *curElementPtr = ElementMap.First();
        while (curElementPtr != 0)
        {
            delete curElementPtr;
            deleteCount++;
            curElementPtr = ElementMap.Next();
        }
    }  //if (DoDelete)
    
    ElementMap.Clear();
    return deleteCount;
}  //ClearElementPtrHMap_NoLock() 
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int ClearElementPtrHMap(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                        bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    int deleteCount = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoDelete)
    {
        MapElementType *curElementPtr = ElementMap.First();
        while (curElementPtr != 0)
        {
            delete curElementPtr;
            deleteCount++;
            curElementPtr = ElementMap.Next();
        }
    }  //if (DoDelete)
    
    ElementMap.Clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementPtrHMap() 
//---------------------------------------------------------------------------
template<typename MapElementType>
int GetStrIdxHMapElementPtrCount_NoLock(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap)
{
    int elementCount = ElementMap.ItemCount();
    return elementCount;
}  //GetStrIdxHMapElementPtrCount_NoLock()
//---------------------------------------------------------------------------
template<typename MapElementType>
int GetStrIdxHMapElementPtrCount(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = ElementMap.ItemCount();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetStrIdxHMapElementPtrCount()
//---------------------------------------------------------------------------
template<typename MapElementType>
MapElementType *FindElementPtrFromStrIdxHMap_NoLock(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, const UFC::AnsiString& KeyValue)
{
    MapElementType *elementPtr = 0;
    if (KeyValue.Length() <= 0) return elementPtr;
    
    if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == FALSE) elementPtr = 0;
    return elementPtr;
}  //FindElementPtrFromStrIdxHMap_NoLock()
//---------------------------------------------------------------------------
template<typename MapElementType>
MapElementType *FindElementPtrFromStrIdxHMap(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, 
                                             const UFC::AnsiString& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    MapElementType *elementPtr = 0;
    if (KeyValue.Length() <= 0) return elementPtr;
    
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == FALSE) elementPtr = 0;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //FindElementPtrFromStrIdxHMap()
//---------------------------------------------------------------------------
template<typename MapElementType>
bool InsertElementPtrIntoStrIdxHMap_NoLock(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, 
                                           const UFC::AnsiString& KeyValue, MapElementType *ElementPtr, bool DoCheckExist)
{
    bool isSuccess = false;
    if ((KeyValue.Length() <= 0) || (ElementPtr == 0)) return isSuccess;
    
    if (DoCheckExist && (FindElementPtrFromStrIdxHMap_NoLock(ElementMap, KeyValue) != 0)) 
        return isSuccess;
    if (ElementMap.Add(KeyValue, ElementPtr) == TRUE) isSuccess = true;
    return isSuccess;
}  //InsertElementPtrIntoStrIdxHMap_NoLock()
//---------------------------------------------------------------------------
template<typename MapElementType>
bool InsertElementPtrIntoStrIdxHMap(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, 
                                    const UFC::AnsiString& KeyValue, MapElementType *ElementPtr, bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    bool isSuccess = false;
    if ((KeyValue.Length() <= 0) || (ElementPtr == 0)) return isSuccess;
    
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoCheckExist && (FindElementPtrFromStrIdxHMap(ElementMap, KeyValue, LockPtr, UFC::rwNotLock) != 0)) 
    {
        UFC::DoReadWriteUnlock(LockPtr, LockType);
        return isSuccess;
    }
    if (ElementMap.Add(KeyValue, ElementPtr) == TRUE) isSuccess = true;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isSuccess;
}  //InsertElementPtrIntoStrIdxHMap()
//---------------------------------------------------------------------------
template<typename MapElementType>
MapElementType *RemoveElementPtrFromStrIdxHMap_NoLock(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, const UFC::AnsiString& KeyValue)
{
	MapElementType *elementPtr = 0;
	if (KeyValue.Length() <= 0) return elementPtr;

	if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == TRUE)
            ElementMap.DeleteByKey(KeyValue);
	else
            elementPtr = 0;
	return elementPtr;
}  //RemoveElementPtrFromStrIdxHMap_NoLock()
//---------------------------------------------------------------------------
template<typename MapElementType>
MapElementType *RemoveElementPtrFromStrIdxHMap(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, 
                                               const UFC::AnsiString& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
        MapElementType *elementPtr = 0;
        if (KeyValue.Length() <= 0) return elementPtr;

       UFC::DoReadWriteLock(LockPtr, LockType);
	if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == TRUE)
            ElementMap.DeleteByKey(KeyValue);
	else
            elementPtr = 0;
        UFC::DoReadWriteUnlock(LockPtr, LockType);
        return elementPtr;
}  //RemoveElementPtrFromStrIdxHMap()
//---------------------------------------------------------------------------
template<typename MapElementType>
int ClearElementPtrStrIdxHMap_NoLock(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, bool DoDelete)
{
    int deleteCount = 0;
    if (DoDelete)
    {
        MapElementType *curElementPtr = ElementMap.First();
        while (curElementPtr != 0)
        {
            delete curElementPtr;
            deleteCount++;
            curElementPtr = ElementMap.Next();
        }
    }  //if (DoDelete)
    
    ElementMap.Clear();
    return deleteCount;
}  //ClearElementPtrStrIdxHMap_NoLock()
//---------------------------------------------------------------------------
template<typename MapElementType>
int ClearElementPtrStrIdxHMap(UFC::PHashMap<UFC::AnsiString, MapElementType*>& ElementMap, 
                              bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    int deleteCount = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoDelete)
    {
        MapElementType *curElementPtr = ElementMap.First();
        while (curElementPtr != 0)
        {
            delete curElementPtr;
            deleteCount++;
            curElementPtr = ElementMap.Next();
        }
    }  //if (DoDelete)
    
    ElementMap.Clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementPtrStrIdxHMap()

//declare Hash List Function template
//---------------------------------------------------------------------------
template<typename ListKeyType, typename ListElementType>
int GetHListElementPtrCount(UFC::PHashedList<ListKeyType, ListElementType*>& ElementList, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = ElementList.ItemCount();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetHListElementPtrCount()
//---------------------------------------------------------------------------
template<typename ListKeyType, typename ListElementType>
ListElementType *FindElementPtrFromHList(UFC::PHashedList<ListKeyType, ListElementType*>& ElementList, 
                                         const ListKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    ListElementType *elementPtr = ElementList.GetObjectByKey(KeyValue);
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //FindElementPtrFromHList()
//---------------------------------------------------------------------------
template<typename ListKeyType, typename ListElementType>
bool InsertElementPtrIntoHList(UFC::PHashedList<ListKeyType, ListElementType*>& ElementList, 
                               const ListKeyType& KeyValue, ListElementType *ElementPtr, bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    bool isSuccess = false;
    if (ElementPtr == 0) return isSuccess;
    
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoCheckExist)
    {
        ListElementType *existElementPtr = ElementList.GetObjectByKey(KeyValue);
        if (existElementPtr != 0)
        {
            UFC::DoReadWriteUnlock(LockPtr, LockType);
            return isSuccess;
        }
    }
    
    if (ElementList.Add(KeyValue, ElementPtr) == TRUE) isSuccess = true;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isSuccess;
}  //InsertElementPtrIntoHList()
//---------------------------------------------------------------------------
template<typename ListKeyType, typename ListElementType>
ListElementType *RemoveElementPtrFromHList(UFC::PHashedList<ListKeyType, ListElementType*>& ElementList, 
                                           const ListKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    ListElementType *elementPtr = ElementList.GetObjectByKey(KeyValue);
    if (elementPtr != 0) ElementList.DeleteByKey(KeyValue);
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //RemoveElementPtrFromHList()
//---------------------------------------------------------------------------
template<typename ListKeyType, typename ListElementType>
int ClearElementPtrHList(UFC::PHashedList<ListKeyType, ListElementType*>& ElementList, 
                         bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    int deleteCount = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoDelete)
    {
        ListElementType *curElementPtr = ElementList.First();
        while (curElementPtr != 0)
        {
            delete curElementPtr;
            deleteCount++;
            curElementPtr = ElementList.Next();
        }
    }  //if (DoDelete)
    
    ElementList.Clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementPtrHList() 
//---------------------------------------------------------------------------
template<typename ListElementType>
int GetStrIdxHListElementPtrCount(UFC::PHashedList<UFC::AnsiString, ListElementType*>& ElementList, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = ElementList.ItemCount();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetStrIdxHListElementPtrCount()
//---------------------------------------------------------------------------
template<typename ListElementType>
ListElementType *FindElementPtrFromStrIdxHList(UFC::PHashedList<UFC::AnsiString, ListElementType*>& ElementList, 
                                              const UFC::AnsiString& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    ListElementType *elementPtr = 0;
    if (KeyValue.Length() <= 0) return elementPtr;
    
    UFC::DoReadWriteLock(LockPtr, LockType);
    elementPtr = ElementList.GetObjectByKey(KeyValue);
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //FindElementPtrFromStrIdxHList()
//---------------------------------------------------------------------------
template<typename ListElementType>
bool InsertElementPtrIntoStrIdxHList(UFC::PHashedList<UFC::AnsiString, ListElementType*>& ElementList, 
                                     const UFC::AnsiString& KeyValue, ListElementType *ElementPtr, bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    bool isSuccess = false;
    if ((KeyValue.Length() <= 0) || (ElementPtr == 0)) return isSuccess;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoCheckExist)
    {
        ListElementType *existElementPtr = ElementList.GetObjectByKey(KeyValue);
        if (existElementPtr != 0)
        {
            UFC::DoReadWriteUnlock(LockPtr, LockType);
            return isSuccess;
        }
    }

    if (ElementList.Add(KeyValue, ElementPtr) == TRUE) isSuccess = true;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isSuccess;
}  //InsertElementPtrIntoStrIdxHList()
//---------------------------------------------------------------------------
template<typename ListElementType>
ListElementType *RemoveElementPtrFromStrIdxHList(UFC::PHashedList<UFC::AnsiString, ListElementType*>& ElementList, 
                                                 const UFC::AnsiString& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    ListElementType *elementPtr = 0;
    if (KeyValue.Length() <= 0) return elementPtr;

    UFC::DoReadWriteLock(LockPtr, LockType);
    elementPtr = ElementList.GetObjectByKey(KeyValue);
    if (elementPtr != 0)
        ElementList.DeleteByKey(KeyValue);
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //RemoveElementPtrFromStrIdxHList()
//---------------------------------------------------------------------------
template<typename ListElementType>
int ClearElementPtrStrIdxHList(UFC::PHashedList<UFC::AnsiString, ListElementType*>& ElementList, 
                               bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    int deleteCount = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoDelete)
    {
        ListElementType *curElementPtr = ElementList.First();
        while (curElementPtr != 0)
        {
            delete curElementPtr;
            deleteCount++;
            curElementPtr = ElementList.Next();
        }
    }  //if (DoDelete)
    
    ElementList.Clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementPtrStrIdxHList()

//declare Map Function template
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int GetMapElementCount(std::map<MapKeyType, MapElementType>& ElementMap, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = static_cast<int>(ElementMap.size());
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetMapElementCount()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool FindElementPtrFromMap(std::map<MapKeyType, MapElementType*>& ElementMap, 
                           const MapKeyType& KeyValue, MapElementType Element, 
                           UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<MapKeyType, MapElementType>::iterator MapIterator;
    
    bool isExist = false;
    UFC::DoReadWriteLock(LockPtr, LockType);
    MapIterator it = ElementMap.find(KeyValue);
    if (it != ElementMap.end())
    {
        Element = it->second;
        isExist = true;
    }
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isExist;
}  //FindElementFromMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool InsertElementIntoMap(std::map<MapKeyType, MapElementType>& ElementMap, 
                          const MapKeyType& KeyValue, MapElementType& Element, 
                          bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<MapKeyType, MapElementType>::iterator MapIterator;
    typedef typename std::pair<MapIterator, bool> MapPair;
    typedef typename std::map<MapKeyType, MapElementType>::value_type MapValue;
    
    bool isSuccess = false;
    UFC::DoReadWriteLock(LockPtr, LockType);
    bool isExist = false;
    if (DoCheckExist)
    {
        MapIterator it = ElementMap.find(KeyValue);
        if (it != ElementMap.end()) isExist = true;
    }
    
    if (!isExist)
    {
        MapPair insertPair = ElementMap.insert(MapValue(KeyValue, Element));
        isSuccess = insertPair.second;
        
    }
    
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isSuccess;
}  //InsertElementIntoMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool RemoveElementFromMap(std::map<MapKeyType, MapElementType>& ElementMap, 
                          const MapKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<MapKeyType, MapElementType>::iterator MapIterator;

    bool isExist = false;    
    UFC::DoReadWriteLock(LockPtr, LockType);
    MapIterator it = ElementMap.find(KeyValue);
    if (it != ElementMap.end())
    {
        isExist = true;
        ElementMap.erase(it);
    }
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isExist;
}  //RemoveElementFromMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int ClearElementMap(std::map<MapKeyType, MapElementType>& ElementMap, 
                    bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int deleteCount = static_cast<int>(ElementMap.size());;
    ElementMap.clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int GetMapElementPtrCount(std::map<MapKeyType, MapElementType*>& ElementMap, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = static_cast<int>(ElementMap.size());
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetMapElementPtrCount()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
MapElementType *FindElementPtrFromMap(std::map<MapKeyType, MapElementType*>& ElementMap, 
                                      const MapKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<MapKeyType, MapElementType*>::iterator MapIterator;
    
    MapElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    MapIterator it = ElementMap.find(KeyValue);
    if (it != ElementMap.end()) elementPtr = it->second;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //FindElementPtrFromMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool InsertElementPtrIntoMap(std::map<MapKeyType, MapElementType*>& ElementMap, 
                             const MapKeyType& KeyValue, MapElementType *ElementPtr, bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<MapKeyType, MapElementType*>::iterator MapIterator;
    typedef typename std::pair<MapIterator, bool> MapPair;
    typedef typename std::map<MapKeyType, MapElementType*>::value_type MapValue;
    
    if (ElementPtr == 0) return false;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoCheckExist && (FindElementPtrFromMap(ElementMap, KeyValue, LockPtr, UFC::rwNotLock) != 0)) 
    {
        UFC::DoReadWriteUnlock(LockPtr, LockType);
        return false;
    }
    MapPair insertPair = ElementMap.insert(MapValue(KeyValue, ElementPtr));
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return insertPair.second;
}  //InsertElementPtrIntoMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
MapElementType *RemoveElementPtrFromMap(std::map<MapKeyType, MapElementType*>& ElementMap, 
                                        const MapKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<MapKeyType, MapElementType*>::iterator MapIterator;
    
    MapElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    MapIterator it = ElementMap.find(KeyValue);
    if (it != ElementMap.end())
    {
        elementPtr = it->second;
        ElementMap.erase(it);
    }
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //RemoveElementPtrFromMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int ClearElementPtrMap(std::map<MapKeyType, MapElementType*>& ElementMap, 
                       bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<MapKeyType, MapElementType*>::iterator MapIterator;
    
    int deleteCount = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoDelete)
    {
        MapIterator it;
        for (it = ElementMap.begin(); it != ElementMap.end(); it++)
        {
            MapElementType *curElementPtr = it->second;
            if (curElementPtr != 0)
            {
                deleteCount++;
                delete curElementPtr;
            }
        }  //for (it = ElementMap.begin(); it != ElementMap.end(); it++)
    }  //if (DoDelete)
    ElementMap.clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementPtrMap()
//---------------------------------------------------------------------------
template<typename MapElementType>
int GetStrIdxMapElementPtrCount(std::map<UFC::AnsiString, MapElementType*>& ElementMap, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = static_cast<int>(ElementMap.size());
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetStrIdxMapElementPtrCount()
//---------------------------------------------------------------------------
template<typename MapElementType>
MapElementType *FindElementPtrFromStrIdxMap(std::map<UFC::AnsiString, MapElementType*>& ElementMap, 
                                            const UFC::AnsiString& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    MapElementType *elementPtr = 0;
    if (KeyValue.Length() <= 0) return elementPtr;
    
    typedef typename std::map<UFC::AnsiString, MapElementType*>::iterator MapIterator;    
    UFC::DoReadWriteLock(LockPtr, LockType);
    MapIterator it = ElementMap.find(KeyValue);
    if (it != ElementMap.end()) elementPtr = it->second;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //FindElementPtrFromStrIdxMap()
//---------------------------------------------------------------------------
template<typename MapElementType>
bool InsertElementPtrIntoStrIdxMap(std::map<UFC::AnsiString, MapElementType*>& ElementMap, 
                                   const UFC::AnsiString& KeyValue, MapElementType *ElementPtr, bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<UFC::AnsiString, MapElementType*>::iterator MapIterator;
    typedef typename std::pair<MapIterator, bool> MapPair;
    typedef typename std::map<UFC::AnsiString, MapElementType*>::value_type MapValue;
    
    if ((KeyValue.Length() <= 0) || (ElementPtr == 0)) return false;
    
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoCheckExist && (FindElementPtrFromStrIdxMap(ElementMap, KeyValue, LockPtr, UFC::rwNotLock) != 0)) 
    {
        UFC::DoReadWriteUnlock(LockPtr, LockType);
        return false;
    }
    MapPair insertPair = ElementMap.insert(MapValue(KeyValue, ElementPtr));
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return insertPair.second;
}  //InsertElementPtrIntoStrIdxMap()
//---------------------------------------------------------------------------
template<typename MapElementType>
MapElementType *RemoveElementPtrFromStrIdxMap(std::map<UFC::AnsiString, MapElementType*>& ElementMap, 
                                              const UFC::AnsiString& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<UFC::AnsiString, MapElementType*>::iterator MapIterator;
    
	MapElementType *elementPtr = 0;
    if (KeyValue.Length() <= 0) return elementPtr;
    
    UFC::DoReadWriteLock(LockPtr, LockType);
    MapIterator it = ElementMap.find(KeyValue);
    if (it != ElementMap.end())
    {
        elementPtr = it->second;
        ElementMap.erase(it);
    }
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //RemoveElementPtrFromStrIdxMap()
//---------------------------------------------------------------------------
template<typename MapElementType>
int ClearElementPtrStrIdxMap(std::map<UFC::AnsiString, MapElementType*>& ElementMap, 
                             bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    typedef typename std::map<UFC::AnsiString, MapElementType*>::iterator MapIterator;
    
    int deleteCount = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoDelete)
    {
        MapIterator it;
        for (it = ElementMap.begin(); it != ElementMap.end(); it++)
        {
            MapElementType *curElementPtr = it->second;
            if (curElementPtr != 0)
            {
                deleteCount++;
                delete curElementPtr;
            }
        }  //for (it = ElementMap.begin(); it != ElementMap.end(); it++)
    }  //if (DoDelete)
    ElementMap.clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementPtrStrIdxMap()

//---------------------------------------------------------------------------
template<typename KeyType, typename ObjectType, typename FindType, typename CompareType>
class CObjectPtrIndexStore
{
public:
    typedef bool (*TObjectCompareFunction) (ObjectType* ObjPtr1, ObjectType* ObjPtr2);

private:
    bool                                FIsMyLock;
    UFC::PReadWriteLock*                FLockPtr;
    UFC::PHashMap<KeyType, ObjectType*> FObjectPtrMap;
    std::vector<ObjectType*>            FObjectPtrArray;
    bool                                FIsSorted;

public:
    CObjectPtrIndexStore(bool CreateLock)
    :FIsMyLock(CreateLock)
    ,FLockPtr(0)
    ,FIsSorted(false)
    {
        if (FIsMyLock) FLockPtr = new UFC::PReadWriteLock();
    };  //CObjectPtrIndexStore()

    CObjectPtrIndexStore(UFC::PReadWriteLock* LockPtr)
    :FIsMyLock(false)
    ,FLockPtr(LockPtr)
    ,FIsSorted(false)
    {
    };  //CObjectPtrIndexStore()

    ~CObjectPtrIndexStore()
    {
        Clear(cstNotDelete, UFC::rwLockForWrite);
        if (FIsMyLock && (FLockPtr != 0)) delete FLockPtr;
    };  //~CObjectPtrIndexStore()

    bool IsMyLock() {return FIsMyLock;};
    
    bool SetLock(UFC::PReadWriteLock* LockPtr)
    {
        if (FIsMyLock) return false;
        FLockPtr = LockPtr;
        return true;
    }
    
    int Lock(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        if (FLockPtr != 0) 
            return FLockPtr->Lock(LockType);
        else
            return 0;
    }
    
    int Unlock(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        if (FLockPtr != 0) 
            return FLockPtr->Unlock(LockType);
        else
            return 0;
    }
    
    int GetMapObjectCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        int objectCount = FObjectPtrMap.ItemCount();
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return objectCount;
    };

    int GetObjectCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        int objectCount = static_cast<int>(FObjectPtrArray.size());
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return objectCount;
    };

    ObjectType* FindObject(const KeyType& KeyValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        ObjectType* objectPtr = 0;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (FObjectPtrMap.GetObjectByKey(KeyValue, objectPtr) == FALSE) objectPtr = 0;
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return objectPtr;
    };

    ObjectType* FindObjectFromArray(const KeyType& KeyValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        ObjectType* objectPtr = 0;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        typename std::vector<ObjectType*>::iterator it = std::find_if(FObjectPtrArray.begin(), FObjectPtrArray.end(), FindType(KeyValue));
        if (it != FObjectPtrArray.end()) objectPtr = *it;
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return objectPtr;
    };

    ObjectType* GetObject(int Index, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        ObjectType* objectPtr = 0;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if ((Index >= 0) && (Index < static_cast<int>(FObjectPtrArray.size())))
            objectPtr = FObjectPtrArray[Index];
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return objectPtr;
    };

    bool AddObject(const KeyType& KeyValue, ObjectType* ObjectPtr, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        bool isSuccess = false;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (FObjectPtrMap.Add(KeyValue, ObjectPtr) == TRUE)
        {
            FObjectPtrArray.push_back(ObjectPtr);
            isSuccess = true;
            FIsSorted = false;
        }
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return isSuccess;
    };

    bool AddObject(const KeyType& KeyValue, ObjectType* ObjectPtr, bool DoSort, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        bool isSuccess = false;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (FObjectPtrMap.Add(KeyValue, ObjectPtr) == TRUE)
        {
            FObjectPtrArray.push_back(ObjectPtr);
            isSuccess = true;
            if (FObjectPtrArray.size() >= 2)
            {
                if (DoSort)
                {
                    FIsSorted = true;
                    std::sort(FObjectPtrArray.begin(), FObjectPtrArray.end(), CompareType());
                }
                else
                    FIsSorted = false;
            }
            else
                FIsSorted = true;
        }
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return isSuccess;
    };

    bool AddObjectToMap(const KeyType& KeyValue, ObjectType* ObjectPtr, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        bool isSuccess = false;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (FObjectPtrMap.Add(KeyValue, ObjectPtr) == TRUE)
        {
            isSuccess = true;
            FIsSorted = false;
        }
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return isSuccess;
    };
    
    ObjectType* RemoveObject(const KeyType& KeyValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        ObjectType* objectPtr = 0;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (FObjectPtrMap.GetObjectByKey(KeyValue, objectPtr) == TRUE)
        {
            FObjectPtrMap.DeleteByKey(KeyValue);
            typename std::vector<ObjectType*>::iterator it = std::find_if(FObjectPtrArray.begin(), FObjectPtrArray.end(), FindType(KeyValue));
            if (it != FObjectPtrArray.end()) FObjectPtrArray.erase(it);
        }
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return objectPtr;
    };

    ObjectType* RemoveObjectFromMap(const KeyType& KeyValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        ObjectType* objectPtr = 0;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (FObjectPtrMap.GetObjectByKey(KeyValue, objectPtr) == TRUE)
            FObjectPtrMap.DeleteByKey(KeyValue);
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return objectPtr;
    };

    int CopyObjectFromMapToArray(bool NeedClearArrayFirst, bool DoSort, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (NeedClearArrayFirst) FObjectPtrArray.clear();
        int copyCount = 0;
        ObjectType *curObjectPtr = FObjectPtrMap.First();
        while (curObjectPtr != 0)
        {
            FObjectPtrArray.push_back(curObjectPtr);
            copyCount++;
            curObjectPtr = FObjectPtrMap.Next();
        }
        
        if (DoSort)
        {
            FIsSorted = true;
            std::sort(FObjectPtrArray.begin(), FObjectPtrArray.end(), CompareType());
        }
        else
            FIsSorted = false;
        
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return copyCount;
    };

    int Clear(bool DoDelete = UFC::cstNotDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        int deleteCount = 0;
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (DoDelete)
        {
            ObjectType *curObjectPtr = FObjectPtrMap.First();
            while (curObjectPtr != 0)
            {
                delete curObjectPtr;
                deleteCount++;
                curObjectPtr = FObjectPtrMap.Next();
            }
        }  //if (DoDelete)

        FObjectPtrMap.Clear();
        FObjectPtrArray.clear();
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return deleteCount;
    };

    int ClearArray(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        int objectCount = static_cast<int>(FObjectPtrArray.size());
        FObjectPtrArray.clear();
        FIsSorted = false;
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return objectCount;
    };
    
    void Sort(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        if (FObjectPtrArray.size() >= 2)
        {
            FIsSorted = true;
            std::sort(FObjectPtrArray.begin(), FObjectPtrArray.end(), CompareType());
        }
        else
            FIsSorted = true;
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
    }

    bool IsSorted(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
    {
        if (FLockPtr != 0) FLockPtr->Lock(LockType);
        bool isSorted = FIsSorted;
        if (FLockPtr != 0) FLockPtr->Unlock(LockType);
        return isSorted;
    };
};  //CObjectPtrIndexStore

//---------------------------------------------------------------------------
template<typename QueueElementType>
int GetElementPtrQueueCount(std::queue<QueueElementType*>& ElementQueue)
{
    int elementCount = static_cast<int>(ElementQueue.size());
    return elementCount;
}  //GetElementPtrQueueCount()
//---------------------------------------------------------------------------
template<typename QueueElementType>
int GetElementPtrQueueCount_RWLock(std::queue<QueueElementType*>& ElementQueue, UFC::PReadWriteLock* LockPtr, UFC::RWLockTypeEnum LockType)
{
    if (LockPtr != 0) LockPtr->Lock(LockType);
    int elementCount = static_cast<int>(ElementQueue.size());
    if (LockPtr != 0) LockPtr->Unlock(LockType);
    return elementCount;
}  //GetElementPtrQueueCount_RWLock()
//---------------------------------------------------------------------------
template<typename QueueElementType>
int GetElementPtrQueueCount_CondLock(std::queue<QueueElementType*>& ElementQueue, UFC::PConditionMutex* LockPtr, UFC::RWLockTypeEnum LockType)
{
    if ((LockPtr != 0) && (LockType != rwNotLock)) LockPtr->Lock();
    int elementCount = static_cast<int>(ElementQueue.size());
    if ((LockPtr != 0) && (LockType != rwNotLock)) LockPtr->Unlock();
    return elementCount;
}  //GetElementPtrQueueCount_CondLock()
//---------------------------------------------------------------------------
template<typename QueueElementType>
QueueElementType *PopElementPtrFromQueue(std::queue<QueueElementType*>& ElementQueue)
{
    QueueElementType *elementPtr = 0;
    if (ElementQueue.size() > 0)
    {
        elementPtr = ElementQueue.front();
        ElementQueue.pop();
    }
    return elementPtr;    
}  //PopElementPtrFromQueue()
//---------------------------------------------------------------------------
template<typename QueueElementType>
QueueElementType *PopElementPtrFromQueue_RWLock(std::queue<QueueElementType*>& ElementQueue, UFC::PReadWriteLock* LockPtr, UFC::RWLockTypeEnum LockType)
{
    QueueElementType *elementPtr = 0;
    if (LockPtr != 0) LockPtr->Lock(LockType);
    if (ElementQueue.size() > 0)
    {
        elementPtr = ElementQueue.front();
        ElementQueue.pop();
    }
    if (LockPtr != 0) LockPtr->Unlock(LockType);
    return elementPtr;
}  //PopElementPtrFromQueue_RWLock()
//---------------------------------------------------------------------------
template<typename QueueElementType>
QueueElementType *PopElementPtrFromQueue_CondLock(std::queue<QueueElementType*>& ElementQueue, UFC::PConditionMutex* LockPtr, UFC::RWLockTypeEnum LockType)
{
    QueueElementType *elementPtr = 0;
    if ((LockPtr != 0) && (LockType != rwNotLock)) LockPtr->Lock();
    if (ElementQueue.size() > 0)
    {
        elementPtr = ElementQueue.front();
        ElementQueue.pop();
    }
    if ((LockPtr != 0) && (LockType != rwNotLock)) LockPtr->Unlock();
    return elementPtr;
}  //PopElementPtrFromQueue_CondLock()
//---------------------------------------------------------------------------
template<typename QueueElementType>
void PushElementPtrToQueue(std::queue<QueueElementType*>& ElementQueue, QueueElementType *ElementPtr)
{
    if (ElementPtr == 0) return;
    ElementQueue.push(ElementPtr);
}  //PushElementPtrToQueue()
//---------------------------------------------------------------------------
template<typename QueueElementType>
void PushElementPtrToQueue_RWLock(std::queue<QueueElementType*>& ElementQueue, QueueElementType *ElementPtr, UFC::PReadWriteLock* LockPtr, UFC::RWLockTypeEnum LockType)
{
    if (ElementPtr == 0) return;
    if (LockPtr != 0) LockPtr->Lock(LockType);
    ElementQueue.push(ElementPtr);
    if (LockPtr != 0) LockPtr->Unlock(LockType);
}  //PushElementPtrToQueue_RWLock()
//---------------------------------------------------------------------------
template<typename QueueElementType>
void PushElementPtrToQueue_CondLock(std::queue<QueueElementType*>& ElementQueue, QueueElementType *ElementPtr, UFC::PConditionMutex* LockPtr, UFC::RWLockTypeEnum LockType)
{
    if (ElementPtr == 0) return;
    if ((LockPtr != 0) && (LockType != rwNotLock)) LockPtr->Lock();
    ElementQueue.push(ElementPtr);
    if ((LockPtr != 0) && (LockType != rwNotLock)) LockPtr->Unlock();
}  //PushElementPtrToQueue_CondLock()
//---------------------------------------------------------------------------
template<typename QueueElementType>
int ClearElementPtrQueue(std::queue<QueueElementType*>& ElementQueue)
{
    int deleteCount = 0;
    while(ElementQueue.size() > 0)
    {
        QueueElementType *curPkgPtr = ElementQueue.front();
        ElementQueue.pop();
        delete curPkgPtr;
        deleteCount++;
    }
    return deleteCount;
}  //ClearElementPtrQueue_RWLock()
//---------------------------------------------------------------------------
template<typename QueueElementType>
int ClearElementPtrQueue_RWLock(std::queue<QueueElementType*>& ElementQueue, UFC::PReadWriteLock* LockPtr, UFC::RWLockTypeEnum LockType)
{
    int deleteCount = 0;
    if (LockPtr != 0) LockPtr->Lock(LockType);
    while(ElementQueue.size() > 0)
    {
        QueueElementType *curPkgPtr = ElementQueue.front();
        ElementQueue.pop();
        delete curPkgPtr;
        deleteCount++;
    }
    if (LockPtr != 0) LockPtr->Unlock(LockType);
    return deleteCount;
}  //ClearElementPtrQueue_RWLock()
//---------------------------------------------------------------------------
template<typename QueueElementType>
int ClearElementPtrQueue_CondLock(std::queue<QueueElementType*>& ElementQueue, UFC::PConditionMutex* LockPtr, UFC::RWLockTypeEnum LockType)
{
    int deleteCount = 0;
    if ((LockPtr != 0) && (LockType != rwNotLock)) LockPtr->Lock();
    while(ElementQueue.size() > 0)
    {
        QueueElementType *curPkgPtr = ElementQueue.front();
        ElementQueue.pop();
        delete curPkgPtr;
        deleteCount++;
    }
    if ((LockPtr != 0) && (LockType != rwNotLock)) LockPtr->Unlock();
    return deleteCount;
}  //ClearElementPtrQueue_CondLock()

}  //namespace UFC
#endif /* CONTAINERTEMPLATE_H */
