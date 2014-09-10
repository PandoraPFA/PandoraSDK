/**
 *  @file   PandoraSDK/src/LCHelpers/SortingHelper.cc
 * 
 *  @brief  Implementation of the sorting helper class.
 * 
 *  $Log: $
 */

#include "Objects/Cluster.h"
#include "Objects/MCParticle.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"

#include "LCHelpers/SortingHelper.h"

#include <cmath>

using namespace pandora;

namespace lc_content
{

bool SortingHelper::SortClustersByHadronicEnergy(const Cluster *const pLhs, const Cluster *const pRhs)
{
    return (pLhs->GetHadronicEnergy() > pRhs->GetHadronicEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SortingHelper::SortClustersByElectromagneticEnergy(const Cluster *const pLhs, const Cluster *const pRhs)
{
    return (pLhs->GetElectromagneticEnergy() > pRhs->GetElectromagneticEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SortingHelper::SortClustersByInnerLayer(const Cluster *const pLhs, const Cluster *const pRhs)
{
    const unsigned int innerLayerLhs(pLhs->GetInnerPseudoLayer()), innerLayerRhs(pRhs->GetInnerPseudoLayer());

    if (innerLayerLhs != innerLayerRhs)
        return (innerLayerLhs < innerLayerRhs);

    const unsigned int nCaloHitsLhs(pLhs->GetNCaloHits()), nCaloHitsRhs(pRhs->GetNCaloHits());

    if (nCaloHitsLhs != nCaloHitsRhs)
        return (nCaloHitsLhs > nCaloHitsRhs);

    return (pLhs->GetHadronicEnergy() > pRhs->GetHadronicEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SortingHelper::SortMCParticlesByEnergy(const MCParticle *const pLhs, const MCParticle *const pRhs)
{
    return (pLhs->GetEnergy() > pRhs->GetEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SortingHelper::SortPfosByEnergy(const ParticleFlowObject *const pLhs, const ParticleFlowObject *const pRhs)
{
    return (pLhs->GetEnergy() > pRhs->GetEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SortingHelper::SortTracksByMomentum(const Track *const pLhs, const Track *const pRhs)
{
    return (pLhs->GetMomentumAtDca().GetMagnitudeSquared() > pRhs->GetMomentumAtDca().GetMagnitudeSquared());
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SortingHelper::SortTracksByEnergy(const Track *const pLhs, const Track *const pRhs)
{
    return (pLhs->GetEnergyAtDca() > pRhs->GetEnergyAtDca());
}

} // namespace lc_content
