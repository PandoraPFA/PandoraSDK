/**
 *  @file   PandoraSDK/src/Managers/Manager.cc
 * 
 *  @brief  Implementation of the manager class.
 * 
 *  $Log: $
 */

#include "Managers/Manager.h"

namespace pandora
{

template<typename T>
const std::string Manager<T>::NULL_LIST_NAME = "NullList";

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
Manager<T>::Manager(const Pandora *const pPandora) :
    m_currentListName(NULL_LIST_NAME),
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
Manager<T>::~Manager()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::GetList(const std::string &listName, const ObjectList *&pObjectList) const
{
    typename NameToListMap::const_iterator iter = m_nameToListMap.find(listName);

    if (m_nameToListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    pObjectList = iter->second;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::GetCurrentList(const ObjectList *&pObjectList, std::string &listName) const
{
    listName = m_currentListName;
    return this->GetList(listName, pObjectList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode Manager<T>::GetCurrentListName(std::string &listName) const
{
    if (m_currentListName.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    listName = m_currentListName;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::GetAlgorithmInputList(const Algorithm *const pAlgorithm, const ObjectList *&pObjectList, std::string &listName) const
{
    typename AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() != iter)
    {
        listName = iter->second.m_parentListName;
    }
    else
    {
        listName = m_currentListName;
    }

    return this->GetList(listName, pObjectList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode Manager<T>::GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &listName) const
{
    typename AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return this->GetCurrentListName(listName);

    listName = iter->second.m_parentListName;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm)
{
    return this->GetAlgorithmInputListName(pAlgorithm, m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &listName)
{
    if (m_nameToListMap.end() == m_nameToListMap.find(listName))
        return STATUS_CODE_NOT_FOUND;

    if (m_savedLists.end() == m_savedLists.find(listName))
        return STATUS_CODE_NOT_ALLOWED;

    if (m_algorithmInfoMap.end() == m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_FAILURE;

    m_currentListName = listName;

    for (typename AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.begin(), iterEnd = m_algorithmInfoMap.end(); iter != iterEnd; ++iter)
    {
        iter->second.m_parentListName = listName;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::DropCurrentList(const Algorithm *const pAlgorithm)
{
    return this->ReplaceCurrentAndAlgorithmInputLists(pAlgorithm, NULL_LIST_NAME);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName)
{
    typename AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

    if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
        return STATUS_CODE_ALREADY_PRESENT;

    m_nameToListMap[temporaryListName] = new ObjectList;
    m_currentListName = temporaryListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::RegisterAlgorithm(const Algorithm *const pAlgorithm)
{
    if (m_algorithmInfoMap.end() != m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_ALREADY_PRESENT;

    AlgorithmInfo algorithmInfo;
    algorithmInfo.m_parentListName = m_currentListName;
    algorithmInfo.m_numberOfListsCreated = 0;

    if (!m_algorithmInfoMap.insert(typename AlgorithmInfoMap::value_type(pAlgorithm, algorithmInfo)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    typename AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        typename NameToListMap::iterator iter = m_nameToListMap.find(*listNameIter);

        if (m_nameToListMap.end() == iter)
            return STATUS_CODE_FAILURE;

        delete iter->second;
        m_nameToListMap.erase(iter);
    }

    algorithmListIter->second.m_temporaryListNames.clear();
    m_currentListName = algorithmListIter->second.m_parentListName;

    if (isAlgorithmFinished)
        m_algorithmInfoMap.erase(algorithmListIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::ResetForNextEvent()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->EraseAllContent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::EraseAllContent()
{
    for (typename NameToListMap::iterator iter = m_nameToListMap.begin(); iter != m_nameToListMap.end();)
    {
        delete iter->second;
        m_nameToListMap.erase(iter++);
    }

    m_currentListName = NULL_LIST_NAME;
    m_nameToListMap.clear();
    m_savedLists.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::CreateInitialLists()
{
    if (!m_nameToListMap.empty() || !m_savedLists.empty())
        return STATUS_CODE_NOT_ALLOWED;

    m_nameToListMap[NULL_LIST_NAME] = new ObjectList;
    m_savedLists.insert(NULL_LIST_NAME);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class Manager<CaloHit>;
template class Manager<Cluster>;
template class Manager<MCParticle>;
template class Manager<ParticleFlowObject>;
template class Manager<Track>;
template class Manager<Vertex>;

} // namespace pandora
