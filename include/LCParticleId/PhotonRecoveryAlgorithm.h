/**
 *  @file   LCContent/include/LCParticleId/PhotonRecoveryAlgorithm.h
 * 
 *  @brief  Header file for the photon recovery algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_PHOTON_RECOVERY_ALGORITHM_H
#define LC_PHOTON_RECOVERY_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  PhotonRecoveryAlgorithm class
 */
class PhotonRecoveryAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief Default constructor
     */
    PhotonRecoveryAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Examine clusters in a specified list, identifying and flagging those clusters that are likely photons
     * 
     *  @param  pClusterList address of the cluster list
     */
    void FindPhotonsIdentifiedAsHadrons(const pandora::ClusterList *const pClusterList) const;

    /**
     *  @brief  Get the maximum of 1) the fraction of total cluster electromagnetic energy deposited in the barrel, or
     *          2) the fraction of total cluster electromagnetic energy deposited in the endcap
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return the barrel endcap energy split
     */
    float GetBarrelEndCapEnergySplit(const pandora::Cluster *const pCluster) const;

    /**
     *  @brief  Examine clusters in a specified list, identifying and flagging candidate "soft" photons
     * 
     *  @param  pClusterList address of the cluster list
     */
    void PerformSoftPhotonId(const pandora::ClusterList *const pClusterList) const;

    /**
     *  @brief  Whether a cluster should be considered as a "soft" photon cluster
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    bool IsSoftPhoton(pandora::Cluster *const pCluster) const;

    std::string     m_trackClusterAssociationAlgName;   ///< The name of the track-cluster association algorithm to run

    float           m_minElectromagneticEnergy;         ///< Minimum cluster electromagnetic energy for identification as a photon
    unsigned int    m_maxInnerLayer;                    ///< Maximum cluster inner layer

    float           m_profileStartCut1;                 ///< Initial cut on shower profile start for identification as a photon
    float           m_profileStartEnergyCut;            ///< Electromagnetic energy above which shower profile start cut will be altered
    float           m_profileStartCut2;                 ///< Cut on shower profile start for clusters passing energy cut
    float           m_profileStartCut3;                 ///< Cut on shower profile start for clusters also passing profile discrepancy cuts

    float           m_profileDiscrepancyCut1;           ///< Initial cut on shower profile discrepancy for identification as a photon
    float           m_profileDiscrepancyEnergyCut;      ///< Electromagnetic energy above which profile discrepancy cut will be altered
    float           m_profileDiscrepancyCutParameter1;  ///< Parameter1 for calculation of discrepancy cut (for clusters passing energy cut)
    float           m_profileDiscrepancyCutParameter2;  ///< Parameter2 for calculation of discrepancy cut (for clusters passing energy cut)
    float           m_minProfileDiscrepancy;            ///< Minimum shower profile discrepancy for identification as a photon
    float           m_profileDiscrepancyCut2;           ///< Cut on shower profile discrepancy for clusters also passing profile start cuts

    unsigned int    m_maxOverlapInnerLayer;             ///< Maximum inner layer for clusters in barrel-endcap overlap region
    float           m_maxOverlapMipFraction;            ///< Maximum mip fraction for clusters in barrel-endcap overlap region
    float           m_minOverlapRadialDirectionCosine;  ///< Minimum radial direction cosine for clusters in barrel-endcap overlap region
    float           m_maxBarrelEndCapSplit;             ///< Maximum barrel-endcap electromagnetic energy split for cluster

    unsigned int    m_softPhotonMinCaloHits;            ///< Soft photon id min number of calo hits in cluster
    unsigned int    m_softPhotonMaxCaloHits;            ///< Soft photon id max number of calo hits in cluster
    float           m_softPhotonMaxEnergy;              ///< Soft photon id max cluster electromagnetic energy
    unsigned int    m_softPhotonMaxInnerLayer;          ///< Soft photon id max cluster inner pseudolayer
    float           m_softPhotonMaxDCosR;               ///< Soft photon id max cluster radial direction cosine
    float           m_softPhotonLowEnergyCut;           ///< Soft photon id low electromagnetic energy cut
    float           m_softPhotonLowEnergyMaxDCosR;      ///< Soft photon id max radial direction cosine for low energy cluster
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonRecoveryAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonRecoveryAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_PHOTON_RECOVERY_ALGORITHM_H
