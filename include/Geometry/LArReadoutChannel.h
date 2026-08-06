/**
 *  @file   PandoraSDK/include/Geometry/LArReadoutChannel.h
 *
 *  @brief  This class describes a readout channel for a LArTPC. For example, in a horizontal drift TPC, this class represents a wire
 *          associated with a particular wire plane.
 *
 *  $Log: $
 */
#ifndef PANDORA_LAR_READOUT_CHANNEL_H
#define PANDORA_LAR_READOUT_CHANNEL_H 1

#include "Pandora/PandoraEnumeratedTypes.h"

#include <array>
#include <utility>
#include <vector>

namespace pandora
{
class LArReadoutUnit;

/**
 *  @brief  LArReadoutChannel class. This class describes a readout channel for a LARTPC. For example, in a horizontal drift TPC, this class
 *          represents a wire associated with a particular wire plane.
 */
class LArReadoutChannel
{
public:
    typedef std::pair<unsigned int, unsigned int> ChannelInterval;
    typedef std::pair<HitType, ChannelInterval> ViewChannelInterval;
    typedef std::array<ViewChannelInterval, 2> ViewChannelIntervalArray;
    typedef std::vector<LArReadoutChannel> ReadoutChannels;

    /**
     *  @brief  Constructor
     *
     *  @param  id the readout unit id (e,g, plane id in a horizontal drift TPC)
     *  @param  channelIntervalArray an array describing the channel id interval for channels in other views that this channel can
     *          'intersect'
     */
    LArReadoutChannel(unsigned int id, const ViewChannelIntervalArray &channelIntervalArray);

    /**
     *  @brief  Destructor
     */
    ~LArReadoutChannel();

    /**
     *  @brief  Get the id of the readout unit.
     *
     *  @return the readout unit id
     */
    unsigned int GetId() const;

    /**
     *  @brief  Get the channel interval for the specified view.
     *
     *  @param  view the view for which to retrieve the channel interval
     *
     *  @return the channel interval [min, max] for the specified view
     *  @throws StatusCodeException if the specified view is not present in the channel interval array
     */
    const ChannelInterval &GetChannelInterval(const pandora::HitType view) const;

    /**
     *  @brief  Get the channel interval array for all views.
     *
     *  @return the channel interval array for all views
     */
    const ViewChannelIntervalArray &GetChannelIntervals() const;

    /**
     *  @brief  Get the parent readout unit (e.g. wire plane) to which this channel belongs.
     *
     *  @return a pointer to the parent readout unit
     */
    const LArReadoutUnit *GetParentReadoutUnit() const;

private:
    /**
     *  @brief  Set the parent readout unit (e.g. wire plane) to which this channel belongs. Only accessible by the friend class LArReadoutUnit.
     *
     *  @param  pParent a pointer to the parent readout unit
     */
    void SetParent(const LArReadoutUnit *pParent) const;
    friend class LArReadoutUnit;

    unsigned int m_id;                                  ///< The id of the readout channel
    ViewChannelIntervalArray m_channelIntervalArray;    ///< An array describing the channel id 'intersection' intervals for each view
    mutable const LArReadoutUnit *m_pParentReadoutUnit{nullptr};    ///< Pointer to the parent readout unit (e.g. wire plane) to which this channel belongs
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int LArReadoutChannel::GetId() const
{
    return m_id;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArReadoutChannel::ViewChannelIntervalArray &LArReadoutChannel::GetChannelIntervals() const
{
    return m_channelIntervalArray;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArReadoutUnit *LArReadoutChannel::GetParentReadoutUnit() const
{
    return m_pParentReadoutUnit;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void LArReadoutChannel::SetParent(const LArReadoutUnit *pParent) const
{
    m_pParentReadoutUnit = pParent;
}

} // namespace pandora

#endif // #ifndef PANDORA_LAR_READOUT_CHANNEL_H

