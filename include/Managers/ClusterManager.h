/**
 *  @file   PandoraSDK/include/Managers/ClusterManager.h
 * 
 *  @brief  Header file for the cluster manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CLUSTER_MANAGER_H
#define PANDORA_CLUSTER_MANAGER_H 1

#include "Api/PandoraContentApi.h"

#include "Managers/AlgorithmObjectManager.h"

#include "Objects/Cluster.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *    @brief ClusterManager class
 */
class ClusterManager : public AlgorithmObjectManager<Cluster>
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
     ClusterManager(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
     ~ClusterManager();

private:
    /**
     *  @brief  Create cluster
     * 
     *  @param  parameters the cluster parameters
     *  @param  pCluster to receive the address of the cluster created
     */
    StatusCode CreateCluster(const PandoraContentApi::Cluster::Parameters &parameters, Cluster *&pCluster);

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Remove a calo hit from a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Add an isolated calo hit to a cluster. This is not counted as a regular calo hit: it contributes only
     *          towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Remove an isolated calo hit from a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Merge two clusters in the current list, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete);

    /**
     *  @brief  Merge two clusters from two specified lists, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     *  @param  enlargeListName name of the list containing the cluster to enlarge
     *  @param  deleteListName name of the list containing the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
        const std::string &deleteListName);

    /**
     *  @brief  Remove all cluster to track associations
     */
    StatusCode RemoveAllTrackAssociations() const;

    /**
     *  @brief  Remove cluster to track associations from all clusters in the current list
     * 
     *  @param  danglingTracks to receive the list of "dangling" associations
     */
    StatusCode RemoveCurrentTrackAssociations(TrackList &danglingTracks) const;

    /**
     *  @brief  Remove a specified list of cluster to track associations
     * 
     *  @param  trackToClusterList the specified track to cluster list
     */
    StatusCode RemoveTrackAssociations(const TrackToClusterMap &trackToClusterList) const;

    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->RemoveCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->AddIsolatedCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->RemoveIsolatedCaloHit(pCaloHit);
}

} // namespace pandora

#endif // #ifndef PANDORA_CLUSTER_MANAGER_H
