/**
 *  @file   PandoraSDK/src/Managers/InputObjectManager.cc
 * 
 *  @brief  Implementation of the input object manager class.
 * 
 *  $Log: $
 */

#include "Managers/InputObjectManager.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"

namespace pandora
{

template<typename T>
const std::string InputObjectManager<T>::INPUT_LIST_NAME = "Input";

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
InputObjectManager<T>::InputObjectManager(const Pandora *const pPandora) :
    Manager<T>(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
InputObjectManager<T>::~InputObjectManager()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::CreateInputList()
{
    typename Manager<T>::NameToListMap::iterator existingListIter = Manager<T>::m_nameToListMap.find(INPUT_LIST_NAME);

    if (Manager<T>::m_nameToListMap.end() == existingListIter)
        return STATUS_CODE_FAILURE;

    Manager<T>::m_currentListName = INPUT_LIST_NAME;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ObjectList &objectList,
    std::string &temporaryListName)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, Manager<T>::CreateTemporaryListAndSetCurrent(pAlgorithm, temporaryListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddObjectsToList(temporaryListName, objectList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::SaveList(const std::string &listName, const ObjectList &objectList)
{
    if (Manager<T>::m_nameToListMap.end() != Manager<T>::m_nameToListMap.find(listName))
        return this->AddObjectsToList(listName, objectList);

    if (!Manager<T>::m_nameToListMap.insert(typename Manager<T>::NameToListMap::value_type(listName, new ObjectList)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    *(Manager<T>::m_nameToListMap[listName]) = objectList;
    Manager<T>::m_savedLists.insert(listName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::AddObjectsToList(const std::string &listName, const ObjectList &objectList)
{
    if (Manager<T>::NULL_LIST_NAME == listName)
        return STATUS_CODE_NOT_ALLOWED;

    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(listName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    ObjectList *pSavedList = listIter->second;

    if (pSavedList == &objectList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (typename ObjectList::const_iterator iter = objectList.begin(), iterEnd = objectList.end(); iter != iterEnd; ++iter)
    {
        if (!pSavedList->insert(*iter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::RemoveObjectsFromList(const std::string &listName, const ObjectList &objectList)
{
    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(listName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    ObjectList *pSavedList = listIter->second;

    if (pSavedList == &objectList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (typename ObjectList::const_iterator iter = objectList.begin(), iterEnd = objectList.end(); iter != iterEnd; ++iter)
    {
        typename ObjectList::iterator savedObjectIter = pSavedList->find(*iter);

        if (pSavedList->end() != savedObjectIter)
            pSavedList->erase(savedObjectIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::EraseAllContent()
{
    typename Manager<T>::NameToListMap::const_iterator inputIter = Manager<T>::m_nameToListMap.find(INPUT_LIST_NAME);

    if (Manager<T>::m_nameToListMap.end() == inputIter)
    {
        std::cout << "InputObjectManager::EraseAllContent cannot retrieve object list " << std::endl;
    }
    else
    {
        for (typename ObjectList::iterator iter = inputIter->second->begin(), iterEnd = inputIter->second->end(); iter != iterEnd; ++iter)
            delete *iter;
    }

    return Manager<T>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::CreateInitialLists()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, Manager<T>::CreateInitialLists());
    Manager<T>::m_nameToListMap[INPUT_LIST_NAME] = new ObjectList;
    Manager<T>::m_savedLists.insert(INPUT_LIST_NAME);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class InputObjectManager<CaloHit>;
template class InputObjectManager<MCParticle>;
template class InputObjectManager<Track>;

} // namespace pandora
