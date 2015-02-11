/**
 *  @file   LCContent/include/LCFragmentRemoval/NeutralFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the neutral fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H
#define LC_NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "LCHelpers/FragmentRemovalHelper.h"

namespace lc_content
{

/**
 *  @brief  NeutralClusterContact class, describing the interactions and proximity between parent and daughter candidate clusters
 */
class NeutralClusterContact : public ClusterContact
{
public:
    /**
     *  @brief  Parameters class
     */
    class Parameters : public ClusterContact::Parameters
    {
    public:
        float           m_coneCosineHalfAngle2;         ///< Cosine half angle for second cone comparison in cluster contact object
        float           m_coneCosineHalfAngle3;         ///< Cosine half angle for third cone comparison in cluster contact object
    };

    /**
     *  @brief  Constructor
     * 
     *  @param  pandora the associated pandora instance
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  pParentCluster address of the parent candidate cluster
     *  @param  parameters the cluster contact parameters
     */
    NeutralClusterContact(const pandora::Pandora &pandora, const pandora::Cluster *const pDaughterCluster, const pandora::Cluster *const pParentCluster,
        const Parameters &parameters);

    /**
     *  @brief  Get the fraction of daughter hits that lie within specified cone 2 along parent direction
     * 
     *  @return The daughter cone fraction
     */
    float GetConeFraction2() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within specified cone 3 along parent direction
     * 
     *  @return The daughter cone fraction
     */
    float GetConeFraction3() const;

private:
    float               m_coneFraction2;                ///< Fraction of daughter hits that lie within specified cone 2 along parent direction
    float               m_coneFraction3;                ///< Fraction of daughter hits that lie within specified cone 3 along parent direction
};

typedef std::vector<NeutralClusterContact> NeutralClusterContactVector;
typedef std::map<const pandora::Cluster *, NeutralClusterContactVector> NeutralClusterContactMap;

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  NeutralFragmentRemovalAlgorithm class
 */
class NeutralFragmentRemovalAlgorithm : public pandora::Algorithm
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
    NeutralFragmentRemovalAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Get cluster contact map, linking each daughter candidate cluster to a list of parent candidates and describing
     *          the proximity/contact between each pairing
     * 
     *  @param  isFirstPass whether this is the first call to GetNeutralClusterContactMap
     *  @param  affectedClusters list of those clusters affected by previous cluster merging, for which contact details must be updated
     *  @param  neutralClusterContactMap to receive the populated cluster contact map
     */
    pandora::StatusCode GetNeutralClusterContactMap(bool &isFirstPass, const pandora::ClusterList &affectedClusters,
        NeutralClusterContactMap &neutralClusterContactMap) const;

    /**
     *  @brief  Whether candidate daughter cluster can be considered as photon-like
     * 
     *  @param  pDaughterCluster address of the candidate daughter cluster
     * 
     *  @return boolean
     */
    bool IsPhotonLike(const pandora::Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Whether candidate parent and daughter clusters are sufficiently in contact to warrant further investigation
     * 
     *  @param  neutralClusterContact the cluster contact
     * 
     *  @return boolean
     */
    bool PassesClusterContactCuts(const NeutralClusterContact &neutralClusterContact) const;

    /**
     *  @brief  Find the best candidate parent and daughter clusters for fragment removal merging
     * 
     *  @param  neutralClusterContactMap the populated cluster contact map
     *  @param  pBestParentCluster to receive the address of the best parent cluster candidate
     *  @param  pBestDaughterCluster to receive the address of the best daughter cluster candidate
     */
    pandora::StatusCode GetClusterMergingCandidates(const NeutralClusterContactMap &neutralClusterContactMap, const pandora::Cluster *&pBestParentCluster,
        const pandora::Cluster *&pBestDaughterCluster) const;

    /**
     *  @brief  Get a measure of the evidence for merging the parent and daughter candidate clusters
     * 
     *  @param  neutralClusterContact the cluster contact details for parent/daughter candidate merge
     * 
     *  @return the evidence
     */
    float GetEvidenceForMerge(const NeutralClusterContact &neutralClusterContact) const;

    /**
     *  @brief  Get the list of clusters for which cluster contact information will be affected by a specified cluster merge
     * 
     *  @param  neutralClusterContactMap the cluster contact map
     *  @param  pBestParentCluster address of the parent cluster to be merged
     *  @param  pBestDaughterCluster address of the daughter cluster to be merged
     *  @param  affectedClusters to receive the list of affected clusters
     */
    pandora::StatusCode GetAffectedClusters(const NeutralClusterContactMap &neutralClusterContactMap, const pandora::Cluster *const pBestParentCluster,
        const pandora::Cluster *const pBestDaughterCluster, pandora::ClusterList &affectedClusters) const;

    typedef NeutralClusterContact::Parameters ContactParameters;
    ContactParameters   m_contactParameters;                        ///< The neutral cluster contact parameters

    unsigned int        m_nMaxPasses;                               ///< Maximum number of passes over cluster contact information

    unsigned int        m_minDaughterCaloHits;                      ///< Min number of calo hits in daughter candidate clusters
    float               m_minDaughterHadronicEnergy;                ///< Min hadronic energy for daughter candidate clusters

    unsigned int        m_photonLikeMaxInnerLayer;                  ///< Max inner layer to identify daughter cluster as photon-like
    float               m_photonLikeMinDCosR;                       ///< Max radial direction cosine to identify daughter as photon-like
    float               m_photonLikeMaxShowerStart;                 ///< Max shower profile start to identify daughter as photon-like
    float               m_photonLikeMaxProfileDiscrepancy;          ///< Max shower profile discrepancy to identify daughter as photon-like

    float               m_contactCutMaxDistance;                    ///< Max distance between closest hits to store cluster contact info
    unsigned int        m_contactCutNLayers;                        ///< Number of contact layers to store cluster contact info
    float               m_contactCutConeFraction1;                  ///< Cone fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction1;              ///< Close hit fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction2;              ///< Close hit fraction 2 value to store cluster contact info
    float               m_contactCutNearbyDistance;                 ///< Distance between closest hits to mark clusters as nearby
    float               m_contactCutNearbyCloseHitFraction2;        ///< Close hit fraction 2 in nearby hits to store cluster contact info

    unsigned int        m_contactEvidenceNLayers1;                  ///< Contact evidence n layers cut 1
    unsigned int        m_contactEvidenceNLayers2;                  ///< Contact evidence n layers cut 2
    unsigned int        m_contactEvidenceNLayers3;                  ///< Contact evidence n layers cut 3
    float               m_contactEvidence1;                         ///< Contact evidence contribution 1
    float               m_contactEvidence2;                         ///< Contact evidence contribution 2
    float               m_contactEvidence3;                         ///< Contact evidence contribution 3

    float               m_coneEvidenceFraction1;                    ///< Cone fraction 1 value required for cone evidence contribution
    float               m_coneEvidenceFineGranularityMultiplier;    ///< Cone evidence multiplier for fine granularity daughter clusters

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

inline float NeutralClusterContact::GetConeFraction2() const
{
    return m_coneFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float NeutralClusterContact::GetConeFraction3() const
{
    return m_coneFraction3;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *NeutralFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new NeutralFragmentRemovalAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H
