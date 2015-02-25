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
    StatusCode Create(const PandoraContentApi::Cluster::Parameters &parameters, const Cluster *&pCluster);

    /**
     *  @brief  Alter the metadata information stored in a cluster
     * 
     *  @param  pCluster address of the cluster to modify
     *  @param  metaData the metadata (only populated metadata fields will be propagated to the object)
     */
    StatusCode AlterMetadata(const Cluster *const pCluster, const PandoraContentApi::Cluster::Metadata &metadata) const;

    /**
     *  @brief  Is a cluster, or a list of clusters, available to add to a particle flow object
     * 
     *  @param  pT address of the object or object list
     * 
     *  @return boolean
     */
    template <typename T>
    bool IsAvailable(const T *const pT) const;

    /**
     *  @brief  Set availability of a cluster, or a list of clusters, to be added to a particle flow object
     * 
     *  @param  pT the address of the object or object list
     *  @param  isAvailable the availability
     */
    template <typename T>
    void SetAvailability(const T *const pT, bool isAvailable) const;

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddToCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit);

    /**
     *  @brief  Remove a calo hit from a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveFromCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit);

    /**
     *  @brief  Add an isolated calo hit to a cluster. This is not counted as a regular calo hit: it contributes only
     *          towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddIsolatedToCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit);

    /**
     *  @brief  Remove an isolated calo hit from a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveIsolatedFromCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit);

    /**
     *  @brief  Merge two clusters from two specified lists, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     *  @param  enlargeListName name of the list containing the cluster to enlarge
     *  @param  deleteListName name of the list containing the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(const Cluster *const pClusterToEnlarge, const Cluster *const pClusterToDelete,
        const std::string &enlargeListName, const std::string &deleteListName);

    /**
     *  @brief  Add an association between a cluster and a track
     * 
     *  @param  pCluster the address of the relevant cluster
     *  @param  pTrack the address of the track with which the cluster is associated
     */
    StatusCode AddTrackAssociation(const Cluster *const pCluster, const Track *const pTrack) const;

    /**
     *  @brief  Remove an association between a cluster and a track
     * 
     *  @param  pCluster the address of the relevant cluster
     *  @param  pTrack the address of the track with which the cluster is no longer associated
     */
    StatusCode RemoveTrackAssociation(const Cluster *const pCluster, const Track *const pTrack) const;

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

} // namespace pandora

#endif // #ifndef PANDORA_CLUSTER_MANAGER_H
