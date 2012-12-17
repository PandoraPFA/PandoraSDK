/**
 *  @file   PandoraPFANew/Framework/include/Helpers/ClusterHelper.h
 * 
 *  @brief  Header file for the cluster helper class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_HELPER_H
#define CLUSTER_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace pandora
{

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ClusterHelper class
 */
class ClusterHelper
{
public:
    /**
     *  @brief  ClusterFitPoint class
     */
    class ClusterFitPoint
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pCaloHit address of a calo hit on which to base the cluster fit point
         */
        ClusterFitPoint(const CaloHit *const pCaloHit);

        /**
         *  @brief  Constructor
         * 
         *  @param  position the position vector of the fit point
         *  @param  cellNormalVector the unit normal vector to the cell in which the point was recorded
         *  @param  cellSize the size of the cell in which the point was recorded
         *  @param  energy the energy deposited in the cell in which the point was recorded
         *  @param  pseudoLayer the pseudolayer in which the point was recorded
         */
        ClusterFitPoint(const CartesianVector &position, const CartesianVector &cellNormalVector, const float cellSize,
            const float energy, const PseudoLayer pseudoLayer);

        /**
         *  @brief  Get the position vector of the fit point
         * 
         *  @return the position vector of the fit point
         */
        const CartesianVector &GetPosition() const;

        /**
         *  @brief  Get the unit normal vector to the cell in which the point was recorded
         * 
         *  @return the unit normal vector to the cell in which the point was recorded
         */
        const CartesianVector &GetCellNormalVector() const;

        /**
         *  @brief  Get the size of the cell in which the point was recorded
         * 
         *  @return the size of the cell in which the point was recorded
         */
        float GetCellSize() const;

        /**
         *  @brief  Get the energy deposited in the cell in which the point was recorded
         * 
         *  @return energy deposited in the cell in which the point was recorded
         */
        float GetEnergy() const;

        /**
         *  @brief  Get the pseudolayer in which the point was recorded
         * 
         *  @return the pseudolayer in which the point was recorded
         */
        PseudoLayer GetPseudoLayer() const;

    private:
        CartesianVector         m_position;              ///< The position vector of the fit point
        CartesianVector         m_cellNormalVector;      ///< The unit normal vector to the cell in which the point was recorded
        float                   m_cellSize;              ///< The size of the cell in which the point was recorded
        float                   m_energy;                ///< The energy deposited in the cell in which the point was recorded
        PseudoLayer             m_pseudoLayer;           ///< The pseudolayer in which the point was recorded
    };

    typedef std::vector<ClusterFitPoint> ClusterFitPointList;

    /**
     *  @brief  ClusterFitResult class
     */
    class ClusterFitResult
    {
    public:
        /**
         *  @brief  Default constructor
         */
        ClusterFitResult();

        /**
         *  @brief  Query whether fit was successful
         * 
         *  @return boolean
         */
        bool IsFitSuccessful() const;

        /**
         *  @brief  Get the fit direction
         * 
         *  @return the fit direction
         */
        const CartesianVector &GetDirection() const;

        /**
         *  @brief  Get the fit intercept
         * 
         *  @return the fit intercept
         */
        const CartesianVector &GetIntercept() const;

        /**
         *  @brief  Get the fit ch2
         * 
         *  @return the fit chi2
         */
        float GetChi2() const;

        /**
         *  @brief  Get the fit rms
         * 
         *  @return the fit rms
         */
        float GetRms() const;

        /**
         *  @brief  Get the fit direction cosine w.r.t. the radial direction
         * 
         *  @return the fit direction cosine w.r.t. the radial direction
         */
        float GetRadialDirectionCosine() const;

        /**
         *  @brief  Set the fit success flag
         * 
         *  @param  successFlag the fit success flag
         */
        void SetSuccessFlag(const bool successFlag);

        /**
         *  @brief  Set the fit direction
         * 
         *  @param  direction the fit direction
         */
        void SetDirection(const CartesianVector &direction);

        /**
         *  @brief  Set the fit intercept
         * 
         *  @param  intercept the fit intercept
         */
        void SetIntercept(const CartesianVector &intercept);

        /**
         *  @brief  Set the fit chi2
         * 
         *  @param  chi2 the fit chi2
         */
        void SetChi2(const float chi2);

        /**
         *  @brief  Set the fit rms
         * 
         *  @param  rms the fit rms
         */
        void SetRms(const float rms);

        /**
         *  @brief  Set the fit direction cosine w.r.t. the radial direction
         * 
         *  @param  radialDirectionCosine the fit direction cosine w.r.t. the radial direction
         */
        void SetRadialDirectionCosine(const float radialDirectionCosine);

        /**
         *  @brief  Reset the cluster fit result
         */
        void Reset();

    private:
        bool                    m_isFitSuccessful;       ///< Whether the fit was successful
        CartesianVector         m_direction;             ///< The best fit direction
        CartesianVector         m_intercept;             ///< The best fit intercept
        InputFloat              m_chi2;                  ///< The chi2 value for the fit
        InputFloat              m_rms;                   ///< The rms of the fit
        InputFloat              m_dirCosR;               ///< The direction cosine wrt to the radial direction
    };

    /**
     *  @brief  Fit points in first n occupied pseudolayers of a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  maxOccupiedLayers the maximum number of occupied pseudo layers to consider
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitStart(const Cluster *const pCluster, const unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit points in last n occupied pseudolayers of a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  maxOccupiedLayers the maximum number of occupied pseudo layers to consider
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitEnd(const Cluster *const pCluster, const unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit all points in a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitFullCluster(const Cluster *const pCluster, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit all cluster points within the specified (inclusive) pseudolayer range
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  startLayer the start of the pseudolayer range
     *  @param  endLayer the end of the pseudolayer range
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitLayers(const Cluster *const pCluster, const PseudoLayer startLayer, const PseudoLayer endLayer,
        ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit all cluster centroids within the specified (inclusive) pseudolayer range
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  startLayer the start of the pseudolayer range
     *  @param  endLayer the end of the pseudolayer range
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitLayerCentroids(const Cluster *const pCluster, const PseudoLayer startLayer, const PseudoLayer endLayer,
        ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Perform linear regression of x vs d and y vs d and z vs d (assuming same error on all hits)
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Get the closest distance of approach between two cluster fit results
     * 
     *  @param  lhs first cluster fit result
     *  @param  rhs second cluster fit result
     *  @param  closestApproach to receive the closest distance of approach
     */
    static StatusCode GetFitResultsClosestApproach(const ClusterFitResult &lhs, const ClusterFitResult &rhs, float &closestApproach);

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
    static float GetDistanceToClosestHit(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
        const PseudoLayer startLayer, const PseudoLayer endLayer);

    /**
     *  @brief  Get smallest distance between pairs of hits in two clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     * 
     *  @return the smallest distance
     */
    static float GetDistanceToClosestHit(const Cluster *const pClusterI, const Cluster *const pClusterJ);

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
    static float GetDistanceToClosestCentroid(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
        const PseudoLayer startLayer, const PseudoLayer endLayer);

    /**
     *  @brief  Get the closest distance between layer centroid positions in two overlapping clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     *  @param  intraLayerDistance to receive the closest centroid distance
     */
    static StatusCode GetDistanceToClosestCentroid(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &centroidDistance);

    /**
     *  @brief  Get the closest distance between same layer centroid positions in two overlapping clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     *  @param  intraLayerDistance to receive the closest intra layer distance
     */
    static StatusCode GetClosestIntraLayerDistance(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &intraLayerDistance);

    /**
     *  @brief  Get the distance of closest approach between the projected track direction at calorimeter and the hits within a cluster.
     *          Note that only a specified number of layers are examined.
     * 
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     *  @param  maxSearchLayer the maximum pseudolayer to examine
     *  @param  parallelDistanceCut maximum allowed projection of track-cluster separation along track direction
     *  @param  trackClusterDistance to receive the track cluster distance
     */
    static StatusCode GetTrackClusterDistance(const Track *const pTrack, const Cluster *const pCluster, const PseudoLayer maxSearchLayer,
        const float parallelDistanceCut, float &trackClusterDistance);

    /**
     *  @brief  Whether a cluster can be merged with another. Uses simple suggested criteria, including cluster photon id flag
     *          and supplied cuts on cluster mip fraction and all hits fit rms.
     * 
     *  @param  pCluster address of the cluster
     *  @param  minMipFraction the minimum mip fraction for clusters (flagged as photons) to be merged
     *  @param  maxAllHitsFitRms the maximum all hit fit rms for clusters (flagged as photons) to be merged
     * 
     *  @return boolean
     */
    static bool CanMergeCluster(const Cluster *const pCluster, const float minMipFraction, const float maxAllHitsFitRms);

    /**
     *  @brief  Get the layer at which shower can be considered to start; this function evaluates the the starting point of
     *          a series of "showerStartNonMipLayers" successive layers, each with mip fraction below "showerLayerMipFraction"
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return The shower start layer
     */
    static PseudoLayer GetShowerStartLayer(const Cluster *const pCluster);

    /**
     *  @brief  Get the energy-weighted mean time of the non-isolated hits in a specified cluster, units ns
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return The energy-weighted mean time, units ns
     */
    static float GetEnergyWeightedMeanTime(const Cluster *const pCluster);

    /**
     *  @brief  Whether a cluster should be considered as leaving the calorimeters, leading to leakage of its energy
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsClusterLeavingDetector(const Cluster *const pCluster);

    /**
     *  @brief  Whether a linear fit to a cluster crosses a registered gap region. Only the region between the startlayer and
     *          endlayer is considered in the fit and in the comparison with registered gap regions.
     * 
     *  @param  pCluster address of the cluster
     *  @param  startLayer the start layer (adjusted to maximum of specified layer and cluster inner layer)
     *  @param  endLayer the end layer (adjusted to minimum of specified layer and cluster outer layer)
     *  @param  nSamplingPoints number of points at which to sample the fit within the specified layer region
     * 
     *  @return boolean
     */
    static bool DoesClusterCrossGapRegion(const Cluster *const pCluster, const PseudoLayer startLayer, const PseudoLayer endLayer,
        const unsigned int nSamplingPoints = 50);

    /**
     *  @brief  Whether a linear fit crosses a registered gap region. The fit will be propagated through the specified distance
     *          from its closest approach to the startPosition. Within this propagation, the fit will be sampled a specified number
     *          of times and the resulting position compared with registered gap regions.
     * 
     *  @param  clusterFitResult the clsuter fit result
     *  @param  startPosition the propagation start position (adjusted to closest point on fit trajectory)
     *  @param  propagationDistance the propagation distance, which can be negative for propagation towards the ip
     *  @param  nSamplingPoints number of points at which to sample the fit within its propagation
     * 
     *  @return boolean
     */
    static bool DoesFitCrossGapRegion(const ClusterFitResult &clusterFitResult, const CartesianVector &startPosition,
        const float propagationDistance, const unsigned int nSamplingPoints = 50);

private:
    /**
     *  @brief  Perform linear fit to cluster fit points
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  centralPosition central position of the cluster fit points
     *  @param  centralDirection central direction of normal to cluster fit calorimeter cells
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode PerformLinearFit(const ClusterFitPointList &clusterFitPointList, const CartesianVector &centralPosition,
        const CartesianVector &centralDirection, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Read the cluster helper settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle *const pXmlHandle);

    static float        m_minTrackClusterCosAngle;              ///< Track/cluster dist: min cos(angle) between track and cluster inner centroid
    static float        m_showerStartMipFraction;               ///< Max layer mip-fraction to declare layer as shower-like
    static unsigned int m_showerStartNonMipLayers;              ///< Number of successive shower-like layers to identify shower start

    static unsigned int m_leavingNOuterLayersToExamine;         ///< Number of outer layers to examine to identify leaving cluster
    static unsigned int m_leavingMipLikeNOccupiedLayers;        ///< Number of occupied outer layers for a mip-like leaving cluster
    static unsigned int m_leavingShowerLikeNOccupiedLayers;     ///< Number of occupied outer layers for a shower-like leaving cluster
    static float        m_leavingShowerLikeEnergyInOuterLayers; ///< Energy deposited in outer layers by a shower-like leaving cluster

    friend class PandoraSettings;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterHelper::ClusterFitPoint::GetPosition() const
{
    return m_position;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterHelper::ClusterFitPoint::GetCellNormalVector() const
{
    return m_cellNormalVector;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitPoint::GetCellSize() const
{
    return m_cellSize;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitPoint::GetEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayer ClusterHelper::ClusterFitPoint::GetPseudoLayer() const
{
    return m_pseudoLayer;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline ClusterHelper::ClusterFitResult::ClusterFitResult() :
    m_isFitSuccessful(false),
    m_direction(0.f, 0.f, 0.f),
    m_intercept(0.f, 0.f, 0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ClusterHelper::ClusterFitResult::IsFitSuccessful() const
{
    return m_isFitSuccessful;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterHelper::ClusterFitResult::GetDirection() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_direction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterHelper::ClusterFitResult::GetIntercept() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_intercept;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitResult::GetChi2() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_chi2.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitResult::GetRms() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_rms.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitResult::GetRadialDirectionCosine() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_dirCosR.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetSuccessFlag(bool successFlag)
{
    m_isFitSuccessful = successFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetDirection(const CartesianVector &direction)
{
    m_direction = direction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetIntercept(const CartesianVector &intercept)
{
    m_intercept = intercept;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetChi2(const float chi2)
{
    if (!(m_chi2 = chi2))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetRms(const float rms)
{
    if (!(m_rms = rms))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetRadialDirectionCosine(const float radialDirectionCosine)
{
    if (!(m_dirCosR = radialDirectionCosine))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::Reset()
{
    m_isFitSuccessful = false;
    m_direction.SetValues(0.f, 0.f, 0.f);
    m_intercept.SetValues(0.f, 0.f, 0.f);
    m_chi2.Reset();
    m_rms.Reset();
    m_dirCosR.Reset();
}

} // namespace pandora

#endif // #ifndef CLUSTER_HELPER_H
