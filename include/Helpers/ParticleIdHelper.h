/**
 *  @file   PandoraPFANew/Framework/include/Helpers/ParticleIdHelper.h
 * 
 *  @brief  Header file for the particle id helper class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_ID_HELPER_H
#define PARTICLE_ID_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

class ShowerProfileCalculator;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ParticleIdHelper class
 */
class ParticleIdHelper
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
        ShowerPeak(const float peakEnergy, const float peakRms, const CaloHitList &peakCaloHitList);

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
         *  @brief  Get peak calo hit list
         * 
         *  @return the peak calo hit list
         */
        const CaloHitList &GetPeakCaloHitList() const;

    private:
        float           m_peakEnergy;                   ///< The peak energy
        float           m_peakRms;                      ///< The peak rms
        CaloHitList     m_peakCaloHitList;              ///< The peak calo hit list
    };

    typedef std::vector<ShowerPeak> ShowerPeakList;

    /**
     *  @brief  Provide fast identification of whether a cluster is an electromagnetic shower
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsEmShowerFast(const Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is an electromagnetic shower
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsEmShowerFull(const Cluster *const pCluster);

    /**
     *  @brief  Provide fast identification of whether a cluster is a photon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsPhotonFast(const Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is a photon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsPhotonFull(const Cluster *const pCluster);

    /**
     *  @brief  Provide fast identification of whether a cluster is an electron
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsElectronFast(const Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is an electron
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsElectronFull(const Cluster *const pCluster);

    /**
     *  @brief  Provide fast identification of whether a cluster is a muon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsMuonFast(const Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is a muon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsMuonFull(const Cluster *const pCluster);

    /**
     *  @brief  Calculate longitudinal shower profile for a cluster and compare it with the expected profile for a photon
     * 
     *  @param  pCluster address of the cluster to investigate
     *  @param  profileStart to receive the profile start, in radiation lengths
     *  @param  profileDiscrepancy to receive the profile discrepancy
     */
    static void CalculateLongitudinalProfile(const Cluster *const pCluster, float &profileStart, float &profileDiscrepancy);

    /**
     *  @brief  Calculate transverse shower profile for a cluster and get the list of peaks identified in the profile
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  showerPeakList to receive the shower peak list
     */
    static void CalculateTransverseProfile(const Cluster *const pCluster, const PseudoLayer maxPseudoLayer, ShowerPeakList &showerPeakList);

private:
    /**
     *  @brief  Set the shower profile calculator
     * 
     *  @param  pPseudoLayerCalculator address of the shower profile calculator
     */
    static StatusCode SetShowerProfileCalculator(ShowerProfileCalculator *pShowerProfileCalculator);

    /**
     *  @brief  Read the particle id helper settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle *const pXmlHandle);

    static ParticleIdFunction      *m_pEmShowerFastFunction;        ///< The fast electromagnetic shower id function pointer
    static ParticleIdFunction      *m_pEmShowerFullFunction;        ///< The full electromagnetic shower id function pointer
    static ParticleIdFunction      *m_pPhotonFastFunction;          ///< The fast photon id function pointer
    static ParticleIdFunction      *m_pPhotonFullFunction;          ///< The full photon id function pointer
    static ParticleIdFunction      *m_pElectronFastFunction;        ///< The fast electron id function pointer
    static ParticleIdFunction      *m_pElectronFullFunction;        ///< The full electron id function pointer
    static ParticleIdFunction      *m_pMuonFastFunction;            ///< The fast muon id function pointer
    static ParticleIdFunction      *m_pMuonFullFunction;            ///< The full muon id function pointer

    static ShowerProfileCalculator *m_pShowerProfileCalculator;     ///< Address of the shower profile calculator 

    friend class PandoraApiImpl;
    friend class PandoraSettings;
    friend class PluginManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline ParticleIdHelper::ShowerPeak::ShowerPeak(const float peakEnergy, const float peakRms, const CaloHitList &peakCaloHitList) :
    m_peakEnergy(peakEnergy),
    m_peakRms(peakRms),
    m_peakCaloHitList(peakCaloHitList)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ParticleIdHelper::ShowerPeak::GetPeakEnergy() const
{
    return m_peakEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ParticleIdHelper::ShowerPeak::GetPeakRms() const
{
    return m_peakRms;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CaloHitList &ParticleIdHelper::ShowerPeak::GetPeakCaloHitList() const
{
    return m_peakCaloHitList;
}

} // namespace pandora

#endif // #ifndef PARTICLE_ID_HELPER_H
