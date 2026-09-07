/**
 *  @file   PandoraSDK/src/Geometry/LArReadoutVolume.cc
 *
 *  @brief  Implementation of the LArReadoutVolume class.
 *
 *  $Log: $
 */

#include "Geometry/LArReadoutVolume.h"

namespace pandora
{

LArReadoutVolume::LArReadoutVolume(unsigned int id, const pandora::CartesianVector &center, const pandora::CartesianVector &size,
    const LArReadoutUnit::ReadoutUnits &readoutUnits) :
    m_id(id),
    m_center(center),
    m_size(size),
    m_readoutUnits(readoutUnits)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

LArReadoutVolume::~LArReadoutVolume()
{
}

} // namespace pandora
