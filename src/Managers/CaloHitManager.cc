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
#include "Objects/CaloHit.h"

#include "Pandora/ObjectFactory.h"
#include "Pandora/Pandora.h"
#include "Pandora/PandoraInternal.h"

#include "Plugins/PseudoLayerPlugin.h"

#include <algorithm>
#include <cmath>

namespace pandora
{

CaloHitManager::CaloHitManager(const Pandora *const pPandora) :
    InputObjectManager<CaloHit>(pPandora),
    m_nReclusteringProcesses(0),
    m_pCurrentReclusterMetadata(nullptr)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::~CaloHitManager()
{
    (void) this->EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Create(const object_creation::CaloHit::Parameters &parameters, const CaloHit *&pCaloHit,
    const ObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object> &factory)
{
    pCaloHit = nullptr;

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pCaloHit));

        NameToListMap::iterator inputIter = m_nameToListMap.find(m_inputListName);

        if (!pCaloHit || (m_nameToListMap.end() == inputIter))
            throw StatusCodeException(STATUS_CODE_FAILURE);

        // ATTN No longer require presence of pseudo layer plugin, accepting use of a single dummy value for all hits
        const unsigned int pseudoLayer(m_pPandora->GetPlugins()->HasPseudoLayerPlugin() ?
            m_pPandora->GetPlugins()->GetPseudoLayerPlugin()->GetPseudoLayer(pCaloHit->GetPositionVector()) : 0);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pCaloHit)->SetPseudoLayer(pseudoLayer));

        inputIter->second->push_back(pCaloHit);
        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create calo hit: " << statusCodeException.ToString() << std::endl;
        delete pCaloHit;
        pCaloHit = nullptr;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::AlterMetadata(const CaloHit *const pCaloHit, const object_creation::CaloHit::Metadata &metadata) const
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

        for (const CaloHit *const pCaloHit : *pCaloHitList)
            isAvailable &= this->IsAvailable(pCaloHit);

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
        for (const CaloHit *const pCaloHit : *pCaloHitList)
            this->Modifiable(pCaloHit)->SetAvailability(isAvailable);

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

    for (const Cluster *const pCluster : clusterList)
    {
        pCluster->GetOrderedCaloHitList().FillCaloHitList(caloHitList);
        caloHitList.insert(caloHitList.end(), pCluster->GetIsolatedCaloHitList().begin(), pCluster->GetIsolatedCaloHitList().end());
    }

    return InputObjectManager<CaloHit>::CreateTemporaryListAndSetCurrent(pAlgorithm, caloHitList, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::EraseAllContent()
{
    for (const ReclusterMetadata *const pMetaData : m_reclusterMetadataList)
        delete pMetaData;

    m_nReclusteringProcesses = 0;
    m_pCurrentReclusterMetadata = nullptr;
    m_reclusterMetadataList.clear();

    return InputObjectManager<CaloHit>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::MatchCaloHitsToMCPfoTargets(const UidToMCParticleWeightMap &caloHitToPfoTargetsMap)
{
    if (caloHitToPfoTargetsMap.empty())
        return STATUS_CODE_SUCCESS;

    NameToListMap::const_iterator inputIter = m_nameToListMap.find(m_inputListName);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (const CaloHit *const pCaloHit : *inputIter->second)
    {
        UidToMCParticleWeightMap::const_iterator pfoTargetIter = caloHitToPfoTargetsMap.find(pCaloHit->GetParentAddress());

        if (caloHitToPfoTargetsMap.end() == pfoTargetIter)
            continue;

        this->Modifiable(pCaloHit)->SetMCParticleWeightMap(pfoTargetIter->second);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::RemoveAllMCParticleRelationships()
{
    NameToListMap::const_iterator inputIter = m_nameToListMap.find(m_inputListName);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (const CaloHit *const pCaloHit : *inputIter->second)
        this->Modifiable(pCaloHit)->RemoveMCParticles();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::FragmentCaloHit(const CaloHit *const pOriginalCaloHit, const float fraction1, const CaloHit *&pDaughterCaloHit1,
    const CaloHit *&pDaughterCaloHit2, const ObjectFactory<object_creation::CaloHitFragment::Parameters, object_creation::CaloHitFragment::Object> &factory)
{
    pDaughterCaloHit1 = nullptr; pDaughterCaloHit2 = nullptr;

    if (!this->CanFragmentCaloHit(pOriginalCaloHit, fraction1))
        return STATUS_CODE_NOT_ALLOWED;

    object_creation::CaloHitFragment::Parameters parameters1;
    parameters1.m_pOriginalCaloHit = pOriginalCaloHit;
    parameters1.m_weight = fraction1;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters1, pDaughterCaloHit1));

    object_creation::CaloHitFragment::Parameters parameters2;
    parameters2.m_pOriginalCaloHit = pOriginalCaloHit;
    parameters2.m_weight = 1.f - fraction1;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters2, pDaughterCaloHit2));

    if (!pDaughterCaloHit1 || !pDaughterCaloHit2)
        return STATUS_CODE_FAILURE;

    CaloHitReplacement caloHitReplacement;
    caloHitReplacement.m_oldCaloHits.push_back(pOriginalCaloHit);
    caloHitReplacement.m_newCaloHits.push_back(pDaughterCaloHit1); caloHitReplacement.m_newCaloHits.push_back(pDaughterCaloHit2);

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
    const CaloHit *&pMergedCaloHit, const ObjectFactory<object_creation::CaloHitFragment::Parameters, object_creation::CaloHitFragment::Object> &factory)
{
    pMergedCaloHit = nullptr;

    if (!this->CanMergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2) || (pFragmentCaloHit1->GetCellGeometry() != pFragmentCaloHit2->GetCellGeometry()))
        return STATUS_CODE_NOT_ALLOWED;

    const float newWeight((pFragmentCaloHit1->GetWeight() + pFragmentCaloHit2->GetWeight()) / pFragmentCaloHit1->GetWeight());

    object_creation::CaloHitFragment::Parameters parameters;
    parameters.m_pOriginalCaloHit = pFragmentCaloHit1;
    parameters.m_weight = newWeight;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pMergedCaloHit));

    if (!pMergedCaloHit)
        return STATUS_CODE_FAILURE;

    CaloHitReplacement caloHitReplacement;
    caloHitReplacement.m_newCaloHits.push_back(pMergedCaloHit);
    caloHitReplacement.m_oldCaloHits.push_back(pFragmentCaloHit1); caloHitReplacement.m_oldCaloHits.push_back(pFragmentCaloHit2);

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

    if (iter->second->end() == std::find(iter->second->begin(), iter->second->end(), pOriginalCaloHit))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::CanMergeCaloHitFragments(const CaloHit *const pFragmentCaloHit1, const CaloHit *const pFragmentCaloHit2) const
{
    if (pFragmentCaloHit1->GetWeight() < std::numeric_limits<float>::epsilon())
        return false;

    if (pFragmentCaloHit1->GetParentAddress() != pFragmentCaloHit2->GetParentAddress())
        return false;

    if (!this->IsAvailable(pFragmentCaloHit1) || !this->IsAvailable(pFragmentCaloHit2))
        return false;

    NameToListMap::const_iterator iter = m_nameToListMap.find(m_currentListName);

    if (m_nameToListMap.end() == iter)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    if ((iter->second->end() == std::find(iter->second->begin(), iter->second->end(), pFragmentCaloHit1)) ||
        (iter->second->end() == std::find(iter->second->begin(), iter->second->end(), pFragmentCaloHit2)))
    {
        return false;
    }

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

    CaloHitMetadata *pSelectedCaloHitMetaData(nullptr);
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
        m_pCurrentReclusterMetadata = nullptr;
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
        this->Modifiable(pCaloHit)->SetAvailability(caloHitUsageMap.at(pCaloHit));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Update(const CaloHitReplacement &caloHitReplacement)
{
    for (const NameToListMap::value_type &mapEntry : m_nameToListMap)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(mapEntry.second, caloHitReplacement));
    }

    for (const CaloHit *const pCaloHit : caloHitReplacement.m_oldCaloHits)
        delete pCaloHit;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Update(CaloHitList *const pCaloHitList, const CaloHitReplacement &caloHitReplacement)
{
    if (caloHitReplacement.m_newCaloHits.empty() || caloHitReplacement.m_oldCaloHits.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    if (pCaloHitList == &caloHitReplacement.m_oldCaloHits)
        return STATUS_CODE_FAILURE;

    bool replacementFound(false), allReplacementsFound(true);

    for (const CaloHit *const pCaloHit : caloHitReplacement.m_oldCaloHits)
    {
        CaloHitList::iterator listIter = std::find(pCaloHitList->begin(), pCaloHitList->end(), pCaloHit);

        if (pCaloHitList->end() != listIter)
        {
            listIter = pCaloHitList->erase(listIter);
            replacementFound = true;
            continue;
        }

        allReplacementsFound = false;
    }

    if (!replacementFound)
        return STATUS_CODE_SUCCESS;

    if (!allReplacementsFound)
        std::cout << "CaloHitManager::Update - imperfect calo hit replacements made to list " << std::endl;

    for (const CaloHit *const pCaloHit : caloHitReplacement.m_newCaloHits)
    {
        if (pCaloHitList->end() != std::find(pCaloHitList->begin(), pCaloHitList->end(), pCaloHit))
            return STATUS_CODE_ALREADY_PRESENT;

        pCaloHitList->push_back(pCaloHit);
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
