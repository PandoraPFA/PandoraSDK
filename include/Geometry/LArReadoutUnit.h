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

#include "Objects/CartesianVector.h"

#include "Pandora/PandoraEnumeratedTypes.h"
#include "Pandora/StatusCodes.h"

#include <vector>

namespace pandora
{
class LArReadoutVolume;

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
     *  @param  referenceCoordinate the reference coordinate of the readout unit. This is the centre of the first channel in the plane.
     *  @param  pitch the signed pitch of the readout unit. This is the distance between channels in the plane, with a sign that indicates
     *  @param  unitCenter the center of the readout unit's own active-area box.
     *  @param  unitSize the size of the readout unit's own active-area box.
     *  @param  readoutChannels the collection of readout channels associated with this readout unit
     */
    LArReadoutUnit(unsigned int id, pandora::HitType view, float referenceCoordinate, float pitch, const pandora::CartesianVector &unitCenter,
        const pandora::CartesianVector &unitSize, const LArReadoutChannel::ReadoutChannels &readoutChannels);

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
     *  @brief  Get the reference coordinate of the readout unit. This is the centre of the first channel in the plane.
     *
     *  @return the reference coordinate of the readout unit
     */
    float GetReferenceCoordinate() const;

    /**
     *  @brief  Get the signed pitch of the readout unit. This is the distance between channels in the plane, with a sign that indicates
     *          whether the channel number increases or decreases with increasing coordinate.
     *
     *  @return the signed pitch of the readout unit
     */
    float GetPitch() const;

    /**
     *  @brief  Get the center of the readout unit's own active-area box. This is used to define the readout unit's own coordinate system.
     *
     *  @return the center of the readout unit's own active-area box
     */
    const pandora::CartesianVector &GetUnitCenter() const;

    /**
     *  @brief  Get the size of the readout unit's own active-area box. This is used to define the readout unit's own coordinate system.
     *
     *  @return the size of the readout unit's own active-area box
     */
    const pandora::CartesianVector &GetUnitSize() const;

    /**
     *  @brief  Get the readout channels associated with the readout unit. In a horizontal drift TPC, this would be the collection of wires.
     *
     *  @return the readout channels associated with the readout unit
     */
    const LArReadoutChannel::ReadoutChannels &GetReadoutChannels() const;

    /**
     *  @brief  Get the readout channel associated with the readout unit with the specified id.
     *
     *  @param  id the readout channel id
     *
     *  @return the readout channel
     *  @throws StatusCodeException if the specified readout channel id is not present in the readout channels
     */
    const LArReadoutChannel &GetReadoutChannel(const unsigned int id) const;

    /**
     *  @brief  Get the parent readout volume to which this readout unit belongs. In a horizontal drift TPC, this would be the APA associated
     *          with the wire plane.
     *
     *  @return a pointer to the parent readout volume
     */
    const LArReadoutVolume *GetParentReadoutVolume() const;

private:
    /**
     *  @brief  Set the parent readout volume to which this unit. Only accessible by the friend class LArReadoutVolume.
     *
     *  @param  pParent a pointer to the parent readout volume
     */
    void SetParent(const LArReadoutVolume *pParent) const;

    /**
     *  @brief  Finalize the readout unit. This method is called by the parent readout volume. In this way, the various links back to parent
     *          objects Channel -> Unit -> Volume -> TPC are valid.
     */
    void Finalize() const;
    friend class LArReadoutVolume;


    unsigned int m_id;                  ///< The id of the readout unit
    pandora::HitType m_view;            ///< The view of the readout unit
    float m_referenceCoordinate;        ///< The reference coordinate of the readout unit
    float m_pitch;                      ///< The signed pitch of the readout unit

    pandora::CartesianVector m_unitCenter; ///< The center of the readout unit's own active-area box (X unused)
    pandora::CartesianVector m_unitSize;   ///< The size of the readout unit's own active-area box (X unused)

    LArReadoutChannel::ReadoutChannels m_readoutChannels;   ///< The collection of readout channels associated with this readout unit
    mutable const LArReadoutVolume *m_pParentReadoutVolume{nullptr};   ///< Pointer to the parent readout volume (e.g. an APA) to which this unit belongs
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

inline float LArReadoutUnit::GetReferenceCoordinate() const
{
    return m_referenceCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArReadoutUnit::GetPitch() const
{
    return m_pitch;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::CartesianVector &LArReadoutUnit::GetUnitCenter() const
{
    return m_unitCenter;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::CartesianVector &LArReadoutUnit::GetUnitSize() const
{
    return m_unitSize;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArReadoutChannel::ReadoutChannels &LArReadoutUnit::GetReadoutChannels() const
{
    return m_readoutChannels;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArReadoutChannel &LArReadoutUnit::GetReadoutChannel(const unsigned int id) const
{
    if (id >= m_readoutChannels.size())
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return m_readoutChannels[id];
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArReadoutVolume *LArReadoutUnit::GetParentReadoutVolume() const
{
    return m_pParentReadoutVolume;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void LArReadoutUnit::SetParent(const LArReadoutVolume *pParent) const
{
    m_pParentReadoutVolume = pParent;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void LArReadoutUnit::Finalize() const
{
    for (const LArReadoutChannel &channel : m_readoutChannels)
        channel.SetParent(this);
}

} // namespace pandora

#endif // #ifndef PANDORA_LAR_READOUT_UNIT_H

