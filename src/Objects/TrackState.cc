/**
 *  @file PandoraPFANew/Framework/src/Objects/TrackState.cc
 * 
 *  @brief Implementation of the track state class.
 * 
 *  $Log: $
 */

#include "Objects/TrackState.h"

#include <iostream>

namespace pandora
{

TrackState::TrackState(float x, float y, float z, float px, float py, float pz) :
    m_position(CartesianVector(x, y, z)),
    m_momentum(CartesianVector(px, py, pz))
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackState::TrackState(const CartesianVector &position, const CartesianVector &momentum) :
    m_position(position),
    m_momentum(momentum)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &stream, const TrackState &trackState)
{
    stream  << " Position:   " << trackState.GetPosition() << std::endl
            << " Momentum:   " << trackState.GetMomentum();

    return stream;
}

} // namespace pandora
