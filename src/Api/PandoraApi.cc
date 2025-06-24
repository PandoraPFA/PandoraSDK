/**
 *  @file   PandoraSDK/src/Api/PandoraApi.cc
 * 
 *  @brief  Redirection for pandora api class to its implementation.
 * 
 *  $Log: $
 */

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"

#include "Pandora/ExternallyConfiguredAlgorithm.h"

pandora::StatusCode PandoraApi::ProcessEvent(const pandora::Pandora &pandora)
{
    return pandora.GetPandoraApiImpl()->ProcessEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetEventInformation(const pandora::Pandora &pandora, const unsigned int run, 
    const unsigned int subrun, const unsigned int event)
{
    return pandora.GetPandoraApiImpl()->SetEventInformation(run, subrun, event);
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

pandora::StatusCode PandoraApi::RegisterAlgorithmToolFactory(const pandora::Pandora &pandora, const std::string &algorithmToolType,
    pandora::AlgorithmToolFactory *const pAlgorithmToolFactory)
{
    return pandora.GetPandoraApiImpl()->RegisterAlgorithmToolFactory(algorithmToolType, pAlgorithmToolFactory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetMCParentDaughterRelationship(const pandora::Pandora &pandora, const void *const pParentAddress,
    const void *const pDaughterAddress)
{
    return pandora.GetPandoraApiImpl()->SetMCParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetTrackParentDaughterRelationship(const pandora::Pandora &pandora, const void *const pParentAddress,
    const void *const pDaughterAddress)
{
    return pandora.GetPandoraApiImpl()->SetTrackParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetTrackSiblingRelationship(const pandora::Pandora &pandora, const void *const pFirstSiblingAddress,
    const void *const pSecondSiblingAddress)
{
    return pandora.GetPandoraApiImpl()->SetTrackSiblingRelationship(pFirstSiblingAddress, pSecondSiblingAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetCaloHitToMCParticleRelationship(const pandora::Pandora &pandora, const void *const pCaloHitParentAddress,
    const void *const pMCParticleParentAddress, const float mcParticleWeight)
{
    return pandora.GetPandoraApiImpl()->SetCaloHitToMCParticleRelationship(pCaloHitParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetTrackToMCParticleRelationship(const pandora::Pandora &pandora, const void *const pTrackParentAddress,
    const void *const pMCParticleParentAddress, const float mcParticleWeight)
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

pandora::StatusCode PandoraApi::SetExternalParameters(const pandora::Pandora &pandora, const std::string &algorithmType,
    pandora::ExternalParameters *const pExternalParameters)
{
    return pandora::ExternallyConfiguredAlgorithm::SetExternalParameters(pandora, algorithmType, pExternalParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetHitTypeGranularity(const pandora::Pandora &pandora, const pandora::HitType hitType,
    const pandora::Granularity granularity)
{
    return pandora.GetPandoraApiImpl()->SetHitTypeGranularity(hitType, granularity);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetBFieldPlugin(const pandora::Pandora &pandora, pandora::BFieldPlugin *const pBFieldPlugin)
{
    return pandora.GetPandoraApiImpl()->SetBFieldPlugin(pBFieldPlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetLArTransformationPlugin(const pandora::Pandora &pandora, pandora::LArTransformationPlugin *const pLArTransformationPlugin)
{
    return pandora.GetPandoraApiImpl()->SetLArTransformationPlugin(pLArTransformationPlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetPseudoLayerPlugin(const pandora::Pandora &pandora, pandora::PseudoLayerPlugin *const pPseudoLayerPlugin)
{
    return pandora.GetPandoraApiImpl()->SetPseudoLayerPlugin(pPseudoLayerPlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::SetShowerProfilePlugin(const pandora::Pandora &pandora, pandora::ShowerProfilePlugin *const pShowerProfilePlugin)
{
    return pandora.GetPandoraApiImpl()->SetShowerProfilePlugin(pShowerProfilePlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::RegisterEnergyCorrectionPlugin(const pandora::Pandora &pandora, const std::string &name,
    const pandora::EnergyCorrectionType energyCorrectionType, pandora::EnergyCorrectionPlugin *const pEnergyCorrectionPlugin)
{
    return pandora.GetPandoraApiImpl()->RegisterEnergyCorrectionPlugin(name, energyCorrectionType, pEnergyCorrectionPlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::RegisterParticleIdPlugin(const pandora::Pandora &pandora, const std::string &name,
    pandora::ParticleIdPlugin *const pParticleIdPlugin)
{
    return pandora.GetPandoraApiImpl()->RegisterParticleIdPlugin(name, pParticleIdPlugin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraApi::Reset(const pandora::Pandora &pandora)
{
    return pandora.GetPandoraApiImpl()->ResetEvent();
}
