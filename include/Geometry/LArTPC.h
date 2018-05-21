/**
 *  @file   PandoraSDK/include/Geometry/LArTPC.h
 *
 *  @brief  Header file for the lar tpc class.
 *
 *  $Log: $
 */
#ifndef PANDORA_LAR_TPC_H
#define PANDORA_LAR_TPC_H 1

#include "Pandora/ObjectCreation.h"

#include <string>

namespace pandora
{

template<typename T, typename S> class PandoraObjectFactory;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  LArTPC class
 */
class LArTPC
{
public:
    /**
     *  @brief  Get the lar volume id, uniquely specifying the lar tpc
     *
     *  @return the lar tpc volume id
     */
    unsigned int GetLArTPCVolumeId() const;

    /**
     *  @brief  Get center in x, units mm
     *
     *  @return the center in x
     */
    float GetCenterX() const;

    /**
     *  @brief  Get center in y, units mm
     *
     *  @return the center in y
     */
    float GetCenterY() const;

    /**
     *  @brief  Get center in z, units mm
     *
     *  @return the center in z
     */
    float GetCenterZ() const;

    /**
     *  @brief  Get the width in x, units mm
     *
     *  @return the width in x
     */
    float GetWidthX() const;

    /**
     *  @brief  Get the width in y, units mm
     *
     *  @return the width in y
     */
    float GetWidthY() const;

    /**
     *  @brief  Get the width in z, units mm
     *
     *  @return the width in z
     */
    float GetWidthZ() const;

    /**
     *  @brief  Get the u wire pitch, units mm
     *
     *  @return the u wire pitch
     */
    float GetWirePitchU() const;

    /**
     *  @brief  Get the v wire pitch, units mm
     *
     *  @return the v wire pitch
     */
    float GetWirePitchV() const;

    /**
     *  @brief  Get the w wire pitch, units mm
     *
     *  @return the w wire pitch
     */
    float GetWirePitchW() const;

    /**
     *  @brief  Get the u wire angle to the vertical, units radians
     *
     *  @return the u wire angle to the vertical
     */
    float GetWireAngleU() const;

    /**
     *  @brief  Get the v wire angle to the vertical, units radians
     *
     *  @return the v wire angle to the vertical
     */
    float GetWireAngleV() const;

    /**
     *  @brief  Get the w wire angle to the vertical, units radians
     *
     *  @return the w wire angle to the vertical
     */
    float GetWireAngleW() const;

    /**
     *  @brief  Get the u, v, w resolution, units mm
     *
     *  @return the u, v, w resolution
     */
    float GetSigmaUVW() const;

    /**
     *  @brief  Whether the electron drift is in the positive x direction
     *
     *  @return boolean
     */
    bool IsDriftInPositiveX() const;

protected:
    /**
     *  @brief  Constructor
     *
     *  @param  inputParameters the input lar tpc parameters
     */
    LArTPC(const object_creation::Geometry::LArTPC::Parameters &inputParameters);

    /**
     *  @brief  Destructor
     */
    virtual ~LArTPC();

    unsigned int    m_larTPCVolumeId;           ///< The lar tpc volume id, must uniquely specify a single lar tpc
    float           m_centerX;                  ///< The center in x, units mm
    float           m_centerY;                  ///< The center in y, units mm
    float           m_centerZ;                  ///< The center in z, units mm
    float           m_widthX;                   ///< The width in x, units mm
    float           m_widthY;                   ///< The width in y, units mm
    float           m_widthZ;                   ///< The width in z, units mm
    float           m_wirePitchU;               ///< The u wire pitch, units mm
    float           m_wirePitchV;               ///< The v wire pitch, units mm
    float           m_wirePitchW;               ///< The w wire pitch, units mm
    float           m_wireAngleU;               ///< The u wire angle to the vertical, units radians
    float           m_wireAngleV;               ///< The v wire angle to the vertical, units radians
    float           m_wireAngleW;               ///< The w wire angle to the vertical, units radians
    float           m_sigmaUVW;                 ///< The u, v, w resolution, units mm
    bool            m_isDriftInPositiveX;       ///< Whether the electron drift is in the positive x direction

    friend class GeometryManager;
    friend class PandoraObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object>;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int LArTPC::GetLArTPCVolumeId() const
{
    return m_larTPCVolumeId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetCenterX() const
{
    return m_centerX;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetCenterY() const
{
    return m_centerY;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetCenterZ() const
{
    return m_centerZ;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWidthX() const
{
    return m_widthX;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWidthY() const
{
    return m_widthY;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWidthZ() const
{
    return m_widthZ;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWirePitchU() const
{
    return m_wirePitchU;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWirePitchV() const
{
    return m_wirePitchV;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWirePitchW() const
{
    return m_wirePitchW;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWireAngleU() const
{
    return m_wireAngleU;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWireAngleV() const
{
    return m_wireAngleV;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetWireAngleW() const
{
    return m_wireAngleW;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetSigmaUVW() const
{
    return m_sigmaUVW;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool LArTPC::IsDriftInPositiveX() const
{
    return m_isDriftInPositiveX;
}

} // namespace pandora

#endif // #ifndef PANDORA_LAR_TPC_H
