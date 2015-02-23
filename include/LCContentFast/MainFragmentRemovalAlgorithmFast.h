/**
 *  @file   LCContent/include/LCFragmentRemoval/MainFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the main fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_MAIN_FRAGMENT_REMOVAL_ALGORITHM_FAST_H
#define LC_MAIN_FRAGMENT_REMOVAL_ALGORITHM_FAST_H 1

#include "Pandora/Algorithm.h"

#include "LCContentFast/FragmentRemovalHelperFast.h"

#include "LCContentFast/KDTreeLinkerAlgoT.h"
#include <unordered_map>

namespace lc_content_fast
{  
  

typedef KDTreeLinkerAlgo<const pandora::CaloHit*,3> HitKDTree;
typedef KDTreeNodeInfoT<const pandora::CaloHit*,3> HitKDNode;
typedef KDTreeLinkerAlgo<unsigned,3> HitKDTreeByIndex;
typedef KDTreeNodeInfoT<unsigned,3> HitKDNodeByIndex;
typedef std::unordered_map<const pandora::Cluster*,const pandora::Cluster*> ClusterToClusterMap; // maps the beginning cluster list to the final
typedef std::unordered_map<const pandora::CaloHit*,const pandora::Cluster*> HitsToClustersMap; // note that this map is used indirected
typedef std::unordered_map<const pandora::Cluster*,pandora::ClusterList> ClusterToNeighbourClustersMap;
typedef std::unordered_map<const pandora::Cluster*,std::unique_ptr<HitKDTree> > ClusterToKDTreeMap;

/**
 *  @brief  ChargedClusterContact class, describing the interactions and proximity between parent and daughter candidate clusters
 */
class ChargedClusterContact : public ClusterContact
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
        float           m_helixComparisonMipFractionCut;///< Mip fraction cut used in cluster contact helix comparison
        unsigned int    m_helixComparisonStartOffset;   ///< Start layer offset used in cluster contact helix comparison
        unsigned int    m_helixComparisonStartOffsetMip;///< Start layer offset used for mip-like clusters in helix comparison
        unsigned int    m_nHelixComparisonLayers;       ///< Max number of layers used in helix comparison for non mip-like clusters
        unsigned int    m_maxLayersCrossedByHelix;      ///< Max no. of layers crossed by helix between track projection and cluster
        float           m_maxTrackClusterDeltaZ;        ///< Max z separation between track projection and cluster
    };

    /**
     *  @brief  Constructor
     * 
     *  @param  pandora the associated pandora instance
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  pParentCluster address of the parent candidate cluster
     *  @param  parameters the cluster contact parameters
     */
    ChargedClusterContact(const pandora::Pandora &pandora, const pandora::Cluster *const pDaughterCluster, const pandora::Cluster *const pParentCluster,
        const Parameters &parameters);

    ChargedClusterContact(const pandora::Pandora &pandora, const pandora::Cluster *const pDaughterCluster, const pandora::Cluster *const pParentCluster,
        const Parameters &parameters, const std::unique_ptr<HitKDTree>&);

    /**
     *  @brief  Get the sum of energies of tracks associated with parent cluster
     * 
     *  @return The sum of energies of tracks associated with parent cluster
     */
    float GetParentTrackEnergy() const;

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

    /**
     *  @brief  Get the mean distance of daughter cluster from closest helix fix to parent associated tracks
     * 
     *  @return The mean daughter distance to the closest helix
     */
    float GetMeanDistanceToHelix() const;

    /**
     *  @brief  Get the closest distance between daughter cluster and helix fits to parent associated tracks
     * 
     *  @return The closest daughter distance to helix
     */
    float GetClosestDistanceToHelix() const;

private:
    /**
     *  @brief  Compare daughter cluster with helix fits to parent associated tracks
     * 
     *  @param  pandora the associated pandora instance
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  pParentCluster address of the parent candidate cluster
     *  @param  parameters the cluster contact parameters
     */
    void ClusterHelixComparison(const pandora::Pandora &pandora, const pandora::Cluster *const pDaughterCluster, const pandora::Cluster *const pParentCluster,
        const Parameters &parameters);

    float               m_parentTrackEnergy;            ///< Sum of energies of tracks associated with parent cluster
    float               m_coneFraction2;                ///< Fraction of daughter hits that lie within specified cone 2 along parent direction
    float               m_coneFraction3;                ///< Fraction of daughter hits that lie within specified cone 3 along parent direction
    float               m_meanDistanceToHelix;          ///< Mean distance of daughter cluster from closest helix fit to parent associated tracks
    float               m_closestDistanceToHelix;       ///< Closest distance between daughter cluster and helix fits to parent associated tracks
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

typedef std::vector<ChargedClusterContact> ChargedClusterContactVector;
typedef std::unordered_map<const pandora::Cluster *, ChargedClusterContactVector> ChargedClusterContactMap;

/**
 *  @brief  MainFragmentRemovalAlgorithm class
 */
class MainFragmentRemovalAlgorithm : public pandora::Algorithm
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
    MainFragmentRemovalAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Get cluster contact map, linking each daughter candidate cluster to a list of parent candidates and describing
     *          the proximity/contact between each pairing
     * 
     *  @param  isFirstPass whether this is the first call to GetChargedClusterContactMap
     *  @param  affectedClusters list of those clusters affected by previous cluster merging, for which contact details must be updated
     *  @param  chargedClusterContactMap to receive the populated cluster contact map
     *  @param  tree, the kd-tree of rechits so we can quickly find the NNs
     *  @param  hits_to_clusters, the map of hits back to their containing clusters (indirected as clusters are merged)
     *  @param  clus_to_clus, the indirection map
     *  @param  neighbours_cache, the map of clusters to all neighbouring clusters
     */
    pandora::StatusCode GetChargedClusterContactMap(bool &isFirstPass, const pandora::ClusterList &affectedClusters,
						    ChargedClusterContactMap &chargedClusterContactMap, 
						    const ClusterToClusterMap& clusters_to_clusters,
						    const ClusterToNeighbourClustersMap& neighbours_cache,
						    const ClusterToKDTreeMap& cluster_to_tree) const;

    /**
     *  @brief  Whether candidate parent and daughter clusters are sufficiently in contact to warrant further investigation
     * 
     *  @param  chargedClusterContact the cluster contact
     * 
     *  @return boolean
     */
    bool PassesClusterContactCuts(const ChargedClusterContact &chargedClusterContact) const;

    /**
     *  @brief  Find the best candidate parent and daughter clusters for fragment removal merging
     * 
     *  @param  chargedClusterContactMap the populated cluster contact map
     *  @param  pBestParentCluster to receive the address of the best parent cluster candidate
     *  @param  pBestDaughterCluster to receive the address of the best daughter cluster candidate
     */
    pandora::StatusCode GetClusterMergingCandidates(const ChargedClusterContactMap &chargedClusterContactMap, const pandora::Cluster *&pBestParentCluster,
        const pandora::Cluster *&pBestDaughterCluster);

    /**
     *  @brief  Whether the candidate parent and daughter clusters pass quick preselection for fragment removal merging
     * 
     *  @param  pDaughterCluster address of the daughter cluster candidate
     *  @param  chargedClusterContactVector list cluster contact details for the given daughter cluster
     *  @param  globalDeltaChi2 to receive global delta chi2, indicating whether daughter cluster would be better merged
     *          with group of all nearby clusters
     * 
     *  @return boolean
     */
    bool PassesPreselection(const pandora::Cluster *const pDaughterCluster, const ChargedClusterContactVector &chargedClusterContactVector,
        float &globalDeltaChi2) const;

    /**
     *  @brief  Get a measure of the total evidence for merging the parent and daughter candidate clusters
     * 
     *  @param  chargedClusterContact the cluster contact details for parent/daughter candidate merge
     * 
     *  @return the total evidence
     */
    float GetTotalEvidenceForMerge(const ChargedClusterContact &chargedClusterContact) const;

    /**
     *  @brief  Get the required evidence for merging the parent and daughter candidate clusters
     * 
     *  @param  pDaughterCluster address of the daughter cluster candidate
     *  @param  chargedClusterContact the cluster contact details for parent/daughter candidate merge
     *  @param  correctionLayer the daughter cluster correction layer
     *  @param  globalDeltaChi2 global delta chi2, indicating whether daughter cluster would be better merged
     *          with group of all nearby clusters
     * 
     *  @return the required evidence
     */
    float GetRequiredEvidenceForMerge(const pandora::Cluster *const pDaughterCluster, const ChargedClusterContact &chargedClusterContact,
        const unsigned int correctionLayer, const float globalDeltaChi2);

    /**
     *  @brief  Get the cluster correction layer. Working from innermost to outermost layer, the correction layer is that in which:
     *          i)  the number of hit-layers passes m_correctionLayerNHitLayers, or
     *          ii) the total calo hit hadronic energy passes (m_correctionLayerEnergyFraction * cluster energy)
     * 
     *  @param  pDaughterCluster address of the daughter cluster candidate
     * 
     *  @return the cluster correction layer
     */
    unsigned int GetClusterCorrectionLayer(const pandora::Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Get the list of clusters for which cluster contact information will be affected by a specified cluster merge
     * 
     *  @param  chargedClusterContactMap the cluster contact map
     *  @param  pBestParentCluster address of the parent cluster to be merged
     *  @param  pBestDaughterCluster address of the daughter cluster to be merged
     *  @param  affectedClusters to receive the list of affected clusters
     */
    pandora::StatusCode GetAffectedClusters(const ChargedClusterContactMap &chargedClusterContactMap, const pandora::Cluster *const pBestParentCluster,
        const pandora::Cluster *const pBestDaughterCluster, pandora::ClusterList &affectedClusters) const;

    typedef ChargedClusterContact::Parameters ContactParameters;
    ContactParameters   m_contactParameters;                        ///< The charged cluster contact parameters

    unsigned int        m_minDaughterCaloHits;                      ///< Min number of calo hits in daughter candidate clusters
    float               m_minDaughterHadronicEnergy;                ///< Min hadronic energy for daughter candidate clusters

    float               m_contactCutMaxDistance;                    ///< Max distance between closest hits to store cluster contact info
    unsigned int        m_contactCutNLayers;                        ///< Number of contact layers to store cluster contact info
    float               m_contactCutConeFraction1;                  ///< Cone fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction1;              ///< Close hit fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction2;              ///< Close hit fraction 2 value to store cluster contact info
    float               m_contactCutMeanDistanceToHelix;            ///< Mean distance to helix value to store cluster contact info
    float               m_contactCutClosestDistanceToHelix;         ///< Closest distance to helix value to store cluster contact info
    float               m_contactCutMaxHitDistance;                 ///< Hit separation to store contact info
    unsigned int        m_contactCutMinDaughterInnerLayer;          ///< Min daughter cluster inner layer to store contact info

    float               m_maxChi2;                                  ///< Pre-selection: new chi2 value to allow cluster merging
    float               m_maxGlobalChi2;                            ///< Pre-selection: new global chi2 value to allow cluster merging
    float               m_chi2Base;                                 ///< Required evidence: min contribution from change in chi2
    float               m_globalChi2Penalty;                        ///< Required evidence penalty for using global chi2 measure

    unsigned int        m_correctionLayerNHitLayers;                ///< Number of hit-layers passed to identify correction layer
    float               m_correctionLayerEnergyFraction;            ///< Fraction of total hadronic energy passed to identify correction layer

    unsigned int        m_contactEvidenceNLayers1;                  ///< Contact evidence n layers cut 1
    unsigned int        m_contactEvidenceNLayers2;                  ///< Contact evidence n layers cut 2
    unsigned int        m_contactEvidenceNLayers3;                  ///< Contact evidence n layers cut 3
    float               m_contactEvidence1;                         ///< Contact evidence contribution 1
    float               m_contactEvidence2;                         ///< Contact evidence contribution 2
    float               m_contactEvidence3;                         ///< Contact evidence contribution 3

    float               m_coneEvidenceFraction1;                    ///< Cone fraction 1 value required for cone evidence contribution
    float               m_coneEvidenceFineGranularityMultiplier;    ///< Cone evidence multiplier for fine granularity daughter clusters

    float               m_closestTrackEvidence1;                    ///< Offset for closest distance to helix evidence contribution 1
    float               m_closestTrackEvidence1d;                   ///< Denominator for closest distance to helix evidence contribution 1
    float               m_closestTrackEvidence2;                    ///< Offset for closest distance to helix evidence contribution 2
    float               m_closestTrackEvidence2d;                   ///< Denominator for closest distance to helix evidence contribution 2
    float               m_meanTrackEvidence1;                       ///< Offset for mean distance to helix evidence contribution 1
    float               m_meanTrackEvidence1d;                      ///< Denominator for mean distance to helix evidence contribution 1
    float               m_meanTrackEvidence2;                       ///< Offset for mean distance to helix evidence contribution 2
    float               m_meanTrackEvidence2d;                      ///< Denominator for mean distance to helix evidence contribution 1

    float               m_distanceEvidence1;                        ///< Offset for distance evidence contribution 1
    float               m_distanceEvidence1d;                       ///< Denominator for distance evidence contribution 1
    float               m_distanceEvidenceCloseFraction1Multiplier; ///< Distance evidence multiplier for close hit fraction 1
    float               m_distanceEvidenceCloseFraction2Multiplier; ///< Distance evidence multiplier for close hit fraction 2

    float               m_contactWeight;                            ///< Weight for layers in contact evidence
    float               m_coneWeight;                               ///< Weight for cone extrapolation evidence
    float               m_distanceWeight;                           ///< Weight for distance of closest approach evidence
    float               m_trackExtrapolationWeight;                 ///< Weight for track extrapolation evidence

    unsigned int        m_layerCorrectionLayerValue1;               ///< Max value of correction layer for layer correction contribution 1
    unsigned int        m_layerCorrectionLayerValue2;               ///< Max value of correction layer for layer correction contribution 2
    unsigned int        m_layerCorrectionLayerValue3;               ///< Max value of correction layer for layer correction contribution 3
    float               m_layerCorrection1;                         ///< Layer correction contribution 1
    float               m_layerCorrection2;                         ///< Layer correction contribution 2
    float               m_layerCorrection3;                         ///< Layer correction contribution 3
    float               m_layerCorrection4;                         ///< Layer correction contribution 4, applied if 1,2,3 not applicable

    float               m_layerCorrectionLayerSpan;                 ///< Daughter layer span for layer correction contribution 5
    float               m_layerCorrectionMinInnerLayer;             ///< Daughter min inner layer for layer correction contribution 5
    float               m_layerCorrection5;                         ///< Layer correction contribution 5

    float               m_leavingCorrection;                        ///< Correction for clusters leaving calorimeters

    float               m_energyCorrectionThreshold;                ///< Energy correction threshold

    float               m_lowEnergyCorrectionThreshold;             ///< Low energy correction threshold
    unsigned int        m_lowEnergyCorrectionNHitLayers1;           ///< Number of hit layers below which to apply contribution 1
    unsigned int        m_lowEnergyCorrectionNHitLayers2;           ///< Number of hit layers below which to apply contribution 2   
    unsigned int        m_lowEnergyCorrectionNHitLayers3;           ///< Number of hit layers above which to apply contribution 3
    float               m_lowEnergyCorrection1;                     ///< Low energy correction contribution 1
    float               m_lowEnergyCorrection2;                     ///< Low energy correction contribution 2
    float               m_lowEnergyCorrection3;                     ///< Low energy correction contribution 3

    float               m_angularCorrectionOffset;                  ///< Offset value for angular correction
    float               m_angularCorrectionConstant;                ///< Constant value for angular correction
    float               m_angularCorrectionGradient;                ///< Gradient value for angular correction

    float               m_photonCorrectionEnergy1;                  ///< Photon correction energy value 1
    float               m_photonCorrectionEnergy2;                  ///< Photon correction energy value 2
    float               m_photonCorrectionEnergy3;                  ///< Photon correction energy value 3
    float               m_photonCorrectionShowerStart1;             ///< Photon correction profile shower start value 1
    float               m_photonCorrectionShowerStart2;             ///< Photon correction profile shower start value 1
    float               m_photonCorrectionShowerDiscrepancy1;       ///< Photon correction profile discrepancy value 1
    float               m_photonCorrectionShowerDiscrepancy2;       ///< Photon correction profile discrepancy value 2
    float               m_photonCorrection1;                        ///< Photon correction contribution 1
    float               m_photonCorrection2;                        ///< Photon correction contribution 2
    float               m_photonCorrection3;                        ///< Photon correction contribution 3
    float               m_photonCorrection4;                        ///< Photon correction contribution 4
    float               m_photonCorrection5;                        ///< Photon correction contribution 5
    float               m_photonCorrection6;                        ///< Photon correction contribution 6
    float               m_photonCorrection7;                        ///< Photon correction contribution 7

    float               m_minRequiredEvidence;                      ///< Minimum required evidence to merge parent/daughter clusters
    float               m_minimalSearchRadius;                      ///< Search radius to do the nn search in, presently defined in constructor
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ChargedClusterContact::GetParentTrackEnergy() const
{
    return m_parentTrackEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ChargedClusterContact::GetConeFraction2() const
{
    return m_coneFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ChargedClusterContact::GetConeFraction3() const
{
    return m_coneFraction3;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ChargedClusterContact::GetMeanDistanceToHelix() const
{
    return m_meanDistanceToHelix;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ChargedClusterContact::GetClosestDistanceToHelix() const
{
    return m_closestDistanceToHelix;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MainFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new MainFragmentRemovalAlgorithm();
}

} // namespace lc_content_fast

#endif // #ifndef LC_FRAGMENT_REMOVAL_ALGORITHM_H
