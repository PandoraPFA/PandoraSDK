/**
 *  @file   LCContent/include/LCReclustering/TrackDrivenMergingAlg.h
 * 
 *  @brief  Header file for the track driven merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_TRACK_DRIVEN_MERGING_ALGORITHM_H
#define LC_TRACK_DRIVEN_MERGING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Objects/Cluster.h"

namespace lc_content
{

/**
 *  @brief  ClusterConeFraction class
 */
class ClusterConeFraction
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  coneFraction the cone fraction
     *  @param  pCluster address of the cluster associated with the specified cone fraction
     *  @param  clusterIndex the index of the cluster in the associated cluster vector (needed for tidying up in this algorithm)
     */
    ClusterConeFraction(float coneFraction, pandora::Cluster *const pCluster, unsigned int clusterIndex);

    /**
     *  @brief  Operator < for determining cluster cone fraction ordering.
     * 
     *  @param  rhs cluster cone fraction for comparison
     */
    bool operator< (const ClusterConeFraction &rhs) const;

    /**
     *  @brief  Get the cone fraction
     * 
     *  @return The cone fraction
     */
    float GetConeFraction() const;

    /**
     *  @brief  Get the address of the cluster
     * 
     *  @return The address of the cluster
     */
    pandora::Cluster *GetCluster() const;

    /**
     *  @brief  Get the index of the cluster in the associated cluster vector
     * 
     *  @return The index of the cluster in the associated cluster vector
     */
    unsigned int GetClusterIndex() const;

private:
    const float                 m_coneFraction;                     ///< The cone fraction
    pandora::Cluster *const     m_pCluster;                         ///< The address of the cluster
    const unsigned int          m_clusterIndex;                     ///< The index of the cluster in the associated cluster vector
};

typedef std::multiset<ClusterConeFraction> ClusterConeFractionList;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  TrackDrivenMergingAlg class
 */
class TrackDrivenMergingAlg : public pandora::Algorithm
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
    TrackDrivenMergingAlg();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string             m_trackClusterAssociationAlgName;       ///< The name of the track-cluster association algorithm to run

    unsigned int            m_minTrackAssociations;                 ///< The min number of associated tracks required to start merging
    unsigned int            m_maxTrackAssociations;                 ///< The max number of associated tracks required to start merging
    float                   m_chiToAttemptMerging;                  ///< The max track/cluster chi value required to start merging

    float                   m_coarseDaughterChiCut;                 ///< Coarse chi cut for candidate daughter clusters

    float                   m_coneCosineHalfAngle;                  ///< Angle of cone used to identify parent-daughter relationships
    float                   m_minConeFractionSingle;                ///< For single merging: min fraction of daughter hits in parent cone
    float                   m_minConeFractionMultiple;              ///< For multiple merging: min fraction of daughter hits in parent cone

    unsigned int            m_maxLayerSeparationMultiple;           ///< Max layers between parent/daughter clusters for multiple merging
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackDrivenMergingAlg::Factory::CreateAlgorithm() const
{
    return new TrackDrivenMergingAlg();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline ClusterConeFraction::ClusterConeFraction(float coneFraction, pandora::Cluster *const pCluster, unsigned int clusterIndex) :
    m_coneFraction(coneFraction),
    m_pCluster(pCluster),
    m_clusterIndex(clusterIndex)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ClusterConeFraction::operator< (const ClusterConeFraction &rhs) const
{
    if (this->m_coneFraction != rhs.m_coneFraction)
        return (this->m_coneFraction > rhs.m_coneFraction);

    return (this->m_pCluster->GetHadronicEnergy() > rhs.m_pCluster->GetHadronicEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterConeFraction::GetConeFraction() const
{
    return m_coneFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Cluster *ClusterConeFraction::GetCluster() const
{
    return m_pCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ClusterConeFraction::GetClusterIndex() const
{
    return m_clusterIndex;
}

} // namespace lc_content

#endif // #ifndef LC_TRACK_DRIVEN_MERGING_ALGORITHM_H
