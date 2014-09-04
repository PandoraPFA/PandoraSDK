/**
 *  @file   PandoraSDK/include/Api/PandoraContentApi.h
 *
 *  @brief  Header file for the pandora content api class.
 *
 *  $Log: $
 */
#ifndef PANDORA_CONTENT_API_H
#define PANDORA_CONTENT_API_H 1

#include "Api/PandoraApi.h"

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace pandora { class Algorithm; class AlgorithmTool; class TiXmlElement; }
namespace pandora { class CaloHit; class Cluster; class MCParticle; class ParticleFlowObject; class Track; class Vertex; }

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraContentApi class
 */
class PandoraContentApi
{
public:
    /* Object-creation functions */

    /**
     *  @brief  Object creation helper class
     * 
     *  @param  PARAMETERS the type of object parameters
     */
    template <typename PARAMETERS, typename OBJECT>
    class ObjectCreationHelper
    {
    public:
        typedef PARAMETERS Parameters;
        typedef OBJECT Object;

        /**
         *  @brief  Create a new object
         * 
         *  @param  algorithm the algorithm calling this function
         *  @param  parameters the object parameters
         *  @param  pObject to receive the address of the object created
         */
        static pandora::StatusCode Create(const pandora::Algorithm &algorithm, const Parameters &parameters, Object *&pObject);
    };

    /**
     *  @brief  ClusterParameters class. To build a cluster must provide at least one hit (which may be isolated) or a track address.
     */
    class ClusterParameters
    {
    public:
        pandora::CaloHitList            m_caloHitList;          ///< The calo hit(s) to include
        pandora::CaloHitList            m_isolatedCaloHitList;  ///< The isolated calo hit(s) to include
        pandora::InputTrackAddress      m_pTrack;               ///< The address of the track seeding the cluster
    };

    /**
     *  @brief  ParticleFlowObjectParameters class
     */
    class ParticleFlowObjectParameters
    {
    public:
        pandora::InputInt               m_particleId;           ///< The particle flow object id (PDG code)
        pandora::InputInt               m_charge;               ///< The particle flow object charge
        pandora::InputFloat             m_mass;                 ///< The particle flow object mass
        pandora::InputFloat             m_energy;               ///< The particle flow object energy
        pandora::InputCartesianVector   m_momentum;             ///< The particle flow object momentum
        pandora::ClusterList            m_clusterList;          ///< The clusters in the particle flow object
        pandora::TrackList              m_trackList;            ///< The tracks in the particle flow object
        pandora::VertexList             m_vertexList;           ///< The vertices in the particle flow object
    };

    /**
     *  @brief  Vertex creation class
     */
    class VertexParameters
    {
    public:
        pandora::InputCartesianVector   m_position;             ///< The vertex position
        pandora::InputVertexType        m_vertexType;           ///< The vertex type
    };

    typedef ObjectCreationHelper<ClusterParameters, pandora::Cluster> Cluster;
    typedef ObjectCreationHelper<ParticleFlowObjectParameters, pandora::ParticleFlowObject> ParticleFlowObject;
    typedef ObjectCreationHelper<VertexParameters, pandora::Vertex> Vertex;
    typedef ObjectCreationHelper<PandoraApi::MCParticle::Parameters, pandora::MCParticle> MCParticle;
    typedef ObjectCreationHelper<PandoraApi::Track::Parameters, pandora::Track> Track;
    typedef ObjectCreationHelper<PandoraApi::RectangularCaloHit::Parameters, pandora::CaloHit> CaloHit;
    typedef ObjectCreationHelper<PandoraApi::RectangularCaloHit::Parameters, pandora::CaloHit> RectangularCaloHit;
    typedef ObjectCreationHelper<PandoraApi::PointingCaloHit::Parameters, pandora::CaloHit> PointingCaloHit;


    /* Accessors for plugins and global settings */

    /**
     *  @brief  Get the pandora settings instance
     * 
     *  @param  algorithm the algorithm calling this function
     * 
     *  @return the address of the pandora settings instance
     */
    static const pandora::PandoraSettings *GetSettings(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Get the pandora geometry instance
     * 
     *  @param  algorithm the algorithm calling this function
     * 
     *  @return the address of the pandora geometry instance
     */
    static const pandora::GeometryManager *GetGeometry(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Get the pandora plugin instance, providing access to user registered functions and calculators
     * 
     *  @param  algorithm the algorithm calling this function
     * 
     *  @return the address of the pandora plugin instance
     */
    static const pandora::PluginManager *GetPlugins(const pandora::Algorithm &algorithm);


    /* High-level steering functions */

    /**
     *  @brief  Repeat the event preparation stages, which are used to calculate properties of input objects for later use in algorithms
     *
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode RepeatEventPreparation(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Create an algorithm tool instance, via one of the algorithm tool factories registered with pandora.
     *          This function is expected to be called whilst reading the settings for a parent algorithm.
     * 
     *  @param  algorithm the parent algorithm, which will later run this algorithm tool
     *  @param  pXmlElement address of the xml element describing the algorithm tool type and settings
     *  @param  pAlgorithmTool to receive the address of the algorithm tool instance
     */
    static pandora::StatusCode CreateAlgorithmTool(const pandora::Algorithm &algorithm, pandora::TiXmlElement *const pXmlElement,
        pandora::AlgorithmTool *&pAlgorithmTool);

    /**
     *  @brief  Create an algorithm instance, via one of the algorithm factories registered with pandora.
     *          This function is expected to be called whilst reading the settings for a parent algorithm.
     * 
     *  @param  algorithm address of the parent algorithm, which will later run this daughter algorithm
     *  @param  pXmlElement address of the xml element describing the daughter algorithm type and settings
     *  @param  daughterAlgorithmName to receive the name of the daughter algorithm instance
     */
    static pandora::StatusCode CreateDaughterAlgorithm(const pandora::Algorithm &algorithm, pandora::TiXmlElement *const pXmlElement,
        std::string &daughterAlgorithmName);

    /**
     *  @brief  Run an algorithm registered with pandora, from within a parent algorithm
     * 
     *  @param  algorithm the parent algorithm, now attempting to run a daughter algorithm
     *  @param  daughterAlgorithmName the name of the daughter algorithm instance to run
     */
    static pandora::StatusCode RunDaughterAlgorithm(const pandora::Algorithm &algorithm, const std::string &daughterAlgorithmName);

    /**
     *  @brief  Run a clustering algorithm (an algorithm that will create new cluster objects)
     * 
     *  @param  algorithm the parent algorithm, now attempting to run a daughter clustering algorithm
     *  @param  clusteringAlgorithmName the name of the clustering algorithm to run
     *  @param  pNewClusterList to receive the address of the new cluster list populated
     *  @param  newClusterListName to receive the name of the new cluster list populated
     */
    static pandora::StatusCode RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName,
        const pandora::ClusterList *&pNewClusterList, std::string &newClusterListName);


    /* List-manipulation functions */

    /**
     *  @brief  Get the current list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pT to receive the address of the current list
     */
    template <typename T>
    static pandora::StatusCode GetCurrentList(const pandora::Algorithm &algorithm, const T *&pT);

    /**
     *  @brief  Get the current list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pT to receive the address of the current list
     *  @param  listName to receive the current list name
     */
    template <typename T>
    static pandora::StatusCode GetCurrentList(const pandora::Algorithm &algorithm, const T *&pT, std::string &listName);

    /**
     *  @brief  Get the current list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  listName to receive the current list name
     */
    template <typename T>
    static pandora::StatusCode GetCurrentListName(const pandora::Algorithm &algorithm, std::string &listName);

    /**
     *  @brief  Replace the current list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement list
     */
    template <typename T>
    static pandora::StatusCode ReplaceCurrentList(const pandora::Algorithm &algorithm, const std::string &newListName);

    /**
     *  @brief  Drop the current list, returning the current list to its default empty/null state
     * 
     *  @param  algorithm the algorithm calling this function
     */
    template <typename T>
    static pandora::StatusCode DropCurrentList(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Get a named list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  listName the name of the list
     *  @param  pT to receive the address of the list
     */
    template <typename T>
    static pandora::StatusCode GetList(const pandora::Algorithm &algorithm, const std::string &listName, const T *&pT);


    /* List-manipulation functions: input objects only (CaloHits, Tracks, MCParticles) */

    /**
     *  @brief  Save a provided input object list under a new name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  t the provided input object list
     *  @param  newListName the new list name
     */
    template <typename T>
    static pandora::StatusCode SaveList(const pandora::Algorithm &algorithm, const T &t, const std::string &newListName);


    /* List-manipulation functions: algorithm objects only (Clusters, Pfos, Vertices) */

    /**
     *  @brief  Save the current list in a list with the specified new name. Note that this will empty the list; the objects
     *          will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new list name
     */
    template <typename T>
    static pandora::StatusCode SaveList(const pandora::Algorithm &algorithm, const std::string &newListName);

    /**
     *  @brief  Save a named list in a list with the specified new name. Note that this will empty the old list; the objects
     *          will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  oldListName the old list name
     *  @param  newListName the new list name
     */
    template <typename T>
    static pandora::StatusCode SaveList(const pandora::Algorithm &algorithm, const std::string &oldListName, const std::string &newListName);

    /**
     *  @brief  Save elements of the current list in a list with the specified new name. If all the objects in the current
     *          list are saved, this will empty the current list; the objects will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new list name
     *  @param  t a subset of the current object list - only objects in both this and the current list will be saved
     */
    template <typename T>
    static pandora::StatusCode SaveList(const pandora::Algorithm &algorithm, const std::string &newListName, const T &t);

    /**
     *  @brief  Save elements of a named list in a list with the specified new name. If all the objects in the old
     *          list are saved, this will empty the old list; the objects will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  oldClusterListName the old cluster list name
     *  @param  newClusterListName the new cluster list name
     *  @param  t a subset of the old object list - only objects in both this and the old list will be saved
     */
    template <typename T>
    static pandora::StatusCode SaveList(const pandora::Algorithm &algorithm, const std::string &oldListName, const std::string &newListName,
        const T &t);

    /**
     *  @brief  Temporarily replace the current list with another list, which may only be a temporary list. This switch
     *          will persist only for the duration of the algorithm and its daughters; unless otherwise specified, the current list
     *          will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement list
     */
    template <typename T>
    static pandora::StatusCode TemporarilyReplaceCurrentList(const pandora::Algorithm &algorithm, const std::string &newListName);

    /**
     *  @brief  Create a temporary list and set it to be the current list, enabling object creation
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pT to receive the address of the temporary list
     *  @param  temporaryListName to receive the temporary list name
     */
    template <typename T>
    static pandora::StatusCode CreateTemporaryListAndSetCurrent(const pandora::Algorithm &algorithm, const T *&pT, std::string &temporaryListName);


    /* Object-related functions */

    /**
     *  @brief  Is object, or a list of objects, available as a building block
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pT address of the object
     * 
     *  @return boolean
     */
    template <typename T>
    static bool IsAvailable(const pandora::Algorithm &algorithm, T *pT);


    /* Object-related functions: algorithm objects only (Clusters, Pfos, Vertices) */

    /**
     *  @brief  Delete an object from the current list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pT address of the object, or a list of objects, to delete
     */
    template <typename T>
    static pandora::StatusCode Delete(const pandora::Algorithm &algorithm, T *pT);

    /**
     *  @brief  Delete an object from a specified list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pT address of the object, or a list of objects, to delete
     *  @param  listName name of the list containing the object
     */
    template <typename T>
    static pandora::StatusCode Delete(const pandora::Algorithm &algorithm, T *pT, const std::string &listName);


    /* CaloHit-related functions */

    /**
     *  @brief  Add a calo hit, or a list of calo hits, to a cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pT address of the calo hit, or list of calo hits, to add
     */
    template <typename T>
    static pandora::StatusCode AddToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, T *pT);

    /**
     *  @brief  Remove a calo hit from a cluster. Note this function will not remove the final calo hit from a cluster, and
     *          will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    static pandora::StatusCode RemoveFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit);

    /**
     *  @brief  Add an isolated calo hit, or a list of isolated calo hits, to a cluster. An isolated calo hit is not counted as a
     *          regular calo hit: it contributes only towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pT address of the isolated calo hit, or list of isolated calo hits, to add
     */
    template <typename T>
    static pandora::StatusCode AddIsolatedToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, T *pT);

    /**
     *  @brief  Remove an isolated calo hit from a cluster. Note this function will not remove the final calo hit from a cluster, and
     *          will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the isolated hit to remove
     */
    static pandora::StatusCode RemoveIsolatedFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit);

    /**
     *  @brief  Fragment a calo hit into two daughter calo hits, with a specified energy division
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pOriginalCaloHit address of the original calo hit, which will be deleted
     *  @param  fraction1 the fraction of energy to be assigned to daughter fragment 1
     *  @param  pDaughterCaloHit1 to receive the address of daughter fragment 1
     *  @param  pDaughterCaloHit2 to receive the address of daughter fragment 2
     */
    static pandora::StatusCode Fragment(const pandora::Algorithm &algorithm, pandora::CaloHit *pOriginalCaloHit, const float fraction1,
        pandora::CaloHit *&pDaughterCaloHit1, pandora::CaloHit *&pDaughterCaloHit2);

    /**
     *  @brief  Merge two calo hit fragments, originally from the same parent hit, to form a new calo hit
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pFragmentCaloHit1 address of calo hit fragment 1, which will be deleted
     *  @param  pFragmentCaloHit2 address of calo hit fragment 2, which will be deleted
     *  @param  pMergedCaloHit to receive the address of the merged calo hit
     */
    static pandora::StatusCode MergeFragments(const pandora::Algorithm &algorithm, pandora::CaloHit *pFragmentCaloHit1,
        pandora::CaloHit *pFragmentCaloHit2, pandora::CaloHit *&pMergedCaloHit);


    /* Track-related functions */

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


    /* MCParticle-related functions */

    /**
     *  @brief  Remove all mc particle relationships previously registered with the mc manager and linked to tracks/calo hits
     *
     *  @param  algorithm the algorithm calling this function
     */
    static pandora::StatusCode RemoveAllMCParticleRelationships(const pandora::Algorithm &algorithm);


    /* Cluster-related functions */

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
     *  @brief  Add a cluster to a particle flow object
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to add
     */
    template <typename T>
    static pandora::StatusCode AddToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo, T *pT);

    /**
     *  @brief  Remove a cluster from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to remove
     */
    template <typename T>
    static pandora::StatusCode RemoveFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo, T *pT);

    /**
     *  @brief  Set parent-daughter particle flow object relationship
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pParentPfo address of parent particle flow object
     *  @param  pDaughterPfo address of daughter particle flow object
     */
    static pandora::StatusCode SetPfoParentDaughterRelationship(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParentPfo,
        pandora::ParticleFlowObject *pDaughterPfo);

    /**
     *  @brief  Remove parent-daughter particle flow object relationship
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pParentPfo address of parent particle flow object
     *  @param  pDaughterPfo address of daughter particle flow object
     */
    static pandora::StatusCode RemovePfoParentDaughterRelationship(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParentPfo,
        pandora::ParticleFlowObject *pDaughterPfo);


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
