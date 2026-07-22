/**
 *  @file   PandoraSDK/include/Geometry/LArReadoutUnit.h
 *
 *  @brief  This class describes a readout unit for a LArTPC. For example, in a horizontal drift TPC, this class represents a wire plane
 *          associated with a single APA.
 *
 *  $Log: $
 */
#ifndef PANDORA_LAR_READOUT_UNIT_H
#define PANDORA_LAR_READOUT_UNIT_H 1

#include "Geometry/LArReadoutChannel.h"

#include "Pandora/PandoraEnumeratedTypes.h"

#include <vector>

namespace pandora
{
/**
 *  @brief  LArReadoutUnit class. This class describes a readout unit for a LArTPC. For example, in a horizontal drift TPC, this class
 *          represents a wire plane associated with a single APA.
 */
class LArReadoutUnit
{
public:
    typedef std::vector<LArReadoutUnit> ReadoutUnits;

    /**
     *  @brief  Constructor
     *
     *  @param  id the readout unit id (e,g, plane id in a horizontal drift TPC)
     *  @param  view the view of the readout unit
     *  @param  readoutChannels the collection of readout channels associated with this readout unit
     */
    LArReadoutUnit(unsigned int id, pandora::HitType view, const LArReadoutChannel::ReadoutChannels &readoutChannels);

    /**
     *  @brief  Destructor
     */
    ~LArReadoutUnit();

    /**
     *  @brief  Get the id of the readout unit.
     *
     *  @return the readout unit id
     */
    unsigned int GetId() const;

    /**
     *  @brief  Get the view of the readout unit.
     *
     *  @return the view of the readout unit
     */
    pandora::HitType GetView() const;

    /**
     *  @brief  Get the readout channels associated with the readout unit. In a horizontal drift TPC, this would be the collection of wires.
     *
     *  @return the readout channels associated with the readout unit
     */
    const LArReadoutChannel::ReadoutChannels &GetReadoutChannels() const;

private:
    unsigned int m_id;                  ///< The id of the readout unit
    pandora::HitType m_view;            ///< The view of the readout unit
    LArReadoutChannel::ReadoutChannels m_readoutChannels;   ///< The collection of readout channels associated with this readout unit
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int LArReadoutUnit::GetId() const
{
    return m_id;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::HitType LArReadoutUnit::GetView() const
{
    return m_view;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArReadoutChannel::ReadoutChannels &LArReadoutUnit::GetReadoutChannels() const
{
    return m_readoutChannels;
}

} // namespace pandora

#endif // #ifndef PANDORA_LAR_READOUT_UNIT_H

