/**
 *  @file   PandoraSDK/src/Api/PandoraContentApi.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Pandora/Algorithm.h"
#include "Pandora/Pandora.h"

template <typename PARAMETERS, typename OBJECT>
pandora::StatusCode PandoraContentApi::ObjectCreationHelper<PARAMETERS, OBJECT>::Create(const pandora::Algorithm &algorithm,
    const Parameters &parameters, Object *&pObject)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->CreateObject(parameters, pObject);
}

//------------------------------------------------------------------------------------------------------------------------------------------

const pandora::PandoraSettings *PandoraContentApi::GetSettings(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->GetSettings();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const pandora::GeometryManager *PandoraContentApi::GetGeometry(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->GetGeometry();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const pandora::PluginManager *PandoraContentApi::GetPlugins(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->GetPlugins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RepeatEventPreparation(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RepeatEventPreparation();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::CreateAlgorithmTool(const pandora::Algorithm &algorithm, pandora::TiXmlElement *const pXmlElement,
    pandora::AlgorithmTool *&pAlgorithmTool)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->CreateAlgorithmTool(pXmlElement, pAlgorithmTool);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::CreateDaughterAlgorithm(const pandora::Algorithm &algorithm, pandora::TiXmlElement *const pXmlElement,
    std::string &daughterAlgorithmName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->CreateDaughterAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunDaughterAlgorithm(const pandora::Algorithm &algorithm, const std::string &daughterAlgorithmName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RunAlgorithm(daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName,
    const pandora::ClusterList *&pNewClusterList, std::string &newClusterListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::GetCurrentList(const pandora::Algorithm &algorithm, const T *&pT)
{
    std::string listName;
    return algorithm.GetPandora().GetPandoraContentApiImpl()->GetCurrentList(pT, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::GetCurrentList(const pandora::Algorithm &algorithm, const T *&pT, std::string &listName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->GetCurrentList(pT, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::GetCurrentListName(const pandora::Algorithm &algorithm, std::string &listName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->GetCurrentListName<T>(listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::ReplaceCurrentList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->ReplaceCurrentList<T>(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::DropCurrentList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->DropCurrentList<T>(algorithm);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::GetList(const pandora::Algorithm &algorithm, const std::string &listName, const T *&pT)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->GetList(listName, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const T &t, const std::string &newListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->SaveList(t, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->SaveList<T>(newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const std::string &oldListName,
    const std::string &newListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->SaveList<T>(oldListName, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const std::string &newListName, const T &t)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->SaveList(newListName, t);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const std::string &oldListName,
    const std::string &newListName, const T &t)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->SaveList(oldListName, newListName, t);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->TemporarilyReplaceCurrentList<T>(newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::CreateTemporaryListAndSetCurrent(const pandora::Algorithm &algorithm, const T *&pT,
    std::string &temporaryListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->CreateTemporaryListAndSetCurrent(algorithm, pT, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool PandoraContentApi::IsAvailable(const pandora::Algorithm &algorithm, T *pT)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->IsAvailable(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::Delete(const pandora::Algorithm &algorithm, T *pT)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->Delete(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::Delete(const pandora::Algorithm &algorithm, T *pT, const std::string &listName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->Delete(pT, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::AddToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, T *pT)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->AddToCluster(pCluster, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RemoveFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::AddIsolatedToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, T *pT)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->AddIsolatedToCluster(pCluster, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveIsolatedFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RemoveIsolatedFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::Fragment(const pandora::Algorithm &algorithm, pandora::CaloHit *pOriginalCaloHit,
    const float fraction1, pandora::CaloHit *&pDaughterCaloHit1, pandora::CaloHit *&pDaughterCaloHit2)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->Fragment(pOriginalCaloHit, fraction1, pDaughterCaloHit1, pDaughterCaloHit2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeFragments(const pandora::Algorithm &algorithm, pandora::CaloHit *pFragmentCaloHit1,
    pandora::CaloHit *pFragmentCaloHit2, pandora::CaloHit *&pMergedCaloHit)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->MergeFragments(pFragmentCaloHit1, pFragmentCaloHit2, pMergedCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->AddTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RemoveTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveCurrentTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RemoveCurrentTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveAllTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RemoveAllTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveAllMCParticleRelationships(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RemoveAllMCParticleRelationships();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete, const std::string &enlargeListName, const std::string &deleteListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete, enlargeListName, deleteListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::AddToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo, T *pT)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->AddToPfo(pPfo, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::RemoveFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo, T *pT)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RemoveFromPfo(pPfo, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SetPfoParentDaughterRelationship(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParentPfo,
    pandora::ParticleFlowObject *pDaughterPfo)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->SetPfoParentDaughterRelationship(pParentPfo, pDaughterPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemovePfoParentDaughterRelationship(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParentPfo,
    pandora::ParticleFlowObject *pDaughterPfo)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->RemovePfoParentDaughterRelationship(pParentPfo, pDaughterPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::InitializeFragmentation(const pandora::Algorithm &algorithm, const pandora::ClusterList &inputClusterList,
    std::string &originalClustersListName, std::string &fragmentClustersListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->InitializeFragmentation(algorithm, inputClusterList, originalClustersListName,
        fragmentClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::EndFragmentation(const pandora::Algorithm &algorithm, const std::string &clusterListToSaveName,
    const std::string &clusterListToDeleteName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->EndFragmentation(algorithm, clusterListToSaveName, clusterListToDeleteName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::InitializeReclustering(const pandora::Algorithm &algorithm, const pandora::TrackList &inputTrackList,
    const pandora::ClusterList &inputClusterList, std::string &originalClustersListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->InitializeReclustering(algorithm, inputTrackList, inputClusterList, originalClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName)
{
    return algorithm.GetPandora().GetPandoraContentApiImpl()->EndReclustering(algorithm, selectedClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class PandoraContentApi::ObjectCreationHelper<PandoraContentApi::Cluster::Parameters, pandora::Cluster>;
template class PandoraContentApi::ObjectCreationHelper<PandoraContentApi::ParticleFlowObject::Parameters, pandora::ParticleFlowObject>;
template class PandoraContentApi::ObjectCreationHelper<PandoraContentApi::Vertex::Parameters, pandora::Vertex>;
template class PandoraContentApi::ObjectCreationHelper<PandoraApi::MCParticle::Parameters, pandora::MCParticle>;
template class PandoraContentApi::ObjectCreationHelper<PandoraApi::Track::Parameters, pandora::Track>;
template class PandoraContentApi::ObjectCreationHelper<PandoraApi::RectangularCaloHit::Parameters, pandora::CaloHit>;
template class PandoraContentApi::ObjectCreationHelper<PandoraApi::PointingCaloHit::Parameters, pandora::CaloHit>;

template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::CaloHitList>(const pandora::Algorithm &, const pandora::CaloHitList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::TrackList>(const pandora::Algorithm &, const pandora::TrackList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::MCParticleList>(const pandora::Algorithm &, const pandora::MCParticleList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::ClusterList>(const pandora::Algorithm &, const pandora::ClusterList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::PfoList>(const pandora::Algorithm &, const pandora::PfoList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::VertexList>(const pandora::Algorithm &, const pandora::VertexList *&);

template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::CaloHitList>(const pandora::Algorithm &, const pandora::CaloHitList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::TrackList>(const pandora::Algorithm &, const pandora::TrackList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::MCParticleList>(const pandora::Algorithm &, const pandora::MCParticleList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::ClusterList>(const pandora::Algorithm &, const pandora::ClusterList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::PfoList>(const pandora::Algorithm &, const pandora::PfoList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::VertexList>(const pandora::Algorithm &, const pandora::VertexList *&, std::string &);

template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::CaloHit>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::Track>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::MCParticle>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::Cluster>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::ParticleFlowObject>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::Vertex>(const pandora::Algorithm &, std::string &);

template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::CaloHit>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::Track>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::MCParticle>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::Cluster>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::ParticleFlowObject>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::Vertex>(const pandora::Algorithm &, const std::string &);

template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::CaloHit>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::Track>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::MCParticle>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::Cluster>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::ParticleFlowObject>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::Vertex>(const pandora::Algorithm &);

template pandora::StatusCode PandoraContentApi::GetList<pandora::CaloHitList>(const pandora::Algorithm &, const std::string &, const pandora::CaloHitList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::TrackList>(const pandora::Algorithm &, const std::string &, const pandora::TrackList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::MCParticleList>(const pandora::Algorithm &, const std::string &, const pandora::MCParticleList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::ClusterList>(const pandora::Algorithm &, const std::string &, const pandora::ClusterList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::PfoList>(const pandora::Algorithm &, const std::string &, const pandora::PfoList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::VertexList>(const pandora::Algorithm &, const std::string &, const pandora::VertexList *&);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::CaloHitList>(const pandora::Algorithm &, const pandora::CaloHitList &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::TrackList>(const pandora::Algorithm &, const pandora::TrackList &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::MCParticleList>(const pandora::Algorithm &, const pandora::MCParticleList &, const std::string &);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::Cluster>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::ParticleFlowObject>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::Vertex>(const pandora::Algorithm &, const std::string &);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::Cluster>(const pandora::Algorithm &, const std::string &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::ParticleFlowObject>(const pandora::Algorithm &, const std::string &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::Vertex>(const pandora::Algorithm &, const std::string &, const std::string &);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::ClusterList>(const pandora::Algorithm &, const std::string &, const pandora::ClusterList &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::PfoList>(const pandora::Algorithm &, const std::string &, const pandora::PfoList &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::VertexList>(const pandora::Algorithm &, const std::string &, const pandora::VertexList &);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::ClusterList>(const pandora::Algorithm &, const std::string &, const std::string &, const pandora::ClusterList &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::PfoList>(const pandora::Algorithm &, const std::string &, const std::string &, const pandora::PfoList &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::VertexList>(const pandora::Algorithm &, const std::string &, const std::string &, const pandora::VertexList &);

template pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentList<pandora::Cluster>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentList<pandora::ParticleFlowObject>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentList<pandora::Vertex>(const pandora::Algorithm &, const std::string &);

template pandora::StatusCode PandoraContentApi::CreateTemporaryListAndSetCurrent<pandora::ClusterList>(const pandora::Algorithm &, const pandora::ClusterList *&, std::string &);
template pandora::StatusCode PandoraContentApi::CreateTemporaryListAndSetCurrent<pandora::PfoList>(const pandora::Algorithm &, const pandora::PfoList *&, std::string &);
template pandora::StatusCode PandoraContentApi::CreateTemporaryListAndSetCurrent<pandora::VertexList>(const pandora::Algorithm &, const pandora::VertexList *&, std::string &);

template bool PandoraContentApi::IsAvailable<pandora::CaloHit>(const pandora::Algorithm &, pandora::CaloHit *);
template bool PandoraContentApi::IsAvailable<pandora::CaloHitList>(const pandora::Algorithm &, pandora::CaloHitList *);
template bool PandoraContentApi::IsAvailable<pandora::Track>(const pandora::Algorithm &, pandora::Track *);
template bool PandoraContentApi::IsAvailable<pandora::Cluster>(const pandora::Algorithm &, pandora::Cluster *);
template bool PandoraContentApi::IsAvailable<pandora::Vertex>(const pandora::Algorithm &, pandora::Vertex *);

template pandora::StatusCode PandoraContentApi::Delete<pandora::Cluster>(const pandora::Algorithm &, pandora::Cluster *);
template pandora::StatusCode PandoraContentApi::Delete<pandora::ClusterList>(const pandora::Algorithm &, pandora::ClusterList *);
template pandora::StatusCode PandoraContentApi::Delete<pandora::ParticleFlowObject>(const pandora::Algorithm &, pandora::ParticleFlowObject *);
template pandora::StatusCode PandoraContentApi::Delete<pandora::PfoList>(const pandora::Algorithm &, pandora::PfoList *);
template pandora::StatusCode PandoraContentApi::Delete<pandora::Vertex>(const pandora::Algorithm &, pandora::Vertex *);
template pandora::StatusCode PandoraContentApi::Delete<pandora::VertexList>(const pandora::Algorithm &, pandora::VertexList *);

template pandora::StatusCode PandoraContentApi::Delete<pandora::Cluster>(const pandora::Algorithm &, pandora::Cluster *, const std::string &);
template pandora::StatusCode PandoraContentApi::Delete<pandora::ClusterList>(const pandora::Algorithm &, pandora::ClusterList *, const std::string &);
template pandora::StatusCode PandoraContentApi::Delete<pandora::ParticleFlowObject>(const pandora::Algorithm &, pandora::ParticleFlowObject *, const std::string &);
template pandora::StatusCode PandoraContentApi::Delete<pandora::PfoList>(const pandora::Algorithm &, pandora::PfoList *, const std::string &);
template pandora::StatusCode PandoraContentApi::Delete<pandora::Vertex>(const pandora::Algorithm &, pandora::Vertex *, const std::string &);
template pandora::StatusCode PandoraContentApi::Delete<pandora::VertexList>(const pandora::Algorithm &, pandora::VertexList *, const std::string &);

template pandora::StatusCode PandoraContentApi::AddToCluster<pandora::CaloHit>(const pandora::Algorithm &, pandora::Cluster *, pandora::CaloHit *);
template pandora::StatusCode PandoraContentApi::AddToCluster<pandora::CaloHitList>(const pandora::Algorithm &, pandora::Cluster *, pandora::CaloHitList *);

template pandora::StatusCode PandoraContentApi::AddIsolatedToCluster<pandora::CaloHit>(const pandora::Algorithm &, pandora::Cluster *, pandora::CaloHit *);
template pandora::StatusCode PandoraContentApi::AddIsolatedToCluster<pandora::CaloHitList>(const pandora::Algorithm &, pandora::Cluster *, pandora::CaloHitList *);

template pandora::StatusCode PandoraContentApi::AddToPfo<pandora::Cluster>(const pandora::Algorithm &, pandora::ParticleFlowObject *, pandora::Cluster *);
template pandora::StatusCode PandoraContentApi::AddToPfo<pandora::Track>(const pandora::Algorithm &, pandora::ParticleFlowObject *, pandora::Track *);
template pandora::StatusCode PandoraContentApi::AddToPfo<pandora::Vertex>(const pandora::Algorithm &, pandora::ParticleFlowObject *, pandora::Vertex *);

template pandora::StatusCode PandoraContentApi::RemoveFromPfo<pandora::Cluster>(const pandora::Algorithm &, pandora::ParticleFlowObject *, pandora::Cluster *);
template pandora::StatusCode PandoraContentApi::RemoveFromPfo<pandora::Track>(const pandora::Algorithm &, pandora::ParticleFlowObject *, pandora::Track *);
template pandora::StatusCode PandoraContentApi::RemoveFromPfo<pandora::Vertex>(const pandora::Algorithm &, pandora::ParticleFlowObject *, pandora::Vertex *);
