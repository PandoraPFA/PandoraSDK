/**
 *  @file   PandoraSDK/src/Managers/AlgorithmObjectManager.cc
 * 
 *  @brief  Implementation of the algorithm object manager class.
 * 
 *  $Log: $
 */

#include "Managers/AlgorithmObjectManager.h"

#include "Objects/Cluster.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Vertex.h"

#include <algorithm>

namespace pandora
{

template<typename T>
AlgorithmObjectManager<T>::AlgorithmObjectManager(const Pandora *const pPandora) :
    Manager<T>(pPandora),
    m_canMakeNewObjects(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
AlgorithmObjectManager<T>::~AlgorithmObjectManager()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName)
{
    m_canMakeNewObjects = true;
    return Manager<T>::CreateTemporaryListAndSetCurrent(pAlgorithm, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::MoveObjectsToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
    std::string &temporaryListName, const ObjectList &objectsToMove)
{
    if (objectsToMove.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateTemporaryListAndSetCurrent(pAlgorithm, temporaryListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MoveObjectsBetweenLists(temporaryListName, originalListName, &objectsToMove));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::SaveObjects(const std::string &targetListName, const std::string &sourceListName)
{
    typename Manager<T>::NameToListMap::iterator targetObjectListIter = Manager<T>::m_nameToListMap.find(targetListName);

    if (Manager<T>::m_nameToListMap.end() == targetObjectListIter)
    {
        Manager<T>::m_nameToListMap[targetListName] = new ObjectList;
        Manager<T>::m_savedLists.insert(targetListName);
    }

    return this->MoveObjectsBetweenLists(targetListName, sourceListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::SaveObjects(const std::string &targetListName, const std::string &sourceListName, const ObjectList &objectsToSave)
{
    if (objectsToSave.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    typename Manager<T>::NameToListMap::iterator targetObjectListIter = Manager<T>::m_nameToListMap.find(targetListName);

    if (Manager<T>::m_nameToListMap.end() == targetObjectListIter)
    {
        Manager<T>::m_nameToListMap[targetListName] = new ObjectList;
        Manager<T>::m_savedLists.insert(targetListName);
    }

    return this->MoveObjectsBetweenLists(targetListName, sourceListName, &objectsToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::MoveObjectsBetweenLists(const std::string &targetListName, const std::string &sourceListName,
    const ObjectList *const pObjectSubset)
{
    if (Manager<T>::m_nullListName == targetListName)
        return STATUS_CODE_NOT_ALLOWED;

    typename Manager<T>::NameToListMap::iterator sourceListIter = Manager<T>::m_nameToListMap.find(sourceListName);

    if (Manager<T>::m_nameToListMap.end() == sourceListIter)
        return STATUS_CODE_NOT_FOUND;

    if (sourceListIter->second->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    typename Manager<T>::NameToListMap::iterator targetListIter = Manager<T>::m_nameToListMap.find(targetListName);

    if (Manager<T>::m_nameToListMap.end() == targetListIter)
        return STATUS_CODE_FAILURE;

    if (!pObjectSubset)
    {
        for (const T *const pT : *sourceListIter->second)
        {
            if (targetListIter->second->end() != std::find(targetListIter->second->begin(), targetListIter->second->end(), pT))
                return STATUS_CODE_ALREADY_PRESENT;

            targetListIter->second->push_back(pT);
        }

        sourceListIter->second->clear();
    }
    else
    {
        if ((sourceListIter->second == pObjectSubset) || (targetListIter->second == pObjectSubset))
            return STATUS_CODE_INVALID_PARAMETER;

        for (const T *const pT : *pObjectSubset)
        {
            typename ObjectList::iterator objectIter = std::find(sourceListIter->second->begin(), sourceListIter->second->end(), pT);

            if (sourceListIter->second->end() == objectIter)
                return STATUS_CODE_NOT_FOUND;

            if (targetListIter->second->end() != std::find(targetListIter->second->begin(), targetListIter->second->end(), pT))
                return STATUS_CODE_ALREADY_PRESENT;

            targetListIter->second->push_back(pT);
            objectIter = sourceListIter->second->erase(objectIter);
        }
    }

    m_canMakeNewObjects = false;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::TemporarilyReplaceCurrentList(const std::string &listName)
{
    if (Manager<T>::m_nameToListMap.end() == Manager<T>::m_nameToListMap.find(listName))
        return STATUS_CODE_NOT_FOUND;

    m_canMakeNewObjects = false;
    Manager<T>::m_currentListName = listName;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::DeleteObject(const T *const pT, const std::string &listName)
{
    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(listName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    typename ObjectList::iterator deletionIter = std::find(listIter->second->begin(), listIter->second->end(), pT);

    if (listIter->second->end() == deletionIter)
        return STATUS_CODE_NOT_FOUND;

    deletionIter = listIter->second->erase(deletionIter);
    delete pT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::DeleteObjects(const ObjectList &objectList, const std::string &listName)
{
    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(listName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    if (listIter->second == &objectList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (const T *const pT : objectList)
    {
        typename ObjectList::iterator deletionIter = std::find(listIter->second->begin(), listIter->second->end(), pT);

        if (listIter->second->end() == deletionIter)
            return STATUS_CODE_NOT_FOUND;

        deletionIter = listIter->second->erase(deletionIter);
        delete pT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::DeleteTemporaryObjects(const Algorithm *const pAlgorithm, const std::string &temporaryListName)
{
    if (Manager<T>::m_savedLists.end() != Manager<T>::m_savedLists.find(temporaryListName))
        return STATUS_CODE_NOT_ALLOWED;

    typename Manager<T>::AlgorithmInfoMap::const_iterator algorithmIter = Manager<T>::m_algorithmInfoMap.find(pAlgorithm);

    if (Manager<T>::m_algorithmInfoMap.end() == algorithmIter)
        return STATUS_CODE_NOT_FOUND;

    if (algorithmIter->second.m_temporaryListNames.end() == algorithmIter->second.m_temporaryListNames.find(temporaryListName))
        return STATUS_CODE_NOT_ALLOWED;

    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(temporaryListName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_FAILURE;

    for (const T *const pT : *listIter->second)
        delete pT;

    listIter->second->clear();
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::GetResetDeletionObjects(const Algorithm *const pAlgorithm, ObjectList &objectList) const
{
    typename Manager<T>::AlgorithmInfoMap::const_iterator algorithmIter = Manager<T>::m_algorithmInfoMap.find(pAlgorithm);

    if (Manager<T>::m_algorithmInfoMap.end() == algorithmIter)
        return STATUS_CODE_NOT_FOUND;

    for (const std::string &temporaryListName : algorithmIter->second.m_temporaryListNames)
    {
        typename Manager<T>::NameToListMap::const_iterator listIter = Manager<T>::m_nameToListMap.find(temporaryListName);

        if (Manager<T>::m_nameToListMap.end() == listIter)
            return STATUS_CODE_FAILURE;

        objectList.insert(objectList.end(), listIter->second->begin(), listIter->second->end());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm)
{
    m_canMakeNewObjects = false;
    return Manager<T>::ResetCurrentListToAlgorithmInputList(pAlgorithm);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &listName)
{
    m_canMakeNewObjects = false;
    return Manager<T>::ReplaceCurrentAndAlgorithmInputLists(pAlgorithm, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::DropCurrentList(const Algorithm *const pAlgorithm)
{
    m_canMakeNewObjects = false;
    return Manager<T>::DropCurrentList(pAlgorithm);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::RenameList(const std::string &oldListName, const std::string &newListName)
{
    if (m_canMakeNewObjects)
        return STATUS_CODE_NOT_ALLOWED;

    return Manager<T>::RenameList(oldListName, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    ObjectList objectList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetResetDeletionObjects(pAlgorithm, objectList));

    for (const T *const pT : objectList)
        delete pT;

    m_canMakeNewObjects = false;
    return Manager<T>::ResetAlgorithmInfo(pAlgorithm, isAlgorithmFinished);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::EraseAllContent()
{
    for (const typename Manager<T>::NameToListMap::value_type &mapEntry : Manager<T>::m_nameToListMap)
    {
        for (const T *const pT : *mapEntry.second)
            delete pT;
    }

    m_canMakeNewObjects = false;
    return Manager<T>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class AlgorithmObjectManager<Cluster>;
template class AlgorithmObjectManager<ParticleFlowObject>;
template class AlgorithmObjectManager<Vertex>;

} // namespace pandora
