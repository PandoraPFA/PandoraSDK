/**
 *  @file   PandoraPFANew/Framework/include/Api/PandoraContentApiImpl.h
 *
 *  @brief  Header file for the pandora content api implementation class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CONTENT_API_IMPL_H
#define PANDORA_CONTENT_API_IMPL_H 1

#include "Api/PandoraContentApi.h"

#include "Pandora/StatusCodes.h"

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief PandoraContentApiImpl class
 */
class PandoraContentApiImpl
{
public:
    /* Object-creation functions */

    /**
     *  @brief  Create a cluster
     *
     *  @param  pClusterParameters address of either 1) a calo hit, 2) a calo hit vector or 3) a track
     *  @param  pCluster to receive the address of the cluster created
     */
    template <typename CLUSTER_PARAMETERS>
    StatusCode CreateCluster(CLUSTER_PARAMETERS *pClusterParameters, Cluster *&pCluster) const;

    /**
     *  @brief  Create a particle flow object
     * 
     *  @param  particleFlowObjectParameters the particle flow object parameters
     */
    StatusCode CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters) const;


    /* High-level steering functions */

    /**
     *  @brief  Repeat the event preparation stages, which are used to calculate properties of input objects for later use in algorithms
     */
    StatusCode RepeatEventPreparation() const;

    /**
     *  @brief  Create an algorithm instance, via one of the algorithm factories registered with pandora.
     *          This function is expected to be called whilst reading the settings for a parent algorithm.
     * 
     *  @param  pXmlElement address of the xml element describing the daughter algorithm type and settings
     *  @param  daughterAlgorithmName to receive the name of the daughter algorithm instance
     */
    StatusCode CreateDaughterAlgorithm(TiXmlElement *const pXmlElement, std::string &daughterAlgorithmName) const;

    /**
     *  @brief  Run an algorithm registered with pandora
     * 
     *  @param  algorithmName the algorithm name
     */
    StatusCode RunAlgorithm(const std::string &algorithmName) const;

    /**
     *  @brief  Run a clustering algorithm (an algorithm that will create new cluster objects)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusteringAlgorithmName the name of the clustering algorithm to run
     *  @param  pNewClusterList the address of the new cluster list populated
     *  @param  newClusterListName the name of the new cluster list populated
     */
     StatusCode RunClusteringAlgorithm(const Algorithm &algorithm, const std::string &clusteringAlgorithmName,
        const ClusterList *&pNewClusterList, std::string &newClusterListName) const;


    /* CaloHit-related functions */

    /**
     *  @brief  Get the current calo hit list
     * 
     *  @param  pCaloHitList to receive the address of the current calo hit list
     *  @param  caloHitListName to receive the current calo hit list name
     */
    StatusCode GetCurrentCaloHitList(const CaloHitList *&pCaloHitList, std::string &caloHitListName) const;

    /**
     *  @brief  Get the current calo hit list name
     * 
     *  @param  caloHitListName to receive the current calo hit list name
     */
    StatusCode GetCurrentCaloHitListName(std::string &caloHitListName) const;

    /**
     *  @brief  Get a named calo hit list
     * 
     *  @param  caloHitListName the name of the calo hit list
     *  @param  pCaloHitList to receive the address of the calo hit list
     */
    StatusCode GetCaloHitList(const std::string &caloHitListName, const CaloHitList *&pCaloHitList) const;

    /**
     *  @brief  Save the current calo hit list under a new name
     * 
     *  @param  newListName the new calo hit list name
     */
    StatusCode SaveCaloHitList(const CaloHitList &caloHitList, const std::string &newListName) const;

    /**
     *  @brief  Replace the current calo hit list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement calo hit list
     */
    StatusCode ReplaceCurrentCaloHitList(const Algorithm &algorithm, const std::string &newListName) const;

    /**
     *  @brief  Drop the current calo hit list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentCaloHitList() const;

    /**
     *  @brief  Is calo hit available to add to a cluster
     * 
     *  @param  pCaloHit address of the calo hit
     * 
     *  @return boolean
     */
    bool IsCaloHitAvailable(pandora::CaloHit *pCaloHit) const;

    /**
     *  @brief  Are all calo hits in list available to add to a cluster
     * 
     *  @param  caloHitList the list of calo hits
     * 
     *  @return boolean
     */
    bool AreCaloHitsAvailable(const pandora::CaloHitList &caloHitList) const;

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Remove a calo hit from a cluster. Note this function will not remove the final calo hit from a cluster, and
     *          will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Add an isolated calo hit to a cluster. This is not counted as a regular calo hit: it contributes only
     *          towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the isolated hit to add
     */
    StatusCode AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Remove an isolated calo hit from a cluster.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the isolated hit to remove
     */
    StatusCode RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Fragment a calo hit into two daughter calo hits, with a specified energy division
     *
     *  @param  pOriginalCaloHit address of the original calo hit, which will be deleted
     *  @param  fraction1 the fraction of energy to be assigned to daughter fragment 1
     *  @param  pDaughterCaloHit1 to receive the address of daughter fragment 1
     *  @param  pDaughterCaloHit2 to receive the address of daughter fragment 2
     */
    StatusCode FragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1, CaloHit *&pDaughterCaloHit1, CaloHit *&pDaughterCaloHit2) const;

    /**
     *  @brief  Merge two calo hit fragments, originally from the same parent hit, to form a new calo hit
     *
     *  @param  pFragmentCaloHit1 address of calo hit fragment 1, which will be deleted
     *  @param  pFragmentCaloHit2 address of calo hit fragment 2, which will be deleted
     *  @param  pMergedCaloHit to receive the address of the merged calo hit
     */
    StatusCode MergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2, CaloHit *&pMergedCaloHit) const;


    /* Track-related functions */

    /**
     *  @brief  Get the current track list
     * 
     *  @param  pTrackList to receive the address of the current track list
     *  @param  trackListName to receive the current track list name
     */
    StatusCode GetCurrentTrackList(const TrackList *&pTrackList, std::string &trackListName) const;

    /**
     *  @brief  Get the current track list name
     * 
     *  @param  trackListName to receive the current track list name
     */
    StatusCode GetCurrentTrackListName(std::string &trackListName) const;

    /**
     *  @brief  Get a named track list
     * 
     *  @param  trackListName the name of the track list
     *  @param  pTrackList to receive the address of the track list
     */
    StatusCode GetTrackList(const std::string &trackListName, const TrackList *&pTrackList) const;

    /**
     *  @brief  Save the current track list under a new name
     * 
     *  @param  newListName the new track list name
     */
    StatusCode SaveTrackList(const TrackList &trackList, const std::string &newListName) const;

    /**
     *  @brief  Replace the current track list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement track list
     */
    StatusCode ReplaceCurrentTrackList(const Algorithm &algorithm, const std::string &newListName) const;

    /**
     *  @brief  Drop the current track list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentTrackList() const;

    /**
     *  @brief  Add an association between a track and a cluster
     * 
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     */
    StatusCode AddTrackClusterAssociation(Track *const pTrack, Cluster *const pCluster) const;

    /**
     *  @brief  Remove an association between a track and a cluster
     * 
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     */
    StatusCode RemoveTrackClusterAssociation(Track *const pTrack, Cluster *const pCluster) const;

    /**
     *  @brief  Remove all track-cluster associations from objects in the current track and cluster lists
     */
    StatusCode RemoveCurrentTrackClusterAssociations() const;

    /**
     *  @brief  Remove all associations between tracks and clusters
     */
    StatusCode RemoveAllTrackClusterAssociations() const;


    /* Cluster-related functions */

    /**
     *  @brief  Get the current cluster list
     * 
     *  @param  pClusterList to receive the address of the current cluster list
     *  @param  clusterListName to receive the current cluster list name
     */
    StatusCode GetCurrentClusterList(const ClusterList *&pClusterList, std::string &clusterListName) const;

    /**
     *  @brief  Get the current cluster list name
     * 
     *  @param  clusterListName to receive the current cluster list name
     */
    StatusCode GetCurrentClusterListName(std::string &clusterListName) const;

    /**
     *  @brief  Get a named cluster list
     * 
     *  @param  clusterListName the name of the cluster list
     *  @param  pClusterList to receive the address of the cluster list
     */
    StatusCode GetClusterList(const std::string &clusterListName, const ClusterList *&pClusterList) const;

    /**
     *  @brief  Make a temporary cluster list and set it to be the current list, enabling cluster creation
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterList to receive the address of the temporary cluster list
     *  @param  temporaryListName to receive the temporary list name
     */
    StatusCode CreateTemporaryClusterListAndSetCurrent(const Algorithm &algorithm, const ClusterList *&pClusterList,
        std::string &temporaryListName) const;

    /**
     *  @brief  Save the current cluster list in a list with the specified new name. Note that this will empty the current
     *          cluster list; the clusters will all be moved to the new named list.
     * 
     *  @param  newClusterListName the new cluster list name
     */
    StatusCode SaveClusterList(const std::string &newClusterListName) const;

    /**
     *  @brief  Save elements of the current cluster list in a list with the specified new name. If all the clusters in the
     *          current list are saved, this will empty the current list; the clusters will all be moved to the new named list.
     * 
     *  @param  newClusterListName the new cluster list name
     *  @param  clustersToSave a subset of the current cluster list - only clusters in both this and the current list will be saved
     */
    StatusCode SaveClusterList(const std::string &newClusterListName, const ClusterList &clustersToSave) const;

    /**
     *  @brief  Save a named cluster list in a list with the specified new name. Note that this will empty the old cluster list;
     *          the clusters will all be moved to the new named list.
     * 
     *  @param  oldClusterListName the old cluster list name
     *  @param  newClusterListName the new cluster list name
     */
    StatusCode SaveClusterList(const std::string &oldClusterListName,  const std::string &newClusterListName) const;

    /**
     *  @brief  Save elements of a named cluster list in a list with the specified new name. If all the clusters in the old
     *          list are saved, this will empty the old cluster list; the clusters will all be moved to the new named list.
     * 
     *  @param  oldClusterListName the old cluster list name
     *  @param  newClusterListName the new cluster list name
     *  @param  clustersToSave a subset of the old cluster list - only clusters in both this and the old cluster list will be saved
     */
    StatusCode SaveClusterList(const std::string &oldClusterListName, const std::string &newClusterListName, const ClusterList &clustersToSave) const;

    /**
     *  @brief  Replace the current cluster list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the name of the replacement cluster list
     */
    StatusCode ReplaceCurrentClusterList(const Algorithm &algorithm, const std::string &newClusterListName) const;

    /**
     *  @brief  Temporarily replace the current cluster list with another list, which may only be a temporary list. This switch
     *          will persist only for the duration of the algorithm and its daughters; unless otherwise specified, the current list
     *          will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  newClusterListName the name of the replacement cluster list
     */
    StatusCode TemporarilyReplaceCurrentClusterList(const std::string &newClusterListName) const;

    /**
     *  @brief  Drop the current cluster list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentClusterList() const;

    /**
     *  @brief  Delete a cluster in the current list
     * 
     *  @param  pCluster address of the cluster to delete
     */
    StatusCode DeleteCluster(Cluster *pCluster) const;

    /**
     *  @brief  Delete a cluster from a specified list
     * 
     *  @param  pCluster address of the cluster to delete
     *  @param  clusterListName name of the list containing the cluster
     */
    StatusCode DeleteCluster(Cluster *pCluster, const std::string &clusterListName) const;

    /**
     *  @brief  Delete a list of clusters from the current list
     * 
     *  @param  clusterList the list of clusters to delete
     */
    StatusCode DeleteClusters(const ClusterList &clusterList) const;

    /**
     *  @brief  Delete a list of clusters from a specified list
     * 
     *  @param  clusterList the list of clusters to delete
     *  @param  clusterListName name of the list containing the clusters
     */
     StatusCode DeleteClusters(const ClusterList &clusterList, const std::string &clusterListName) const;

    /**
     *  @brief  Merge two clusters in the current list, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete) const;

    /**
     *  @brief  Merge two clusters from two specified lists, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     *  @param  enlargeListName name of the list containing the cluster to enlarge
     *  @param  deleteListName name of the list containing the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
        const std::string &deleteListName) const;


    /* Pfo-related functions */

    /**
     *  @brief  Get the current pfo list
     * 
     *  @param  pPfoList to receive the address of the current pfo list
     *  @param  pfoListName to receive the current pfo list name
     */
    StatusCode GetCurrentPfoList(const PfoList *&pPfoList, std::string &pfoListName) const;

    /**
     *  @brief  Get the current pfo list name
     * 
     *  @param  pfoListName to receive the current pfo list name
     */
    StatusCode GetCurrentPfoListName(std::string &pfoListName) const;

    /**
     *  @brief  Get a named pfo list
     * 
     *  @param  pfoListName the name of the pfo list
     *  @param  pPfoList to receive the address of the pfo list
     */
    StatusCode GetPfoList(const std::string &pfoListName, const PfoList *&pPfoList) const;

    /**
     *  @brief  Make a temporary pfo list and set it to be the current list, enabling pfo creation
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfoList to receive the address of the temporary pfo list
     *  @param  temporaryListName to receive the temporary list name
     */
    StatusCode CreateTemporaryPfoListAndSetCurrent(const Algorithm &algorithm, const PfoList *&pPfoList,
        std::string &temporaryListName) const;

    /**
     *  @brief  Save the current pfo list in a list with the specified new name. Note that this will empty the current
     *          pfo list; the pfos will all be moved to the new named list.
     * 
     *  @param  newPfoListName the new pfo list name
     */
    StatusCode SavePfoList(const std::string &newPfoListName) const;

    /**
     *  @brief  Save elements of the current pfo list in a list with the specified new name. If all the pfos in the
     *          current list are saved, this will empty the current list; the pfos will all be moved to the new named list.
     * 
     *  @param  newPfoListName the new pfo list name
     *  @param  pfosToSave a subset of the current pfo list - only pfos in both this and the current list will be saved
     */
    StatusCode SavePfoList(const std::string &newPfoListName, const PfoList &pfosToSave) const;

    /**
     *  @brief  Save a named pfo list in a list with the specified new name. Note that this will empty the old pfo list;
     *          the pfos will all be moved to the new named list.
     * 
     *  @param  oldPfoListName the old pfo list name
     *  @param  newPfoListName the new pfo list name
     */
    StatusCode SavePfoList(const std::string &oldPfoListName,  const std::string &newPfoListName) const;

    /**
     *  @brief  Save elements of a named pfo list in a list with the specified new name. If all the pfos in the old
     *          list are saved, this will empty the old pfo list; the pfos will all be moved to the new named list.
     * 
     *  @param  oldPfoListName the old pfo list name
     *  @param  newPfoListName the new pfo list name
     *  @param  pfosToSave a subset of the old pfo list - only pfos in both this and the old pfo list will be saved
     */
    StatusCode SavePfoList(const std::string &oldPfoListName, const std::string &newPfoListName, const PfoList &pfosToSave) const;

    /**
     *  @brief  Replace the current pfo list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newPfoListName the name of the replacement pfo list
     */
    StatusCode ReplaceCurrentPfoList(const Algorithm &algorithm, const std::string &newPfoListName) const;

    /**
     *  @brief  Temporarily replace the current pfo list with another list, which may only be a temporary list. This switch
     *          will persist only for the duration of the algorithm and its daughters; unless otherwise specified, the current list
     *          will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  newPfoListName the name of the replacement pfo list
     */
    StatusCode TemporarilyReplaceCurrentPfoList(const std::string &newPfoListName) const;

    /**
     *  @brief  Drop the current pfo list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentPfoList() const;

    /**
     *  @brief  Delete a pfo in the current list
     * 
     *  @param  pPfo address of the pfo to delete
     */
    StatusCode DeletePfo(ParticleFlowObject *pPfo) const;

    /**
     *  @brief  Delete a pfo from a specified list
     * 
     *  @param  pPfo address of the pfo to delete
     *  @param  pfoListName name of the list containing the pfo
     */
    StatusCode DeletePfo(ParticleFlowObject *pPfo, const std::string &pfoListName) const;

    /**
     *  @brief  Delete a list of pfos from the current list
     * 
     *  @param  pfoList the list of pfos to delete
     */
    StatusCode DeletePfos(const PfoList &pfoList) const;

    /**
     *  @brief  Delete a list of pfos from a specified list
     * 
     *  @param  pfoList the list of pfos to delete
     *  @param  pfoListName name of the list containing the pfos
     */
     StatusCode DeletePfos(const PfoList &pfoList, const std::string &pfoListName) const;

    /**
     *  @brief  Add a cluster to a particle flow object
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pPfo address of the cluster to add
     */
    StatusCode AddClusterToPfo(ParticleFlowObject *pPfo, Cluster *pCluster) const;

    /**
     *  @brief  Add a track to a particle flow object
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pTrack address of the track to add
     */
    StatusCode AddTrackToPfo(ParticleFlowObject *pPfo, Track *pTrack) const;

    /**
     *  @brief  Remove a cluster from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to remove
     */
    StatusCode RemoveClusterFromPfo(ParticleFlowObject *pPfo, Cluster *pCluster) const;

    /**
     *  @brief  Remove a track from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pTrack address of the track to remove
     */
    StatusCode RemoveTrackFromPfo(ParticleFlowObject *pPfo, Track *pTrack) const;


    /* MCParticle-related functions */

    /**
     *  @brief  Get the current mc particle list
     * 
     *  @param  pMCParticleList to receive the address of the current mc particle list
     *  @param  mcParticleListName to receive the current mc particle list name
     */
    StatusCode GetCurrentMCParticleList(const MCParticleList *&pMCParticleList, std::string &mcParticleListName) const;

    /**
     *  @brief  Get the current mc particle list name
     * 
     *  @param  mcParticleListName to receive the current mc particle list name
     */
    StatusCode GetCurrentMCParticleListName(std::string &mcParticleListName) const;

    /**
     *  @brief  Get a named mc particle list
     * 
     *  @param  mcParticleListName the name of the mc particle list
     *  @param  pMCParticleList to receive the address of the mc particle list
     */
    StatusCode GetMCParticleList(const std::string &mcParticleListName, const MCParticleList *&pMCParticleList) const;

    /**
     *  @brief  Save the current mc particle list under a new name
     * 
     *  @param  newListName the new mc particle list name
     */
    StatusCode SaveMCParticleList(const MCParticleList &mcParticleList, const std::string &newListName) const;

    /**
     *  @brief  Replace the current mc particle list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement mc particle list
     */
    StatusCode ReplaceCurrentMCParticleList(const Algorithm &algorithm, const std::string &newListName) const;

    /**
     *  @brief  Drop the current mc particle list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentMCParticleList() const;

    /**
     *  @brief  Repeat the mc particle preparation, performing pfo target identification and forming relationships with tracks/calo hits
     */
    StatusCode RepeatMCParticlePreparation() const;

    /**
     *  @brief  Remove all mc particle relationships previously registered with the mc manager and linked to tracks/calo hits
     */
    StatusCode RemoveAllMCParticleRelationships() const;


    /* Reclustering functions */

    /**
     *  @brief  Initialize cluster fragmentation operations on clusters in the algorithm input list. This allows hits in a list
     *          of clusters (a subset of the algorithm input list) to be redistributed.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  inputClusterList the input cluster list
     *  @param  originalClustersListName to receive the name of the list in which the original clusters are stored
     *  @param  fragmentClustersListName to receive the name of the list in which the fragment clusters are stored
     */
    StatusCode InitializeFragmentation(const Algorithm &algorithm, const ClusterList &inputClusterList,
        std::string &originalClustersListName, std::string &fragmentClustersListName) const;

    /**
     *  @brief  End cluster fragmentation operations on clusters in the algorithm input list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterListToSaveName the name of the list containing the clusters chosen to be saved (original or fragments)
     *  @param  clusterListToDeleteName the name of the list containing the clusters chosen to be deleted (original or fragments)
     */
    StatusCode EndFragmentation(const Algorithm &algorithm, const std::string &clusterListToSaveName,
        const std::string &clusterListToDeleteName) const;

    /**
     *  @brief  Initialize reclustering operations on clusters in the algorithm input list. This allows hits in a list
     *          of clusters (a subset of the algorithm input list) to be redistributed.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  inputTrackList the input track list
     *  @param  inputClusterList the input cluster list
     *  @param  originalClustersListName to receive the name of the list in which the original clusters are stored
     */
    StatusCode InitializeReclustering(const Algorithm &algorithm, const TrackList &inputTrackList, const ClusterList &inputClusterList,
        std::string &originalClustersListName) const;

    /**
     *  @brief  End reclustering operations on clusters in the algorithm input list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pandora the pandora instance performing reclustering
     *  @param  selectedClusterListName the name of the list containing the chosen recluster candidates (or the original candidates)
     */
    StatusCode EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the pandora object to interface
     */
    PandoraContentApiImpl(Pandora *pPandora);

    /**
     *  @brief  Prepare a cluster for deletion, flagging constituent calo hits as available and removing track associations
     * 
     *  @param  pCluster address of the cluster to prepare for deletion
     */
    StatusCode PrepareForDeletion(const Cluster *const pCluster) const;

    /**
     *  @brief  Prepare a list of clusters for deletion, flagging constituent calo hits as available and removing track associations
     * 
     *  @param  clusterList the list of clusters to prepare for deletion
     */
    StatusCode PrepareForDeletion(const ClusterList &clusterList) const;

    /**
     *  @brief  Prepare a pfo for deletion, flagging constituents as available
     * 
     *  @param  pPfo address of the pfo to prepare for deletion
     */
    StatusCode PrepareForDeletion(const ParticleFlowObject *const pPfo) const;

    /**
     *  @brief  Prepare a list of pfos for deletion, flagging constituents as available
     * 
     *  @param  pfoList the list of pfos to prepare for deletion
     */
    StatusCode PrepareForDeletion(const PfoList &pfoList) const;

    /**
     *  @brief  Prepare a list of clusters (formed as recluster candidates) for deletion, removing any track associations.
     * 
     *  @param  clusterList the list of clusters to prepare for deletion
     */
    StatusCode PrepareForReclusteringDeletion(const ClusterList &clusterList) const;

    Pandora    *m_pPandora;    ///< The pandora object to provide an interface to

    friend class Pandora;
};

} // namespace pandora

#endif // #ifndef PANDORA_CONTENT_API_IMPL_H
