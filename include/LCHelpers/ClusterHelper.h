/**
 *  @file   LCContent/include/LCHelpers/ClusterHelper.h
 * 
 *  @brief  Header file for the cluster helper class.
 * 
 *  $Log: $
 */
#ifndef LC_CLUSTER_HELPER_H
#define LC_CLUSTER_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace pandora { class ClusterFitResult; }

//------------------------------------------------------------------------------------------------------------------------------------------

namespace lc_content
{

/**
 *  @brief  ClusterHelper class
 */
class ClusterHelper
{
public:
    /**
     *  @brief  Get the closest distance of approach between two cluster fit results
     * 
     *  @param  lhs first cluster fit result
     *  @param  rhs second cluster fit result
     *  @param  closestApproach to receive the closest distance of approach
     */
    static pandora::StatusCode GetFitResultsClosestApproach(const pandora::ClusterFitResult &lhs, const pandora::ClusterFitResult &rhs,
        float &closestApproach);

    /**
     *  @brief  Get closest distance of approach between projected cluster fit result and hits in a second cluster
     * 
     *  @param  clusterFitResult the fit result to the first cluster
     *  @param  pCluster address of the second cluster
     *  @param  startLayer first layer to examine
     *  @param  endLayer last layer to examine
     * 
     *  @return the closest distance of approach
     */
    static float GetDistanceToClosestHit(const pandora::ClusterFitResult &clusterFitResult, const pandora::Cluster *const pCluster,
        const unsigned int startLayer, const unsigned int endLayer);

    /**
     *  @brief  Get smallest distance between pairs of hits in two clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     * 
     *  @return the smallest distance
     */
    static float GetDistanceToClosestHit(const pandora::Cluster *const pClusterI, const pandora::Cluster *const pClusterJ);

    /**
     *  @brief  Get closest distance of approach between projected cluster fit result and layer centroid position of a second cluster
     * 
     *  @param  clusterFitResult the fit result to the first cluster
     *  @param  pCluster address of the second cluster
     *  @param  startLayer first layer to examine
     *  @param  endLayer last layer to examine
     * 
     *  @return the closest distance of approach
     */
    static float GetDistanceToClosestCentroid(const pandora::ClusterFitResult &clusterFitResult, const pandora::Cluster *const pCluster,
        const unsigned int startLayer, const unsigned int endLayer);

    /**
     *  @brief  Get the closest distance between layer centroid positions in two overlapping clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     *  @param  intraLayerDistance to receive the closest centroid distance
     */
    static pandora::StatusCode GetDistanceToClosestCentroid(const pandora::Cluster *const pClusterI, const pandora::Cluster *const pClusterJ,
        float &centroidDistance);

    /**
     *  @brief  Get the closest distance between same layer centroid positions in two overlapping clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     *  @param  intraLayerDistance to receive the closest intra layer distance
     */
    static pandora::StatusCode GetClosestIntraLayerDistance(const pandora::Cluster *const pClusterI, const pandora::Cluster *const pClusterJ,
        float &intraLayerDistance);

    /**
     *  @brief  Get the distance of closest approach between the projected track direction at calorimeter and the hits within a cluster.
     *          Note that only a specified number of layers are examined.
     * 
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     *  @param  maxSearchLayer the maximum pseudolayer to examine
     *  @param  parallelDistanceCut maximum allowed projection of track-cluster separation along track direction
     *  @param  minTrackClusterCosAngle min cos(angle) between track and cluster initial direction
     *  @param  trackClusterDistance to receive the track cluster distance
     */
    static pandora::StatusCode GetTrackClusterDistance(const pandora::Track *const pTrack, const pandora::Cluster *const pCluster,
        const unsigned int maxSearchLayer, const float parallelDistanceCut, const float minTrackClusterCosAngle, float &trackClusterDistance);

    /**
     *  @brief  Whether a cluster can be merged with another. Uses simple suggested criteria, including cluster photon id flag
     *          and supplied cuts on cluster mip fraction and all hits fit rms.
     * 
     *  @param  pandora the associated pandora instance
     *  @param  pCluster address of the cluster
     *  @param  minMipFraction the minimum mip fraction for clusters (flagged as photons) to be merged
     *  @param  maxAllHitsFitRms the maximum all hit fit rms for clusters (flagged as photons) to be merged
     * 
     *  @return boolean
     */
    static bool CanMergeCluster(const pandora::Pandora &pandora, const pandora::Cluster *const pCluster, const float minMipFraction,
        const float maxAllHitsFitRms);

    /**
     *  @brief  Get the energy-weighted mean time of the non-isolated hits in a specified cluster, units ns
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return The energy-weighted mean time, units ns
     */
    static float GetEnergyWeightedMeanTime(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Whether a linear fit to a cluster crosses a registered gap region. Only the region between the startlayer and
     *          endlayer is considered in the fit and in the comparison with registered gap regions.
     * 
     *  @param  pandora the associated pandora instance
     *  @param  pCluster address of the cluster
     *  @param  startLayer the start layer (adjusted to maximum of specified layer and cluster inner layer)
     *  @param  endLayer the end layer (adjusted to minimum of specified layer and cluster outer layer)
     *  @param  nSamplingPoints number of points at which to sample the fit within the specified layer region
     * 
     *  @return boolean
     */
    static bool DoesClusterCrossGapRegion(const pandora::Pandora &pandora, const pandora::Cluster *const pCluster, const unsigned int startLayer,
        const unsigned int endLayer, const unsigned int nSamplingPoints = 50);

    /**
     *  @brief  Whether a linear fit crosses a registered gap region. The fit will be propagated through the specified distance
     *          from its closest approach to the startPosition. Within this propagation, the fit will be sampled a specified number
     *          of times and the resulting position compared with registered gap regions.
     * 
     *  @param  pandora the associated pandora instance
     *  @param  clusterFitResult the clsuter fit result
     *  @param  startPosition the propagation start position (adjusted to closest point on fit trajectory)
     *  @param  propagationDistance the propagation distance, which can be negative for propagation towards the ip
     *  @param  nSamplingPoints number of points at which to sample the fit within its propagation
     * 
     *  @return boolean
     */
    static bool DoesFitCrossGapRegion(const pandora::Pandora &pandora, const pandora::ClusterFitResult &clusterFitResult,
        const pandora::CartesianVector &startPosition, const float propagationDistance, const unsigned int nSamplingPoints = 50);

    /**
     *  @brief  Whether a cluster should be considered as leaving the calorimeters, leading to leakage of its energy
     * 
     *  @param  pCluster address of the cluster
     *  @param  nOuterLayersToExamine number of outer layers to examine in order to identify a leaving cluster
     *  @param  nMipLikeNOccupiedLayers number of occupied outer layers for a mip-like leaving cluster
     *  @param  nShowerLikeNOccupiedLayers number of occupied outer layers for a shower-like leaving cluster
     *  @param  showerLikeEnergyInOuterLayers energy deposited in outer layers by a shower-like leaving cluster
     * 
     *  @return boolean
     */
    static bool IsClusterLeavingDetector(const pandora::Cluster *const pCluster, const unsigned int nOuterLayersToExamine = 4,
        const unsigned int nMipLikeOccupiedLayers = 4, const unsigned int nShowerLikeOccupiedLayers = 3,
        const float showerLikeEnergyInOuterLayers = 1.f);

    /**
     *  @brief  Whether a cluster contains a calo hit of the specified hit type
     * 
     *  @param  pCluster address of the cluster
     *  @param  hitType the hit type
     * 
     *  @return boolean
     */
    static bool ContainsHitType(const pandora::Cluster *const pCluster, const pandora::HitType hitType);

    /**
     *  @brief  Whether a cluster contains a calo hit flagged as being in an outer sampling layer
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool ContainsHitInOuterSamplingLayer(const pandora::Cluster *const pCluster);
};

} // namespace lc_content

#endif // #ifndef LC_CLUSTER_HELPER_H
