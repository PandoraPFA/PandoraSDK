/**
 *  @file   PandoraSDK/include/Geometry/LArReadoutVolume.h
 *
 *  @brief  This class describes a readout volume for a LArTPC. For example, in a horizontal drift TPC, this class represents the volume
 *          associated with a single APA.
 *
 *  $Log: $
 */
#ifndef PANDORA_LAR_READOUT_VOLUME_H
#define PANDORA_LAR_READOUT_VOLUME_H 1

#include "Geometry/LArReadoutUnit.h"
#include "Objects/CartesianVector.h"

#include <map>

namespace pandora
{
class LArTPC;

/**
 *  @brief  LArReadoutVolume class. This class describes a readout volume for a LArTPC. For example, in a horizontal drift TPC, this class
 *          represents the volume associated with a single APA.
 */
class LArReadoutVolume
{
public:
    typedef std::map<unsigned int, LArReadoutVolume> ReadoutVolumes;

    /**
     *  @brief  Constructor
     *
     *  @param  id the readout volume id
     *  @param  center the center of the readout volume (x, y, z)
     *  @param  size the size of the readout volume (x, y, z)
     *  @param  readoutUnits the collection of readout units associated with this readout volume
     */
    LArReadoutVolume(unsigned int id, const pandora::CartesianVector &center, const pandora::CartesianVector &size,
        const LArReadoutUnit::ReadoutUnits &readoutUnits);

    /**
     *  @brief  Destructor
     */
    ~LArReadoutVolume();

    /**
     *  @brief  Get the id of the readout volume.
     *
     *  @return the readout volume id
     */
    unsigned int GetId() const;

    /**
     *  @brief  Get the center of the readout volume.
     *
     *  @return the center of the readout volume (x, y, z)
     */
    const pandora::CartesianVector &GetCenter() const;

    /**
     *  @brief  Get the size of the readout volume.
     *
     *  @return the size of the readout volume (x, y, z)
     */
     const pandora::CartesianVector &GetSize() const;

    /**
     *  @brief  Get the readout units associated with the readout volume. In a horizontal drift TPC, this would be the collection of wire planes.
     *
     *  @return the readout units associated with the readout volume
     */
    const LArReadoutUnit::ReadoutUnits &GetReadoutUnits() const;

    /**
     *  @brief  Get the parent TPC to which this readout volume belongs. In a horizontal drift TPC, this would be the TPC associated with the APA.
     *
     *  @return a pointer to the parent TPC
     */
    const LArTPC *GetParentTPC() const;

    /**
     *  @brief  Get the neighbouring readout volume in the specified direction. This function works within either a single physical TPC, or within
     *          a single virtual TPC (e.g. merged TPCs for slicing).
     *
     *  @param  direction the direction of the neighbouring readout volume
     *
     *  @return a pointer to the neighbouring readout volume, or nullptr if no neighbour exists in that direction
     */
    const LArReadoutVolume *GetNeighbour(ReadoutVolumeNeighbour direction) const;

private:
    /**
     *  @brief  Set the parent TPC to which this readout volume belongs. Only accessible by the friend class LArTPC.
     *
     *  @param  pParent a pointer to the parent TPC
     */
    void SetParent(const LArTPC *pParent) const;

    /**
     *  @brief  Set the neighbouring readout volume in the specified direction. Only accessible by the friend class LArTPC.
     *
     *  @param  direction the direction of the neighbouring readout volume
     *  @param  pNeighbour a pointer to the neighbouring readout volume
     */
    void SetNeighbour(ReadoutVolumeNeighbour direction, const LArReadoutVolume *pNeighbour) const;

    /**
     *  @brief  Finalize the readout volume. This method is called by the parent TPC once all readout volumes have been added. In this way, the
     *          various links back to parent objects Channel -> Unit -> Volume -> TPC are valid.
     */
    void Finalize() const;
    friend class LArTPC;

    unsigned int m_id;                  ///< The id of the readout volume
    pandora::CartesianVector m_center;  ///< The center of the readout volume (x, y, z)
    pandora::CartesianVector m_size;    ///< The size of the readout volume (x, y, z)
    LArReadoutUnit::ReadoutUnits m_readoutUnits;    ///< The collection of readout units associated with this readout volume
    mutable const LArTPC *m_pParentTPC{nullptr};    ///< A pointer to the parent TPC
    mutable std::array<const LArReadoutVolume *, 6> m_neighbours{}; ///< The neighbouring readout volumes - nullptr if no neighbour in direction
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int LArReadoutVolume::GetId() const
{
    return m_id;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::CartesianVector &LArReadoutVolume::GetCenter() const
{
    return m_center;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::CartesianVector &LArReadoutVolume::GetSize() const
{
    return m_size;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArReadoutUnit::ReadoutUnits &LArReadoutVolume::GetReadoutUnits() const
{
    return m_readoutUnits;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArTPC *LArReadoutVolume::GetParentTPC() const
{
    return m_pParentTPC;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void LArReadoutVolume::SetParent(const LArTPC *pParent) const
{
    m_pParentTPC = pParent;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArReadoutVolume *LArReadoutVolume::GetNeighbour(ReadoutVolumeNeighbour direction) const
{
    return m_neighbours[static_cast<unsigned int>(direction)];
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void LArReadoutVolume::SetNeighbour(ReadoutVolumeNeighbour direction, const LArReadoutVolume *pNeighbour) const
{
    m_neighbours[static_cast<unsigned int>(direction)] = pNeighbour;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void LArReadoutVolume::Finalize() const
{
    for (const LArReadoutUnit &unit : m_readoutUnits)
    {
        unit.SetParent(this);
        unit.Finalize();
    }
}

} // namespace pandora

#endif // #ifndef PANDORA_LAR_READOUT_VOLUME_H

