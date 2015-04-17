/**
 *  @file   PandoraSDK/src/Managers/CaloHitManager.cc
 * 
 *  @brief  Implementation of the calo hit manager class.
 * 
 *  $Log: $
 */

#include "Managers/CaloHitManager.h"
#include "Managers/PluginManager.h"

#include "Objects/Cluster.h"

#include "Pandora/Pandora.h"
#include "Pandora/ObjectFactory.h"

#include "Plugins/PseudoLayerPlugin.h"

#include <cmath>

namespace pandora
{

CaloHitManager::CaloHitManager(const Pandora *const pPandora) :
    InputObjectManager<CaloHit>(pPandora),
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

StatusCode CaloHitManager::Create(const PandoraApi::CaloHit::Parameters &parameters, const CaloHit *&pCaloHit,
    const ObjectFactory<PandoraApi::CaloHit::Parameters, CaloHit> &factory)
{
    pCaloHit = NULL;

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pCaloHit));

        if (NULL == pCaloHit)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        const unsigned int pseudoLayer(m_pPandora->GetPlugins()->GetPseudoLayerPlugin()->GetPseudoLayer(pCaloHit->GetPositionVector()));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pCaloHit)->SetPseudoLayer(pseudoLayer));

        NameToListMap::iterator inputIter = m_nameToListMap.find(INPUT_LIST_NAME);

        if ((m_nameToListMap.end() == inputIter) || !inputIter->second->insert(pCaloHit).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create calo hit: " << statusCodeException.ToString() << std::endl;
        delete pCaloHit;
        pCaloHit = NULL;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::AlterMetadata(const CaloHit *const pCaloHit, const PandoraContentApi::CaloHit::Metadata &metadata) const
{
    return this->Modifiable(pCaloHit)->AlterMetadata(metadata);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
bool CaloHitManager::IsAvailable(const CaloHit *const pCaloHit) const
{
    if (0 == m_nReclusteringProcesses)
        return pCaloHit->IsAvailable();

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->IsAvailable(pCaloHit);
}

template <>
bool CaloHitManager::IsAvailable(const CaloHitList *const pCaloHitList) const
{
    if (0 == m_nReclusteringProcesses)
    {
        bool isAvailable(true);

        for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
            isAvailable &= this->IsAvailable(*iter);

        return isAvailable;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->IsAvailable(pCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode CaloHitManager::SetAvailability(const CaloHit *const pCaloHit, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
    {
        this->Modifiable(pCaloHit)->SetAvailability(isAvailable);
        return STATUS_CODE_SUCCESS;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->SetAvailability(pCaloHit, isAvailable);
}

template <>
StatusCode CaloHitManager::SetAvailability(const CaloHitList *const pCaloHitList, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
    {
        for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
            this->Modifiable(*iter)->SetAvailability(isAvailable);

        return STATUS_CODE_SUCCESS;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->SetAvailability(pCaloHitList, isAvailable);
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
        const Cluster *const pCluster = *iter;
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

StatusCode CaloHitManager::MatchCaloHitsToMCPfoTargets(const UidToMCParticleWeightMap &caloHitToPfoTargetsMap)
{
    if (caloHitToPfoTargetsMap.empty())
        return STATUS_CODE_SUCCESS;

    NameToListMap::const_iterator inputIter = m_nameToListMap.find(INPUT_LIST_NAME);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (CaloHitList::iterator iter = inputIter->second->begin(), iterEnd = inputIter->second->end(); iter != iterEnd; ++iter)
    {
        UidToMCParticleWeightMap::const_iterator pfoTargetIter = caloHitToPfoTargetsMap.find((*iter)->GetParentCaloHitAddress());

        if (caloHitToPfoTargetsMap.end() == pfoTargetIter)
            continue;

        this->Modifiable(*iter)->SetMCParticleWeightMap(pfoTargetIter->second);
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
        this->Modifiable(*iter)->RemoveMCParticles();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::FragmentCaloHit(const CaloHit *const pOriginalCaloHit, const float fraction1, const CaloHit *&pDaughterCaloHit1,
    const CaloHit *&pDaughterCaloHit2, const ObjectFactory<PandoraContentApi::CaloHitFragment::Parameters, CaloHit> &factory)
{
    pDaughterCaloHit1 = NULL; pDaughterCaloHit2 = NULL;

    if (!this->CanFragmentCaloHit(pOriginalCaloHit, fraction1))
        return STATUS_CODE_NOT_ALLOWED;

    PandoraContentApi::CaloHitFragment::Parameters parameters1;
    parameters1.m_pOriginalCaloHit = pOriginalCaloHit;
    parameters1.m_weight = fraction1;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters1, pDaughterCaloHit1));

    PandoraContentApi::CaloHitFragment::Parameters parameters2;
    parameters2.m_pOriginalCaloHit = pOriginalCaloHit;
    parameters2.m_weight = 1.f - fraction1;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters2, pDaughterCaloHit2));

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

StatusCode CaloHitManager::MergeCaloHitFragments(const CaloHit *const pFragmentCaloHit1, const CaloHit *const pFragmentCaloHit2,
    const CaloHit *&pMergedCaloHit, const ObjectFactory<PandoraContentApi::CaloHitFragment::Parameters, CaloHit> &factory)
{
    pMergedCaloHit = NULL;

    if (!this->CanMergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2) || (pFragmentCaloHit1->GetCellGeometry() != pFragmentCaloHit2->GetCellGeometry()))
        return STATUS_CODE_NOT_ALLOWED;

    const float newWeight((pFragmentCaloHit1->GetWeight() + pFragmentCaloHit2->GetWeight()) / pFragmentCaloHit1->GetWeight());

    PandoraContentApi::CaloHitFragment::Parameters parameters;
    parameters.m_pOriginalCaloHit = pFragmentCaloHit1;
    parameters.m_weight = newWeight;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pMergedCaloHit));

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

bool CaloHitManager::CanFragmentCaloHit(const CaloHit *const pOriginalCaloHit, const float fraction1) const
{
    if ((fraction1 < std::numeric_limits<float>::epsilon()) || (fraction1 > 1.f))
        return false;

    if (!this->IsAvailable(pOriginalCaloHit))
        return false;

    NameToListMap::const_iterator iter = m_nameToListMap.find(m_currentListName);

    if (m_nameToListMap.end() == iter)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    if (iter->second->end() == iter->second->find(pOriginalCaloHit))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::CanMergeCaloHitFragments(const CaloHit *const pFragmentCaloHit1, const CaloHit *const pFragmentCaloHit2) const
{
    if (pFragmentCaloHit1->GetWeight() < std::numeric_limits<float>::epsilon())
        return false;

    if (pFragmentCaloHit1->GetParentCaloHitAddress() != pFragmentCaloHit2->GetParentCaloHitAddress())
        return false;

    if (!this->IsAvailable(pFragmentCaloHit1) || !this->IsAvailable(pFragmentCaloHit2))
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
    CaloHitList *const pCaloHitList = m_nameToListMap[caloHitListName];

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
    CaloHitList *const pCaloHitList = m_nameToListMap[caloHitListName];

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->CreateCaloHitMetadata(pCaloHitList, caloHitListName,
        newReclusterListName, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::EndReclustering(const Algorithm *const /*const pAlgorithm*/, const std::string &selectedReclusterListName)
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
        CaloHitMetadata *const pCurrentCaloHitMetaData = m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata();
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
        this->Modifiable(iter->first)->SetAvailability(iter->second);
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

StatusCode CaloHitManager::Update(CaloHitList *const pCaloHitList, const CaloHitReplacement &caloHitReplacement)
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

} // namespace pandora
