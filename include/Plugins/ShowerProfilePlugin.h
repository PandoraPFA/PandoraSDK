/**
 *  @file   PandoraSDK/include/Utilities/ShowerProfilePlugin.h
 * 
 *  @brief  Header file for the shower profile plugin interface class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_SHOWER_PROFILE_PLUGIN_H
#define PANDORA_SHOWER_PROFILE_PLUGIN_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/Process.h"

namespace pandora
{

/**
 *  @brief  ShowerProfilePlugin class
 */
class ShowerProfilePlugin : public Process
{
public:
    /**
     *  @brief  ShowerPeak class
     */
    class ShowerPeak
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  peakEnergy the peak energy
         *  @param  peakRms the peak rms
         *  @param  peakCaloHitList the peak calo hit list
         */
        ShowerPeak(const float peakEnergy, const float peakRms, const float rmsRatio, const CaloHitList &peakCaloHitList);

        /**
         *  @brief  Get peak energy
         * 
         *  @return the peak energy
         */
        float GetPeakEnergy() const;

        /**
         *  @brief  Get peak rms
         * 
         *  @return the peak rms
         */
        float GetPeakRms() const;
        
        /**
         *  @brief  Get rms ratio
         * 
         *  @return the rms ratio
         */
        float GetRmsXYRatio() const;
        /**
         *  @brief  Get peak calo hit list
         * 
         *  @return the peak calo hit list
         */
        const CaloHitList &GetPeakCaloHitList() const;

    private:
        float                       m_peakEnergy;                    ///< The peak energy
        float                       m_peakRms;                       ///< The peak rms
        float                       m_rmsXYRatio;                    ///< The peak rms in X direction over Y direction (the bigger one over smaller one)
        CaloHitList                 m_peakCaloHitList;               ///< The peak calo hit list
    };

    typedef std::vector<ShowerPeak> ShowerPeakList;

    /**
     *  @brief  Get the layer at which shower can be considered to start; this function evaluates the the starting point of
     *          a series of "showerStartNonMipLayers" successive layers, each with mip fraction below "showerLayerMipFraction"
     * 
     *  @param  pCluster address of the cluster
     *  @param  showerStartLayer to receive the shower start layer
     */
    virtual void CalculateShowerStartLayer(const Cluster *const pCluster, unsigned int &showerStartLayer) const = 0;

    /**
     *  @brief  Calculate longitudinal shower profile for a cluster and compare it with the expected profile for a photon
     * 
     *  @param  pCluster address of the cluster to investigate
     *  @param  profileStart to receive the profile start, in radiation lengths
     *  @param  profileDiscrepancy to receive the profile discrepancy
     */
    virtual void CalculateLongitudinalProfile(const Cluster *const pCluster, float &profileStart, float &profileDiscrepancy) const = 0;

    /**
     *  @brief  Calculate transverse shower profile for a cluster and get the list of peaks identified in the profile, for clusters without tracks
     *          Default is exclusive mode
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  showerPeakList to receive the shower peak list
     */
    virtual void CalculateTransverseProfile(const Cluster *const pCluster, const unsigned int maxPseudoLayer, ShowerPeakList &showerPeakList) const = 0;

    /**
     *  @brief  Calculate transverse shower profile for a cluster and get the list of peaks identified in the profile, for clusters without tracks
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  showerPeakList to receive the shower peak list
     *  @param  inclusiveMode true for finding shower peaks inclusively
     */
    virtual void CalculateTransverseProfile(const Cluster *const pCluster, const unsigned int maxPseudoLayer, ShowerPeakList &showerPeakList,
        const bool inclusiveMode) const = 0;
    
    /**
     *  @brief  Calculate transverse shower profile for a cluster and get the list of peaks identified in the profile, for clusters close to tracks
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  showerPeakListPhoton to receive the shower peak list that are photon candidates
     *  @param  showerPeakListCharge to receive the shower peak list that are not photon candidates
     */
    virtual void CalculateTrackBasedTransverseProfile(const Cluster *const pCluster, const unsigned int maxPseudoLayer, const Track *const pMinTrack, 
        const TrackVector &trackVector, ShowerPeakList &showerPeakListPhoton, ShowerPeakList &showerPeakListCharge) const = 0;
        
protected:
    friend class PluginManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline ShowerProfilePlugin::ShowerPeak::ShowerPeak(const float peakEnergy, const float peakRms, const float rmsXYRatio, const CaloHitList &peakCaloHitList) :
    m_peakEnergy(peakEnergy),
    m_peakRms(peakRms),
    m_rmsXYRatio(rmsXYRatio),
    m_peakCaloHitList(peakCaloHitList)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ShowerProfilePlugin::ShowerPeak::GetPeakEnergy() const
{
    return m_peakEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ShowerProfilePlugin::ShowerPeak::GetPeakRms() const
{
    return m_peakRms;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ShowerProfilePlugin::ShowerPeak::GetRmsXYRatio() const
{
    return m_rmsXYRatio;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CaloHitList &ShowerProfilePlugin::ShowerPeak::GetPeakCaloHitList() const
{
    return m_peakCaloHitList;
}

} // namespace pandora

#endif // #ifndef PANDORA_SHOWER_PROFILE_PLUGIN_H
