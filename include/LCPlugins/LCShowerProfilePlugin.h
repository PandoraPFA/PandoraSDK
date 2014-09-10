/**
 *  @file   LCContent/include/LCPlugins/LCShowerProfilePlugin.h
 * 
 *  @brief  Header file for the lc shower profile plugin class.
 * 
 *  $Log: $
 */
#ifndef LC_SHOWER_PROFILE_PLUGIN_H
#define LC_SHOWER_PROFILE_PLUGIN_H 1

#include "Plugins/ShowerProfilePlugin.h"

namespace lc_content
{

/**
 *  @brief  LCShowerProfilePlugin class
 */
class LCShowerProfilePlugin : public pandora::ShowerProfilePlugin
{
public:
    /**
     *  @brief  Default constructor
     */
    LCShowerProfilePlugin();

    void CalculateShowerStartLayer(const pandora::Cluster *const pCluster, unsigned int &showerStartLayer) const;
    void CalculateLongitudinalProfile(const pandora::Cluster *const pCluster, float &profileStart, float &profileDiscrepancy) const;
    void CalculateTransverseProfile(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, pandora::ShowerProfilePlugin::ShowerPeakList &showerPeakList) const;

private:
    /**
     *  @brief  ShowerProfileEntry class
     */
    class ShowerProfileEntry
    {
    public:
        /**
         *  @brief  Default constructor
         */
        ShowerProfileEntry();

        bool                        m_isAvailable;          ///< Whether shower profile entry is available (prevent double counting)
        float                       m_energy;               ///< The energy associated with the shower profile entry
        pandora::CaloHitList        m_caloHitList;          ///< The list of calo hits associated with the shower profile entry
    };

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    typedef std::vector<ShowerProfileEntry> ShowerProfile;  ///< The shower profile typedef
    typedef std::vector<ShowerProfile> TwoDShowerProfile;   ///< The two dimensional shower profile typedef

    float        m_showerStartMipFraction;                  ///< Max layer mip-fraction to declare layer as shower-like
    unsigned int m_showerStartNonMipLayers;                 ///< Number of successive shower-like layers to identify shower start

    float        m_longProfileBinWidth;                     ///< Bin width used to construct longitudinal profile, units radiation lengths
    unsigned int m_longProfileNBins;                        ///< Number of bins used to construct longitudinal profile
    float        m_longProfileMinCosAngle;                  ///< Min angular correction used to adjust radiation length measures
    float        m_longProfileCriticalEnergy;               ///< Critical energy, used to calculate argument for gamma function
    float        m_longProfileParameter0;                   ///< Parameter0, used to calculate argument for gamma function
    float        m_longProfileParameter1;                   ///< Parameter1, used to calculate argument for gamma function
    float        m_longProfileMaxDifference;                ///< Max difference between current and best longitudinal profile comparisons

    int          m_transProfileNBins;                       ///< Number of bins used to construct transverse profile
    float        m_transProfilePeakThreshold;               ///< Minimum electrogmagnetic energy for profile peak bin, units GeV
    float        m_transProfileNearbyEnergyRatio;           ///< Max ratio of bin energy to nearby bin energy; used to identify peak extent
    unsigned int m_transProfileMaxPeaksToFind;              ///< Maximum number of peaks to identify in transverse profile
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline LCShowerProfilePlugin::ShowerProfileEntry::ShowerProfileEntry() :
    m_isAvailable(true),
    m_energy(0.f)
{
}

} // namespace lc_content

#endif // #ifndef LC_SHOWER_PROFILE_PLUGIN_H
