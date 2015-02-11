/**
 *  @file   LCContent/include/LCTopologicalAssociation/LoopingTracksAlgorithm.h
 * 
 *  @brief  Header file for the looping tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_LOOPING_TRACKS_ALGORITHM_H
#define LC_LOOPING_TRACKS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Helpers/ClusterFitHelper.h"

namespace lc_content
{

/**
 *  @brief  LoopingTracksAlgorithm class
 */
class LoopingTracksAlgorithm : public pandora::Algorithm
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
    LoopingTracksAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  ClusterFitRelation class
     */
    class ClusterFitRelation
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pCluster the address of the cluster
         *  @param  clusterFitResult the cluster fit result
         */
        ClusterFitRelation(const pandora::Cluster *const pCluster, const pandora::ClusterFitResult &clusterFitResult);

        /**
         *  @brief  Get the address of the cluster
         * 
         *  @return The address of the cluster
         */
        const pandora::Cluster *GetCluster() const;

        /**
         *  @brief  Get the cluster fit result
         * 
         *  @return The cluster fit result
         */
        const pandora::ClusterFitResult &GetClusterFitResult() const;

        /**
         *  @brief  Set the cluster fit result
         * 
         *  @param  clusterFitResult the cluster fit result
         */
        void SetClusterFitResult(const pandora::ClusterFitResult &clusterFitResult);

        /**
         *  @brief  Whether the cluster fit relation is defunct (the cluster has changed or been deleted and the
         *          fit result is no longer valid).
         * 
         *  @return boolean
         */
        bool IsDefunct() const;

        /**
         *  @brief  Set the cluster fit relation as defunct. This should be called when the cluster has changed,
         *          or if it has been deleted or merged with another cluster.
         */
        void SetAsDefunct();

    private:
        bool                        m_isDefunct;            ///< Whether the cluster fit relation is defunct
        const pandora::Cluster     *m_pCluster;             ///< Address of the cluster
        pandora::ClusterFitResult   m_clusterFitResult;     ///< The cluster fit result
    };

    typedef std::vector<ClusterFitRelation *> ClusterFitRelationList;

    /**
     *  @brief  Get the closest distance between hits in the outermost pseudolayer of two clusters
     * 
     *  @param  pClusterI address of first cluster
     *  @param  pClusterJ address of second cluster
     * 
     *  @return the closest distance between outer layer hits
     */
    float GetClosestDistanceBetweenOuterLayerHits(const pandora::Cluster *const pClusterI, const pandora::Cluster *const pClusterJ) const;

    unsigned int    m_nLayersToFit;                     ///< The number of occupied pseudolayers to use in fit to the end of the cluster
    float           m_fitChi2Cut;                       ///< The chi2 cut to apply to fit results

    float           m_canMergeMinMipFraction;           ///< The minimum mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The maximum all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minHitsInCluster;                 ///< Min number of calo hits in cluster
    unsigned int    m_minOccupiedLayersInCluster;       ///< Min number of occupied layers in cluster

    unsigned int    m_maxOuterLayerDifference;          ///< The maximum difference (for merging) between cluster outer pseudo layers
    float           m_maxCentroidDifference;            ///< The maximum difference (for merging) between cluster outer layer centroids

    float           m_fitDirectionDotProductCutFine;    ///< Fine granularity cut on max value of dot product between cluster fit directions
    float           m_fitDirectionDotProductCutCoarse;  ///< Coarse granularity cut on max value of dot product between cluster fit directions

    float           m_closestHitDistanceCutFine;        ///< Fine granularity cut on distance between cluster hits in outermost pseudolayers
    float           m_closestHitDistanceCutCoarse;      ///< Coarse granularity cut on distance between cluster hits in outermost pseudolayers

    float           m_fitResultsClosestApproachCutFine; ///< Fine granularity cut on closest distance of approach between two cluster fits
    float           m_fitResultsClosestApproachCutCoarse; ///< Coarse granularity cut on closest distance of approach between two cluster fits

    unsigned int    m_nGoodFeaturesForClusterMerge;     ///< Number of identified "good features" required to merge fine granularity clusters

    float           m_goodFeaturesMaxFitDotProduct;     ///< Max dot product between cluster fit directions for good feature
    float           m_goodFeaturesMaxFitApproach;       ///< Max distance of closest approach between cluster fit results for good feature
    unsigned int    m_goodFeaturesMaxLayerDifference;   ///< Max difference between cluster outer pseudo layers for good feature
    float           m_goodFeaturesMinMipFraction;       ///< Min cluster mip fraction for good feature
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *LoopingTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new LoopingTracksAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline LoopingTracksAlgorithm::ClusterFitRelation::ClusterFitRelation(const pandora::Cluster *const pCluster, const pandora::ClusterFitResult &clusterFitResult) :
    m_isDefunct(false),
    m_pCluster(pCluster),
    m_clusterFitResult(clusterFitResult)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::Cluster *LoopingTracksAlgorithm::ClusterFitRelation::GetCluster() const
{
    if (m_isDefunct)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_ALLOWED);

    return m_pCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::ClusterFitResult &LoopingTracksAlgorithm::ClusterFitRelation::GetClusterFitResult() const
{
    if (m_isDefunct)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_ALLOWED);

    return m_clusterFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void LoopingTracksAlgorithm::ClusterFitRelation::SetClusterFitResult(const pandora::ClusterFitResult &clusterFitResult)
{
    m_clusterFitResult = clusterFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool LoopingTracksAlgorithm::ClusterFitRelation::IsDefunct() const
{
    return m_isDefunct;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void LoopingTracksAlgorithm::ClusterFitRelation::SetAsDefunct()
{
    m_isDefunct = true;
}

} // namespace lc_content

#endif // #ifndef LC_LOOPING_TRACKS_ALGORITHM_H
