/**
 *  @file   LCContent/include/LCTopologicalAssociation/SoftClusterMergingAlgorithm.h
 * 
 *  @brief  Header file for the soft cluster merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_SOFT_CLUSTER_MERGING_ALGORITHM_H
#define LC_SOFT_CLUSTER_MERGING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  SoftClusterMergingAlgorithm class
 */
class SoftClusterMergingAlgorithm : public pandora::Algorithm
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
    SoftClusterMergingAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Whether a daughter candidate cluster is a "soft" cluster
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     * 
     *  @return boolean
     */
    bool IsSoftCluster(const pandora::Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Whether a soft daughter candidate cluster can be merged with a parent a specified distance away
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  closestDistance the closest distance between a pair of hits in the daughter and parent candidate clusters
     * 
     *  @return boolean
     */
    bool CanMergeSoftCluster(const pandora::Cluster *const pDaughterCluster, const float closestDistance) const;

    typedef std::map<const pandora::ClusterList *, std::string> ClusterListToNameMap;

    /**
     *  @brief  Get the name of the cluster list in which a specified cluster can be found
     * 
     *  @param  pCluster address of the cluster
     *  @param  clusterListToNameMap the cluster list to name map
     *  @param  listName to receive the list name
     */
    pandora::StatusCode GetClusterListName(const pandora::Cluster *const pCluster, const ClusterListToNameMap &clusterListToNameMap,
        std::string &listName) const;

    bool                    m_shouldUseCurrentClusterList;          ///< Whether to use clusters from the current list in the algorithm
    bool                    m_updateCurrentTrackClusterAssociations;///< Whether to update track-cluster associations for current list
    std::string             m_trackClusterAssociationAlgName;       ///< The name of the track-cluster association algorithm to run

    pandora::StringVector   m_additionalClusterListNames;           ///< Additional cluster lists from which to consider clusters

    unsigned int            m_maxHitsInSoftCluster;                 ///< Max number of hits in a soft cluster
    unsigned int            m_maxLayersSpannedBySoftCluster;        ///< Max number of pseudo layers spanned by a soft cluster
    float                   m_maxHadEnergyForSoftClusterNoTrack;    ///< Max hadronic energy for a soft cluster with no track associations

    float                   m_minClusterHadEnergy;                  ///< Min hadronic energy in an "acceptable" cluster
    float                   m_minClusterEMEnergy;                   ///< Min electromagnetic energy in an "acceptable" cluster

    float                   m_minCosOpeningAngle;                   ///< Min cos(angle) between cluster directions to consider merging clusters
    unsigned int            m_minHitsInCluster;                     ///< Min number of hits in an "acceptable" cluster

    float                   m_closestDistanceCut0;                  ///< Single cut: max distance at which can automatically merge parent/daughter

    float                   m_closestDistanceCut1;                  ///< 1st pair of cuts: max distance between daughter and parent clusters
    unsigned int            m_innerLayerCut1;                       ///< 1st pair of cuts: max daughter inner pseudo layer
    float                   m_closestDistanceCut2;                  ///< 2nd pair of cuts: max distance between daughter and parent clusters
    unsigned int            m_innerLayerCut2;                       ///< 2nd pair of cuts: max daughter inner pseudo layer
        
    float                   m_maxClusterDistanceFine;               ///< Fine granularity max distance between parent and daughter clusters
    float                   m_maxClusterDistanceCoarse;             ///< Coarse granularity max distance between parent and daughter clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *SoftClusterMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new SoftClusterMergingAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_SOFT_CLUSTER_MERGING_ALGORITHM_H
