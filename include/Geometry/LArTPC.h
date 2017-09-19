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
     *  @brief  Get the lar tpc name, uniquely specifying the lar tpc
     *
     *  @return the lar tpc name
     */
    const std::string &GetLArTPCName() const;

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
     *  @brief  Get the u wire inclination, units radians
     *
     *  @return the u wire inclination
     */
    float GetThetaU() const;

    /**
     *  @brief  Get the v wire inclination, units radians
     *
     *  @return the v wire inclination
     */
    float GetThetaV() const;

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

    std::string     m_larTPCName;               ///< The lar tpc name, must uniquely specify a single lar tpc
    float           m_centerX;                  ///< The center in x, units mm
    float           m_centerY;                  ///< The center in y, units mm
    float           m_centerZ;                  ///< The center in z, units mm
    float           m_widthX;                   ///< The width in x, units mm
    float           m_widthY;                   ///< The width in y, units mm
    float           m_widthZ;                   ///< The width in z, units mm
    float           m_wirePitchU;               ///< The u wire pitch, units mm
    float           m_wirePitchV;               ///< The v wire pitch, units mm
    float           m_wirePitchW;               ///< The w wire pitch, units mm
    float           m_thetaU;                   ///< The u wire inclination, units radians
    float           m_thetaV;                   ///< The v wire inclination, units radians
    float           m_sigmaUVW;                 ///< The u, v, w resolution, units mm
    bool            m_isDriftInPositiveX;       ///< Whether the electron drift is in the positive x direction

    friend class GeometryManager;
    friend class PandoraObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object>;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string &LArTPC::GetLArTPCName() const
{
    return m_larTPCName;
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

inline float LArTPC::GetThetaU() const
{
    return m_thetaU;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LArTPC::GetThetaV() const
{
    return m_thetaV;
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
