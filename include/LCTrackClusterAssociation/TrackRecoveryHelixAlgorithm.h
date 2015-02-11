/**
 *  @file   LCContent/include/LCTrackClusterAssociation/TrackRecoveryHelixAlgorithm.h
 * 
 *  @brief  Header file for the track recovery helix algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_TRACK_RECOVERY_HELIX_ALGORITHM_H
#define LC_TRACK_RECOVERY_HELIX_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  TrackRecoveryHelixAlgorithm class
 */
class TrackRecoveryHelixAlgorithm : public pandora::Algorithm
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
    TrackRecoveryHelixAlgorithm();

private:
    /**
     *  @brief  AssociationInfo class
     */
    class AssociationInfo
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pCluster address of cluster to which association could be made
         *  @param  closestApproach distance of closest approach between the cluster and the track under consideration
         */
        AssociationInfo(const pandora::Cluster *const pCluster, const float closestApproach);

        /**
         *  @brief  Get the address of the cluster to which association could be made
         * 
         *  @return The address of the cluster
         */
        const pandora::Cluster *GetCluster() const;

        /**
         *  @brief  Get the distance of closest approach between the cluster and the track under consideration
         * 
         *  @return The distance of closest approach
         */
        float GetClosestApproach() const;

        /**
         *  @brief  Operator< to order by address of associated cluster
         * 
         *  @param  rhs association info to compare with
         */
        bool operator< (const AssociationInfo &rhs) const;

    private:
        const pandora::Cluster *m_pCluster;                 ///< The cluster to which an association would be made
        float                   m_closestApproach;          ///< The distance of closest approach
    };

    typedef std::set<AssociationInfo> AssociationInfoSet;
    typedef std::map<const pandora::Track *, AssociationInfoSet> TrackAssociationInfoMap;

    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Get a map specifying cluster association information for every possible matching cluster
     * 
     *  @param  trackAssociationInfoMap the track association info map
     */
    pandora::StatusCode GetTrackAssociationInfoMap(TrackAssociationInfoMap &trackAssociationInfoMap) const;

    /**
     *  @brief  Use information in the track association info map to create track to cluster associations
     * 
     *  @param  trackAssociationInfoMap the track association info map
     */
    pandora::StatusCode MakeTrackClusterAssociations(TrackAssociationInfoMap &trackAssociationInfoMap) const;

    float           m_maxTrackClusterDeltaZ;            ///< Max z separation between track calorimeter projection and cluster to allow association
    float           m_maxAbsoluteTrackClusterChi;       ///< Max absolute track-cluster consistency chi value to allow association
    unsigned int    m_maxLayersCrossed;                 ///< Max number of layers crossed by track helix between calorimeter projection and cluster

    unsigned int    m_maxSearchLayer;                   ///< Max pseudo layer to examine when calculating track-cluster distance
    float           m_parallelDistanceCut;              ///< Max allowed projection of track-hit separation along track direction
    float           m_minTrackClusterCosAngle;          ///< Min cos(angle) between track and cluster initial direction

    unsigned int    m_helixComparisonNLayers;           ///< Number of cluster layers used in cluster-helix comparison
    unsigned int    m_helixComparisonMaxOccupiedLayers; ///< Max number of occupied cluster layers used in cluster-helix comparison

    float           m_maxTrackClusterDistance;          ///< Max track-cluster separation to allow association
    float           m_maxClosestHelixClusterDistance;   ///< Max helix-cluster closest approach to allow association
    float           m_maxMeanHelixClusterDistance;      ///< Max helix-cluster mean approach to allow association
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackRecoveryHelixAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackRecoveryHelixAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline TrackRecoveryHelixAlgorithm::AssociationInfo::AssociationInfo(const pandora::Cluster *const pCluster, const float closestApproach) :
    m_pCluster(pCluster),
    m_closestApproach(closestApproach)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::Cluster *TrackRecoveryHelixAlgorithm::AssociationInfo::GetCluster() const
{
    return m_pCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TrackRecoveryHelixAlgorithm::AssociationInfo::GetClosestApproach() const
{
    return m_closestApproach;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool TrackRecoveryHelixAlgorithm::AssociationInfo::operator< (const TrackRecoveryHelixAlgorithm::AssociationInfo &rhs) const
{
    return (this->m_pCluster > rhs.m_pCluster);
}

} // namespace lc_content

#endif // #ifndef LC_TRACK_RECOVERY_HELIX_ALGORITHM_H
