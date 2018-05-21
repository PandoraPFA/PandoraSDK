/**
 *  @file   PandoraSDK/include/Plugins/LArTransformationPlugin.h
 * 
 *  @brief  Header file for the lar transformation plugin interface class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_LAR_TRANSFORMATION_PLUGIN_H
#define PANDORA_LAR_TRANSFORMATION_PLUGIN_H 1

#include "Pandora/PandoraEnumeratedTypes.h"
#include "Pandora/Process.h"

namespace pandora
{

/**
 *  @brief  LArTransformationPlugin class
 */
class LArTransformationPlugin : public Process
{
public:
    /**
     *  @brief  Transform from (U,V) to W position
     *
     *  @param  U the U position
     *  @param  V the V position
     */
    virtual double UVtoW(const double u, const double v) const = 0;

    /**
     *  @brief  Transform from (V,W) to U position
     *
     *  @param  V the V position
     *  @param  W the W position
     */
    virtual double VWtoU(const double v, const double w) const = 0;

    /**
     *  @brief  Transform from (W,U) to V position
     *
     *  @param  W the W position
     *  @param  U the U position
     */
    virtual double WUtoV(const double w, const double u) const = 0;

    /**
     *  @brief  Transform from (U,V) to Y position
     *
     *  @param  U the U position
     *  @param  V the V position
     */
    virtual double UVtoY(const double u, const double v) const = 0;

    /**
     *  @brief  Transform from (U,V) to Z position
     *
     *  @param  U the U position
     *  @param  V the V position
     */
    virtual double UVtoZ(const double u, const double v) const = 0;

    /**
     *  @brief  Transform from (U,W) to Y position
     *
     *  @param  U the U position
     *  @param  W the W position
     */
    virtual double UWtoY(const double u, const double w) const = 0;

    /**
     *  @brief  Transform from (U,W) to Z position
     *
     *  @param  U the U position
     *  @param  W the W position
     */
    virtual double UWtoZ(const double u, const double w) const = 0;

    /**
     *  @brief  Transform from (V,W) to Y position
     *
     *  @param  V the V position
     *  @param  W the W position
     */
    virtual double VWtoY(const double v, const double w) const = 0;

    /**
     *  @brief  Transform from (V,W) to Z position
     *
     *  @param  V the V position
     *  @param  W the W position
     */
    virtual double VWtoZ(const double v, const double w) const = 0;

    /**
     *  @brief  Transform from (Y,Z) to U position
     *
     *  @param  Y the Y position
     *  @param  Z the Z position
     */
    virtual double YZtoU(const double y, const double z) const = 0;

    /**
     *  @brief  Transform from (Y,Z) to V position
     *
     *  @param  Y the Y position
     *  @param  Z the Z position
     */
    virtual double YZtoV(const double y, const double z) const = 0;

    /**
     *  @brief  Transform from (Y,Z) to W position
     *
     *  @param  Y the Y position
     *  @param  Z the Z position
     */
    virtual double YZtoW(const double y, const double z) const = 0;

    /**
     *  @brief  Get the y, z position that yields the minimum chi squared value with respect to specified u, v and w coordinates
     *
     *  @param  u the u coordinate
     *  @param  v the v coordinate
     *  @param  w the w coordinate
     *  @param  sigmaU the uncertainty in the u coordinate
     *  @param  sigmaV the uncertainty in the v coordinate
     *  @param  sigmaW the uncertainty in the w coordinate
     *  @param  y to receive the y coordinate
     *  @param  z to receive the z coordinate
     *  @param  chiSquared to receive the chi squared value
     */
    virtual void GetMinChiSquaredYZ(const double u, const double v, const double w, const double sigmaU, const double sigmaV, const double sigmaW,
        double &y, double &z, double &chiSquared) const = 0;

    /** 
     *  @brief  Get the y, z position that yields the minimum chi squared value with respect to specified u, v and w coordinates
     *          and a provided fit to an overall trajectory in 3D
     *
     *  @param  u the u coordinate
     *  @param  v the v coordinate
     *  @param  w the w coordinate
     *  @param  sigmaU the uncertainty in the u coordinate
     *  @param  sigmaV the uncertainty in the v coordinate
     *  @param  sigmaW the uncertainty in the w coordinate
     *  @param  uFit the u coordinate from a fit to an overall trajectory
     *  @param  vFit the v coordinate from a fit to an overall trajectory
     *  @param  wFit the w coordinate from a fit to an overall trajectory
     *  @param  sigmaFit the uncertainty in coordinates extracted from the fit to an overall trajectory
     *  @param  y to receive the y coordinate
     *  @param  z to receive the z coordinate
     *  @param  chiSquared to receive the chi squared value
     */
    virtual void GetMinChiSquaredYZ(const double u, const double v, const double w, const double sigmaU, const double sigmaV, const double sigmaW,
        const double uFit, const double vFit, const double wFit, const double sigmaFit, double &y, double &z, double &chiSquared) const = 0;

protected:
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef PANDORA_LAR_TRANSFORMATION_PLUGIN_H
