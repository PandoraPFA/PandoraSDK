/**
 *  @file   PandoraPFANew/Framework/src/Managers/CaloHitManager.cc
 * 
 *  @brief  Implementation of the calo hit manager class.
 * 
 *  $Log: $
 */

#include "Helpers/CaloHitHelper.h"
#include "Helpers/GeometryHelper.h"

#include "Managers/CaloHitManager.h"

#include "Objects/Cluster.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>

namespace pandora
{

CaloHitManager::CaloHitManager() :
    InputObjectManager<CaloHit>(),
    m_nReclusteringProcesses(0),
    m_pCurrentReclusterMetadata(NULL)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::~CaloHitManager()
{
    (void) this->EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS>
StatusCode CaloHitManager::CreateCaloHit(const PARAMETERS &parameters)
{
    CaloHit *pCaloHit = NULL;

    try
    {
        pCaloHit = this->HitInstantiation(parameters);

        if (NULL == pCaloHit)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        const PseudoLayer pseudoLayer = GeometryHelper::GetPseudoLayer(pCaloHit->GetPositionVector());
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCaloHit->SetPseudoLayer(pseudoLayer));

        NameToListMap::iterator inputIter = m_nameToListMap.find(INPUT_LIST_NAME);

        if ((m_nameToListMap.end() == inputIter) || !inputIter->second->insert(pCaloHit).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create calo hit: " << statusCodeException.ToString() << std::endl;
        delete pCaloHit;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS>
CaloHit *CaloHitManager::HitInstantiation(const PARAMETERS &parameters)
{
    return NULL;
}

template <>
CaloHit *CaloHitManager::HitInstantiation(const PandoraApi::RectangularCaloHitParameters &parameters)
{
    return new RectangularCaloHit(parameters);
}

template <>
CaloHit *CaloHitManager::HitInstantiation(const PandoraApi::PointingCaloHitParameters &parameters)
{
    return new PointingCaloHit(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CalculateCaloHitProperties() const
{
    try
    {
        NameToListMap::const_iterator listIter = m_nameToListMap.find(INPUT_LIST_NAME);

        if (m_nameToListMap.end() == listIter)
            return STATUS_CODE_NOT_INITIALIZED;

        OrderedCaloHitList orderedCaloHitList;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(*(listIter->second)));

        for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
        {
            for (CaloHitList::iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                CaloHitHelper::CalculateCaloHitProperties(*hitIter, &orderedCaloHitList);
            }
        }
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "CaloHitManager: Failed to calculate calo hit properties, " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ClusterList &clusterList,
    std::string &temporaryListName)
{
    if (clusterList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    CaloHitList caloHitList;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);
        caloHitList.insert(pCluster->GetIsolatedCaloHitList().begin(), pCluster->GetIsolatedCaloHitList().end());
    }

    return InputObjectManager<CaloHit>::CreateTemporaryListAndSetCurrent(pAlgorithm, caloHitList, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::EraseAllContent()
{
    for (ReclusterMetadataList::iterator iter = m_reclusterMetadataList.begin(), iterEnd = m_reclusterMetadataList.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_nReclusteringProcesses = 0;
    m_pCurrentReclusterMetadata = NULL;
    m_reclusterMetadataList.clear();

    return InputObjectManager<CaloHit>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::MatchCaloHitsToMCPfoTargets(const UidToMCParticleMap &caloHitToPfoTargetMap)
{
    if (caloHitToPfoTargetMap.empty())
        return STATUS_CODE_SUCCESS;

    NameToListMap::const_iterator inputIter = m_nameToListMap.find(INPUT_LIST_NAME);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (CaloHitList::iterator iter = inputIter->second->begin(), iterEnd = inputIter->second->end(); iter != iterEnd; ++iter)
    {
        UidToMCParticleMap::const_iterator pfoTargetIter = caloHitToPfoTargetMap.find((*iter)->GetParentCaloHitAddress());

        if (caloHitToPfoTargetMap.end() != pfoTargetIter)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetMCParticle(pfoTargetIter->second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::RemoveAllMCParticleRelationships()
{
    NameToListMap::const_iterator inputIter = m_nameToListMap.find(INPUT_LIST_NAME);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (CaloHitList::const_iterator iter = inputIter->second->begin(), iterEnd = inputIter->second->end(); iter != iterEnd; ++iter)
        (*iter)->RemoveMCParticle();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::AreCaloHitsAvailable(const CaloHitList &caloHitList) const
{
    if (0 == m_nReclusteringProcesses)
    {
        for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
        {
            if (!(*iter)->m_isAvailable)
                return false;
        }
        return true;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->AreCaloHitsAvailable(caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::SetCaloHitAvailability(CaloHitList &caloHitList, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
    {
        for (CaloHitList::iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
        {
            (*iter)->m_isAvailable = isAvailable;
        }
        return STATUS_CODE_SUCCESS;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->SetCaloHitAvailability(caloHitList, isAvailable);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::FragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1, CaloHit *&pDaughterCaloHit1, CaloHit *&pDaughterCaloHit2)
{
    pDaughterCaloHit1 = NULL; pDaughterCaloHit2 = NULL;

    if (!this->CanFragmentCaloHit(pOriginalCaloHit, fraction1))
        return STATUS_CODE_NOT_ALLOWED;

    if (RECTANGULAR == pOriginalCaloHit->GetCellGeometry())
    {
        RectangularCaloHit *pOriginalRectangularCaloHit = dynamic_cast<RectangularCaloHit *>(pOriginalCaloHit);
        pDaughterCaloHit1 = new RectangularCaloHit(pOriginalRectangularCaloHit, fraction1);
        pDaughterCaloHit2 = new RectangularCaloHit(pOriginalRectangularCaloHit, 1.f - fraction1);
    }
    else if (POINTING == pOriginalCaloHit->GetCellGeometry())
    {
        PointingCaloHit *pOriginalPointingCaloHit = dynamic_cast<PointingCaloHit *>(pOriginalCaloHit);
        pDaughterCaloHit1 = new PointingCaloHit(pOriginalPointingCaloHit, fraction1);
        pDaughterCaloHit2 = new PointingCaloHit(pOriginalPointingCaloHit, 1.f - fraction1);
    }

    if ((NULL == pDaughterCaloHit1) || (NULL == pDaughterCaloHit2))
        return STATUS_CODE_FAILURE;

    CaloHitReplacement caloHitReplacement;
    caloHitReplacement.m_oldCaloHits.insert(pOriginalCaloHit);
    caloHitReplacement.m_newCaloHits.insert(pDaughterCaloHit1); caloHitReplacement.m_newCaloHits.insert(pDaughterCaloHit2);

    if (m_nReclusteringProcesses > 0)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->Update(caloHitReplacement));
    }
    else
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(caloHitReplacement));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::MergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2, CaloHit *&pMergedCaloHit)
{
    pMergedCaloHit = NULL;

    if (!this->CanMergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2))
        return STATUS_CODE_NOT_ALLOWED;

    const float newWeight((pFragmentCaloHit1->GetWeight() + pFragmentCaloHit2->GetWeight()) / pFragmentCaloHit1->GetWeight());

    if ((RECTANGULAR == pFragmentCaloHit1->GetCellGeometry()) && (RECTANGULAR == pFragmentCaloHit2->GetCellGeometry()))
    {
        RectangularCaloHit *pOriginalRectangularCaloHit = dynamic_cast<RectangularCaloHit *>(pFragmentCaloHit1);
        pMergedCaloHit = new RectangularCaloHit(pOriginalRectangularCaloHit, newWeight);
    }
    else if ((POINTING == pFragmentCaloHit1->GetCellGeometry()) && (POINTING == pFragmentCaloHit2->GetCellGeometry()))
    {
        PointingCaloHit *pOriginalPointingCaloHit = dynamic_cast<PointingCaloHit *>(pFragmentCaloHit1);
        pMergedCaloHit = new PointingCaloHit(pOriginalPointingCaloHit, newWeight);
    }

    if (NULL == pMergedCaloHit)
        return STATUS_CODE_FAILURE;

    CaloHitReplacement caloHitReplacement;
    caloHitReplacement.m_newCaloHits.insert(pMergedCaloHit);
    caloHitReplacement.m_oldCaloHits.insert(pFragmentCaloHit1); caloHitReplacement.m_oldCaloHits.insert(pFragmentCaloHit2);

    if (m_nReclusteringProcesses > 0)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->Update(caloHitReplacement));
    }
    else
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(caloHitReplacement));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::CanFragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1) const
{
    if ((fraction1 < std::numeric_limits<float>::epsilon()) || (fraction1 > 1.f))
        return false;

    if (!this->IsCaloHitAvailable(pOriginalCaloHit))
        return false;

    NameToListMap::const_iterator iter = m_nameToListMap.find(m_currentListName);

    if (m_nameToListMap.end() == iter)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    if (iter->second->end() == iter->second->find(pOriginalCaloHit))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::CanMergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2) const
{
    if (pFragmentCaloHit1->GetWeight() < std::numeric_limits<float>::epsilon())
        return false;

    if (pFragmentCaloHit1->GetParentCaloHitAddress() != pFragmentCaloHit2->GetParentCaloHitAddress())
        return false;

    if (!this->IsCaloHitAvailable(pFragmentCaloHit1) || !this->IsCaloHitAvailable(pFragmentCaloHit2))
        return false;

    NameToListMap::const_iterator iter = m_nameToListMap.find(m_currentListName);

    if (m_nameToListMap.end() == iter)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    if ((iter->second->end() == iter->second->find(pFragmentCaloHit1)) || (iter->second->end() == iter->second->find(pFragmentCaloHit2)))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::InitializeReclustering(const Algorithm *const pAlgorithm, const ClusterList &clusterList,
    const std::string &originalReclusterListName)
{
    std::string caloHitListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateTemporaryListAndSetCurrent(pAlgorithm, clusterList, caloHitListName));
    CaloHitList *pCaloHitList = m_nameToListMap[caloHitListName];

    m_pCurrentReclusterMetadata = new ReclusterMetadata(pCaloHitList);
    m_reclusterMetadataList.push_back(m_pCurrentReclusterMetadata);

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->CreateCaloHitMetadata(pCaloHitList, caloHitListName,
        originalReclusterListName, false));

    ++m_nReclusteringProcesses;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::PrepareForClustering(const Algorithm *const pAlgorithm, const std::string &newReclusterListName)
{
    if (0 == m_nReclusteringProcesses)
        return STATUS_CODE_SUCCESS;

    const CaloHitList &caloHitList(m_pCurrentReclusterMetadata->GetCaloHitList());

    std::string caloHitListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, InputObjectManager<CaloHit>::CreateTemporaryListAndSetCurrent(pAlgorithm, caloHitList,
        caloHitListName));
    CaloHitList *pCaloHitList = m_nameToListMap[caloHitListName];

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->CreateCaloHitMetadata(pCaloHitList, caloHitListName,
        newReclusterListName, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::EndReclustering(const Algorithm *const pAlgorithm, const std::string &selectedReclusterListName)
{
    if (0 == m_nReclusteringProcesses)
        return STATUS_CODE_SUCCESS;

    CaloHitMetadata *pSelectedCaloHitMetaData = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->ExtractCaloHitMetadata(selectedReclusterListName,
        pSelectedCaloHitMetaData));

    m_reclusterMetadataList.pop_back();
    delete m_pCurrentReclusterMetadata;

    if (--m_nReclusteringProcesses > 0)
    {
        m_pCurrentReclusterMetadata = m_reclusterMetadataList.back();
        CaloHitMetadata *pCurrentCaloHitMetaData = m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata();
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCurrentCaloHitMetaData->Update(*pSelectedCaloHitMetaData));
    }
    else
    {
        m_pCurrentReclusterMetadata = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(*pSelectedCaloHitMetaData));
    }

    pSelectedCaloHitMetaData->Clear();
    delete pSelectedCaloHitMetaData;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Update(const CaloHitMetadata &caloHitMetadata)
{
    const CaloHitReplacementList &caloHitReplacementList(caloHitMetadata.GetCaloHitReplacementList());

    for (CaloHitReplacementList::const_iterator iter = caloHitReplacementList.begin(), iterEnd = caloHitReplacementList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(*(*iter)));
    }

    const CaloHitUsageMap &caloHitUsageMap(caloHitMetadata.GetCaloHitUsageMap());

    for (CaloHitUsageMap::const_iterator iter = caloHitUsageMap.begin(), iterEnd = caloHitUsageMap.end(); iter != iterEnd; ++iter)
    {
        iter->first->m_isAvailable = iter->second;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Update(const CaloHitReplacement &caloHitReplacement)
{
    for (NameToListMap::const_iterator listIter = m_nameToListMap.begin(), listIterEnd = m_nameToListMap.end();
        listIter != listIterEnd; ++listIter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(listIter->second, caloHitReplacement));
    }

    for (CaloHitList::const_iterator hitIter = caloHitReplacement.m_oldCaloHits.begin(), hitIterEnd = caloHitReplacement.m_oldCaloHits.end();
        hitIter != hitIterEnd; ++hitIter)
    {
        delete *hitIter;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Update(CaloHitList *pCaloHitList, const CaloHitReplacement &caloHitReplacement)
{
    if (caloHitReplacement.m_newCaloHits.empty() || caloHitReplacement.m_oldCaloHits.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    bool replacementFound(false), allReplacementsFound(true);

    for (CaloHitList::const_iterator hitIter = caloHitReplacement.m_oldCaloHits.begin(), hitIterEnd = caloHitReplacement.m_oldCaloHits.end();
        hitIter != hitIterEnd; ++hitIter)
    {
        CaloHitList::iterator listIter = pCaloHitList->find(*hitIter);

        if (pCaloHitList->end() != listIter)
        {
            pCaloHitList->erase(listIter);
            replacementFound = true;
            continue;
        }

        allReplacementsFound = false;
    }

    if (!replacementFound)
        return STATUS_CODE_SUCCESS;

    if (!allReplacementsFound)
        std::cout << "CaloHitManager::Update - imperfect calo hit replacements made to list " << std::endl;

    for (CaloHitList::const_iterator hitIter = caloHitReplacement.m_newCaloHits.begin(), hitIterEnd = caloHitReplacement.m_newCaloHits.end();
        hitIter != hitIterEnd; ++hitIter)
    {
        if (!pCaloHitList->insert(*hitIter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode CaloHitManager::CreateCaloHit<PandoraApi::RectangularCaloHitParameters>(const PandoraApi::RectangularCaloHitParameters &parameters);
template StatusCode CaloHitManager::CreateCaloHit<PandoraApi::PointingCaloHitParameters>(const PandoraApi::PointingCaloHitParameters &parameters);

} // namespace pandora
