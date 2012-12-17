/**
 *  @file   PandoraPFANew/Framework/src/Objects/Track.cc
 * 
 *  @brief  Implementation of the track class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"

#include "Objects/Helix.h"
#include "Objects/Track.h"

#include <cmath>
#include <cstdlib>

namespace pandora
{

Track::Track(const PandoraApi::TrackParameters &trackParameters) :
    m_d0(trackParameters.m_d0.Get()),
    m_z0(trackParameters.m_z0.Get()),
    m_particleId(trackParameters.m_particleId.Get()),
    m_charge(trackParameters.m_charge.Get()),
    m_mass(trackParameters.m_mass.Get()),
    m_momentumAtDca(trackParameters.m_momentumAtDca.Get()),
    m_momentumMagnitudeAtDca(m_momentumAtDca.GetMagnitude()),
    m_energyAtDca(std::sqrt(m_mass * m_mass + m_momentumMagnitudeAtDca * m_momentumMagnitudeAtDca)),
    m_trackStateAtStart(trackParameters.m_trackStateAtStart.Get()),
    m_trackStateAtEnd(trackParameters.m_trackStateAtEnd.Get()),
    m_trackStateAtCalorimeter(trackParameters.m_trackStateAtCalorimeter.Get()),
    m_timeAtCalorimeter(trackParameters.m_timeAtCalorimeter.Get()),
    m_reachesCalorimeter(trackParameters.m_reachesCalorimeter.Get()),
    m_isProjectedToEndCap(trackParameters.m_isProjectedToEndCap.Get()),
    m_canFormPfo(trackParameters.m_canFormPfo.Get()),
    m_canFormClusterlessPfo(trackParameters.m_canFormClusterlessPfo.Get()),
    m_pAssociatedCluster(NULL),
    m_pMCParticle(NULL),
    m_pParentAddress(trackParameters.m_pParentAddress.Get()),
    m_isAvailable(true)
{
    // Consistency checks
    if (0.f == m_energyAtDca)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    if (0 == m_charge)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    // Obtain helix fit to track state at calorimeter
    static const float bField(GeometryHelper::GetBField(CartesianVector(0.f, 0.f, 0.f)));
    m_pHelixFitAtCalorimeter = new Helix(m_trackStateAtCalorimeter.GetPosition(), m_trackStateAtCalorimeter.GetMomentum(), static_cast<float>(m_charge), bField);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Track::~Track()
{
    delete m_pHelixFitAtCalorimeter;

    m_parentTrackList.clear();
    m_siblingTrackList.clear();
    m_daughterTrackList.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::SetMCParticle(MCParticle *const pMCParticle)
{
    if (NULL == pMCParticle)
        return STATUS_CODE_FAILURE;

    m_pMCParticle = pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::SetAssociatedCluster(Cluster *const pCluster)
{
    if (NULL == pCluster)
        return STATUS_CODE_INVALID_PARAMETER;

    if (NULL != m_pAssociatedCluster)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pAssociatedCluster = pCluster;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::RemoveAssociatedCluster(Cluster *const pCluster)
{
    if (pCluster != m_pAssociatedCluster)
        return STATUS_CODE_NOT_FOUND;

    m_pAssociatedCluster = NULL;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::RemoveMCParticle()
{
    m_pMCParticle = NULL;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddParent(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_parentTrackList.insert(pTrack).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddDaughter(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_daughterTrackList.insert(pTrack).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddSibling(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_siblingTrackList.insert(pTrack).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &stream, const Track &track)
{
    stream  << " Track: " << std::endl
            << " d0     " << track.GetD0() << std::endl
            << " z0     " << track.GetZ0() << std::endl
            << " p0     " << track.GetMomentumAtDca() << std::endl;

    return stream;
}

} // namespace pandora
