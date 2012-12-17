/**
 *  @file   PandoraPFANew/Framework/include/Api/PandoraContentApi.h
 *
 *  @brief  Header file for the pandora content api class.
 *
 *  $Log: $
 */
#ifndef PANDORA_CONTENT_API_H
#define PANDORA_CONTENT_API_H 1

#include "Pandora/Pandora.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace pandora { class Algorithm; class TiXmlElement; class TiXmlHandle;}

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraContentApi class
 */
class PandoraContentApi
{
public:
    /* Object-creation functions */

    /**
     *  @brief  Cluster creation class
     */
    class Cluster
    {
    public:
        /**
         *  @brief  Create a cluster
         *
         *  @param  algorithm the algorithm creating the cluster
         *  @param  pClusterParameters address of either 1) a single calo hit, 2) a calo hit vector, or 3) a track
         */
        template <typename CLUSTER_PARAMETERS>
        static pandora::StatusCode Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters);

        /**
         *  @brief  Create a cluster
         *
         *  @param  algorithm the algorithm creating the cluster
         *  @param  pClusterParameters address of either 1) a single calo hit, 2) a calo hit vector, or 3) a track
         *  @param  pCluster to receive the address of the cluster created
         */
        template <typename CLUSTER_PARAMETERS>
        static pandora::StatusCode Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters, pandora::Cluster *&pCluster);
    };

    /**
     *  @brief  Particle flow object creation class
     */
    class ParticleFlowObject
    {
    public:
        /**
         *  @brief  Parameters class
         */
        class Parameters
        {
        public:
            pandora::InputInt               m_particleId;       ///< The particle flow object id (PDG code)
            pandora::InputInt               m_charge;           ///< The particle flow object charge
            pandora::InputFloat             m_mass;             ///< The particle flow object mass
            pandora::InputFloat             m_energy;           ///< The particle flow object energy
            pandora::InputCartesianVector   m_momentum;         ///< The particle flow object momentum
            pandora::ClusterList            m_clusterList;      ///< The clusters in the particle flow object
            pandora::TrackList              m_trackList;        ///< The tracks in the particle flow object
        };

        /**
         *  @brief  Create a particle flow object
         * 
         *  @param  algorithm the algorithm creating the particle flow object
         *  @param  particleFlowObjectParameters the particle flow object parameters
         */
        static pandora::StatusCode Create(const pandora::Algorithm &algorithm, const Parameters &parameters);
    };

    typedef ParticleFlowObject::Parameters ParticleFlowObjectParameters;


    /* High-level steering functions */

    /**
     *  @brief  Repeat the event preparation stages, which are used to calculate properties of input objects for later use in algorithms
     *
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode RepeatEventPreparation(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Create an algorithm instance, via one of the algorithm factories registered with pandora.
     *          This function is expected to be called whilst reading the settings for a parent algorithm.
     * 
     *  @param  parentAlgorithm address of the parent algorithm, which will later run this daughter algorithm
     *  @param  pXmlElement address of the xml element describing the daughter algorithm type and settings
     *  @param  daughterAlgorithmName to receive the name of the daughter algorithm instance
     */
    static pandora::StatusCode CreateDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, pandora::TiXmlElement *const pXmlElement,
        std::string &daughterAlgorithmName);

    /**
     *  @brief  Run an algorithm registered with pandora, from within a parent algorithm
     * 
     *  @param  parentAlgorithm address of the parent algorithm, now attempting to run a daughter algorithm
     *  @param  daughterAlgorithmName the name of the daughter algorithm instance to run
     */
    static pandora::StatusCode RunDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, const std::string &daughterAlgorithmName);

    /**
     *  @brief  Run a clustering algorithm (an algorithm that will create new cluster objects)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusteringAlgorithmName the name of the clustering algorithm to run
     *  @param  pNewClusterList to receive the address of the new cluster list populated
     */
    static pandora::StatusCode RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
        const pandora::ClusterList *&pNewClusterList);

    /**
     *  @brief  Run a clustering algorithm (an algorithm that will create new cluster objects)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusteringAlgorithmName the name of the clustering algorithm to run
     *  @param  pNewClusterList to receive the address of the new cluster list populated
     *  @param  newClusterListName to receive the name of the new cluster list populated
     */
    static pandora::StatusCode RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
        const pandora::ClusterList *&pNewClusterList, std::string &newClusterListName);


    /* CaloHit-related functions */

    /**
     *  @brief  Get the current calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pCaloHitList to receive the address of the current calo hit list
     */
    static pandora::StatusCode GetCurrentCaloHitList(const pandora::Algorithm &algorithm, const pandora::CaloHitList *&pCaloHitList);

    /**
     *  @brief  Get the current calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pCaloHitList to receive the address of the current calo hit list
     *  @param  caloHitListName to receive the current calo hit list name
     */
    static pandora::StatusCode GetCurrentCaloHitList(const pandora::Algorithm &algorithm, const pandora::CaloHitList *&pCaloHitList,
        std::string &caloHitListName);

    /**
     *  @brief  Get the current calo hit list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  caloHitListName to receive the current calo hit list name
     */
    static pandora::StatusCode GetCurrentCaloHitListName(const pandora::Algorithm &algorithm, std::string &caloHitListName);

    /**
     *  @brief  Get a named calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  caloHitListName the name of the calo hit list
     *  @param  pCaloHitList to receive the address of the calo hit list
     */
    static pandora::StatusCode GetCaloHitList(const pandora::Algorithm &algorithm, const std::string &caloHitListName,
        const pandora::CaloHitList *&pCaloHitList);

    /**
     *  @brief  Save the current calo hit list under a new name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new calo hit list name
     */
    static pandora::StatusCode SaveCaloHitList(const pandora::Algorithm &algorithm, const pandora::CaloHitList &caloHitList,
        const std::string &newListName);

    /**
     *  @brief  Replace the current calo hit list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement calo hit list
     */
    static pandora::StatusCode ReplaceCurrentCaloHitList(const pandora::Algorithm &algorithm, const std::string &newListName);

    /**
     *  @brief  Drop the current calo hit list, returning the current list to its default empty/null state
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode DropCurrentCaloHitList(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Is calo hit available to add to a cluster
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pCaloHit address of the calo hit
     * 
     *  @return boolean
     */
    static bool IsCaloHitAvailable(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit);

    /**
     *  @brief  Are all calo hits in list available to add to a cluster
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  caloHitList the list of calo hits
     * 
     *  @return boolean
     */
    static bool AreCaloHitsAvailable(const pandora::Algorithm &algorithm, const pandora::CaloHitList &caloHitList);

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    static pandora::StatusCode AddCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
        pandora::CaloHit *pCaloHit);

    /**
     *  @brief  Remove a calo hit from a cluster. Note this function will not remove the final calo hit from a cluster, and
     *          will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    static pandora::StatusCode RemoveCaloHitFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
        pandora::CaloHit *pCaloHit);

    /**
     *  @brief  Add an isolated calo hit to a cluster. This is not counted as a regular calo hit: it contributes only
     *          towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the isolated hit to add
     */
    static pandora::StatusCode AddIsolatedCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
        pandora::CaloHit *pCaloHit);

    /**
     *  @brief  Remove an isolated calo hit from a cluster.
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the isolated hit to remove
     */
    static pandora::StatusCode RemoveIsolatedCaloHitFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
        pandora::CaloHit *pCaloHit);

    /**
     *  @brief  Fragment a calo hit into two daughter calo hits, with a specified energy division
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pOriginalCaloHit address of the original calo hit, which will be deleted
     *  @param  fraction1 the fraction of energy to be assigned to daughter fragment 1
     *  @param  pDaughterCaloHit1 to receive the address of daughter fragment 1
     *  @param  pDaughterCaloHit2 to receive the address of daughter fragment 2
     */
    static pandora::StatusCode FragmentCaloHit(const pandora::Algorithm &algorithm, pandora::CaloHit *pOriginalCaloHit,
        const float fraction1, pandora::CaloHit *&pDaughterCaloHit1, pandora::CaloHit *&pDaughterCaloHit2);

    /**
     *  @brief  Merge two calo hit fragments, originally from the same parent hit, to form a new calo hit
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pFragmentCaloHit1 address of calo hit fragment 1, which will be deleted
     *  @param  pFragmentCaloHit2 address of calo hit fragment 2, which will be deleted
     *  @param  pMergedCaloHit to receive the address of the merged calo hit
     */
    static pandora::StatusCode MergeCaloHitFragments(const pandora::Algorithm &algorithm, pandora::CaloHit *pFragmentCaloHit1,
        pandora::CaloHit *pFragmentCaloHit2, pandora::CaloHit *&pMergedCaloHit);


    /* Track-related functions */

    /**
     *  @brief  Get the current track list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pTrackList to receive the address of the current track list
     */
    static pandora::StatusCode GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList);

    /**
     *  @brief  Get the current track list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pTrackList to receive the address of the current track list
     *  @param  trackListName to receive the current track list name
     */
    static pandora::StatusCode GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList,
        std::string &trackListName);

    /**
     *  @brief  Get the current track list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  trackListName to receive the current track list name
     */
    static pandora::StatusCode GetCurrentTrackListName(const pandora::Algorithm &algorithm, std::string &trackListName);

    /**
     *  @brief  Get a named track list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  trackListName the name of the track list
     *  @param  pTrackList to receive the address of the track list
     */
    static pandora::StatusCode GetTrackList(const pandora::Algorithm &algorithm, const std::string &trackListName,
        const pandora::TrackList *&pTrackList);

    /**
     *  @brief  Save the current track list under a new name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new track list name
     */
    static pandora::StatusCode SaveTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList &trackList,
        const std::string &newListName);

    /**
     *  @brief  Replace the current track list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement track list
     */
    static pandora::StatusCode ReplaceCurrentTrackList(const pandora::Algorithm &algorithm, const std::string &newListName);

    /**
     *  @brief  Drop the current track list, returning the current list to its default empty/null state
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode DropCurrentTrackList(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Add an association between a track and a cluster
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     */
    static pandora::StatusCode AddTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
        pandora::Cluster *const pCluster);

    /**
     *  @brief  Remove an association between a track and a cluster
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     */
    static pandora::StatusCode RemoveTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
        pandora::Cluster *const pCluster);

    /**
     *  @brief  Remove all track-cluster associations from objects in the current track and cluster lists
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode RemoveCurrentTrackClusterAssociations(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Remove all associations between tracks and clusters
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode RemoveAllTrackClusterAssociations(const pandora::Algorithm &algorithm);


    /* Cluster-related functions */

    /**
     *  @brief  Get the current cluster list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterList to receive the address of the current cluster list
     */
    static pandora::StatusCode GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList);

    /**
     *  @brief  Get the current cluster list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterList to receive the address of the current cluster list
     *  @param  clusterListName to receive the current cluster list name
     */
    static pandora::StatusCode GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList,
        std::string &clusterListName);

    /**
     *  @brief  Get the current cluster list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterListName to receive the current cluster list name
     */
    static pandora::StatusCode GetCurrentClusterListName(const pandora::Algorithm &algorithm, std::string &clusterListName);

    /**
     *  @brief  Get a named cluster list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterListName the name of the cluster list
     *  @param  pClusterList to receive the address of the cluster list
     */
    static pandora::StatusCode GetClusterList(const pandora::Algorithm &algorithm, const std::string &clusterListName,
        const pandora::ClusterList *&pClusterList);

    /**
     *  @brief  Create a temporary cluster list and set it to be the current list, enabling cluster creation
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterList to receive the address of the temporary cluster list
     *  @param  temporaryListName to receive the temporary list name
     */
    static pandora::StatusCode CreateTemporaryClusterListAndSetCurrent(const pandora::Algorithm &algorithm,
        const pandora::ClusterList *&pClusterList, std::string &temporaryListName);

    /**
     *  @brief  Save the current cluster list in a list with the specified new name. Note that this will empty the current
     *          cluster list; the clusters will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     */
    static pandora::StatusCode SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName);

    /**
     *  @brief  Save elements of the current cluster list in a list with the specified new name. If all the clusters in the
     *          current list are saved, this will empty the current list; the clusters will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     *  @param  clustersToSave a subset of the current cluster list - only clusters in both this and the current list will be saved
     */
    static pandora::StatusCode SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
        const pandora::ClusterList &clustersToSave);

    /**
     *  @brief  Save a named cluster list in a list with the specified new name. Note that this will empty the old cluster list;
     *          the clusters will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  oldClusterListName the old cluster list name
     *  @param  newClusterListName the new cluster list name
     */
    static pandora::StatusCode SaveClusterList(const pandora::Algorithm &algorithm, const std::string &oldClusterListName,
        const std::string &newClusterListName);

    /**
     *  @brief  Save elements of a named cluster list in a list with the specified new name. If all the clusters in the old
     *          list are saved, this will empty the old cluster list; the clusters will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  oldClusterListName the old cluster list name
     *  @param  newClusterListName the new cluster list name
     *  @param  clustersToSave a subset of the old cluster list - only clusters in both this and the old cluster list will be saved
     */
    static pandora::StatusCode SaveClusterList(const pandora::Algorithm &algorithm, const std::string &oldClusterListName,
        const std::string &newClusterListName, const pandora::ClusterList &clustersToSave);

    /**
     *  @brief  Replace the current cluster list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the name of the replacement cluster list
     */
    static pandora::StatusCode ReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName);

    /**
     *  @brief  Temporarily replace the current cluster list with another list, which may only be a temporary list. This switch
     *          will persist only for the duration of the algorithm and its daughters; unless otherwise specified, the current list
     *          will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the name of the replacement cluster list
     */
    static pandora::StatusCode TemporarilyReplaceCurrentClusterList(const pandora::Algorithm &algorithm,
        const std::string &newClusterListName);

    /**
     *  @brief  Drop the current cluster list, returning the current list to its default empty/null state
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode DropCurrentClusterList(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Delete a cluster from the current list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to delete
     */
    static pandora::StatusCode DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster);

    /**
     *  @brief  Delete a cluster from a specified list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to delete
     *  @param  clusterListName name of the list containing the cluster
     */
    static pandora::StatusCode DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
        const std::string &clusterListName);

    /**
     *  @brief  Delete a list of clusters from the current list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterList the list of clusters to delete
     */
    static pandora::StatusCode DeleteClusters(const pandora::Algorithm &algorithm, const pandora::ClusterList &clusterList);

    /**
     *  @brief  Delete a list of clusters from a specified list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterList the list of clusters to delete
     *  @param  clusterListName name of the list containing the clusters
     */
    static pandora::StatusCode DeleteClusters(const pandora::Algorithm &algorithm, const pandora::ClusterList &clusterList,
        const std::string &clusterListName);

    /**
     *  @brief  Merge two clusters in the current list, enlarging one cluster and deleting the second
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     */
    static pandora::StatusCode MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
        pandora::Cluster *pClusterToDelete);

    /**
     *  @brief  Merge two clusters from two specified lists, enlarging one cluster and deleting the second
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     *  @param  enlargeListName name of the list containing the cluster to enlarge
     *  @param  deleteListName name of the list containing the cluster to delete
     */
    static pandora::StatusCode MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
        pandora::Cluster *pClusterToDelete, const std::string &enlargeListName, const std::string &deleteListName);


    /* Pfo-related functions */

    /**
     *  @brief  Get the current pfo list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfoList to receive the address of the current pfo list
     */
    static pandora::StatusCode GetCurrentPfoList(const pandora::Algorithm &algorithm, const pandora::PfoList *&pPfoList);

    /**
     *  @brief  Get the current pfo list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfoList to receive the address of the current pfo list
     *  @param  pfoListName to receive the current pfo list name
     */
    static pandora::StatusCode GetCurrentPfoList(const pandora::Algorithm &algorithm, const pandora::PfoList *&pPfoList,
        std::string &pfoListName);

    /**
     *  @brief  Get the current pfo list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pfoListName to receive the current pfo list name
     */
    static pandora::StatusCode GetCurrentPfoListName(const pandora::Algorithm &algorithm, std::string &pfoListName);

    /**
     *  @brief  Get a named pfo list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pfoListName the name of the pfo list
     *  @param  pPfoList to receive the address of the pfo list
     */
    static pandora::StatusCode GetPfoList(const pandora::Algorithm &algorithm, const std::string &pfoListName,
        const pandora::PfoList *&pPfoList);

    /**
     *  @brief  Create a temporary pfo list and set it to be the current list, enabling pfo creation
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfoList to receive the address of the temporary pfo list
     *  @param  temporaryListName to receive the temporary list name
     */
    static pandora::StatusCode CreateTemporaryPfoListAndSetCurrent(const pandora::Algorithm &algorithm,
        const pandora::PfoList *&pPfoList, std::string &temporaryListName);

    /**
     *  @brief  Save the current pfo list in a list with the specified new name. Note that this will empty the current
     *          pfo list; the pfos will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newPfoListName the new pfo list name
     */
    static pandora::StatusCode SavePfoList(const pandora::Algorithm &algorithm, const std::string &newPfoListName);

    /**
     *  @brief  Save elements of the current pfo list in a list with the specified new name. If all the pfos in the
     *          current list are saved, this will empty the current list; the pfos will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newPfoListName the new pfo list name
     *  @param  pfosToSave a subset of the current pfo list - only pfos in both this and the current list will be saved
     */
    static pandora::StatusCode SavePfoList(const pandora::Algorithm &algorithm, const std::string &newPfoListName,
        const pandora::PfoList &pfosToSave);

    /**
     *  @brief  Save a named pfo list in a list with the specified new name. Note that this will empty the old pfo list;
     *          the pfos will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  oldPfoListName the old pfo list name
     *  @param  newPfoListName the new pfo list name
     */
    static pandora::StatusCode SavePfoList(const pandora::Algorithm &algorithm, const std::string &oldPfoListName,
        const std::string &newPfoListName);

    /**
     *  @brief  Save elements of a named cluster list in a list with the specified new name. If all the pfos in the old
     *          list are saved, this will empty the old pfo list; the pfos will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  oldPfoListName the old pfo list name
     *  @param  newPfoListName the new pfo list name
     *  @param  pfosToSave a subset of the old pfo list - only pfos in both this and the old pfo list will be saved
     */
    static pandora::StatusCode SavePfoList(const pandora::Algorithm &algorithm, const std::string &oldPfoListName,
        const std::string &newPfoListName, const pandora::PfoList &pfosToSave);

    /**
     *  @brief  Replace the current pfo list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newPfoListName the name of the replacement pfo list
     */
    static pandora::StatusCode ReplaceCurrentPfoList(const pandora::Algorithm &algorithm, const std::string &newPfoListName);

    /**
     *  @brief  Temporarily replace the current pfo list with another list, which may only be a temporary list. This switch
     *          will persist only for the duration of the algorithm and its daughters; unless otherwise specified, the current list
     *          will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newPfoListName the name of the replacement pfo list
     */
    static pandora::StatusCode TemporarilyReplaceCurrentPfoList(const pandora::Algorithm &algorithm,
        const std::string &newPfoListName);

    /**
     *  @brief  Drop the current pfo list, returning the current list to its default empty/null state
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode DropCurrentPfoList(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Delete a pfo from the current list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the pfo to delete
     */
    static pandora::StatusCode DeletePfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo);

    /**
     *  @brief  Delete a pfo from a specified list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the pfo to delete
     *  @param  pfoListName name of the list containing the pfo
     */
    static pandora::StatusCode DeletePfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
        const std::string &pfoListName);

    /**
     *  @brief  Delete a list of pfos from the current list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pfoList the list of pfos to delete
     */
    static pandora::StatusCode DeletePfos(const pandora::Algorithm &algorithm, const pandora::PfoList &pfoList);

    /**
     *  @brief  Delete a list of pfos from a specified list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pfoList the list of pfos to delete
     *  @param  pfoListName name of the list containing the pfos
     */
    static pandora::StatusCode DeletePfos(const pandora::Algorithm &algorithm, const pandora::PfoList &pfoList,
        const std::string &pfoListName);

    /**
     *  @brief  Add a cluster to a particle flow object
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to add
     */
    static pandora::StatusCode AddClusterToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
        pandora::Cluster *pCluster);

    /**
     *  @brief  Add a track to a particle flow object
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pTrack address of the track to add
     */
    static pandora::StatusCode AddTrackToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
        pandora::Track *pTrack);

    /**
     *  @brief  Remove a cluster from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to remove
     */
    static pandora::StatusCode RemoveClusterFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
        pandora::Cluster *pCluster);

    /**
     *  @brief  Remove a track from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pTrack address of the track to remove
     */
    static pandora::StatusCode RemoveTrackFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
        pandora::Track *pTrack);


    /* MCParticle-related functions */

    /**
     *  @brief  Get the current mc particle list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pMCParticleList to receive the address of the current mc particle list
     */
    static pandora::StatusCode GetCurrentMCParticleList(const pandora::Algorithm &algorithm, const pandora::MCParticleList *&pMCParticleList);

    /**
     *  @brief  Get the current mc particle list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pMCParticleList to receive the address of the current mc particle list
     *  @param  mcParticleListName to receive the current mc particle list name
     */
    static pandora::StatusCode GetCurrentMCParticleList(const pandora::Algorithm &algorithm, const pandora::MCParticleList *&pMCParticleList,
        std::string &mcParticleListName);

    /**
     *  @brief  Get the current mc particle list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  mcParticleListName to receive the current mc particle list name
     */
    static pandora::StatusCode GetCurrentMCParticleListName(const pandora::Algorithm &algorithm, std::string &mcParticleListName);

    /**
     *  @brief  Get a named mc particle list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  mc particleListName the name of the mc particle list
     *  @param  pMCParticleList to receive the address of the mc particle list
     */
    static pandora::StatusCode GetMCParticleList(const pandora::Algorithm &algorithm, const std::string &mcParticleListName,
        const pandora::MCParticleList *&pMCParticleList);

    /**
     *  @brief  Save the current mc particle list under a new name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new mc particle list name
     */
    static pandora::StatusCode SaveMCParticleList(const pandora::Algorithm &algorithm, const pandora::MCParticleList &mcParticleList,
        const std::string &newListName);

    /**
     *  @brief  Replace the current mc particle list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement mc particle list
     */
    static pandora::StatusCode ReplaceCurrentMCParticleList(const pandora::Algorithm &algorithm, const std::string &newListName);

    /**
     *  @brief  Drop the current mc particle list, returning the current list to its default empty/null state
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode DropCurrentMCParticleList(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Repeat the mc particle preparation, performing pfo target identification and forming relationships with tracks/calo hits
     *
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode RepeatMCParticlePreparation(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Remove all mc particle relationships previously registered with the mc manager and linked to tracks/calo hits
     *
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode RemoveAllMCParticleRelationships(const pandora::Algorithm &algorithm);


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
    static pandora::StatusCode InitializeFragmentation(const pandora::Algorithm &algorithm, const pandora::ClusterList &inputClusterList,
        std::string &originalClustersListName, std::string &fragmentClustersListName);

    /**
     *  @brief  End cluster fragmentation operations on clusters in the algorithm input list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterListToSaveName the name of the list containing the clusters chosen to be saved (original or fragments)
     *  @param  clusterListToDeleteName the name of the list containing the clusters chosen to be deleted (original or fragments)
     */
    static pandora::StatusCode EndFragmentation(const pandora::Algorithm &algorithm, const std::string &clusterListToSaveName,
        const std::string &clusterListToDeleteName);

    /**
     *  @brief  Initialize reclustering operations on clusters in the algorithm input list. This allows hits in a list
     *          of clusters (a subset of the algorithm input list) to be redistributed.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  inputTrackList the input track list
     *  @param  inputClusterList the input cluster list
     *  @param  originalClustersListName to receive the name of the list in which the original clusters are stored
     */
    static pandora::StatusCode InitializeReclustering(const pandora::Algorithm &algorithm, const pandora::TrackList &inputTrackList,
        const pandora::ClusterList &inputClusterList, std::string &originalClustersListName);

    /**
     *  @brief  End reclustering operations on clusters in the algorithm input list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  selectedClusterListName the name of the list containing the chosen recluster candidates (or the original candidates)
     */
    static pandora::StatusCode EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName);
};

#endif // #ifndef PANDORA_CONTENT_API_H
