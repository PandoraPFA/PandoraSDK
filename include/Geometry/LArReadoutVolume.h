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

namespace pandora
{
/**
 *  @brief  LArReadoutVolume class. This class describes a readout volume for a LArTPC. For example, in a horizontal drift TPC, this class
 *          represents the volume associated with a single APA.
 */
class LArReadoutVolume
{
public:
    typedef std::vector<LArReadoutVolume> ReadoutVolumes;

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

private:
    unsigned int m_id;                  ///< The id of the readout volume
    pandora::CartesianVector m_center;  ///< The center of the readout volume (x, y, z)
    pandora::CartesianVector m_size;    ///< The size of the readout volume (x, y, z)
    LArReadoutUnit::ReadoutUnits m_readoutUnits;    ///< The collection of readout units associated with this readout volume
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

} // namespace pandora

#endif // #ifndef PANDORA_LAR_READOUT_VOLUME_H

