/**
 *  @file   PandoraPFANew/Framework/src/Api/PandoraApiImpl.cc
 * 
 *  @brief  Implementation of the pandora api class.
 * 
 *  $Log: $
 */

#include "Pandora/Algorithm.h"

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"

#include "Helpers/GeometryHelper.h"
#include "Helpers/ParticleIdHelper.h"
#include "Helpers/ReclusterHelper.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"

#include "Pandora/PandoraSettings.h"

namespace pandora
{

template <typename PARAMETERS>
StatusCode PandoraApiImpl::CreateObject(const PARAMETERS &parameters) const
{
    return STATUS_CODE_FAILURE;
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::MCParticleParameters>(const PandoraApi::MCParticleParameters &parameters) const
{
    return m_pPandora->m_pMCManager->CreateMCParticle(parameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::TrackParameters>(const PandoraApi::TrackParameters &parameters) const
{
    return m_pPandora->m_pTrackManager->CreateTrack(parameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::RectangularCaloHitParameters>(const PandoraApi::RectangularCaloHitParameters &parameters) const
{
    return m_pPandora->m_pCaloHitManager->CreateCaloHit(parameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::PointingCaloHitParameters>(const PandoraApi::PointingCaloHitParameters &parameters) const
{
    return m_pPandora->m_pCaloHitManager->CreateCaloHit(parameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::GeometryParameters>(const PandoraApi::GeometryParameters &parameters) const
{
    return GeometryHelper::Initialize(parameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::BoxGapParameters>(const PandoraApi::BoxGapParameters &parameters) const
{
    return GeometryHelper::CreateBoxGap(parameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::ConcentricGapParameters>(const PandoraApi::ConcentricGapParameters &parameters) const
{
    return GeometryHelper::CreateConcentricGap(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::ProcessEvent() const
{
    return m_pPandora->ProcessEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::ReadSettings(const std::string &xmlFileName) const
{
    return m_pPandora->ReadSettings(xmlFileName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::RegisterAlgorithmFactory(const std::string &algorithmType, AlgorithmFactory *const pAlgorithmFactory) const
{
    return m_pPandora->m_pAlgorithmManager->RegisterAlgorithmFactory(algorithmType, pAlgorithmFactory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetMCParentDaughterRelationship(const void *pParentAddress, const void *pDaughterAddress) const
{
    return m_pPandora->m_pMCManager->SetMCParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetTrackParentDaughterRelationship(const void *pParentAddress, const void *pDaughterAddress) const
{
    return m_pPandora->m_pTrackManager->SetTrackParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetTrackSiblingRelationship(const void *pFirstSiblingAddress, const void *pSecondSiblingAddress) const
{
    return m_pPandora->m_pTrackManager->SetTrackSiblingRelationship(pFirstSiblingAddress, pSecondSiblingAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetCaloHitToMCParticleRelationship(const void *pCaloHitParentAddress, const void *pMCParticleParentAddress,
    const float mcParticleWeight) const
{
    return m_pPandora->m_pMCManager->SetCaloHitToMCParticleRelationship(pCaloHitParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetTrackToMCParticleRelationship(const void *pTrackParentAddress, const void *pMCParticleParentAddress,
    const float mcParticleWeight) const
{
    return m_pPandora->m_pMCManager->SetTrackToMCParticleRelationship(pTrackParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::GetCurrentPfoList(const PfoList *&pPfoList, std::string &pfoListName) const
{
    return m_pPandora->m_pPfoManager->GetCurrentList(pPfoList, pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::GetPfoList(const std::string &pfoListName, const PfoList *&pPfoList) const
{
    return m_pPandora->m_pPfoManager->GetList(pfoListName, pPfoList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetBFieldCalculator(BFieldCalculator *pBFieldCalculator) const
{
    return GeometryHelper::SetBFieldCalculator(pBFieldCalculator);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetPseudoLayerCalculator(PseudoLayerCalculator *pPseudoLayerCalculator) const
{
    return GeometryHelper::SetPseudoLayerCalculator(pPseudoLayerCalculator);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetShowerProfileCalculator(ShowerProfileCalculator *pShowerProfileCalculator) const
{
    return ParticleIdHelper::SetShowerProfileCalculator(pShowerProfileCalculator);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetHitTypeGranularity(const HitType hitType, const Granularity granularity) const
{
    return GeometryHelper::SetHitTypeGranularity(hitType, granularity);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::RegisterEnergyCorrectionFunction(const std::string &functionName, const EnergyCorrectionType energyCorrectionType,
    EnergyCorrectionFunction *pEnergyCorrectionFunction) const
{
    return m_pPandora->m_pPluginManager->RegisterEnergyCorrectionFunction(functionName, energyCorrectionType, pEnergyCorrectionFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::RegisterParticleIdFunction(const std::string &functionName, ParticleIdFunction *pParticleIdFunction) const
{
    return m_pPandora->m_pPluginManager->RegisterParticleIdFunction(functionName, pParticleIdFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::RegisterSettingsFunction(const std::string &xmlTagName, SettingsFunction *pSettingsFunction) const
{
    return PandoraSettings::RegisterSettingsFunction(xmlTagName, pSettingsFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::RegisterResetFunction(ResetFunction *pResetFunction) const
{
    return m_pPandora->RegisterResetFunction(pResetFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::GetReclusterMonitoringResults(const void *pTrackParentAddress, float &netEnergyChange, float &sumModulusEnergyChanges,
    float &sumSquaredEnergyChanges) const
{
    return ReclusterHelper::GetReclusterMonitoringResults(pTrackParentAddress, netEnergyChange, sumModulusEnergyChanges, sumSquaredEnergyChanges);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::ResetEvent() const
{
    return m_pPandora->ResetEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraApiImpl::PandoraApiImpl(Pandora *pPandora) :
    m_pPandora(pPandora)
{
}

} // namespace pandora
