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

#include "Objects/EventContext.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraImpl.h"
#include "Pandora/PandoraSettings.h"

namespace pandora
{

StatusCode PandoraImpl::PrepareMCParticles() const
{
    RETURN_ON_ERROR(m_pPandora->m_pMCManager->CreateInputList());
    RETURN_ON_ERROR(m_pPandora->m_pMCManager->AddMCParticleRelationships());
    RETURN_ON_ERROR(m_pPandora->m_pMCManager->IdentifyPfoTargets());

    UidToMCParticleWeightMap caloHitToPfoTargetsMap;
    RETURN_ON_ERROR(m_pPandora->m_pMCManager->CreateCaloHitToPfoTargetsMap(caloHitToPfoTargetsMap));
    RETURN_ON_ERROR(m_pPandora->m_pCaloHitManager->MatchCaloHitsToMCPfoTargets(caloHitToPfoTargetsMap));

    UidToMCParticleWeightMap trackToPfoTargetsMap;
    RETURN_ON_ERROR(m_pPandora->m_pMCManager->CreateTrackToPfoTargetsMap(trackToPfoTargetsMap));
    RETURN_ON_ERROR(m_pPandora->m_pTrackManager->MatchTracksToMCPfoTargets(trackToPfoTargetsMap));

    RETURN_ON_ERROR(m_pPandora->m_pMCManager->SelectPfoTargets());

    return STATUS_CODE_SUCCESS;
}
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::PrepareTracks() const
{
    RETURN_ON_ERROR(m_pPandora->m_pTrackManager->CreateInputList());
    RETURN_ON_ERROR(m_pPandora->m_pTrackManager->AssociateTracks());

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
    RETURN_ON_ERROR(m_pPandora->m_pCaloHitManager->ResetForNextEvent());
    RETURN_ON_ERROR(m_pPandora->m_pClusterManager->ResetForNextEvent());
    RETURN_ON_ERROR(m_pPandora->m_pMCManager->ResetForNextEvent());
    RETURN_ON_ERROR(m_pPandora->m_pPfoManager->ResetForNextEvent());
    RETURN_ON_ERROR(m_pPandora->m_pTrackManager->ResetForNextEvent());
    RETURN_ON_ERROR(m_pPandora->m_pVertexManager->ResetForNextEvent());
    RETURN_ON_ERROR(m_pPandora->m_pAlgorithmManager->ResetForNextEvent());
    RETURN_ON_ERROR(m_pPandora->m_pPluginManager->ResetForNextEvent());
    RETURN_ON_ERROR(m_pPandora->m_pEventContext->ResetForNextEvent());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraImpl::PandoraImpl(Pandora *const pPandora) :
    m_pPandora(pPandora)
{
}

} // namespace pandora
