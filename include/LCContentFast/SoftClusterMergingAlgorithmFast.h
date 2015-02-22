/**
 *  @file   LCContent/include/LCContentFast/SoftClusterMergingAlgorithmFast.h
 * 
 *  @brief  Header file for the soft cluster merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_SOFT_CLUSTER_MERGING_ALGORITHM_FAST_H
#define LC_SOFT_CLUSTER_MERGING_ALGORITHM_FAST_H 1

#include "Pandora/Algorithm.h"

#include <unordered_map>

namespace lc_content_fast
{

template<typename, unsigned int> class KDTreeLinkerAlgo;
template<typename, unsigned int> class KDTreeNodeInfoT;
class QuickUnion;

//------------------------------------------------------------------------------------------------------------------------------------------

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

    /**
     *  @brief Destructor
     */
    ~SoftClusterMergingAlgorithm();

private:
    typedef KDTreeLinkerAlgo<const pandora::CaloHit*, 3> HitKDTree3D;
    typedef KDTreeNodeInfoT<const pandora::CaloHit*, 3> HitKDNode3D;
    typedef std::unordered_map<const pandora::ClusterList *, std::string> ClusterListToNameMap;
    typedef std::unordered_map<const pandora::CaloHit*, int> HitToClusterMap;
    typedef std::unordered_multimap<const pandora::CaloHit*, const pandora::CaloHit*> HitsToHitsCacheMap;

    pandora::StatusCode Run();

    /**
     *  @brief  Get the input clusters
     * 
     *  @param  clusterList to receive the populated cluster list
     *  @param  clusterListToNameMap to receive the populated cluster list to list name map
     */
    void GetInputClusters(pandora::ClusterList &clusterList, ClusterListToNameMap &clusterListToNameMap) const;

    /**
     *  @brief  Get the input calo hits (constituents of the input clusters)
     * 
     *  @param  clusterVector the vector of input clusters
     *  @param  fullCaloHitList to receive the populated calo hit list
     *  @param  hitToClusterMap to receive the populated calo hit to cluster map
     */
    void GetInputCaloHits(const pandora::ClusterVector &clusterVector, pandora::CaloHitList &fullCaloHitList,
        HitToClusterMap &hitToClusterMap) const;

    /**
     *  @brief  Initialize a kd-tree of the input hits to the preparation alg.
     * 
     *  @param  pCaloHitList -- the calorimeter hit list
     */
    void InitializeKDTree(const pandora::CaloHitList *const pCaloHitList);

    /**
     *  @brief  Whether a daughter candidate cluster is a "soft" cluster
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     * 
     *  @return boolean
     */
    bool IsSoftCluster(const pandora::Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Find the best parent cluster for a provided list of daughter calo hits
     * 
     *  @param  clusterVector the cluster vector
     *  @param  hitToClusterMap the hit to cluster map
     *  @param  quickUnion to handle updating cluster indices
     *  @param  pDaughterCluster the address of the daughter cluster
     *  @param  daughterHits the provided list of daughter calo hits
     *  @param  closestDistance to receive the closest hit separation
     * 
     *  @return the index of the best parent cluster
     */
    int FindBestParentCluster(const pandora::ClusterVector &clusterVector, const HitToClusterMap &hitToClusterMap, QuickUnion &quickUnion,
        const pandora::Cluster *const pDaughterCluster, const pandora::CaloHitList &daughterHits, float &closestDistance) const;

    /**
     *  @brief  Whether a soft daughter candidate cluster can be merged with a parent a specified distance away
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  closestDistance the closest distance between a pair of hits in the daughter and parent candidate clusters
     * 
     *  @return boolean
     */
    bool CanMergeSoftCluster(const pandora::Cluster *const pDaughterCluster, const float closestDistance) const;

    /**
     *  @brief  Merge a provided pair of parent and daughter clusters
     * 
     *  @param  pParentCluster the address of the parent cluster
     *  @param  pDaughterCluster the address of the daughter cluster
     *  @param  clusterListToNameMap the cluster list to list name map
     */
    void MergeClusters(const pandora::Cluster *const pParentCluster, const pandora::Cluster *const pDaughterCluster,
        const ClusterListToNameMap &clusterListToNameMap) const;

    /**
     *  @brief  Get the name of the cluster list in which a specified cluster can be found
     * 
     *  @param  pCluster address of the cluster
     *  @param  clusterListToNameMap the cluster list to name map
     *  @param  listName to receive the list name
     */
    pandora::StatusCode GetClusterListName(const pandora::Cluster *const pCluster, const ClusterListToNameMap &clusterListToNameMap,
        std::string &listName) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

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

    HitsToHitsCacheMap         *m_hitsToHitsCacheMap;               ///< To cache nearby hits retrieved from kd-tree
    std::vector<HitKDNode3D>   *m_hitNodes3D;                       ///< nodes for the KD tree (used for filling)
    HitKDTree3D                *m_hitsKdTree3D;                     ///< the kd-tree itself, 3D in x,y,z
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *SoftClusterMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new SoftClusterMergingAlgorithm();
}

} // namespace lc_content_fast

#endif // #ifndef LC_SOFT_CLUSTER_MERGING_ALGORITHM_FAST_H
