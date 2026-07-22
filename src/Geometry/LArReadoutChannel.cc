/**
 *  @file   PandoraSDK/src/Geometry/LArReadoutChannel.cc
 *
 *  @brief  Implementation of the LArReadoutChannel class.
 *
 *  $Log: $
 */

#include "Geometry/LArReadoutChannel.h"

#include "Pandora/StatusCodes.h"

namespace pandora
{

LArReadoutChannel::LArReadoutChannel(unsigned int id, const ViewChannelIntervalArray &channelIntervalArray) :
    m_id(id),
    m_channelIntervalArray(channelIntervalArray)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

const LArReadoutChannel::ChannelInterval &LArReadoutChannel::GetChannelInterval(const pandora::HitType view) const
{
    for (const auto &[v, interval] : m_channelIntervalArray)
    {
        if (v == view)
            return interval;
    }

    throw StatusCodeException(STATUS_CODE_NOT_FOUND);
}

//------------------------------------------------------------------------------------------------------------------------------------------

LArReadoutChannel::~LArReadoutChannel()
{
}

} // namespace pandora

