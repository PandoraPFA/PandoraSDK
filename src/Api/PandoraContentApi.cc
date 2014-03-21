/**
 *  @file   PandoraPFANew/Framework/src/Api/PandoraContentApi.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Pandora/Algorithm.h"

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

template <typename PARAMETERS>
pandora::StatusCode PandoraContentApi::Cluster::Create(const pandora::Algorithm &algorithm, PARAMETERS *pClusterParameters,
    pandora::Cluster *&pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->CreateCluster(pClusterParameters, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ParticleFlowObject::Create(const pandora::Algorithm &algorithm, const Parameters &parameters,
    pandora::ParticleFlowObject *&pPfo)
{
    return algorithm.GetPandoraContentApiImpl()->CreateParticleFlowObject(parameters, pPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RepeatEventPreparation(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RepeatEventPreparation();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::CreateAlgorithmTool(const pandora::Algorithm &parentAlgorithm, pandora::TiXmlElement *const pXmlElement,
    pandora::AlgorithmTool *&pAlgorithmTool)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->CreateAlgorithmTool(pXmlElement, pAlgorithmTool);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::CreateDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, pandora::TiXmlElement *const pXmlElement,
    std::string &daughterAlgorithmName)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->CreateDaughterAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, const std::string &daughterAlgorithmName)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->RunAlgorithm(daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName,
    const pandora::ClusterList *&pNewClusterList, std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::GetCurrentList(const pandora::Algorithm &algorithm, const T *&pT)
{
    std::string listName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentList(pT, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::GetCurrentList(const pandora::Algorithm &algorithm, const T *&pT, std::string &listName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentList(pT, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::GetCurrentListName(const pandora::Algorithm &algorithm, std::string &listName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentListName<T>(listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::ReplaceCurrentList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentList<T>(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::DropCurrentList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentList<T>();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::GetList(const pandora::Algorithm &algorithm, const std::string &listName, const T *&pT)
{
    return algorithm.GetPandoraContentApiImpl()->GetList(listName, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const T &t, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveList(t, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveList<T>(newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const std::string &oldListName, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveList<T>(oldListName, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const std::string &newListName, const T &t)
{
    return algorithm.GetPandoraContentApiImpl()->SaveList(newListName, t);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::SaveList(const pandora::Algorithm &algorithm, const std::string &oldListName, const std::string &newListName,
    const T &t)
{
    return algorithm.GetPandoraContentApiImpl()->SaveList(oldListName, newListName, t);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->TemporarilyReplaceCurrentList<T>(newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::CreateTemporaryListAndSetCurrent(const pandora::Algorithm &algorithm, const T *&pT, std::string &temporaryListName)
{
    return algorithm.GetPandoraContentApiImpl()->CreateTemporaryListAndSetCurrent(algorithm, pT, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool PandoraContentApi::IsAvailable(const pandora::Algorithm &algorithm, T *pT)
{
    return algorithm.GetPandoraContentApiImpl()->IsAvailable(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::Delete(const pandora::Algorithm &algorithm, T *pT)
{
    return algorithm.GetPandoraContentApiImpl()->Delete(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::Delete(const pandora::Algorithm &algorithm, T *pT, const std::string &listName)
{
    return algorithm.GetPandoraContentApiImpl()->Delete(pT, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->AddToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddIsolatedToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora:: CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->AddIsolatedToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveIsolatedFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveIsolatedFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::Fragment(const pandora::Algorithm &algorithm, pandora::CaloHit *pOriginalCaloHit, const float fraction1,
    pandora::CaloHit *&pDaughterCaloHit1, pandora::CaloHit *&pDaughterCaloHit2)
{
    return algorithm.GetPandoraContentApiImpl()->Fragment(pOriginalCaloHit, fraction1, pDaughterCaloHit1, pDaughterCaloHit2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeFragments(const pandora::Algorithm &algorithm, pandora::CaloHit *pFragmentCaloHit1,
    pandora::CaloHit *pFragmentCaloHit2, pandora::CaloHit *&pMergedCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->MergeFragments(pFragmentCaloHit1, pFragmentCaloHit2, pMergedCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->AddTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveCurrentTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveCurrentTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveAllTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveAllTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RepeatMCParticlePreparation(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RepeatMCParticlePreparation();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveAllMCParticleRelationships(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveAllMCParticleRelationships();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete, const std::string &enlargeListName, const std::string &deleteListName)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete, enlargeListName, deleteListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::AddToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo, T *pT)
{
    return algorithm.GetPandoraContentApiImpl()->AddToPfo(pPfo, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode PandoraContentApi::RemoveFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo, T *pT)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveFromPfo(pPfo, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SetPfoParentDaughterRelationship(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParentPfo,
    pandora::ParticleFlowObject *pDaughterPfo)
{
    return algorithm.GetPandoraContentApiImpl()->SetPfoParentDaughterRelationship(pParentPfo, pDaughterPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemovePfoParentDaughterRelationship(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParentPfo,
    pandora::ParticleFlowObject *pDaughterPfo)
{
    return algorithm.GetPandoraContentApiImpl()->RemovePfoParentDaughterRelationship(pParentPfo, pDaughterPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::InitializeFragmentation(const pandora::Algorithm &algorithm, const pandora::ClusterList &inputClusterList,
    std::string &originalClustersListName, std::string &fragmentClustersListName)
{
    return algorithm.GetPandoraContentApiImpl()->InitializeFragmentation(algorithm, inputClusterList, originalClustersListName,
        fragmentClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::EndFragmentation(const pandora::Algorithm &algorithm, const std::string &clusterListToSaveName,
    const std::string &clusterListToDeleteName)
{
    return algorithm.GetPandoraContentApiImpl()->EndFragmentation(algorithm, clusterListToSaveName, clusterListToDeleteName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::InitializeReclustering(const pandora::Algorithm &algorithm, const pandora::TrackList &inputTrackList,
    const pandora::ClusterList &inputClusterList, std::string &originalClustersListName)
{
    return algorithm.GetPandoraContentApiImpl()->InitializeReclustering(algorithm, inputTrackList, inputClusterList, originalClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->EndReclustering(algorithm, selectedClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &, pandora::CaloHit *, pandora::Cluster *&);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHitList>(const pandora::Algorithm &, pandora::CaloHitList *, pandora::Cluster *&);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &, pandora::Track *, pandora::Cluster *&);

template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::CaloHitList>(const pandora::Algorithm &, const pandora::CaloHitList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::TrackList>(const pandora::Algorithm &, const pandora::TrackList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::MCParticleList>(const pandora::Algorithm &, const pandora::MCParticleList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::ClusterList>(const pandora::Algorithm &, const pandora::ClusterList *&);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::PfoList>(const pandora::Algorithm &, const pandora::PfoList *&);

template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::CaloHitList>(const pandora::Algorithm &, const pandora::CaloHitList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::TrackList>(const pandora::Algorithm &, const pandora::TrackList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::MCParticleList>(const pandora::Algorithm &, const pandora::MCParticleList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::ClusterList>(const pandora::Algorithm &, const pandora::ClusterList *&, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentList<pandora::PfoList>(const pandora::Algorithm &, const pandora::PfoList *&, std::string &);

template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::CaloHit>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::Track>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::MCParticle>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::Cluster>(const pandora::Algorithm &, std::string &);
template pandora::StatusCode PandoraContentApi::GetCurrentListName<pandora::ParticleFlowObject>(const pandora::Algorithm &, std::string &);

template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::CaloHit>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::Track>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::MCParticle>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::Cluster>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::ReplaceCurrentList<pandora::ParticleFlowObject>(const pandora::Algorithm &, const std::string &);

template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::CaloHit>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::Track>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::MCParticle>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::Cluster>(const pandora::Algorithm &);
template pandora::StatusCode PandoraContentApi::DropCurrentList<pandora::ParticleFlowObject>(const pandora::Algorithm &);

template pandora::StatusCode PandoraContentApi::GetList<pandora::CaloHitList>(const pandora::Algorithm &, const std::string &, const pandora::CaloHitList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::TrackList>(const pandora::Algorithm &, const std::string &, const pandora::TrackList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::MCParticleList>(const pandora::Algorithm &, const std::string &, const pandora::MCParticleList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::ClusterList>(const pandora::Algorithm &, const std::string &, const pandora::ClusterList *&);
template pandora::StatusCode PandoraContentApi::GetList<pandora::PfoList>(const pandora::Algorithm &, const std::string &, const pandora::PfoList *&);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::CaloHitList>(const pandora::Algorithm &, const pandora::CaloHitList &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::TrackList>(const pandora::Algorithm &, const pandora::TrackList &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::MCParticleList>(const pandora::Algorithm &, const pandora::MCParticleList &, const std::string &);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::Cluster>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::ParticleFlowObject>(const pandora::Algorithm &, const std::string &);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::Cluster>(const pandora::Algorithm &, const std::string &, const std::string &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::ParticleFlowObject>(const pandora::Algorithm &, const std::string &, const std::string &);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::ClusterList>(const pandora::Algorithm &, const std::string &, const pandora::ClusterList &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::PfoList>(const pandora::Algorithm &, const std::string &, const pandora::PfoList &);

template pandora::StatusCode PandoraContentApi::SaveList<pandora::ClusterList>(const pandora::Algorithm &, const std::string &, const std::string &, const pandora::ClusterList &);
template pandora::StatusCode PandoraContentApi::SaveList<pandora::PfoList>(const pandora::Algorithm &, const std::string &, const std::string &, const pandora::PfoList &);

template pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentList<pandora::Cluster>(const pandora::Algorithm &, const std::string &);
template pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentList<pandora::ParticleFlowObject>(const pandora::Algorithm &, const std::string &);

template pandora::StatusCode PandoraContentApi::CreateTemporaryListAndSetCurrent<pandora::ClusterList>(const pandora::Algorithm &, const pandora::ClusterList *&, std::string &);
template pandora::StatusCode PandoraContentApi::CreateTemporaryListAndSetCurrent<pandora::PfoList>(const pandora::Algorithm &, const pandora::PfoList *&, std::string &);

template bool PandoraContentApi::IsAvailable<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *);
template bool PandoraContentApi::IsAvailable<pandora::CaloHitList>(const pandora::Algorithm &algorithm, pandora::CaloHitList *);

template pandora::StatusCode PandoraContentApi::Delete<pandora::Cluster>(const pandora::Algorithm &, pandora::Cluster *);
template pandora::StatusCode PandoraContentApi::Delete<pandora::ClusterList>(const pandora::Algorithm &, pandora::ClusterList *);
template pandora::StatusCode PandoraContentApi::Delete<pandora::ParticleFlowObject>(const pandora::Algorithm &, pandora::ParticleFlowObject *);
template pandora::StatusCode PandoraContentApi::Delete<pandora::PfoList>(const pandora::Algorithm &, pandora::PfoList *);

template pandora::StatusCode PandoraContentApi::Delete<pandora::Cluster>(const pandora::Algorithm &, pandora::Cluster *, const std::string &);
template pandora::StatusCode PandoraContentApi::Delete<pandora::ClusterList>(const pandora::Algorithm &, pandora::ClusterList *, const std::string &);
template pandora::StatusCode PandoraContentApi::Delete<pandora::ParticleFlowObject>(const pandora::Algorithm &, pandora::ParticleFlowObject *, const std::string &);
template pandora::StatusCode PandoraContentApi::Delete<pandora::PfoList>(const pandora::Algorithm &, pandora::PfoList *, const std::string &);

template pandora::StatusCode PandoraContentApi::AddToPfo<pandora::Cluster>(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *, pandora::Cluster *);
template pandora::StatusCode PandoraContentApi::AddToPfo<pandora::Track>(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *, pandora::Track *);

template pandora::StatusCode PandoraContentApi::RemoveFromPfo<pandora::Cluster>(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *, pandora::Cluster *);
template pandora::StatusCode PandoraContentApi::RemoveFromPfo<pandora::Track>(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *, pandora::Track *);
