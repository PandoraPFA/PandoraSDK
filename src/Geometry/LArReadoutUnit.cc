/**
 *  @file   PandoraSDK/src/Geometry/LArReadoutUnit.cc
 *
 *  @brief  Implementation of the LArReadoutUnit class.
 *
 *  $Log: $
 */

#include "Geometry/LArReadoutUnit.h"

namespace pandora
{

LArReadoutUnit::LArReadoutUnit(unsigned int id, HitType view, float referenceCoordinate, float pitch, const LArReadoutChannel::ReadoutChannels &readoutChannels) :
    m_id(id),
    m_view(view),
    m_referenceCoordinate(referenceCoordinate),
    m_pitch(pitch),
    m_readoutChannels(readoutChannels)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

LArReadoutUnit::~LArReadoutUnit()
{
}

} // namespace pandora

