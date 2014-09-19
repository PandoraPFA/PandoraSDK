/**
 *  @file   LCContent/include/LCFragmentRemoval/PhotonFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the photon fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_PHOTON_FRAGMENT_REMOVAL_ALGORITHM_H
#define LC_PHOTON_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "LCHelpers/FragmentRemovalHelper.h"

namespace lc_content
{

/**
 *  @brief  PhotonFragmentRemovalAlgorithm class
 */
class PhotonFragmentRemovalAlgorithm : public pandora::Algorithm
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
    PhotonFragmentRemovalAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Get cluster contact map, linking each daughter candidate cluster to a list of parent candidates and describing
     *          the proximity/contact between each pairing
     * 
     *  @param  isFirstPass whether this is the first call to GetClusterContactMap
     *  @param  affectedClusters list of those clusters affected by previous cluster merging, for which contact details must be updated
     *  @param  clusterContactMap to receive the populated cluster contact map
     */
    pandora::StatusCode GetClusterContactMap(bool &isFirstPass, const pandora::ClusterList &affectedClusters, ClusterContactMap &clusterContactMap) const;

    /**
     *  @brief  Whether candidate daughter cluster can be considered as photon-like
     * 
     *  @param  pDaughterCluster address of the candidate daughter cluster
     * 
     *  @return boolean
     */
    bool IsPhotonLike(pandora::Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Whether candidate parent and daughter clusters are sufficiently in contact to warrant further investigation
     * 
     *  @param  clusterContact the cluster contact
     * 
     *  @return boolean
     */
    bool PassesClusterContactCuts(const ClusterContact &clusterContact) const;

    /**
     *  @brief  Find the best candidate parent and daughter clusters for fragment removal merging
     * 
     *  @param  clusterContactMap the populated cluster contact map
     *  @param  pBestParentCluster to receive the address of the best parent cluster candidate
     *  @param  pBestDaughterCluster to receive the address of the best daughter cluster candidate
     */
    pandora::StatusCode GetClusterMergingCandidates(const ClusterContactMap &clusterContactMap, pandora::Cluster *&pBestParentCluster,
        pandora::Cluster *&pBestDaughterCluster) const;

    /**
     *  @brief  Get a measure of the evidence for merging the parent and daughter candidate clusters
     * 
     *  @param  clusterContact the cluster contact details for parent/daughter candidate merge
     * 
     *  @return the evidence
     */
    float GetEvidenceForMerge(const ClusterContact &clusterContact) const;

    /**
     *  @brief  Get the list of clusters for which cluster contact information will be affected by a specified cluster merge
     * 
     *  @param  clusterContactMap the cluster contact map
     *  @param  pBestParentCluster address of the parent cluster to be merged
     *  @param  pBestDaughterCluster address of the daughter cluster to be merged
     *  @param  affectedClusters to receive the list of affected clusters
     */
    pandora::StatusCode GetAffectedClusters(const ClusterContactMap &clusterContactMap, pandora::Cluster *const pBestParentCluster,
        pandora::Cluster *const pBestDaughterCluster, pandora::ClusterList &affectedClusters) const;

    typedef ClusterContact::Parameters ContactParameters;
    ContactParameters   m_contactParameters;                        ///< The cluster contact parameters

    unsigned int        m_nMaxPasses;                               ///< Maximum number of passes over cluster contact information

    unsigned int        m_minDaughterCaloHits;                      ///< Min number of calo hits in daughter candidate clusters
    float               m_minDaughterHadronicEnergy;                ///< Min hadronic energy for daughter candidate clusters
    unsigned int        m_innerLayerTolerance;                      ///< Max number of layers by which daughter can exceed parent inner layer
    float               m_minCosOpeningAngle;                       ///< Min cos opening angle between candidate cluster initial directions

    bool                m_useOnlyPhotonLikeDaughters;               ///< Whether to skip photon-like checks for daughter cluster

    unsigned int        m_photonLikeMaxInnerLayer;                  ///< Max inner layer to identify daughter cluster as photon-like
    float               m_photonLikeMinDCosR;                       ///< Max radial direction cosine to identify daughter as photon-like
    float               m_photonLikeMaxShowerStart;                 ///< Max shower profile start to identify daughter as photon-like
    float               m_photonLikeMaxProfileDiscrepancy;          ///< Max shower profile discrepancy to identify daughter as photon-like

    float               m_contactCutMaxDistance;                    ///< Max distance between closest hits to store cluster contact info
    unsigned int        m_contactCutNLayers;                        ///< Number of contact layers to store cluster contact info
    float               m_contactCutConeFraction1;                  ///< Cone fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction1;              ///< Close hit fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction2;              ///< Close hit fraction 2 value to store cluster contact info

    unsigned int        m_contactEvidenceNLayers;                   ///< Contact layers required for contact evidence contribution
    float               m_contactEvidenceFraction;                  ///< Contact fraction required for contact evidence contribution
    float               m_coneEvidenceFraction1;                    ///< Cone fraction 1 value required for cone evidence contribution
    float               m_distanceEvidence1;                        ///< Offset for distance evidence contribution 1
    float               m_distanceEvidence1d;                       ///< Denominator for distance evidence contribution 1
    float               m_distanceEvidenceCloseFraction1Multiplier; ///< Distance evidence multiplier for close hit fraction 1
    float               m_distanceEvidenceCloseFraction2Multiplier; ///< Distance evidence multiplier for close hit fraction 2

    float               m_contactWeight;                            ///< Weight for layers in contact evidence
    float               m_coneWeight;                               ///< Weight for cone extrapolation evidence
    float               m_distanceWeight;                           ///< Weight for distance of closest approach evidence

    float               m_minEvidence;                              ///< Min evidence before parent/daughter candidates can be merged
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonFragmentRemovalAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_PHOTON_FRAGMENT_REMOVAL_ALGORITHM_H
