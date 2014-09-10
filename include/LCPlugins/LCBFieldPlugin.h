/**
 *  @file   LCContent/include/LCPlugins/LCBFieldPlugin.h
 * 
 *  @brief  Header file for the lc bfield plugin class.
 * 
 *  $Log: $
 */

#ifndef LC_BFIELD_PLUGIN_H
#define LC_BFIELD_PLUGIN_H 1

#include "Plugins/BFieldPlugin.h"

namespace lc_content
{

/**
 *  @brief  LCBFieldPlugin class
 */
class LCBFieldPlugin : public pandora::BFieldPlugin
{
public:
    /**
     *  @brief  Default constructor
     * 
     *  @param  innerBField the bfield in the main tracker, ecal and hcal, units Tesla
     *  @param  muonBarrelBField the bfield in the muon barrel, units Tesla
     *  @param  muonEndCapBField the bfield in the muon endcap, units Tesla
     */
    LCBFieldPlugin(const float innerBField, const float muonBarrelBField, const float muonEndCapBField);

    float GetBField(const pandora::CartesianVector &positionVector) const;

private:
    pandora::StatusCode Initialize();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float   m_innerBField;              ///< The bfield in the main tracker, ecal and hcal, units Tesla
    float   m_muonBarrelBField;         ///< The bfield in the muon barrel, units Tesla
    float   m_muonEndCapBField;         ///< The bfield in the muon endcap, units Tesla

    float   m_muonEndCapInnerZ;         ///< The muon endcap inner z coordinate, units mm
    float   m_coilMidPointR;            ///< The r coordinate at the coil midpoint, units mm
};

} // namespace lc_content

#endif // #ifndef LC_BFIELD_PLUGIN_H
