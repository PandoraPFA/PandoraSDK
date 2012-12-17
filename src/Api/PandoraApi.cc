/**
 *  @file   PandoraPFANew/Framework/src/Api/PandoraApi.cc
 * 
 *  @brief  Redirection for pandora api class to its implementation.
 * 
 *  $Log: $
 */

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"

template <typename PARAMETERS>
pandora::StatusCode PandoraApi::ObjectCreationHelper<PARAMETERS>::Create(const pandora::Pandora &pandora, const Parameters &parameters)
{
    return pandora.GetPandoraApiImpl()->CreateObject(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::ProcessEvent(const pandora::Pandora &pandora)
{
    return pandora.GetPandoraApiImpl()->ProcessEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::ReadSettings(const pandora::Pandora &pandora, const std::string &xmlFileName)
{
    return pandora.GetPandoraApiImpl()->ReadSettings(xmlFileName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::RegisterAlgorithmFactory(const pandora::Pandora &pandora, const std::string &algorithmType,
    pandora::AlgorithmFactory *const pAlgorithmFactory)
{
    return pandora.GetPandoraApiImpl()->RegisterAlgorithmFactory(algorithmType, pAlgorithmFactory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetMCParentDaughterRelationship(const pandora::Pandora &pandora, const void *pParentAddress,
    const void *pDaughterAddress)
{
    return pandora.GetPandoraApiImpl()->SetMCParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetTrackParentDaughterRelationship(const pandora::Pandora &pandora, const void *pParentAddress,
    const void *pDaughterAddress)
{
    return pandora.GetPandoraApiImpl()->SetTrackParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetTrackSiblingRelationship(const pandora::Pandora &pandora, const void *pFirstSiblingAddress,
    const void *pSecondSiblingAddress)
{
    return pandora.GetPandoraApiImpl()->SetTrackSiblingRelationship(pFirstSiblingAddress, pSecondSiblingAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetCaloHitToMCParticleRelationship(const pandora::Pandora &pandora, const void *pCaloHitParentAddress,
    const void *pMCParticleParentAddress, const float mcParticleWeight)
{
    return pandora.GetPandoraApiImpl()->SetCaloHitToMCParticleRelationship(pCaloHitParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetTrackToMCParticleRelationship(const pandora::Pandora &pandora, const void *pTrackParentAddress,
    const void *pMCParticleParentAddress, const float mcParticleWeight)
{
    return pandora.GetPandoraApiImpl()->SetTrackToMCParticleRelationship(pTrackParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::GetCurrentPfoList(const pandora::Pandora &pandora, const pandora::PfoList *&pfoList)
{
    std::string pfoListName;
    return pandora.GetPandoraApiImpl()->GetCurrentPfoList(pfoList, pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::GetPfoList(const pandora::Pandora &pandora, const std::string &pfoListName, const pandora::PfoList *&pPfoList)
{
    return pandora.GetPandoraApiImpl()->GetPfoList(pfoListName, pPfoList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetBFieldCalculator(const pandora::Pandora &pandora, pandora::BFieldCalculator *pBFieldCalculator)
{
    return pandora.GetPandoraApiImpl()->SetBFieldCalculator(pBFieldCalculator);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetPseudoLayerCalculator(const pandora::Pandora &pandora, pandora::PseudoLayerCalculator *pPseudoLayerCalculator)
{
    return pandora.GetPandoraApiImpl()->SetPseudoLayerCalculator(pPseudoLayerCalculator);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetShowerProfileCalculator(const pandora::Pandora &pandora, pandora::ShowerProfileCalculator *pShowerProfileCalculator)
{
    return pandora.GetPandoraApiImpl()->SetShowerProfileCalculator(pShowerProfileCalculator);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetHitTypeGranularity(const pandora::Pandora &pandora, const pandora::HitType hitType,
    const pandora::Granularity granularity)
{
    return pandora.GetPandoraApiImpl()->SetHitTypeGranularity(hitType, granularity);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::RegisterEnergyCorrectionFunction(const pandora::Pandora &pandora, const std::string &functionName,
    const pandora::EnergyCorrectionType energyCorrectionType, pandora::EnergyCorrectionFunction *pEnergyCorrectionFunction)
{
    return pandora.GetPandoraApiImpl()->RegisterEnergyCorrectionFunction(functionName, energyCorrectionType, pEnergyCorrectionFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::RegisterParticleIdFunction(const pandora::Pandora &pandora, const std::string &functionName,
    pandora::ParticleIdFunction *pParticleIdFunction)
{
    return pandora.GetPandoraApiImpl()->RegisterParticleIdFunction(functionName, pParticleIdFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::RegisterResetFunction(const pandora::Pandora &pandora, pandora::ResetFunction *pResetFunction)
{
    return pandora.GetPandoraApiImpl()->RegisterResetFunction(pResetFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::RegisterSettingsFunction(const pandora::Pandora &pandora, const std::string &xmlTagName,
    pandora::SettingsFunction *pSettingsFunction)
{
    return pandora.GetPandoraApiImpl()->RegisterSettingsFunction(xmlTagName, pSettingsFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::GetReclusterMonitoringResults(const pandora::Pandora &pandora, const void *pTrackParentAddress,
    float &netEnergyChange, float &sumModulusEnergyChanges, float &sumSquaredEnergyChanges)
{
    return pandora.GetPandoraApiImpl()->GetReclusterMonitoringResults(pTrackParentAddress, netEnergyChange, sumModulusEnergyChanges, sumSquaredEnergyChanges);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::Reset(const pandora::Pandora &pandora)
{
    return pandora.GetPandoraApiImpl()->ResetEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class PandoraApi::ObjectCreationHelper<PandoraApi::MCParticleParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::TrackParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::RectangularCaloHitParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::PointingCaloHitParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::GeometryParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::BoxGapParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::ConcentricGapParameters>;
