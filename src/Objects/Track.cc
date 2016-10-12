/**
 *  @file   PandoraSDK/src/Objects/Track.cc
 * 
 *  @brief  Implementation of the track class.
 * 
 *  $Log: $
 */

#include "Objects/Track.h"

#include <algorithm>
#include <cmath>

namespace pandora
{

const Cluster *Track::GetAssociatedCluster() const
{
    if (!m_pAssociatedCluster)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pAssociatedCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool Track::operator< (const Track &rhs) const
{
    const CartesianVector deltaPosition(rhs.GetTrackStateAtCalorimeter().GetPosition() - this->GetTrackStateAtCalorimeter().GetPosition());

    if (std::fabs(deltaPosition.GetZ()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetZ() > std::numeric_limits<float>::epsilon());

    if (std::fabs(deltaPosition.GetX()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetX() > std::numeric_limits<float>::epsilon());

    if (std::fabs(deltaPosition.GetY()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetY() > std::numeric_limits<float>::epsilon());

    return (this->GetEnergyAtDca() > rhs.GetEnergyAtDca());
}

//------------------------------------------------------------------------------------------------------------------------------------------

Track::Track(const object_creation::Track::Parameters &parameters) :
    m_d0(parameters.m_d0.Get()),
    m_z0(parameters.m_z0.Get()),
    m_particleId(parameters.m_particleId.Get()),
    m_charge(parameters.m_charge.Get()),
    m_mass(parameters.m_mass.Get()),
    m_momentumAtDca(parameters.m_momentumAtDca.Get()),
    m_energyAtDca(std::sqrt(m_mass * m_mass + m_momentumAtDca.GetMagnitudeSquared())),
    m_trackStateAtStart(parameters.m_trackStateAtStart.Get()),
    m_trackStateAtEnd(parameters.m_trackStateAtEnd.Get()),
    m_trackStateAtCalorimeter(parameters.m_trackStateAtCalorimeter.Get()),
    m_timeAtCalorimeter(parameters.m_timeAtCalorimeter.Get()),
    m_reachesCalorimeter(parameters.m_reachesCalorimeter.Get()),
    m_isProjectedToEndCap(parameters.m_isProjectedToEndCap.Get()),
    m_canFormPfo(parameters.m_canFormPfo.Get()),
    m_canFormClusterlessPfo(parameters.m_canFormClusterlessPfo.Get()),
    m_pAssociatedCluster(nullptr),
    m_pParentAddress(parameters.m_pParentAddress.Get()),
    m_isAvailable(true)
{
    // Consistency checks
    if (m_energyAtDca < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    if (0 == m_charge)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Track::~Track()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Track::SetMCParticleWeightMap(const MCParticleWeightMap &mcParticleWeightMap)
{
    m_mcParticleWeightMap = mcParticleWeightMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Track::RemoveMCParticles()
{
    m_mcParticleWeightMap.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::SetAssociatedCluster(const Cluster *const pCluster)
{
    if (!pCluster)
        return STATUS_CODE_INVALID_PARAMETER;

    if (nullptr != m_pAssociatedCluster)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pAssociatedCluster = pCluster;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::RemoveAssociatedCluster(const Cluster *const pCluster)
{
    if (pCluster != m_pAssociatedCluster)
        return STATUS_CODE_NOT_FOUND;

    m_pAssociatedCluster = nullptr;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddParent(const Track *const pTrack)
{
    if (!pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_parentTrackList.end() != std::find(m_parentTrackList.begin(), m_parentTrackList.end(), pTrack))
        return STATUS_CODE_ALREADY_PRESENT;

    m_parentTrackList.push_back(pTrack);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddDaughter(const Track *const pTrack)
{
    if (!pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_daughterTrackList.end() != std::find(m_daughterTrackList.begin(), m_daughterTrackList.end(), pTrack))
        return STATUS_CODE_ALREADY_PRESENT;

    m_daughterTrackList.push_back(pTrack);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddSibling(const Track *const pTrack)
{
    if (!pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_siblingTrackList.end() != std::find(m_siblingTrackList.begin(), m_siblingTrackList.end(), pTrack))
        return STATUS_CODE_ALREADY_PRESENT;

    m_siblingTrackList.push_back(pTrack);
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
