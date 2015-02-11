/**
 *  @file   LCContent/include/LCTopologicalAssociation/BrokenTracksAlgorithm.h
 * 
 *  @brief  Header file for the broken tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_BROKEN_TRACKS_ALGORITHM_H
#define LC_BROKEN_TRACKS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Helpers/ClusterFitHelper.h"

namespace lc_content
{

/**
 *  @brief  BrokenTracksAlgorithm class
 */
class BrokenTracksAlgorithm : public pandora::Algorithm
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
    BrokenTracksAlgorithm();

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
         *  @param  startFitResult the cluster start fit result
         *  @param  endFitResult the cluster end fit result
         */
        ClusterFitRelation(const pandora::Cluster *const pCluster, const pandora::ClusterFitResult &startFitResult, const pandora::ClusterFitResult &endFitResult);

        /**
         *  @brief  Get the address of the cluster
         * 
         *  @return The address of the cluster
         */
        const pandora::Cluster *GetCluster() const;

        /**
         *  @brief  Get the cluster start fit result
         * 
         *  @return The cluster start fit result
         */
        const pandora::ClusterFitResult &GetStartFitResult() const;

        /**
         *  @brief  Get the cluster end fit result
         * 
         *  @return The cluster end fit result
         */
        const pandora::ClusterFitResult &GetEndFitResult() const;

        /**
         *  @brief  Set the cluster start fit result
         * 
         *  @param  startFitResult the cluster start fit result
         */
        void SetStartFitResult(const pandora::ClusterFitResult &startFitResult);

        /**
         *  @brief  Set the cluster start fit result
         * 
         *  @param  endFitResult the cluster end fit result
         */
        void SetEndFitResult(const pandora::ClusterFitResult &endFitResult);

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
        pandora::ClusterFitResult   m_startFitResult;       ///< The cluster start fit result
        pandora::ClusterFitResult   m_endFitResult;         ///< The cluster end fit result
    };

    typedef std::vector<ClusterFitRelation *> ClusterFitRelationList;

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged

    unsigned int    m_minHitsInCluster;                 ///< Min number of calo hits in cluster
    unsigned int    m_minOccupiedLayersForStartFit;     ///< Min number of occupied layers in cluster to allow fit to start of cluster
    unsigned int    m_minOccupiedLayersForEndFit;       ///< Min number of occupied layers in cluster to allow fit to end of cluster

    unsigned int    m_nStartLayersToFit;                ///< The number of occupied pseudolayers to use in fit to the start of the cluster
    unsigned int    m_nEndLayersToFit;                  ///< The number of occupied pseudolayers to use in fit to the end of the cluster
    float           m_maxFitRms;                        ///< The max value of the start/end fit rms for cluster to be considered

    float           m_fitDirectionDotProductCut;        ///< Cut on max value of dot product between cluster fit directions
    float           m_trackMergeCutFine;                ///< Fine granularity cut on closest distance of approach between two cluster fits
    float           m_trackMergeCutCoarse;              ///< Coarse granularity cut on closest distance of approach between two cluster fits

    float           m_trackMergePerpCutFine;            ///< Fine granularity cut on perp. distance between fit directions and centroid difference
    float           m_trackMergePerpCutCoarse;          ///< Coarse granularity cut on perp. distance between fit directions and centroid difference

    unsigned int    m_maxLayerDifference;               ///< The max difference (for merging) between cluster outer and inner pseudo layers
    float           m_maxCentroidDifference;            ///< The max difference (for merging) between cluster outer and inner centroids

    bool            m_shouldPerformGapCheck;            ///< Whether to check if clusters pass through detector gaps
    float           m_maxChi2ForGapCheck;               ///< The max chi2 to check whether clusters pass through detector gaps
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *BrokenTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new BrokenTracksAlgorithm();
}


//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline BrokenTracksAlgorithm::ClusterFitRelation::ClusterFitRelation(const pandora::Cluster *const pCluster, const pandora::ClusterFitResult &startFitResult,
        const pandora::ClusterFitResult &endFitResult) :
    m_isDefunct(false),
    m_pCluster(pCluster),
    m_startFitResult(startFitResult),
    m_endFitResult(endFitResult)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::Cluster *BrokenTracksAlgorithm::ClusterFitRelation::GetCluster() const
{
    if (m_isDefunct)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_ALLOWED);

    return m_pCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::ClusterFitResult &BrokenTracksAlgorithm::ClusterFitRelation::GetStartFitResult() const
{
    if (m_isDefunct)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_ALLOWED);

    return m_startFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::ClusterFitResult &BrokenTracksAlgorithm::ClusterFitRelation::GetEndFitResult() const
{
    if (m_isDefunct)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_ALLOWED);

    return m_endFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void BrokenTracksAlgorithm::ClusterFitRelation::SetStartFitResult(const pandora::ClusterFitResult &startFitResult)
{
    m_startFitResult = startFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void BrokenTracksAlgorithm::ClusterFitRelation::SetEndFitResult(const pandora::ClusterFitResult &endFitResult)
{
    m_endFitResult = endFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool BrokenTracksAlgorithm::ClusterFitRelation::IsDefunct() const
{
    return m_isDefunct;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void BrokenTracksAlgorithm::ClusterFitRelation::SetAsDefunct()
{
    m_isDefunct = true;
}

} // namespace lc_content

#endif // #ifndef LC_BROKEN_TRACKS_ALGORITHM_H
