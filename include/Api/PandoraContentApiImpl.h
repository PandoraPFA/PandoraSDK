/**
 *  @file   PandoraSDK/include/Api/PandoraContentApiImpl.h
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
private:
    /* Object-creation functions */

    /**
     *  @brief  Create an object for pandora
     * 
     *  @param  parameters the object parameters
     *  @param  pObject to receive the address of the object created
     */
    template <typename PARAMETERS, typename OBJECT>
    StatusCode CreateObject(const PARAMETERS &parameters, OBJECT *&pObject) const;

    /**
     *  @brief  Create a particle flow object
     * 
     *  @param  pfoParameters the particle flow object parameters
     *  @param  pPfo to receive the address of the particle flow object created
     */
    StatusCode CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObject::Parameters &pfoParameters, ParticleFlowObject *&pPfo) const;

    /**
     *  @brief  Create a vertex
     * 
     *  @param  vertexPosition the vertex position
     *  @param  pVertex to receive the address of the vertex created
     */
    StatusCode CreateVertex(const CartesianVector &vertexPosition, Vertex *&pVertex) const;


    /* Accessors for plugins and global settings */

    /**
     *  @brief  Get the pandora settings instance
     * 
     *  @return the address of the pandora settings instance
     */
    const PandoraSettings *GetSettings() const;

    /**
     *  @brief  Get the pandora geometry instance
     * 
     *  @return the address of the pandora geometry instance
     */
    const GeometryManager *GetGeometry() const;

    /**
     *  @brief  Get the pandora plugin instance, providing access to user registered functions and calculators
     * 
     *  @return the address of the pandora plugin instance
     */
    const PluginManager *GetPlugins() const;


    /* High-level steering functions */

    /**
     *  @brief  Repeat the event preparation stages, which are used to calculate properties of input objects for later use in algorithms
     */
    StatusCode RepeatEventPreparation() const;

    /**
     *  @brief  Create an algorithm tool instance, via one of the algorithm tool factories registered with pandora.
     *          This function is expected to be called whilst reading the settings for a parent algorithm.
     * 
     *  @param  pXmlElement address of the xml element describing the algorithm tool type and settings
     *  @param  pAlgorithmTool to receive the address of the algorithm tool instance
     */
    StatusCode CreateAlgorithmTool(TiXmlElement *const pXmlElement, AlgorithmTool *&pAlgorithmTool) const;

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


    /* List-manipulation functions */

    /**
     *  @brief  Get the current list
     * 
     *  @param  pT to receive the address of the current list
     *  @param  listName to receive the current list name
     */
    template <typename T>
    StatusCode GetCurrentList(const T *&pT, std::string &listName) const;

    /**
     *  @brief  Get the current list name
     * 
     *  @param  listName to receive the current list name
     */
    template <typename T>
    StatusCode GetCurrentListName(std::string &listName) const;

    /**
     *  @brief  Replace the current list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement list
     */
    template <typename T>
    StatusCode ReplaceCurrentList(const Algorithm &algorithm, const std::string &newListName) const;

    /**
     *  @brief  Drop the current list, returning the current list to its default empty/null state
     * 
     *  @param  algorithm the algorithm calling this function
     */
    template <typename T>
    StatusCode DropCurrentList(const Algorithm &algorithm) const;

    /**
     *  @brief  Get a named list
     * 
     *  @param  listName the name of the list
     *  @param  pT to receive the address of the list
     */
    template <typename T>
    StatusCode GetList(const std::string &listName, const T *&pT) const;


    /* List-manipulation functions: input objects only (CaloHits, Tracks, MCParticles) */

    /**
     *  @brief  Save a provided input object list under a new name
     * 
     *  @param  t the provided input object list
     *  @param  newListName the new list name
     */
    template <typename T>
    StatusCode SaveList(const T &t, const std::string &newListName) const;


    /* List-manipulation functions: algorithm objects only (Clusters, Pfos, Vertices) */

    /**
     *  @brief  Save the current list in a list with the specified new name. Note that this will empty the list; the objects
     *          will all be moved to the new named list.
     * 
     *  @param  newListName the new list name
     */
    template <typename T>
    StatusCode SaveList(const std::string &newListName) const;

    /**
     *  @brief  Save a named list in a list with the specified new name. Note that this will empty the old list; the objects
     *          will all be moved to the new named list.
     * 
     *  @param  oldListName the old list name
     *  @param  newListName the new list name
     */
    template <typename T>
    StatusCode SaveList(const std::string &oldListName, const std::string &newListName) const;

    /**
     *  @brief  Save elements of the current list in a list with the specified new name. If all the objects in the current
     *          list are saved, this will empty the current list; the objects will all be moved to the new named list.
     * 
     *  @param  newListName the new list name
     *  @param  t a subset of the current object list - only objects in both this and the current list will be saved
     */
    template <typename T>
    StatusCode SaveList(const std::string &newListName, const T &t) const;

    /**
     *  @brief  Save elements of a named list in a list with the specified new name. If all the objects in the old
     *          list are saved, this will empty the old list; the objects will all be moved to the new named list.
     * 
     *  @param  oldClusterListName the old cluster list name
     *  @param  newClusterListName the new cluster list name
     *  @param  t a subset of the old object list - only objects in both this and the old list will be saved
     */
    template <typename T>
    StatusCode SaveList(const std::string &oldListName, const std::string &newListName, const T &t) const;

    /**
     *  @brief  Temporarily replace the current list with another list, which may only be a temporary list. This switch
     *          will persist only for the duration of the algorithm and its daughters; unless otherwise specified, the current list
     *          will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  newListName the name of the replacement list
     */
    template <typename T>
    StatusCode TemporarilyReplaceCurrentList(const std::string &newListName) const;

    /**
     *  @brief  Create a temporary list and set it to be the current list, enabling object creation
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pT to receive the address of the temporary list
     *  @param  temporaryListName to receive the temporary list name
     */
    template <typename T>
    StatusCode CreateTemporaryListAndSetCurrent(const Algorithm &algorithm, const T *&pT, std::string &temporaryListName) const;


    /* Object-related functions */

    /**
     *  @brief  Is object, or a list of objects, available as a building block
     * 
     *  @param  pT address of the object
     * 
     *  @return boolean
     */
    template <typename T>
    bool IsAvailable(T *pT) const;


    /* Object-related functions: algorithm objects only (Clusters, Pfos, Vertices) */

    /**
     *  @brief  Delete an object from the current list
     * 
     *  @param  pT address of the object, or a list of objects, to delete
     */
    template <typename T>
    StatusCode Delete(T *pT) const;

    /**
     *  @brief  Delete an object from a specified list
     * 
     *  @param  pT address of the object, or a list of objects, to delete
     *  @param  listName name of the list containing the object     */
    template <typename T>
    StatusCode Delete(T *pT, const std::string &listName) const;


    /* CaloHit-related functions */

    /**
     *  @brief  Add a calo hit, or a list of calo hits, to a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pT address of the calo hit, or list of calo hits, to add
     */
    template <typename T>
    StatusCode AddToCluster(Cluster *pCluster, T *pT) const;

    /**
     *  @brief  Remove a calo hit from a cluster. Note this function will not remove the final calo hit from a cluster, and
     *          will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Add an isolated calo hit, or a list of isolated calo hits, to a cluster. An isolated calo hit is not counted as a
     *          regular calo hit: it contributes only towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pT address of the isolated calo hit, or list of isolated calo hits, to add
     */
    template <typename T>
    StatusCode AddIsolatedToCluster(Cluster *pCluster, T *pT) const;

    /**
     *  @brief  Remove an isolated calo hit from a cluster. Note this function will not remove the final calo hit from a cluster, and
     *          will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the isolated hit to remove
     */
    StatusCode RemoveIsolatedFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Fragment a calo hit into two daughter calo hits, with a specified energy division
     *
     *  @param  pOriginalCaloHit address of the original calo hit, which will be deleted
     *  @param  fraction1 the fraction of energy to be assigned to daughter fragment 1
     *  @param  pDaughterCaloHit1 to receive the address of daughter fragment 1
     *  @param  pDaughterCaloHit2 to receive the address of daughter fragment 2
     */
    StatusCode Fragment(CaloHit *pOriginalCaloHit, const float fraction1, CaloHit *&pDaughterCaloHit1, CaloHit *&pDaughterCaloHit2) const;

    /**
     *  @brief  Merge two calo hit fragments, originally from the same parent hit, to form a new calo hit
     *
     *  @param  pFragmentCaloHit1 address of calo hit fragment 1, which will be deleted
     *  @param  pFragmentCaloHit2 address of calo hit fragment 2, which will be deleted
     *  @param  pMergedCaloHit to receive the address of the merged calo hit
     */
    StatusCode MergeFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2, CaloHit *&pMergedCaloHit) const;


    /* Track-related functions */

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


    /* MCParticle-related functions */

    /**
     *  @brief  Repeat the mc particle preparation, performing pfo target identification and forming relationships with tracks/calo hits
     */
    StatusCode RepeatMCParticlePreparation() const;

    /**
     *  @brief  Remove all mc particle relationships previously registered with the mc manager and linked to tracks/calo hits
     */
    StatusCode RemoveAllMCParticleRelationships() const;


    /* Cluster-related functions */

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
     *  @brief  Add a cluster to a particle flow object
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to add
     */
    template <typename T>
    StatusCode AddToPfo(ParticleFlowObject *pPfo, T *pT) const;

    /**
     *  @brief  Remove a cluster from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to remove
     */
    template <typename T>
    StatusCode RemoveFromPfo(ParticleFlowObject *pPfo, T *pT) const;

    /**
     *  @brief  Set parent-daughter particle flow object relationship
     * 
     *  @param  pParentPfo address of parent particle flow object
     *  @param  pDaughterPfo address of daughter particle flow object
     */
    StatusCode SetPfoParentDaughterRelationship(ParticleFlowObject *pParentPfo, ParticleFlowObject *pDaughterPfo) const;

    /**
     *  @brief  Remove parent-daughter particle flow object relationship
     * 
     *  @param  pParentPfo address of parent particle flow object
     *  @param  pDaughterPfo address of daughter particle flow object
     */
    StatusCode RemovePfoParentDaughterRelationship(ParticleFlowObject *pParentPfo, ParticleFlowObject *pDaughterPfo) const;


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
    StatusCode InitializeReclustering(const Algorithm &algorithm, const TrackList &inputTrackList,
        const ClusterList &inputClusterList, std::string &originalClustersListName) const;

    /**
     *  @brief  End reclustering operations on clusters in the algorithm input list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  selectedClusterListName the name of the list containing the chosen recluster candidates (or the original candidates)
     */
    StatusCode EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const;

    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the pandora object to interface
     */
    PandoraContentApiImpl(Pandora *pPandora);

    /**
     *  @brief  Whether a proposed addition to a cluster is allowed
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     * 
     *  @return boolean
     */
    bool IsAddToClusterAllowed(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Prepare an object, or a list of objects, for deletion
     * 
     *  @param  pT address of the object, or list of objects, to prepare for deletion
     */
    template <typename T>
    StatusCode PrepareForDeletion(T *const pT) const;

    /**
     *  @brief  Prepare an object, or a list of objects, (formed as recluster candidates) for deletion
     * 
     *  @param  pT address of the object, or list of objects, to prepare for deletion
     */
    template <typename T>
    StatusCode PrepareForReclusteringDeletion(const T *const pT) const;

    /**
     *  @brief  Perform necessary operations prior to algorithm execution, e.g. algorithm to manager handshakes
     * 
     *  @param  pAlgorithm address of the algorithm
     */
    StatusCode PreRunAlgorithm(Algorithm *const pAlgorithm) const;

    /**
     *  @brief  Perform necessary operations after algorithm execution, e.g. preparing temporaries for deletion
     * 
     *  @param  pAlgorithm address of the algorithm
     */
    StatusCode PostRunAlgorithm(Algorithm *const pAlgorithm) const;

    Pandora    *m_pPandora;    ///< The pandora object to provide an interface to

    friend class Pandora;
    friend class PandoraImpl;
    friend class ::PandoraContentApi;
};

} // namespace pandora

#endif // #ifndef PANDORA_CONTENT_API_IMPL_H
