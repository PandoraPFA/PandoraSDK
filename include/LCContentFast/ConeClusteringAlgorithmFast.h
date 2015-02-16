/**
 *  @file   LCContent/include/LCContentFast/ConeClusteringAlgorithmFast.h
 * 
 *  @brief  Header file for the clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_CONE_CLUSTERING_ALGORITHM_FAST_H
#define LC_CONE_CLUSTERING_ALGORITHM_FAST_H 1

#include "Pandora/Algorithm.h"

#include "Helpers/ClusterFitHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/CartesianVector.h"

#include "LCContentFast/KDTreeLinkerAlgoT.h"

#include <unordered_map>

namespace lc_content_fast
{

/**
 *  @brief  CustomHitOrder class
 */
class CustomHitOrder
{
public:
    /**
     *  @brief  Operator () for determining custom calo hit ordering. Operator returns true if lhs hit is to be
     *          placed at an earlier position than rhs hit.
     * 
     *  @param  lhs calo hit for comparison
     *  @param  rhs calo hit for comparison
     */
    bool operator()(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const;
};

typedef std::vector<const pandora::CaloHit *> CustomSortedCaloHitList;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ConeClusteringAlgorithm class
 */
class ConeClusteringAlgorithm : public pandora::Algorithm
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
    ConeClusteringAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Fill the kd-trees we will use to do fast lookups of clusters
     * 
     *  @param  none, just internal initialization
     */
    pandora::StatusCode InitializeKDTrees(const pandora::TrackList*, const pandora::CaloHitList*);

    /**
     *  @brief  Use current track list to make seed clusters
     * 
     *  @param  pTrackList address of the track list
     *  @param  clusterVector to receive the addresses of clusters created (which could also be accessed via current cluster list)
     */
    pandora::StatusCode SeedClustersWithTracks(const pandora::TrackList *const pTrackList, pandora::ClusterVector &clusterVector);

    typedef std::map<const pandora::Cluster*, const pandora::ClusterFitResult> ClusterFitResultMap;
    typedef KDTreeLinkerAlgo<const pandora::CaloHit*, 4> HitKDTree;
    typedef KDTreeNodeInfoT<const pandora::CaloHit*, 4> HitKDNode;
    typedef KDTreeLinkerAlgo<const pandora::Track*, 3> TrackKDTree;
    typedef KDTreeNodeInfoT<const pandora::Track*, 3> TrackKDNode;

    /**
     *  @brief  Update the properties of the current clusters, calculating their current directions and storing the fit results
     *          in a provided map
     * 
     *  @param  clusterVector vector containing addresses of current clusters
     *  @param  clusterFitResultMap to receive the populated cluster fit result map
     */
    pandora::StatusCode GetCurrentClusterFitResults(const pandora::ClusterVector &clusterVector, ClusterFitResultMap &clusterFitResultMap) const;

    /**
     *  @brief  Match clusters to calo hits in previous pseudo layers
     * 
     *  @param  pseudoLayer the current pseudo layer
     *  @param  pCustomSortedCaloHitList address of the custom sorted list of calo hits in the current pseudo layer
     *  @param  clusterFitResultMap containing the current cluster fit results
     *  @param  clusterVector vector containing addresses of current clusters
     */
    pandora::StatusCode FindHitsInPreviousLayers(unsigned int pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
        const ClusterFitResultMap &clusterFitResultMap, pandora::ClusterVector &clusterVector);

    /**
     *  @brief  Match clusters to calo hits in current pseudo layer
     * 
     *  @param  pseudoLayer the current pseudo layer
     *  @param  pCustomSortedCaloHitList address of the custom sorted list of calo hits in the current pseudo layer
     *  @param  clusterFitResultMap containing the current cluster fit results
     *  @param  clusterVector vector containing addresses of current clusters
     */
    pandora::StatusCode FindHitsInSameLayer(unsigned int pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
        const ClusterFitResultMap &clusterFitResultMap, pandora::ClusterVector &clusterVector);

    /**
     *  @brief  Get the "generic distance" between a calo hit and a cluster; the smaller the distance, the stronger the association
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  searchLayer the pseudolayer currently being examined
     *  @param  clusterFitResultMap containing the current cluster fit results
     *  @param  genericDistance to receive the generic distance
     */
    pandora::StatusCode GetGenericDistanceToHit(const pandora::Cluster *const pCluster, const pandora::CaloHit *const pCaloHit,
        const unsigned int searchLayer, const ClusterFitResultMap &clusterFitResultMap, float &genericDistance) const;

    /**
     *  @brief  Get the generic distance between a calo hit and a cluster in the same pseudo layer
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  pCaloHitList address of the cluster's constituent hit list
     *  @param  distance to receive the distance
     */
    pandora::StatusCode GetDistanceToHitInSameLayer(const pandora::CaloHit *const pCaloHit, const pandora::CaloHitList *const pCaloHitList,
        float &distance) const;

    /**
     *  @brief  Get the smallest cone approach distance between a calo hit and all the hits in a cluster, using a specified
     *          measurement of the cluster direction
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  pCaloHitList
     *  @param  clusterDirection
     *  @param  distance to receive the generic distance
     */
    pandora::StatusCode GetConeApproachDistanceToHit(const pandora::CaloHit *const pCaloHit, const pandora::CaloHitList *const pCaloHitList,
        const pandora::CartesianVector &clusterDirection, float &distance) const;

    /**
     *  @brief  Get the cone approach distance between a calo hit and a specified point in the cluster, using a specified
     *          measurement of the cluster direction
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  clusterPosition the cluster position vector
     *  @param  clusterDirection the cluster direction unit vector
     *  @param  distance to receive the distance
     */
    pandora::StatusCode GetConeApproachDistanceToHit(const pandora::CaloHit *const pCaloHit, const pandora::CartesianVector &clusterPosition,
        const pandora::CartesianVector &clusterDirection, float &distance) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the calorimeter surface, ONLY calculated if:
     *          1) the calo hit is within the first m_maxLayersToTrackSeed OR
     *          2) cluster contains a hit consistent with the track seed within the last m_maxLayersToTrackLikeHit
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  the pseudolayer currently being examined
     *  @param  to receive the distance
     */
    pandora::StatusCode GetDistanceToTrackSeed(const pandora::Cluster *const pCluster, const pandora::CaloHit *const pCaloHit,
        unsigned int searchLayer, float &distance) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the calorimeter surface
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  distance to receive the distance
     */
    pandora::StatusCode GetDistanceToTrackSeed(const pandora::Cluster *const pCluster, const pandora::CaloHit *const pCaloHit, float &distance) const;

    /**
     *  @brief  Remove any empty clusters at the end of the algorithm
     * 
     *  @param  clusterVector containing the addresses of all clusters created
     */
    pandora::StatusCode RemoveEmptyClusters(const pandora::ClusterVector &clusterVector) const;

    /**
     *  @brief  kd-tree containing all tracks in given to the clusterizer
     */
    std::vector<TrackKDNode> m_trackNodes;
    TrackKDTree m_tracksKdTree;

    /**
     *  @brief  kd-tree containing all rechits given to the clusterizer
     */
    std::vector<HitKDNode> m_hitNodes;
    HitKDTree m_hitsKdTree;

    /**
     *  @brief  hashtable to look up hits in clusters
     */
    std::unordered_map<const pandora::CaloHit*, const pandora::Cluster*> m_hitsToClusters;

    /**
     *  @brief  hashtable to look up hits in clusters
     */
    std::unordered_map<const pandora::Track*, const pandora::Cluster*> m_tracksToClusters;

    unsigned int    m_clusterSeedStrategy;          ///< Flag determining if and how clusters should be seeded with tracks

    bool            m_shouldUseOnlyECalHits;        ///< Whether to only use ecal hits in the clustering algorithm
    bool            m_shouldUseIsolatedHits;        ///< Whether to use isolated hits in the clustering algorithm

    unsigned int    m_layersToStepBackFine;         ///< Number of layers to step back when associating a fine granularity hit to clusters
    unsigned int    m_layersToStepBackCoarse;       ///< Number of layers to step back when associating a coarse granularity hit to clusters

    unsigned int    m_clusterFormationStrategy;     ///< Flag determining when to add "best" calo hit found to clusters
    float           m_genericDistanceCut;           ///< Generic distance cut to apply throughout algorithm

    float           m_minHitTrackCosAngle;          ///< Min cos(angle) between calo hit position and track projection
    float           m_minHitClusterCosAngle;        ///< Min cos(angle) between calo hit position and cluster searchlayer centroid

    bool            m_shouldUseTrackSeed;           ///< Whether to use track seed information in clustering
    unsigned int    m_trackSeedCutOffLayer;         ///< Pseudo layer beyond which track seed is no longer considered
    bool            m_shouldFollowInitialDirection; ///< Whether the cluster should grow preferentially in its initial direction

    float           m_sameLayerPadWidthsFine;       ///< Fine adjacent pad widths used to calculate generic distance to same layer hit
    float           m_sameLayerPadWidthsCoarse;     ///< Coarse adjacent pad widths used to calculate generic distance to same layer hit

    float           m_coneApproachMaxSeparation2;   ///< Maximum separation between calo hit and specified cluster position (squared)
    float           m_tanConeAngleFine;             ///< Fine tan cone angle used to calculate cone approach distance
    float           m_tanConeAngleCoarse;           ///< Coarse tan cone angle used to calculate cone approach distance
    float           m_additionalPadWidthsFine;      ///< Fine adjacent pad widths used to calculate cone approach distance
    float           m_additionalPadWidthsCoarse;    ///< Coarse adjacent pad widths used to calculate cone approach distance
    float           m_maxClusterDirProjection;      ///< Max projection of cluster-hit separation in cluster dir to calculate cone distance
    float           m_minClusterDirProjection;      ///< Min projection of cluster-hit separation in cluster dir to calculate cone distance

    float           m_trackPathWidth;               ///< Track path width, used to determine whether hits are associated with seed track
    float           m_maxTrackSeedSeparation2;      ///< Maximum distance between a calo hit and track seed (squared)

    unsigned int    m_maxLayersToTrackSeed;         ///< Max number of layers to the track seed for trackSeedDistance to be calculated
    unsigned int    m_maxLayersToTrackLikeHit;      ///< Max number of layers to a "track-like" hit for trackSeedDistance to be calculated

    unsigned int    m_nLayersSpannedForFit;         ///< Min number of layers spanned by cluster before current direction fit is performed
    unsigned int    m_nLayersSpannedForApproxFit;   ///< If current fit fails, but cluster spans enough layers, calculate an approximate fit
    unsigned int    m_nLayersToFit;                 ///< Number of layers (from outermost layer) to examine in current direction fit
    float           m_nLayersToFitLowMipCut;        ///< Low cluster mip fraction threshold, below which may need to fit more layers
    unsigned int    m_nLayersToFitLowMipMultiplier; ///< Number of layers to fit multiplier for low mip fraction clusters

    float           m_fitSuccessDotProductCut1;     ///< 1. Min value of dot product (current fit dir dot initial dir) for fit success
    float           m_fitSuccessChi2Cut1;           ///< 1. Max value of fit chi2 for fit success

    float           m_fitSuccessDotProductCut2;     ///< 2. Min value of dot product (current fit dir dot initial dir) for fit success
    float           m_fitSuccessChi2Cut2;           ///< 2. Max value of fit chi2 for fit success

    float           m_mipTrackChi2Cut;              ///< Max value of fit chi2 for track seeded cluster to retain its IsMipTrack status

    unsigned int    m_firstLayer;                   ///< cache the pseudo layer at IP
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CustomHitOrder::operator()(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const
{
    return (!(lhs->GetInputEnergy() > rhs->GetInputEnergy()) && !(rhs->GetInputEnergy() > lhs->GetInputEnergy()) ?
        (lhs > rhs) :
        (lhs->GetInputEnergy() > rhs->GetInputEnergy()));
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ConeClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ConeClusteringAlgorithm();
}

} // namespace lc_content_fast

#endif // #ifndef LC_CONE_CLUSTERING_ALGORITHM_FAST_H
