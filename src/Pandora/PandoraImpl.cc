/**
 *  @file   PandoraSDK/src/Pandora/PandoraImpl.cc
 * 
 *  @brief  Implementation of the pandora impl class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"
#include "Managers/VertexManager.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraImpl.h"
#include "Pandora/PandoraSettings.h"

namespace pandora
{

StatusCode PandoraImpl::PrepareMCParticles() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->AddMCParticleRelationships());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->IdentifyPfoTargets());

    UidToMCParticleWeightMap caloHitToPfoTargetsMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->CreateCaloHitToPfoTargetsMap(caloHitToPfoTargetsMap));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->MatchCaloHitsToMCPfoTargets(caloHitToPfoTargetsMap));

    UidToMCParticleWeightMap trackToPfoTargetsMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->CreateTrackToPfoTargetsMap(trackToPfoTargetsMap));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->MatchTracksToMCPfoTargets(trackToPfoTargetsMap));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->SelectPfoTargets());

    return STATUS_CODE_SUCCESS;
}
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::PrepareTracks() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->CreateInputList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->AssociateTracks());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::PrepareCaloHits() const
{
    return m_pPandora->m_pCaloHitManager->CreateInputList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const StringVector &PandoraImpl::GetPandoraAlgorithms() const
{
    return m_pPandora->m_pAlgorithmManager->GetPandoraAlgorithms();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::RunAlgorithm(const std::string &algorithmName) const
{
    return m_pPandora->m_pPandoraContentApiImpl->RunAlgorithm(algorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializeSettings(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pPandoraSettings->Initialize(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializeAlgorithms(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pAlgorithmManager->InitializeAlgorithms(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializePlugins(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pPluginManager->InitializePlugins(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::ResetEvent() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pVertexManager->ResetForNextEvent());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraImpl::PandoraImpl(Pandora *pPandora) :
    m_pPandora(pPandora)
{
}

} // namespace pandora
