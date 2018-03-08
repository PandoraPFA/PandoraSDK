/**
 *  @file   PandoraSDK/src/Managers/Manager.cc
 * 
 *  @brief  Implementation of the manager class.
 * 
 *  $Log: $
 */

#include "Managers/Manager.h"

#include "Pandora/Algorithm.h"

namespace pandora
{

template<typename T>
Manager<T>::Manager(const Pandora *const pPandora) :
    m_nullListName("NullList"),
    m_pPandora(pPandora),
    m_currentListName(m_nullListName)
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

    for (typename AlgorithmInfoMap::value_type &mapEntry : m_algorithmInfoMap)
    {
        mapEntry.second.m_parentListName = listName;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::DropCurrentList(const Algorithm *const pAlgorithm)
{
    return this->ReplaceCurrentAndAlgorithmInputLists(pAlgorithm, m_nullListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::RenameList(const std::string &oldListName, const std::string &newListName)
{
    if ((oldListName == newListName) || newListName.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_nameToListMap.end() == m_nameToListMap.find(oldListName))
        return STATUS_CODE_NOT_FOUND;

    if ((oldListName == m_nullListName) || (m_savedLists.end() == m_savedLists.find(oldListName)))
        return STATUS_CODE_NOT_ALLOWED;

    if (m_savedLists.end() != m_savedLists.find(newListName))
        return STATUS_CODE_ALREADY_PRESENT;

    m_nameToListMap[newListName] = m_nameToListMap.at(oldListName);
    m_nameToListMap.erase(oldListName);

    m_savedLists.insert(newListName);
    m_savedLists.erase(oldListName);

    if (oldListName == m_currentListName)
        m_currentListName = newListName;

    for (typename AlgorithmInfoMap::value_type &mapEntry : m_algorithmInfoMap)
    {
        if (oldListName == mapEntry.second.m_parentListName)
            mapEntry.second.m_parentListName = newListName;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName)
{
    typename AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = pAlgorithm->GetInstanceName() + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

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
    typename AlgorithmInfoMap::iterator algorithmIter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == algorithmIter)
        return STATUS_CODE_NOT_FOUND;

    for (const std::string &temporaryListName : algorithmIter->second.m_temporaryListNames)
    {
        typename NameToListMap::iterator iter = m_nameToListMap.find(temporaryListName);

        if (m_nameToListMap.end() == iter)
            return STATUS_CODE_FAILURE;

        ObjectList *const pObjectList(iter->second);
        iter = m_nameToListMap.erase(iter);
        delete pObjectList;
    }

    algorithmIter->second.m_temporaryListNames.clear();
    m_currentListName = algorithmIter->second.m_parentListName;

    if (isAlgorithmFinished)
        algorithmIter = m_algorithmInfoMap.erase(algorithmIter);

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
    for (const typename NameToListMap::value_type &mapEntry : m_nameToListMap)
        delete mapEntry.second;

    m_currentListName = m_nullListName;
    m_nameToListMap.clear();
    m_savedLists.clear();
    m_algorithmInfoMap.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode Manager<T>::CreateInitialLists()
{
    if (!m_nameToListMap.empty() || !m_savedLists.empty())
        return STATUS_CODE_NOT_ALLOWED;

    m_nameToListMap[m_nullListName] = new ObjectList;
    m_savedLists.insert(m_nullListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
T *Manager<T>::Modifiable(const T *const pT) const
{
    // Consider alternatives here. Idea: expose algorithms only to pointers to const objects, non-const functions accessible only via APIs.
    return const_cast<T*>(pT);
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
