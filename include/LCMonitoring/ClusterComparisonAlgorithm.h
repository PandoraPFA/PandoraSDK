/**
 *  @file   LCContent/include/LCMonitoring/ClusterComparisonAlgorithm.h
 * 
 *  @brief  Header file for the cluster comparison algorithm class
 * 
 *  $Log: $
 */
#ifndef LC_CLUSTER_COMPARISON_ALGORITHM_H
#define LC_CLUSTER_COMPARISON_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  ClusterComparisonAlgorithm class
 */
class ClusterComparisonAlgorithm : public pandora::Algorithm
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
     *  @brief  Default constructor
     */
    ClusterComparisonAlgorithm();

private:
    pandora::StatusCode Run();

    /**
     *  @brief  Change the current calo hit and track lists, the contents of which are put forward for clustering
     * 
     *  @param  originalCaloHitListName to store the original calo hit list name, so it can be restored if necessary
     *  @param  originalTrackListName to store the original track list name, so it can be restored if necessary
     */
    void TweakCurrentLists(std::string &originalCaloHitListName, std::string &originalTrackListName) const;

    /**
     *  @brief  Create and save a list of clusters for use in future comparisons
     */
    void CreateAndSaveInitialClusters() const;

    /**
     *  @brief  Run the reclustering processes to compare the output of n clustering algorithms with the default saved list
     */
    void RunReclustering() const;

    /**
     *  @brief  Perform a comparison between clusters in two provided lists
     * 
     *  @param  clusterList1 the first cluster list
     *  @param  clusterList2 the second cluster list
     */
    void CompareClusters(const pandora::ClusterList &clusterList1, const pandora::ClusterList &clusterList2) const;

    typedef std::map<pandora::Cluster*, pandora::CaloHitList> ClusterToHitListMap;
    typedef std::map<pandora::CaloHit*, pandora::Cluster*> HitToClusterMap;

    /**
     *  @brief  Populate maps (from cluster to calo hit list and from calo hit to cluster) for a provided cluster list
     * 
     *  @param  clusterList the cluster list
     *  @param  clusterToHitListMap to receive the populated cluster to calo hit list map
     *  @param  hitToClusterMap to receive the populated calo hit to cluster map
     */
    void PopulateMaps(const pandora::ClusterList &clusterList, ClusterToHitListMap &clusterToHitListMap, HitToClusterMap &hitToClusterMap) const;

    /**
     *  @brief  Restor the current lists to those saved at the start of the algorithm
     * 
     *  @param  originalCaloHitListName the original calo hit list name
     *  @param  originalTrackListName the original track list name
     */
    void RestoreInputLists(const std::string &originalCaloHitListName, const std::string &originalTrackListName) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string             m_inputCaloHitListName;         ///< The name of the input calo hit list, containing the hits to be clustered
    bool                    m_restoreOriginalCaloHitList;   ///< Whether to restore the original calo hit list as the "current" list upon completion

    std::string             m_inputTrackListName;           ///< The name of the input track list, containing the tracks for use in seeding clusters
    bool                    m_restoreOriginalTrackList;     ///< Whether to restore the original track list as the "current" list upon completion

    std::string             m_clusteringAlgorithmName;      ///< The name of the original clustering algorithm to run
    pandora::StringVector   m_comparisonAlgorithms;         ///< The ordered list of reclustering algorithms to be used, output then compared with original

    std::string             m_clusterListName;              ///< The name under which to save the new cluster list
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusterComparisonAlgorithm::Factory::CreateAlgorithm() const
{
    return new ClusterComparisonAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_CLUSTER_COMPARISON_ALGORITHM_H
