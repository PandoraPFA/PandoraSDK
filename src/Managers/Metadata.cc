/**
 *  @file   PandoraSDK/src/Managers/Metadata.cc
 * 
 *  @brief  Implementation of metadata classes.
 * 
 *  $Log: $
 */

#include "Managers/Metadata.h"

#include "Pandora/PandoraInternal.h"

#include <algorithm>

namespace pandora
{

CaloHitMetadata::CaloHitMetadata(CaloHitList *const pCaloHitList, const std::string &caloHitListName, const bool initialHitAvailability) :
    m_pCaloHitList(pCaloHitList),
    m_caloHitListName(caloHitListName)
{
    for (const CaloHit *const pCaloHit : *pCaloHitList)
    {
        if (!m_caloHitUsageMap.insert(CaloHitUsageMap::value_type(pCaloHit, initialHitAvailability)).second)
            throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitMetadata::~CaloHitMetadata()
{
    for (const CaloHitReplacement *const pCaloHitReplacement : m_caloHitReplacementList)
    {
        for (const CaloHit *const pCaloHit : pCaloHitReplacement->m_newCaloHits)
            delete pCaloHit;

        delete pCaloHitReplacement;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
bool CaloHitMetadata::IsAvailable(const CaloHit *const pCaloHit) const
{
    CaloHitUsageMap::const_iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

    if ((m_caloHitUsageMap.end()) == usageMapIter || !usageMapIter->second)
        return false;

    return true;
}

template <>
bool CaloHitMetadata::IsAvailable(const CaloHitList *const pCaloHitList) const
{
    for (const CaloHit *const pCaloHit : *pCaloHitList)
    {
        CaloHitUsageMap::const_iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

        if ((m_caloHitUsageMap.end()) == usageMapIter || !usageMapIter->second)
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode CaloHitMetadata::SetAvailability(const CaloHit *const pCaloHit, bool isAvailable)
{
    CaloHitUsageMap::iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

    if (m_caloHitUsageMap.end() == usageMapIter)
        return STATUS_CODE_NOT_FOUND;

    usageMapIter->second = isAvailable;

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode CaloHitMetadata::SetAvailability(const CaloHitList *const pCaloHitList, bool isAvailable)
{
    for (const CaloHit *const pCaloHit : *pCaloHitList)
    {
        CaloHitUsageMap::iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

        if (m_caloHitUsageMap.end() == usageMapIter)
            return STATUS_CODE_NOT_FOUND;

        usageMapIter->second = isAvailable;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitMetadata::Update(const CaloHitMetadata &caloHitMetadata)
{
    const CaloHitReplacementList &caloHitReplacementList(caloHitMetadata.GetCaloHitReplacementList());

    for (const CaloHitReplacement *const pCaloHitReplacement : caloHitReplacementList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(*pCaloHitReplacement));
    }

    const CaloHitUsageMap &caloHitUsageMap(caloHitMetadata.GetCaloHitUsageMap());

    CaloHitVector caloHitVector;
    for (const CaloHitUsageMap::value_type &mapEntry : caloHitUsageMap) caloHitVector.push_back(mapEntry.first);
    std::sort(caloHitVector.begin(), caloHitVector.end(), PointerLessThan<CaloHit>());

    for (const CaloHit *const pCaloHit : caloHitVector)
    {
        CaloHitUsageMap::iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

        if (m_caloHitUsageMap.end() == usageMapIter)
            return STATUS_CODE_FAILURE;

        usageMapIter->second = caloHitUsageMap.at(pCaloHit);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitMetadata::Update(const CaloHitReplacement &caloHitReplacement)
{
    for (const CaloHit *const pCaloHit : caloHitReplacement.m_newCaloHits)
    {
        if (m_pCaloHitList->end() != std::find(m_pCaloHitList->begin(), m_pCaloHitList->end(), pCaloHit))
            return STATUS_CODE_ALREADY_PRESENT;

        m_pCaloHitList->push_back(pCaloHit);

        if (!m_caloHitUsageMap.insert(CaloHitUsageMap::value_type(pCaloHit, true)).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    if (m_pCaloHitList == &caloHitReplacement.m_oldCaloHits)
        return STATUS_CODE_FAILURE;

    for (const CaloHit *const pCaloHit : caloHitReplacement.m_oldCaloHits)
    {
        CaloHitList::iterator listIter = std::find(m_pCaloHitList->begin(), m_pCaloHitList->end(), pCaloHit);

        if (m_pCaloHitList->end() == listIter)
            return STATUS_CODE_FAILURE;

        listIter = m_pCaloHitList->erase(listIter);

        CaloHitUsageMap::iterator mapIter = m_caloHitUsageMap.find(pCaloHit);

        if (m_caloHitUsageMap.end() == mapIter)
            return STATUS_CODE_FAILURE;

        mapIter = m_caloHitUsageMap.erase(mapIter);
    }

    m_caloHitReplacementList.push_back(new CaloHitReplacement(caloHitReplacement));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitMetadata::Clear()
{
    for (const CaloHitReplacement *const pCaloHitReplacement : m_caloHitReplacementList)
        delete pCaloHitReplacement;

    m_pCaloHitList = nullptr;
    m_caloHitListName.clear();
    m_caloHitUsageMap.clear();
    m_caloHitReplacementList.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ReclusterMetadata::ReclusterMetadata(CaloHitList *const pCaloHitList) :
    m_pCurrentCaloHitMetadata(nullptr),
    m_caloHitList(*pCaloHitList)
{
    if (m_caloHitList.empty())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
}

//------------------------------------------------------------------------------------------------------------------------------------------

ReclusterMetadata::~ReclusterMetadata()
{
    for (const NameToMetadataMap::value_type &mapEntry : m_nameToMetadataMap)
        delete mapEntry.second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterMetadata::CreateCaloHitMetadata(CaloHitList *const pCaloHitList, const std::string &caloHitListName,
    const std::string &reclusterListName, const bool initialHitAvailability)
{
    m_pCurrentCaloHitMetadata = new CaloHitMetadata(pCaloHitList, caloHitListName, initialHitAvailability);

    if (!m_nameToMetadataMap.insert(NameToMetadataMap::value_type(reclusterListName, m_pCurrentCaloHitMetadata)).second)
    {
        delete m_pCurrentCaloHitMetadata;
        return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterMetadata::ExtractCaloHitMetadata(const std::string &reclusterListName, CaloHitMetadata *&pCaloHitMetaData)
{
    NameToMetadataMap::iterator iter = m_nameToMetadataMap.find(reclusterListName);

    if (m_nameToMetadataMap.end() == iter)
        return STATUS_CODE_FAILURE;

    pCaloHitMetaData = iter->second;
    iter = m_nameToMetadataMap.erase(iter);

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
